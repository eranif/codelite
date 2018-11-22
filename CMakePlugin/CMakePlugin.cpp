
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
#include "CMakeBuilder.h"
#include "CMakePlugin.h"
#include "asyncprocess.h"
#include "processreaderthread.h"

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

/* ************************************************************************ */
/* VARIABLES                                                                */
/* ************************************************************************ */

static CMakePlugin* g_plugin = NULL;

/* ************************************************************************ */

const wxString CMakePlugin::CMAKELISTS_FILE = "CMakeLists.txt";

/* ************************************************************************ */

static const wxString HELP_TAB_NAME = _("CMake Help");

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
    if(!g_plugin) { g_plugin = new CMakePlugin(manager); }

    return g_plugin;
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

    Notebook* book = m_mgr->GetWorkspacePaneNotebook();
    cmakeImages images;
    const wxBitmap& bmp = images.Bitmap("cmake_16");

    if(IsPaneDetached()) {
        DockablePane* cp =
            new DockablePane(book->GetParent()->GetParent(), book, HELP_TAB_NAME, false, bmp, wxSize(200, 200));
        m_helpTab = new CMakeHelpTab(cp, this);
        cp->SetChildNoReparent(m_helpTab);
    } else {
        m_helpTab = new CMakeHelpTab(book, this);
        book->AddPage(m_helpTab, HELP_TAB_NAME, false, bmp);
        m_mgr->AddWorkspaceTab(HELP_TAB_NAME);
    }

    // Bind events
    EventNotifier::Get()->Bind(wxEVT_SHOW_WORKSPACE_TAB, &CMakePlugin::OnToggleHelpTab, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_PROJECT, &CMakePlugin::OnProjectContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_WORKSPACE, &CMakePlugin::OnWorkspaceContextMenu, this);
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

    if(configPtr) return configPtr->GetName();

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
    wxArrayString generators;

#ifdef __WXMSW__
    // Windows supported generators
    generators.Add("MinGW Makefiles");
#else
    // Linux / Mac supported generators
    generators.Add("Unix Makefiles");
// generators.Add("Ninja");
#endif

    return generators;
}

/* ************************************************************************ */

bool CMakePlugin::IsPaneDetached() const
{
    wxASSERT(m_mgr);
    IConfigTool* configTool = m_mgr->GetConfigTool();
    wxASSERT(configTool);

    DetachedPanesInfo dpi;
    configTool->ReadObject("DetachedPanesList", &dpi);
    const wxArrayString& detachedPanes = dpi.GetPanes();
    return detachedPanes.Index(HELP_TAB_NAME) != wxNOT_FOUND;
}

/* ************************************************************************ */

void CMakePlugin::CreateToolBar(clToolBar* toolbar) { wxUnusedVar(toolbar); }

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
    wxASSERT(m_mgr);
    Notebook* notebook = m_mgr->GetWorkspacePaneNotebook();
    wxASSERT(notebook);

    int pos = notebook->GetPageIndex("CMake Help");
    if(pos != wxNOT_FOUND) {
        CMakeHelpTab* helpTab = dynamic_cast<CMakeHelpTab*>(notebook->GetPage(pos));
        if(helpTab) { helpTab->Stop(); }
        notebook->RemovePage(pos);
    }

    // Unbind events
    wxTheApp->Unbind(wxEVT_COMMAND_MENU_SELECTED, &CMakePlugin::OnSettings, this, XRCID("cmake_settings"));

    EventNotifier::Get()->Unbind(wxEVT_SHOW_WORKSPACE_TAB, &CMakePlugin::OnToggleHelpTab, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_PROJECT, &CMakePlugin::OnProjectContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_WORKSPACE, &CMakePlugin::OnWorkspaceContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_PROJ_FILE_ADDED, &CMakePlugin::OnFileAdded, this);
    EventNotifier::Get()->Unbind(wxEVT_PROJ_FILE_REMOVED, &CMakePlugin::OnFileRemoved, this);
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

    if(!m_mgr->OpenFile(filename.GetFullPath()))
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
    if(dlg.ShowModal() == wxID_OK) {
        m_configuration->SetProgramPath(dlg.GetCMakePath());
        m_configuration->SetDefaultGenerator(dlg.GetDefaultGenerator());
        m_cmake->SetPath(dlg.GetCMakePath());
    }
}

void CMakePlugin::OnToggleHelpTab(clCommandEvent& event)
{
    if(event.GetString() != HELP_TAB_NAME) {
        event.Skip();
        return;
    }

    if(event.IsSelected()) {
        // show it
        cmakeImages images;
        const wxBitmap& bmp = images.Bitmap("cmake_16");
        m_mgr->GetWorkspacePaneNotebook()->AddPage(m_helpTab, HELP_TAB_NAME, true, bmp);
    } else {
        int where = m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(HELP_TAB_NAME);
        if(where != wxNOT_FOUND) { m_mgr->GetWorkspacePaneNotebook()->RemovePage(where); }
    }
}

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
    for(; iter != items.end(); ++iter) {
        if((*iter)->GetId() == XRCID("build_project")) { buildPos = curpos; }
        if((*iter)->GetId() == XRCID("project_properties")) { settingsPos = curpos; }
        ++curpos;
    }

    wxFileName projectFile = p->GetFileName();
    projectFile.SetFullName(CMAKELISTS_FILE);
    if(projectFile.FileExists()) {
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
    if(openWorkspaceCMakeLists) {
        cmakelists = clCxxWorkspaceST::Get()->GetFileName();
    } else {
        ProjectPtr proj = GetSelectedProject();
        CHECK_PTR_RET(proj);
        cmakelists = proj->GetFileName();
    }

    cmakelists.SetFullName(CMAKELISTS_FILE);
    if(cmakelists.FileExists()) { m_mgr->OpenFile(cmakelists.GetFullPath()); }
}

void CMakePlugin::OnExportCMakeLists(wxCommandEvent& event)
{
    ProjectPtr proj = (event.GetId() == XRCID("cmake_export_active_project"))
                          ? clCxxWorkspaceST::Get()->GetActiveProject()
                          : GetSelectedProject();

    CHECK_PTR_RET(proj);

    CMakeGenerator generator;
    if(generator.Generate(proj)) { EventNotifier::Get()->PostReloadExternallyModifiedEvent(); }
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
    if(workspaceFile.FileExists()) {
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
    if(generator.CanGenerate(p)) { generator.Generate(p); }

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
    if(!hasGeneratorInArgs) {
        // On Windows, generate MinGW makefiles
        command << " -G\"MinGW Makefiles\"";
    }
#endif

    // Execute it
    IProcess* proc = ::CreateAsyncProcess(this, command, IProcessCreateDefault, fnWorkingDirectory.GetPath());
    if(!proc) {
        ::wxMessageBox(_("Failed to execute:\n") + command, "CodeLite", wxICON_ERROR | wxOK | wxCENTER,
                       EventNotifier::Get()->TopFrame());
        return;
    }
    m_mgr->ShowOutputPane(_("Build"));
    m_mgr->ClearOutputTab(kOutputTab_Build);
    m_mgr->AppendOutputTabText(kOutputTab_Build, command + "\n");
}

/* ************************************************************************ */
