//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2021 Eran Ifrah
// file name            : RustPlugin.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "RustPlugin.hpp"

#include "AsyncProcess/asyncprocess.h"
#include "CargoToml.hpp"
#include "CompilerLocator/CompilerLocatorRustc.hpp"
#include "FileSystemWorkspace/NewFileSystemWorkspaceDialog.h"
#include "FileSystemWorkspace/clFileSystemWorkspace.hpp"
#include "FileSystemWorkspace/clFileSystemWorkspaceConfig.hpp"
#include "Platform/Platform.hpp"
#include "Rust/clRustLocator.hpp"
#include "RustWorkspace.hpp"
#include "build_settings_config.h"
#include "clFilesCollector.h"
#include "clWorkspaceManager.h"
#include "cl_standard_paths.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "macros.h"
#include "md5/wxmd5.h"

#include <wx/dir.h>
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>
#include <wx/utils.h>

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    return new RustPlugin(manager);
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName("Rust");
    info.SetDescription(_("Rust plugin for CodeLite IDE"));
    info.SetVersion("v1.0");
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

RustPlugin::RustPlugin(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Rust plugin for CodeLite IDE");
    m_shortName = "Rust";
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FOLDER, &RustPlugin::OnFolderContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_FS_NEW_WORKSPACE_FILE_CREATED, &RustPlugin::OnRustWorkspaceFileCreated, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &RustPlugin::OnNewWorkspace, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_OUTPUT_HOTSPOT_CLICKED, &RustPlugin::OnBuildErrorLineClicked, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_ENDED, &RustPlugin::OnBuildEnded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &RustPlugin::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &RustPlugin::OnWorkspaceClosed, this);
    clWorkspaceManager::Get().RegisterWorkspace(new RustWorkspace());
    AddRustcCompilerIfMissing(); // make sure we got the rustc compiler if missing
}

RustPlugin::~RustPlugin() {}

void RustPlugin::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }

void RustPlugin::CreatePluginMenu(wxMenu* pluginsMenu) { wxUnusedVar(pluginsMenu); }

void RustPlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void RustPlugin::UnPlug()
{
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FOLDER, &RustPlugin::OnFolderContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_FS_NEW_WORKSPACE_FILE_CREATED, &RustPlugin::OnRustWorkspaceFileCreated, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &RustPlugin::OnNewWorkspace, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILD_OUTPUT_HOTSPOT_CLICKED, &RustPlugin::OnBuildErrorLineClicked, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILD_ENDED, &RustPlugin::OnBuildEnded, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &RustPlugin::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &RustPlugin::OnWorkspaceClosed, this);
}

void RustPlugin::OnFolderContextMenu(clContextMenuEvent& event) { event.Skip(); }

void RustPlugin::OnRustWorkspaceFileCreated(clFileSystemEvent& event)
{
    event.Skip();
    wxFileName workspaceFile(event.GetPath());
    wxFileName fnCargoToml(workspaceFile);
    fnCargoToml.SetFullName("Cargo.toml");
    if (fnCargoToml.FileExists()) {
        CargoToml cargoToml;
        const wxString& name = cargoToml.Load(fnCargoToml).GetName();

        // update the build commands
        clFileSystemWorkspaceSettings settings;
        if (!settings.Load(workspaceFile)) {
            clWARNING() << "Failed to load file:" << workspaceFile << endl;
            return;
        }

        wxString cargo_exe = "cargo";
#ifdef __WXMSW__
        clRustLocator rust_locator;
        if (rust_locator.Locate()) {
            cargo_exe = rust_locator.GetRustTool("cargo");
            ::WrapWithQuotes(cargo_exe);
        }
#endif

        auto debug = settings.GetConfig("Debug");
        if (debug) {
            clDEBUG() << "Setting project preferences..." << endl;
            debug->SetBuildTargets(
                { { "build", cargo_exe + " build --color always" },
                  { "clean", cargo_exe + " clean --color always" },
                  { "tests", cargo_exe + " test --color always" },
                  { "clippy", cargo_exe + " clippy --color always --all-features --all-targets -- -D warnings" } });
            wxFileName target_exe(workspaceFile.GetPath() + "/target/debug/" + name);
#ifdef __WXMSW__
            target_exe.SetExt("exe");
#endif
            debug->SetExecutable(target_exe.GetFullPath());
            debug->SetFileExtensions(debug->GetFileExtensions() + ";*.rs;*.toml");
            debug->SetCompiler("rustc");
            // set the environment variable to point to rust-gdb
            wxString env_str;
#if !defined(__WXMSW__)
            // use rust-gdb
            wxFileName rust_gdb = GetRustTool("rust-gdb");
            if (rust_gdb.IsOk()) {
                env_str << "GDB=" << rust_gdb.GetFullPath() << "\n";
                env_str << "PATH=" << rust_gdb.GetPath() << clPATH_SEPARATOR << "$PATH"
                        << "\n";
            }
#else
            // use gdb batch file wrapper
            wxFileName gdb_python_module_directory(clStandardPaths::Get().GetUserDataDir(), wxEmptyString);
            gdb_python_module_directory.AppendDir("gdb_printers");
            gdb_python_module_directory.AppendDir("rustlib");
            gdb_python_module_directory.AppendDir("etc");

            wxFileName gdb_exe = GetRustTool("gdb");
            if (gdb_exe.FileExists()) {
                env_str << "RUST_GDB=" << gdb_exe.GetFullPath() << "\n";
                env_str << "PATH=" << gdb_exe.GetPath() << clPATH_SEPARATOR << "$PATH\n";
            } else {
                env_str << "RUST_GDB=gdb\n";
            }

            // copy the rust-gdb.bat script into our local folder and replace any place-holders
            wxFileName rust_gdb_script(clStandardPaths::Get().GetBinFolder(), "rust-gdb.bat");
            wxString rust_gdb_script_content;
            FileUtils::ReadFileContent(rust_gdb_script, rust_gdb_script_content);
            rust_gdb_script_content.Replace("%SCRIPTS_PATH%", gdb_python_module_directory.GetPath());
            rust_gdb_script.SetPath(clStandardPaths::Get().GetUserDataDir());
            FileUtils::WriteFileContent(rust_gdb_script, rust_gdb_script_content);

            debug->SetDebuggerPath(rust_gdb_script.GetFullPath());
            // build the startup commands
            wxString gdbinit_content = R"gdbinit(
python
import sys
sys.path.insert(0, '%GDB_PYTHON_MODULE_DIRECTORY%')
import gdb_load_rust_pretty_printers
end
)gdbinit";
            gdbinit_content.Trim().Trim(false);
            wxString fixed_path = gdb_python_module_directory.GetPath();
            fixed_path.Replace("\\", "/");
            gdbinit_content.Replace("%GDB_PYTHON_MODULE_DIRECTORY%", fixed_path);
            debug->SetDebuggerCommands(gdbinit_content);
#endif
            if (!env_str.empty()) {
                debug->SetEnvironment(env_str);
            }
            AddRustcCompilerIfMissing();
            debug->SetCompiler("rustc");
        }
        settings.Save(workspaceFile);
    }
}

void RustPlugin::OnNewWorkspace(clCommandEvent& e)
{
    e.Skip();
    if (e.GetString() == "Rust") {
        e.Skip(false);
        // Prompt the user for the folder to run 'cargo new'
        NewFileSystemWorkspaceDialog dlg(EventNotifier::Get()->TopFrame(), false /* do not auto update the name */);
        dlg.SetLabel(_("Select the folder to run 'cargo new'"));
        if (dlg.ShowModal() != wxID_OK) {
            return;
        }

        EnvSetter env;
        wxString cargo_exe;
        if (!ThePlatform->Which("cargo", &cargo_exe)) {
            wxMessageBox(_("Could not locate cargo in your PATH"), "CodeLite", wxICON_ERROR | wxCENTRE);
            return;
        }

        ::WrapWithQuotes(cargo_exe);

        wxString command;
        command << cargo_exe << " new " << dlg.GetWorkspaceName();
        IProcess::Ptr_t process(::CreateSyncProcess(command, IProcessCreateDefault | IProcessCreateWithHiddenConsole,
                                                    dlg.GetWorkspacePath()));
        if (!process) {
            clWARNING() << "failed to execute:" << command << endl;
            return;
        }

        wxString output;
        process->WaitForTerminate(output);
        wxFileName cargoToml(dlg.GetWorkspacePath(), "Cargo.toml");
        cargoToml.AppendDir(dlg.GetWorkspaceName());
        if (cargoToml.FileExists()) {
            // we successfully created a new cargo workspace, now, load it (using the standard file system
            // workspace)
            clFileSystemWorkspace::Get().New(cargoToml.GetPath(), cargoToml.GetDirs().Last());
        }
    }
}

wxString RustPlugin::GetRustTool(const wxString& toolname) const
{
    clRustLocator locator;
    if (!locator.Locate()) {
        return wxEmptyString;
    }
    return locator.GetRustTool(toolname);
}

void RustPlugin::OnBuildErrorLineClicked(clBuildEvent& event)
{
    if (!clFileSystemWorkspace::Get().IsOpen()) {
        event.Skip();
        return;
    }

    clDEBUG() << "Rust plugin: handling build line clicked" << endl;
    clDEBUG() << "File:" << event.GetFileName() << endl;
    clDEBUG() << "Line number:" << event.GetLineNumber() << endl;

    if (!FileExtManager::IsFileType(event.GetFileName(), FileExtManager::TypeRust)) {
        clDEBUG() << "Not a rust file, Skip()" << endl;
        event.Skip();
        return;
    }

    event.Skip(false);
    clDEBUG() << "Rust file clicked:" << event.GetFileName() << endl;

    // build the file path:
    // the compiler report the file in relative path to the `Cargo.toml` file
    wxString basepath;
    if (m_cargoTomlFile.FileExists()) {
        basepath = m_cargoTomlFile.GetPath();
        clDEBUG() << "Build root dir is set to Cargo.toml path:" << basepath << endl;
    } else {
        basepath = wxFileName(clFileSystemWorkspace::Get().GetFileName()).GetPath();
        clDEBUG() << "Build root dir is set to workspace path:" << basepath << endl;
    }

    bool is_abspath = event.GetFileName().StartsWith("/") || wxFileName(event.GetFileName()).IsAbsolute();
    if (!is_abspath && basepath.empty()) {
        // unable to determine the root folder
        event.Skip();
        return;
    }

    if (is_abspath) {
        basepath = event.GetFileName();
    } else {
        basepath << "/" << event.GetFileName();
    }

    wxFileName fnFile(basepath);
    if (!fnFile.FileExists()) {
        event.Skip(true); // let others handle this
        return;
    }

    int line_number = event.GetLineNumber();
    auto cb = [=](IEditor* editor) {
        editor->GetCtrl()->ClearSelections();
        // compilers report line numbers starting from `1`
        // our editor sees line numbers starting from `0`
        editor->CenterLine(line_number - 1, wxNOT_FOUND);
        editor->SetActive();
    };
    clGetManager()->OpenFileAndAsyncExecute(fnFile.GetFullPath(), std::move(cb));
}

void RustPlugin::AddRustcCompilerIfMissing()
{
    // Create new "rustc" compiler and place compiler patterns to use
    clDEBUG() << "Searching for rustc compiler..." << endl;
    if (BuildSettingsConfigST::Get()->IsCompilerExist("rustc")) {
        clDEBUG() << "Compiler rustc already exists" << endl;
        return;
    }

    CompilerLocatorRustc locator;
    locator.Locate(); // it never fails
    BuildSettingsConfigST::Get()->SetCompiler(*locator.GetCompilers().begin());
    BuildSettingsConfigST::Get()->Flush();
    clDEBUG() << "Successfully added new compiler 'rustc'" << endl;
}

void RustPlugin::OnBuildEnded(clBuildEvent& event)
{
    event.Skip();
    if (!clFileSystemWorkspace::Get().IsOpen()) {
        return;
    }

    wxFileName fnCargoToml(clFileSystemWorkspace::Get().GetFileName());
    fnCargoToml.SetFullName("Cargo.toml");
    wxString cargo_toml = fnCargoToml.GetFullPath();
    if (!wxFileName::FileExists(cargo_toml)) {
        return;
    }

    wxString new_digest = wxMD5::GetDigest(fnCargoToml);
    wxString old_digest;
    if (m_cargoTomlDigest.count(cargo_toml)) {
        old_digest = m_cargoTomlDigest[cargo_toml];
    }

    if (new_digest != old_digest) {
        // restart is required
        clLanguageServerEvent restart_event(wxEVT_LSP_RESTART_ALL);
        EventNotifier::Get()->ProcessEvent(restart_event);
    }
    m_cargoTomlDigest[cargo_toml] = new_digest;
}

void RustPlugin::OnWorkspaceLoaded(clWorkspaceEvent& event)
{
    event.Skip();
    if (!clFileSystemWorkspace::Get().IsOpen()) {
        return;
    }

    wxFileName workspaceFile = clFileSystemWorkspace::Get().GetFileName();
    wxFileName cargo_toml{ workspaceFile.GetPath(), "Cargo.toml" };

    if (cargo_toml.FileExists()) {
        m_cargoTomlFile = cargo_toml;
    } else {
        // try the parent folder
        cargo_toml.RemoveLastDir();
        if (cargo_toml.FileExists()) {
            m_cargoTomlFile = cargo_toml;
        } else {
            // We might placed our Cargo.toml in one of the children folers (1 level below)
            // check them
            wxString workspace_path = workspaceFile.GetPath();
            wxDir d;
            d.Open(workspace_path);

            wxString filename;
            for (bool cont = d.GetFirst(&filename, wxEmptyString, wxDIR_DIRS); cont; cont = d.GetNext(&filename)) {
                cargo_toml = wxFileName(workspace_path, "Cargo.toml");
                cargo_toml.AppendDir(filename);
                if (cargo_toml.FileExists()) {
                    m_cargoTomlFile = cargo_toml;
                    break;
                }
            }
        }
    }
    clDEBUG() << "Cargo.toml file found:" << m_cargoTomlFile << endl;
}

void RustPlugin::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    m_cargoTomlFile.Clear();
}
