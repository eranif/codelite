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

#include "CargoToml.hpp"
#include "NewFileSystemWorkspaceDialog.h"
#include "RustPlugin.hpp"
#include "RustWorkspace.hpp"
#include "asyncprocess.h"
#include "build_settings_config.h"
#include "clFileSystemWorkspace.hpp"
#include "clFileSystemWorkspaceConfig.hpp"
#include "clRustLocator.hpp"
#include "clWorkspaceManager.h"
#include "cl_standard_paths.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "macros.h"
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>
#include <wx/utils.h>

static RustPlugin* thePlugin = NULL;
// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new RustPlugin(manager);
    }
    return thePlugin;
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
    clWorkspaceManager::Get().RegisterWorkspace(new RustWorkspace());
}

RustPlugin::~RustPlugin() {}

void RustPlugin::CreateToolBar(clToolBar* toolbar) { wxUnusedVar(toolbar); }

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
}

void RustPlugin::OnFolderContextMenu(clContextMenuEvent& event) { event.Skip(); }

void RustPlugin::OnRustWorkspaceFileCreated(clFileSystemEvent& event)
{
    event.Skip();
    wxFileName workspaceFile(event.GetPath());
    wxFileName fnCargoToml(workspaceFile);
    fnCargoToml.SetFullName("Cargo.toml");
    if(fnCargoToml.FileExists()) {
        CargoToml cargoToml;
        const wxString& name = cargoToml.Load(fnCargoToml).GetName();

        // update the build commands
        clFileSystemWorkspaceSettings settings;
        if(!settings.Load(workspaceFile)) {
            clWARNING() << "Failed to load file:" << workspaceFile << endl;
            return;
        }
        auto debug = settings.GetConfig("Debug");
        if(debug) {
            debug->SetBuildTargets({ { "build", "cargo build" }, { "clean", "cargo clean" } });
            debug->SetExecutable("./target/debug/" + name);
            debug->SetFileExtensions(debug->GetFileExtensions() + ";*.rs;*.toml");

            // set the environment variable to point to rust-gdb
            wxString env_str;
#if !defined(__WXMSW__)
            // use rust-gdb
            wxFileName rust_gdb = GetRustTool("rust-gdb");
            if(rust_gdb.IsOk()) {
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
            if(gdb_exe.FileExists()) {
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
            if(!env_str.empty()) {
                debug->SetEnvironment(env_str);
            }
        }
        settings.Save(workspaceFile);
    }
}

void RustPlugin::OnNewWorkspace(clCommandEvent& e)
{
    e.Skip();
    if(e.GetString() == "Rust") {
        e.Skip(false);
        // Prompt the user for the folder to run 'cargo new'
        NewFileSystemWorkspaceDialog dlg(EventNotifier::Get()->TopFrame(), false /* do not auto update the name */);
        dlg.SetLabel("Select the folder to run 'cargo new'");
        if(dlg.ShowModal() != wxID_OK) {
            return;
        }

        EnvSetter env;
        wxFileName cargo;
        if(!::clFindExecutable("cargo", cargo)) {
            wxMessageBox(_("Could not locate cargo in your PATH"), "CodeLite", wxICON_ERROR | wxCENTRE);
            return;
        }

        wxString command;
        command << "cargo new " << dlg.GetWorkspaceName();
        IProcess::Ptr_t process(::CreateSyncProcess(command, IProcessCreateDefault | IProcessCreateWithHiddenConsole,
                                                    dlg.GetWorkspacePath()));
        if(!process) {
            clWARNING() << "failed to execute:" << command << endl;
            return;
        }

        wxString output;
        process->WaitForTerminate(output);
        wxFileName cargoToml(dlg.GetWorkspacePath(), "Cargo.toml");
        cargoToml.AppendDir(dlg.GetWorkspaceName());
        if(cargoToml.FileExists()) {
            // we successfully created a new cargo workspace, now, load it (using the standard file system workspace)
            clFileSystemWorkspace::Get().New(cargoToml.GetPath(), cargoToml.GetDirs().Last());
        }
    }
}

wxString RustPlugin::GetRustTool(const wxString& toolname) const
{
    clRustLocator locator;
    if(!locator.Locate()) {
        return wxEmptyString;
    }
    return locator.GetRustTool(toolname);
}
