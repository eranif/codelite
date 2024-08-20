
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CMakePlugin.cpp
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

/* ************************************************************************ */
/*                                                                          */
/* CMakePlugin for Codelite                                                 */
/* Copyright (C) 2013 Jiří Fatka <ntsfka@gmail.com>                         */
/*                                                                          */
/* This program is free software: you can redistribute it and/or modify     */
/* it under the terms of the GNU General Public License as published by     */
/* the Free Software Foundation, either version 3 of the License, or        */
/* (at your option) any later version.                                      */
/*                                                                          */
/* This program is distributed in the hope that it will be useful,          */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             */
/* GNU General Public License for more details.                             */
/*                                                                          */
/* You should have received a copy of the GNU General Public License        */
/* along with this program. If not, see <http://www.gnu.org/licenses/>.     */
/*                                                                          */
/* ************************************************************************ */

/* ************************************************************************ */
/* INCLUDES                                                                 */
/* ************************************************************************ */

// Declaration
#include "CMakePlugin.h"

#include "AsyncProcess/asyncprocess.h"
#include "AsyncProcess/processreaderthread.h"
#include "CMakeBuilder.h"
#include "ICompilerLocator.h"
#include "StdToWX.h"

// wxWidgets
#include <wx/app.h>
#include <wx/busyinfo.h>
#include <wx/dir.h>
#include <wx/event.h>
#include <wx/menu.h>
#include <wx/mimetype.h>
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>
#include <wx/xrc/xmlres.h>

// CodeLite
#include "async_executable_cmd.h"
#include "build_config.h"
#include "build_settings_config.h"
#include "build_system.h"
#include "detachedpanesinfo.h"
#include "dirsaver.h"
#include "dockablepane.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "macromanager.h"
#include "procutils.h"
#include "project.h"
#include "workspace.h"

// CMakePlugin
#include "CMake.h"
#include "CMakeGenerator.h"
#include "CMakeHelpTab.h"
#include "CMakeProjectSettings.h"
#include "CMakeSettingsDialog.h"
#include "CMakeSettingsManager.h"

#include <wx/textdlg.h>

/* ************************************************************************ */
/* VARIABLES                                                                */
/* ************************************************************************ */

/* ************************************************************************ */

const wxString CMakePlugin::CMAKELISTS_FILE = "CMakeLists.txt";

/* ************************************************************************ */

static const wxString HELP_TAB_NAME = _("CMake");

/* ************************************************************************ */
/* FUNCTIONS                                                                */
/* ************************************************************************ */

/**
 * @brief Creates plugin.
 *
 * @param manager Pointer to plugin manager.
 *
 * @return CMake plugin instance.
 */
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    return new CMakePlugin(manager);
}

/* ************************************************************************ */

/**
 * @brief Returns plugin's info.
 *
 * @return Plugin info.
 */
CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;

    info.SetAuthor(L"Jiří Fatka");
    info.SetName("CMakePlugin");
    info.SetDescription(_("CMake integration for CodeLite"));
    info.SetVersion("0.8");

    return &info;
}

/* ************************************************************************ */

/**
 * @brief Returns required Codelite interface version.
 *
 * @return Interface version.
 */
CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

CMakePlugin::CMakePlugin(IManager* manager)
    : IPlugin(manager)
    , m_configuration(NULL)
    , m_cmake(NULL)
{
    m_longName = _("CMake integration with CodeLite");
    m_shortName = "CMakePlugin";

    // Create CMake configuration file
    m_configuration.reset(new CMakeConfiguration(clStandardPaths::Get().GetUserDataDir() +
                                                 wxFileName::GetPathSeparator() + "config/cmake.ini"));

    // Create cmake application
    m_cmake.reset(new CMake(m_configuration->GetProgramPath()));

    m_helpTab = new CMakeHelpTab(clGetManager()->BookGet(PaneId::SIDE_BAR), this);
    clGetManager()->BookAddPage(PaneId::SIDE_BAR, m_helpTab, HELP_TAB_NAME, "cmake-button");
    m_mgr->AddWorkspaceTab(HELP_TAB_NAME);

    // Bind events
    EventNotifier::Get()->Bind(wxEVT_SHOW_WORKSPACE_TAB, &CMakePlugin::OnToggleHelpTab, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_PROJECT, &CMakePlugin::OnProjectContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_WORKSPACE, &CMakePlugin::OnWorkspaceContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_FOLDER, &CMakePlugin::OnFolderContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_PROJ_FILE_ADDED, &CMakePlugin::OnFileAdded, this);
    EventNotifier::Get()->Bind(wxEVT_PROJ_FILE_REMOVED, &CMakePlugin::OnFileRemoved, this);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &CMakePlugin::OnCMakeOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &CMakePlugin::OnCMakeTerminated, this);
}

/* ************************************************************************ */

CMakePlugin::~CMakePlugin()
{
    // Nothing to do
}

/* ************************************************************************ */

wxFileName CMakePlugin::GetWorkspaceDirectory() const
{
    const clCxxWorkspace* workspace = m_mgr->GetWorkspace();
    wxASSERT(workspace);

    return wxFileName::DirName(workspace->GetWorkspaceFileName().GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
}

/* ************************************************************************ */

wxFileName CMakePlugin::GetProjectDirectory(const wxString& projectName) const
{
    const clCxxWorkspace* workspace = m_mgr->GetWorkspace();
    wxASSERT(workspace);

    wxString errMsg;
    const ProjectPtr proj = workspace->FindProjectByName(projectName, errMsg);
    wxASSERT(proj);

    return wxFileName::DirName(proj->GetFileName().GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
}

/* ************************************************************************ */

wxString CMakePlugin::GetSelectedProjectConfig() const
{
    BuildConfigPtr configPtr = GetSelectedBuildConfig();

    if (configPtr)
        return configPtr->GetName();

    return wxEmptyString;
}

/* ************************************************************************ */

BuildConfigPtr CMakePlugin::GetSelectedBuildConfig() const
{
    const clCxxWorkspace* workspace = m_mgr->GetWorkspace();
    wxASSERT(workspace);

    const ProjectPtr projectPtr = GetSelectedProject();
    wxASSERT(projectPtr);

    return workspace->GetProjBuildConf(projectPtr->GetName(), wxEmptyString);
}

/* ************************************************************************ */

wxArrayString CMakePlugin::GetSupportedGenerators() const
{
#ifdef __WXMSW__
    // Windows supported generators
    return StdToWX::ToArrayString({ "MinGW Makefiles" });
#else
    // Linux / Mac supported generators
    return StdToWX::ToArrayString({
        "Unix Makefiles",
        // "Ninja",
    });
#endif
}

/* ************************************************************************ */

void CMakePlugin::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }

/* ************************************************************************ */

void CMakePlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    menu->Append(new wxMenuItem(menu, XRCID("cmake_settings"), _("Settings...")));

    pluginsMenu->Append(wxID_ANY, "CMake", menu);

    wxTheApp->Bind(wxEVT_COMMAND_MENU_SELECTED, &CMakePlugin::OnSettings, this, XRCID("cmake_settings"));
}

/* ************************************************************************ */

void CMakePlugin::UnPlug()
{
    auto page = clGetManager()->BookGetPage(PaneId::SIDE_BAR, HELP_TAB_NAME);
    if (page) {
        CMakeHelpTab* helpTab = dynamic_cast<CMakeHelpTab*>(page);
        if (helpTab) {
            helpTab->Stop();
        }
        if (!clGetManager()->BookDeletePage(PaneId::SIDE_BAR, page)) {
            // failed to delete, delete it manually
            page->Destroy();
        }
    }

    // Unbind events
    wxTheApp->Unbind(wxEVT_COMMAND_MENU_SELECTED, &CMakePlugin::OnSettings, this, XRCID("cmake_settings"));

    EventNotifier::Get()->Unbind(wxEVT_SHOW_WORKSPACE_TAB, &CMakePlugin::OnToggleHelpTab, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_PROJECT, &CMakePlugin::OnProjectContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_WORKSPACE, &CMakePlugin::OnWorkspaceContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_PROJ_FILE_ADDED, &CMakePlugin::OnFileAdded, this);
    EventNotifier::Get()->Unbind(wxEVT_PROJ_FILE_REMOVED, &CMakePlugin::OnFileRemoved, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_FOLDER, &CMakePlugin::OnFolderContextMenu, this);

    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &CMakePlugin::OnCMakeOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &CMakePlugin::OnCMakeTerminated, this);
}

/* ************************************************************************ */

bool CMakePlugin::ExistsCMakeLists(wxFileName directory) const
{
    // Add CMakeLists.txt
    directory.SetFullName(CMAKELISTS_FILE);

    return directory.Exists();
}

/* ************************************************************************ */

void CMakePlugin::OpenCMakeLists(wxFileName filename) const
{
    filename.SetFullName(CMAKELISTS_FILE);

    if (!m_mgr->OpenFile(filename.GetFullPath()))
        wxMessageBox("Unable to open \"" + filename.GetFullPath() + "\"", wxMessageBoxCaptionStr,
                     wxOK | wxCENTER | wxICON_ERROR);
}

/* ************************************************************************ */

void CMakePlugin::OnSettings(wxCommandEvent& event)
{
    CMakeSettingsDialog dlg(NULL, this);

    // Set original value
    dlg.SetCMakePath(m_configuration->GetProgramPath());
    dlg.SetDefaultGenerator(m_configuration->GetDefaultGenerator());

    // Store change
    if (dlg.ShowModal() == wxID_OK) {
        m_configuration->SetProgramPath(dlg.GetCMakePath());
        m_configuration->SetDefaultGenerator(dlg.GetDefaultGenerator());
        m_cmake->SetPath(dlg.GetCMakePath());
    }
}

void CMakePlugin::OnToggleHelpTab(clCommandEvent& event) { wxUnusedVar(event); }

void CMakePlugin::OnProjectContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    CHECK_COND_RET(clCxxWorkspaceST::Get()->IsOpen());

    ProjectPtr p = GetSelectedProject();
    CHECK_COND_RET(p);

    BuildConfigPtr buildConf = p->GetBuildConfiguration();
    CHECK_COND_RET(buildConf);

    CHECK_COND_RET(buildConf->GetBuilder()->GetName() == "CMake");

    // The selected project is using CMake builder
    // Add our context menu
    wxMenu* menu = event.GetMenu()->GetParent(); // We want to attach this action to the main menu, not the subclass
    CHECK_PTR_RET(menu);

    const wxMenuItemList& items = menu->GetMenuItems();

    size_t buildPos = 0;
    size_t settingsPos = 0;
    size_t curpos = 0;
    wxMenuItemList::const_iterator iter = items.begin();
    for (; iter != items.end(); ++iter) {
        if ((*iter)->GetId() == XRCID("build_project")) {
            buildPos = curpos;
        }
        if ((*iter)->GetId() == XRCID("project_properties")) {
            settingsPos = curpos;
        }
        ++curpos;
    }

    wxFileName projectFile = p->GetFileName();
    projectFile.SetFullName(CMAKELISTS_FILE);
    if (projectFile.FileExists()) {
        wxMenuItem* item = new wxMenuItem(NULL, XRCID("cmake_open_cmake"), _("Open CMakeLists.txt"));
        item->SetBitmap(m_mgr->GetStdIcons()->LoadBitmap("cmake"));
        menu->Insert(settingsPos, item);
    }

    menu->Insert(buildPos, XRCID("cmake_run_cmake"), _("Run CMake"));
    menu->InsertSeparator(buildPos);
    menu->Insert(buildPos, XRCID("cmake_export_cmakelists"), _("Export CMakeLists.txt"));
    menu->Bind(wxEVT_MENU, &CMakePlugin::OnRunCMake, this, XRCID("cmake_run_cmake"));
    menu->Bind(wxEVT_MENU, &CMakePlugin::OnOpenCMakeLists, this, XRCID("cmake_open_cmake"));
    menu->Bind(wxEVT_MENU, &CMakePlugin::OnExportCMakeLists, this, XRCID("cmake_export_cmakelists"));
}

void CMakePlugin::OnRunCMake(wxCommandEvent& event)
{
    wxUnusedVar(event);

    // first, some sanity
    ProjectPtr p = GetSelectedProject();
    DoRunCMake(p);
}

void CMakePlugin::OnCMakeOutput(clProcessEvent& event)
{
    m_mgr->AppendOutputTabText(kOutputTab_Build, event.GetOutput());
}

void CMakePlugin::OnCMakeTerminated(clProcessEvent& event)
{
    m_mgr->AppendOutputTabText(kOutputTab_Build, event.GetOutput());
    IProcess* process = event.GetProcess();
    wxDELETE(process);
    event.SetProcess(NULL);
    m_mgr->AppendOutputTabText(kOutputTab_Build, "==== Done ====\n");
}

void CMakePlugin::OnOpenCMakeLists(wxCommandEvent& event)
{
    bool openWorkspaceCMakeLists = (event.GetId() == XRCID("cmake_open_active_project_cmake"));
    wxFileName cmakelists;
    if (openWorkspaceCMakeLists) {
        cmakelists = clCxxWorkspaceST::Get()->GetFileName();
    } else {
        ProjectPtr proj = GetSelectedProject();
        CHECK_PTR_RET(proj);
        cmakelists = proj->GetFileName();
    }

    cmakelists.SetFullName(CMAKELISTS_FILE);
    if (cmakelists.FileExists()) {
        m_mgr->OpenFile(cmakelists.GetFullPath());
    }
}

void CMakePlugin::OnExportCMakeLists(wxCommandEvent& event)
{
    ProjectPtr proj = (event.GetId() == XRCID("cmake_export_active_project"))
                          ? clCxxWorkspaceST::Get()->GetActiveProject()
                          : GetSelectedProject();

    CHECK_PTR_RET(proj);

    CMakeGenerator generator;
    if (generator.Generate(proj)) {
        EventNotifier::Get()->PostReloadExternallyModifiedEvent();
    }
}

void CMakePlugin::OnFolderContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    // Add context menu with following items:
    // - Create Executable CMakeLists.txt file
    // - Create Library (static) CMakeLists.txt file
    // - Create Library (shared) CMakeLists.txt file
    wxMenu* menu = event.GetMenu();
    wxMenu* cmake_menu = new wxMenu;
    cmake_menu->Append(XRCID("cmake_new_cmake_exe"), "Executable", wxEmptyString);
    cmake_menu->Append(XRCID("cmake_new_cmake_dll"), "Shared object", wxEmptyString);
    cmake_menu->Append(XRCID("cmake_new_cmake_lib"), "Static library", wxEmptyString);
    menu->AppendSeparator();
    menu->AppendSubMenu(cmake_menu, "Generate CMakeLists.txt for...");
    cmake_menu->Bind(wxEVT_MENU, &CMakePlugin::OnCreateCMakeListsExe, this, XRCID("cmake_new_cmake_exe"));
    cmake_menu->Bind(wxEVT_MENU, &CMakePlugin::OnCreateCMakeListsDll, this, XRCID("cmake_new_cmake_dll"));
    cmake_menu->Bind(wxEVT_MENU, &CMakePlugin::OnCreateCMakeListsLib, this, XRCID("cmake_new_cmake_lib"));
}

void CMakePlugin::OnWorkspaceContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    CHECK_COND_RET(clCxxWorkspaceST::Get()->IsOpen());

    ProjectPtr p = clCxxWorkspaceST::Get()->GetActiveProject();
    CHECK_COND_RET(p);

    BuildConfigPtr buildConf = p->GetBuildConfiguration();
    CHECK_COND_RET(buildConf);

    CHECK_COND_RET(buildConf->GetBuilder()->GetName() == "CMake");

    // The active project is using CMake builder
    // Add our context menu
    wxMenu* menu = event.GetMenu();
    CHECK_PTR_RET(menu);

    wxFileName workspaceFile = clCxxWorkspaceST::Get()->GetFileName();
    workspaceFile.SetFullName(CMAKELISTS_FILE);

    menu->AppendSeparator();
    if (workspaceFile.FileExists()) {
        wxMenuItem* item = new wxMenuItem(NULL, XRCID("cmake_open_active_project_cmake"), _("Open CMakeLists.txt"));
        item->SetBitmap(m_mgr->GetStdIcons()->LoadBitmap("cmake"));
        menu->Append(item);
    }
    menu->Append(XRCID("cmake_export_active_project"), _("Export CMakeLists.txt"));
    menu->Bind(wxEVT_MENU, &CMakePlugin::OnOpenCMakeLists, this, XRCID("cmake_open_active_project_cmake"));
    menu->Bind(wxEVT_MENU, &CMakePlugin::OnExportCMakeLists, this, XRCID("cmake_export_active_project"));
}

void CMakePlugin::OnFileRemoved(clCommandEvent& event)
{
    event.Skip();
    CHECK_COND_RET(clCxxWorkspaceST::Get()->IsOpen());

    // The affected project is passed in the string member of the event
    ProjectPtr p = clCxxWorkspaceST::Get()->GetProject(event.GetString());
    CHECK_PTR_RET(p);

    BuildConfigPtr buildConf = p->GetBuildConfiguration();
    CHECK_COND_RET(buildConf);

    // Ensure we are a CMake project
    CHECK_COND_RET(buildConf->GetBuilder()->GetName() == "CMake");

    DoRunCMake(p);
}

void CMakePlugin::OnFileAdded(clCommandEvent& event) { OnFileRemoved(event); }

void CMakePlugin::DoRunCMake(ProjectPtr p)
{
    CHECK_PTR_RET(p);

    BuildConfigPtr buildConf = p->GetBuildConfiguration();
    CHECK_COND_RET(buildConf);

// Apply the environment variables before we do anything here
#ifdef __WXMSW__
    // On Windows, we need to set the bin folder of the selected compiler
    wxFileName fnCxx(buildConf->GetCompiler()->GetTool("CXX"));
    wxStringMap_t om;
    wxString pathvar;
    pathvar << fnCxx.GetPath() << clPATH_SEPARATOR << "$PATH";
    om["PATH"] = pathvar;
    EnvSetter es(NULL, &om, p->GetName(), buildConf->GetName());
#else
    EnvSetter es(p);
#endif

    CMakeGenerator generator;
    if (generator.CanGenerate(p)) {
        generator.Generate(p);
    }

    wxString args = buildConf->GetBuildSystemArguments();

    // Expand CodeLite macros
    args = MacroManager::Instance()->Expand(args, m_mgr, p->GetName(), buildConf->GetName());
    wxString cmakeExe = GetCMake()->GetPath().GetFullPath();

    // Did the user provide a generator to use?
    bool hasGeneratorInArgs = (args.Find("-G") != wxNOT_FOUND);
    wxUnusedVar(hasGeneratorInArgs);

    // Build the working directory
    wxFileName fnWorkingDirectory(CMakeBuilder::GetProjectBuildFolder(p->GetName(), false), "");

    // Ensure that the build directory exists
    fnWorkingDirectory.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    ::WrapWithQuotes(cmakeExe);

    // We run the cmake
    wxString command;
    wxString projectFolder = p->GetFileName().GetPath();
    ::WrapWithQuotes(projectFolder);

    command << cmakeExe << " " << projectFolder << " " << args;

#ifdef __WXMSW__
    if (!hasGeneratorInArgs) {
        bool is_msys =
            buildConf->GetCompiler() && buildConf->GetCompiler()->GetCompilerFamily() == COMPILER_FAMILY_MSYS2;
        // On Windows, generate MinGW makefiles
        if (is_msys) {
            command << " -G\"MSYS Makefiles\"";
        } else {
            command << " -G\"MinGW Makefiles\"";
        }
    }
#endif

    // Execute it
    IProcess* proc =
        ::CreateAsyncProcess(this, command, IProcessCreateDefault | IProcessWrapInShell, fnWorkingDirectory.GetPath());
    if (!proc) {
        ::wxMessageBox(_("Failed to execute:\n") + command, "CodeLite", wxICON_ERROR | wxOK | wxCENTER,
                       EventNotifier::Get()->TopFrame());
        return;
    }
    m_mgr->ShowOutputPane(_("Build"));
    m_mgr->ClearOutputTab(kOutputTab_Build);
    m_mgr->AppendOutputTabText(kOutputTab_Build, command + "\n");
}

/* ************************************************************************ */

bool CMakePlugin::IsCMakeListsExists() const
{
    wxFileName cmakelists_txt{ ::wxGetCwd(), "CMakeLists.txt" };
    if (cmakelists_txt.FileExists()) {
        ::wxMessageBox(_("This folder already contains a CMakeLists.txt file"), "CodeLite",
                       wxICON_WARNING | wxOK | wxCENTER);
        return true;
    }
    return false;
}

wxString CMakePlugin::WriteCMakeListsAndOpenIt(const std::vector<wxString>& lines) const
{
    wxFileName cmakelists_txt{ ::wxGetCwd(), "CMakeLists.txt" };
    const wxArrayString wx_lines = StdToWX::ToArrayString(lines);
    FileUtils::WriteFileContent(cmakelists_txt, wxJoin(wx_lines, '\n'));
    clGetManager()->OpenFile(cmakelists_txt.GetFullPath());
    return cmakelists_txt.GetFullPath();
}

clResultString CMakePlugin::CreateCMakeListsFile(CMakePlugin::TargetType type) const
{
    // Check for an already existing CMakeLists.txt in this folder
    if (IsCMakeListsExists()) {
        return clResultString::make_error(wxEmptyString);
    }

    wxString name;
    wxString target_line;
    switch (type) {
    case TargetType::EXECUTABLE:
        name = ::wxGetTextFromUser(_("Executable name:"), "Executable name");
        target_line = wxString::Format("add_executable(%s ${CXX_SRCS} ${C_SRCS})", name);
        break;
    case TargetType::SHARED_LIB:
        name = ::wxGetTextFromUser(_("Library name:"), "Library name");
        target_line = wxString::Format("add_library(%s SHARED ${CXX_SRCS} ${C_SRCS})", name);
        break;
    case TargetType::STATIC_LIB:
        name = ::wxGetTextFromUser(_("Library name:"), "Library name");
        target_line = wxString::Format("add_library(%s STATIC ${CXX_SRCS} ${C_SRCS})", name);
        break;
    }

    if (name.empty()) {
        return clResultString::make_error(std::move(wxString("User cancelled")));
    }

    wxString cmakelists_txt = WriteCMakeListsAndOpenIt({
        "cmake_minimum_required(VERSION 3.16)",
        wxString::Format("project(%s)", name),
        wxEmptyString,
        wxEmptyString,
        "set(CMAKE_EXPORT_COMPILE_COMMANDS 1)",
        "set(CMAKE_CXX_STANDARD 17)",
        "set(CMAKE_CXX_STANDARD_REQUIRED ON)",
        wxEmptyString,
        "file(GLOB CXX_SRCS \"*.cpp\")",
        "file(GLOB C_SRCS \"*.c\")",
        wxEmptyString,
        target_line,
        "if(NOT ${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})",
        "    add_custom_command(",
        wxString::Format("        TARGET %s", name),
        "        POST_BUILD",
        "        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/compile_commands.json",
        "                ${CMAKE_SOURCE_DIR}/compile_commands.json)",
        "endif()",
        wxEmptyString,
    });
    return cmakelists_txt;
}

void CMakePlugin::OnCreateCMakeListsExe(wxCommandEvent& event)
{
    wxUnusedVar(event);
    auto res = CreateCMakeListsFile(CMakePlugin::TargetType::EXECUTABLE);
    CHECK_COND_RET(res);
    FireCMakeListsFileCreatedEvent(res.success());
}

void CMakePlugin::OnCreateCMakeListsDll(wxCommandEvent& event)
{
    wxUnusedVar(event);
    auto res = CreateCMakeListsFile(CMakePlugin::TargetType::SHARED_LIB);
    CHECK_COND_RET(res);
    FireCMakeListsFileCreatedEvent(res.success());
}

void CMakePlugin::OnCreateCMakeListsLib(wxCommandEvent& event)
{
    wxUnusedVar(event);
    auto res = CreateCMakeListsFile(CMakePlugin::TargetType::STATIC_LIB);
    CHECK_COND_RET(res);
    FireCMakeListsFileCreatedEvent(res.success());
}

void CMakePlugin::FireCMakeListsFileCreatedEvent(const wxString& cmakelists_txt) const
{
    // fire an event
    clFileSystemEvent fsEvent(wxEVT_FILE_CREATED);
    fsEvent.SetPath(cmakelists_txt);
    fsEvent.SetFileName(cmakelists_txt);
    fsEvent.GetPaths().Add(cmakelists_txt);
    EventNotifier::Get()->AddPendingEvent(fsEvent);
}
