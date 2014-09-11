//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
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

// wxWidgets
#include <wx/app.h>
#include <wx/stdpaths.h>
#include <wx/msgdlg.h>
#include <wx/xrc/xmlres.h>
#include <wx/mimetype.h>
#include <wx/menu.h>
#include <wx/dir.h>
#include <wx/event.h>
#include <wx/busyinfo.h>

// CodeLite
#include "environmentconfig.h"
#include "event_notifier.h"
#include "globals.h"
#include "dirsaver.h"
#include "procutils.h"
#include "project.h"
#include "workspace.h"
#include "build_settings_config.h"
#include "build_config.h"
#include "build_system.h"
#include "file_logger.h"
#include "macromanager.h"
#include "async_executable_cmd.h"
#include "dockablepane.h"
#include "detachedpanesinfo.h"

// CMakePlugin
#include "CMake.h"
#include "CMakeGenerator.h"
#include "CMakeWorkspaceMenu.h"
#include "CMakeProjectMenu.h"
#include "CMakeSettingsDialog.h"
#include "CMakeSettingsManager.h"
#include "CMakeProjectSettings.h"
#include "CMakeProjectSettingsPanel.h"
#include "CMakeGenerator.h"
#include "CMakeHelpTab.h"

/* ************************************************************************ */
/* VARIABLES                                                                */
/* ************************************************************************ */

static CMakePlugin* g_plugin = NULL;

/* ************************************************************************ */

const wxString CMakePlugin::CMAKELISTS_FILE = "CMakeLists.txt";

/* ************************************************************************ */

static const wxString HELP_TAB_NAME = "CMake Help";

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
extern "C" EXPORT IPlugin* CreatePlugin(IManager* manager)
{
    if(!g_plugin) {
        g_plugin = new CMakePlugin(manager);
    }

    return g_plugin;
}

/* ************************************************************************ */

/**
 * @brief Returns plugin's info.
 *
 * @return Plugin info.
 */
extern "C" EXPORT PluginInfo GetPluginInfo()
{
    PluginInfo info;

    info.SetAuthor(L"Jiří Fatka");
    info.SetName("CMakePlugin");
    info.SetDescription(_("CMake integration for CodeLite"));
    info.SetVersion("0.8");

    return info;
}

/* ************************************************************************ */

/**
 * @brief Returns required Codelite interface version.
 *
 * @return Interface version.
 */
extern "C" EXPORT int GetPluginInterfaceVersion()
{
    return PLUGIN_INTERFACE_VERSION;
}

/* ************************************************************************ */

/**
 * @brief Join arguments for project settings.
 *
 * @param settings      Project settings.
 * @param configuration CMakePlugin global configuration.
 *
 * @return Command line arguments.
 */
static wxString CreateArguments(const CMakeProjectSettings& settings, const CMakeConfiguration& configuration)
{
    wxArrayString args;

    // Get generator
    wxString generator = settings.generator;

    // Use global value
    if(generator.IsEmpty()) generator = configuration.GetDefaultGenerator();

    // Generator
    if(!generator.IsEmpty()) args.Add("-G \"" + generator + "\"");

    if(!settings.buildType.IsEmpty()) args.Add("-DCMAKE_BUILD_TYPE=" + settings.buildType);

    // Copy additional arguments
    for(wxArrayString::const_iterator it = settings.arguments.begin(), ite = settings.arguments.end(); it != ite;
        ++it) {
        args.Add(*it);
    }

    return wxJoin(args, ' ', '\0');
}

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

CMakePlugin::CMakePlugin(IManager* manager)
    : IPlugin(manager)
    , m_configuration(NULL)
    , m_cmake(NULL)
    , m_settingsManager(new CMakeSettingsManager(this))
    , m_panel(NULL)
{
    m_longName = _("CMake integration with CodeLite");
    m_shortName = "CMakePlugin";

    // Create CMake configuration file
    m_configuration.reset(new CMakeConfiguration(wxStandardPaths::Get().GetUserDataDir() +
                                                 wxFileName::GetPathSeparator() + "config/cmake.ini"));

    // Create cmake application
    m_cmake.reset(new CMake(m_configuration->GetProgramPath()));

    Notebook* book = m_mgr->GetWorkspacePaneNotebook();
    cmakeImages images;
    const wxBitmap& bmp = images.Bitmap("cmake_16");
    if(IsPaneDetached()) {
        DockablePane* cp = new DockablePane(book->GetParent()->GetParent(), book, HELP_TAB_NAME, bmp, wxSize(200, 200));
        cp->SetChildNoReparent(new CMakeHelpTab(cp, this));

    } else {
        book->AddPage(new CMakeHelpTab(book, this), HELP_TAB_NAME, false, bmp);
    }

    // Bind events
    EventNotifier::Get()->Bind(
        wxEVT_CMD_PROJ_SETTINGS_SAVED, clProjectSettingsEventHandler(CMakePlugin::OnSaveConfig), this);
    EventNotifier::Get()->Bind(wxEVT_GET_PROJECT_BUILD_CMD, clBuildEventHandler(CMakePlugin::OnGetBuildCommand), this);
    EventNotifier::Get()->Bind(wxEVT_GET_PROJECT_CLEAN_CMD, clBuildEventHandler(CMakePlugin::OnGetCleanCommand), this);
    EventNotifier::Get()->Bind(
        wxEVT_GET_IS_PLUGIN_MAKEFILE, clBuildEventHandler(CMakePlugin::OnGetIsPluginMakefile), this);
    EventNotifier::Get()->Bind(wxEVT_PLUGIN_EXPORT_MAKEFILE, clBuildEventHandler(CMakePlugin::OnExportMakefile), this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(CMakePlugin::OnWorkspaceLoaded), this);
}

/* ************************************************************************ */

CMakePlugin::~CMakePlugin()
{
    // Nothing to do
}

/* ************************************************************************ */

wxFileName CMakePlugin::GetWorkspaceDirectory() const
{
    const Workspace* workspace = m_mgr->GetWorkspace();
    wxASSERT(workspace);

    return wxFileName::DirName(workspace->GetWorkspaceFileName().GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME));
}

/* ************************************************************************ */

wxFileName CMakePlugin::GetProjectDirectory(const wxString& projectName) const
{
    const Workspace* workspace = m_mgr->GetWorkspace();
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
    const Workspace* workspace = m_mgr->GetWorkspace();
    wxASSERT(workspace);

    const ProjectPtr projectPtr = GetSelectedProject();
    wxASSERT(projectPtr);

    return workspace->GetProjBuildConf(projectPtr->GetName(), wxEmptyString);
}

/* ************************************************************************ */

const CMakeProjectSettings* CMakePlugin::GetSelectedProjectSettings() const
{
    const ProjectPtr projectPtr = GetSelectedProject();
    wxASSERT(projectPtr);

    const wxString project = projectPtr->GetName();
    const wxString config = GetSelectedProjectConfig();

    wxASSERT(m_settingsManager);
    return m_settingsManager->GetProjectSettings(project, config);
}

/* ************************************************************************ */

bool CMakePlugin::IsSeletedProjectEnabled() const
{
    const CMakeProjectSettings* settings = GetSelectedProjectSettings();

    return settings && settings->enabled;
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

clToolBar* CMakePlugin::CreateToolBar(wxWindow* parent)
{
    wxUnusedVar(parent);
    return NULL;
}

/* ************************************************************************ */

void CMakePlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    menu->Append(new wxMenuItem(menu, XRCID("cmake_settings"), _("Settings...")));

    pluginsMenu->Append(wxID_ANY, "CMake", menu);

    wxTheApp->Bind(wxEVT_COMMAND_MENU_SELECTED, &CMakePlugin::OnSettings, this, XRCID("cmake_settings"));
}

/* ************************************************************************ */

void CMakePlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    // Add menu to project menu
    if(type == MenuTypeFileView_Project) {
        if(!menu->FindItem(XRCID("cmake_project_menu"))) {
            menu->PrependSeparator();
            menu->Prepend(XRCID("cmake_project_menu"), _("CMake"), new CMakeProjectMenu(this));
        }
    } else if(type == MenuTypeFileView_Workspace) {
        if(!menu->FindItem(XRCID("cmake_workspace_menu"))) {
            menu->PrependSeparator();
            menu->Prepend(XRCID("cmake_workspace_menu"), _("CMake"), new CMakeWorkspaceMenu(this));
        }
    }
}

/* ************************************************************************ */

void
CMakePlugin::HookProjectSettingsTab(wxBookCtrlBase* notebook, const wxString& projectName, const wxString& configName)
{
    wxASSERT(notebook);

    /*
     * Let's save some memory and use only one panel for all
     * configurations.
     */

    if(!m_panel) {
        wxASSERT(m_mgr);
        wxASSERT(m_mgr->GetWorkspace());

        // Create panel
        m_panel = new CMakeProjectSettingsPanel(notebook, this);

        // Add panel to the notebook
        notebook->AddPage(m_panel, wxT("CMake"), true);
    }

    wxASSERT(m_panel);
    // Check panel owner
    wxASSERT(notebook == m_panel->GetParent());

    // It's not exactly good idea to load config everytime
    m_settingsManager->LoadProject(projectName);

    // Find settings or create new one
    m_panel->SetSettings(m_settingsManager->GetProjectSettings(projectName, configName, true), projectName, configName);
}

/* ************************************************************************ */

void
CMakePlugin::UnHookProjectSettingsTab(wxBookCtrlBase* notebook, const wxString& projectName, const wxString& configName)
{
    // The configName is empty :(
    wxASSERT(notebook);

    wxUnusedVar(projectName);
    wxUnusedVar(configName);

    // Try to find panel
    int pos = notebook->FindPage(m_panel);

    if(pos != wxNOT_FOUND) {
        // Remove and destroy
        notebook->RemovePage(pos);
        m_panel->Destroy();
        m_panel = NULL;
    }
}

/* ************************************************************************ */

void CMakePlugin::UnPlug()
{
    wxASSERT(m_mgr);
    Notebook* notebook = m_mgr->GetWorkspacePaneNotebook();
    wxASSERT(notebook);

    size_t pos = notebook->GetPageIndex("CMake Help");
    if(pos != Notebook::npos) {
        CMakeHelpTab* helpTab = dynamic_cast<CMakeHelpTab*>(notebook->GetPage(pos));
        if(helpTab) {
            helpTab->Stop();
        }
        notebook->RemovePage(pos);
    }

    // Unbind events
    wxTheApp->Unbind(wxEVT_COMMAND_MENU_SELECTED, &CMakePlugin::OnSettings, this, XRCID("cmake_settings"));

    EventNotifier::Get()->Unbind(
        wxEVT_CMD_PROJ_SETTINGS_SAVED, clProjectSettingsEventHandler(CMakePlugin::OnSaveConfig), this);
    EventNotifier::Get()->Unbind(
        wxEVT_GET_PROJECT_BUILD_CMD, clBuildEventHandler(CMakePlugin::OnGetBuildCommand), this);
    EventNotifier::Get()->Unbind(
        wxEVT_GET_PROJECT_CLEAN_CMD, clBuildEventHandler(CMakePlugin::OnGetCleanCommand), this);
    EventNotifier::Get()->Unbind(
        wxEVT_GET_IS_PLUGIN_MAKEFILE, clBuildEventHandler(CMakePlugin::OnGetIsPluginMakefile), this);
    EventNotifier::Get()->Unbind(
        wxEVT_PLUGIN_EXPORT_MAKEFILE, clBuildEventHandler(CMakePlugin::OnExportMakefile), this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(CMakePlugin::OnWorkspaceLoaded), this);
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
        wxMessageBox("Unable to open \"" + filename.GetFullPath() + "\"",
                     wxMessageBoxCaptionStr,
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

/* ************************************************************************ */

void CMakePlugin::OnSaveConfig(clProjectSettingsEvent& event)
{
    event.Skip();

    CL_DEBUG("Saving CMake config...");

    // Panel doesn't exists
    if(!m_panel) return;

    // Get project name
    const wxString& project = event.GetProjectName();

    // Store settings into pointer
    m_panel->StoreSettings();

    wxASSERT(m_settingsManager);
    // Save settings
    m_settingsManager->SaveProject(project);
}

/* ************************************************************************ */

void CMakePlugin::OnGetCleanCommand(clBuildEvent& event)
{
    ProcessBuildEvent(event, "clean");
}

/* ************************************************************************ */

void CMakePlugin::OnGetBuildCommand(clBuildEvent& event)
{
    ProcessBuildEvent(event);
}

/* ************************************************************************ */

void CMakePlugin::OnGetIsPluginMakefile(clBuildEvent& event)
{
    wxString project = event.GetProjectName();
    const wxString config = event.GetConfigurationName();

    // Get settings
    const CMakeProjectSettings* settings = GetSettingsManager()->GetProjectSettings(project, config);

    // Doesn't exists or not enabled
    if(!settings || !settings->enabled) {
        // Cannot provide custom makefile
        event.Skip();
        return;
    }

    // Custom makefile is provided
}

/* ************************************************************************ */

void CMakePlugin::OnExportMakefile(clBuildEvent& event)
{
    const wxString project = event.GetProjectName();
    const wxString config = event.GetConfigurationName();

    // Get settings
    const CMakeProjectSettings* settings = GetSettingsManager()->GetProjectSettings(project, config);

    // Doesn't exists or not enabled
    if(!settings || !settings->enabled) {
        // Unable to export makefile
        event.Skip();
        return;
    }

    // Get project directory - this is directory where the makefile is stored
    const wxFileName projectDir = GetProjectDirectory(project);

    // Targets forward inspired by
    // https://gist.github.com/doitian/4978329

    // Content of the generated makefile
    wxString content = wxString() << "# Generated by CMakePlugin\n"
                                     ".PHONY: all clean $(MAKECMDGOALS)\n"
                                     "\n";

    // Parent project is set
    if(!settings->parentProject.IsEmpty()) {
        // Configure parent project instead
        const wxString& parentProject = settings->parentProject;
        settings = GetSettingsManager()->GetProjectSettings(parentProject, config);

        // Parent project not found
        if(!settings || !settings->enabled) {
            CL_ERROR("Unable to find or not enabled parent project "
                     "'" +
                     parentProject + "' for project '" + project + "'");
            return;
        }

        // Get parent project directory
        wxFileName parentProjectDir = GetProjectDirectory(parentProject);
        parentProjectDir.MakeRelativeTo(projectDir.GetFullPath());

        // Path is relative so UNIX path system can be used
        const wxString parentProjectDirEsc = parentProjectDir.GetPath(wxPATH_NO_SEPARATOR, wxPATH_UNIX);

        // Redirect make to the parent project
        content << "# Parent project\n"
                   "PARENT          := " << parentProjectDirEsc << "\n"
                                                                   "PARENT_MAKEFILE := " << parentProject
                << ".mk\n"
                   "\n"
                   "all:\n"
                   "\t$(MAKE) -C \"$(PARENT)\" -f \"$(PARENT_MAKEFILE)\" " << project
                << "\n"
                   "\n"
                   "clean:\n"
                   "\t$(MAKE) -C \"$(PARENT)\" -f \"$(PARENT_MAKEFILE)\" " << project << " clean\n"
                                                                                         "\n";

    } else {

        // Macro expander
        // FIXME use IMacroManager (unable to find it yet)
        MacroManager* macro = MacroManager::Instance();
        wxASSERT(macro);

        // Get variables
        // Expand variables for final project
        const wxString cmake = GetConfiguration()->GetProgramPath();
        wxFileName sourceDir =
            wxFileName::DirName(macro->Expand(settings->sourceDirectory, GetManager(), project, config));
        wxFileName buildDir =
            wxFileName::DirName(macro->Expand(settings->buildDirectory, GetManager(), project, config));

        // Source dir must be relative to build directory (here is cmake called)
        sourceDir.MakeRelativeTo(buildDir.GetFullPath());
        // Build dir must be relative to project directory
        buildDir.MakeRelativeTo(projectDir.GetFullPath());

        // Relative paths
        const wxString sourceDirEsc = sourceDir.GetPath(wxPATH_NO_SEPARATOR, wxPATH_UNIX);
        const wxString buildDirEsc = buildDir.GetPath(wxPATH_NO_SEPARATOR, wxPATH_UNIX);

        // Generated makefile
        content << "CMAKE      := \"" << cmake << "\"\n"
                                                  "BUILD_DIR  := " << buildDirEsc << "\n"
                                                                                     "SOURCE_DIR := " << sourceDirEsc
                << "\n"
                   "CMAKE_ARGS := " << CreateArguments(*settings, *m_configuration.get())
                << "\n"
                   "\n"
                   "# Building project(s)\n"
                   "$(or $(lastword $(MAKECMDGOALS)), all): $(BUILD_DIR)/Makefile\n"
                   "\t$(MAKE) -C \"$(BUILD_DIR)\" $(MAKECMDGOALS)\n"
                   "\n"
                   "# Building directory\n"
                   "$(BUILD_DIR):\n"
                   "\t$(CMAKE) -E make_directory \"$(BUILD_DIR)\"\n"
                   "\n"
                   "# Rule that detects if cmake is called\n"
                   "$(BUILD_DIR)/Makefile: .cmake_dirty | $(BUILD_DIR)\n"
                   "\tcd \"$(BUILD_DIR)\" && $(CMAKE) $(CMAKE_ARGS) \"$(SOURCE_DIR)\"\n"
                   "\n"
                   "# This rule / file allows force cmake run\n"
                   ".cmake_dirty:\n"
                   "\t@echo '' > .cmake_dirty\n"
                   "\n";
    }

    // Path to makefile - called project directory required
    wxFileName makefile = projectDir;
    makefile.SetName(project);
    makefile.SetExt("mk");

    // Read old content from disk
    wxString oldContent;
    bool ok = ReadFileWithConversion(makefile.GetFullPath(), oldContent);

    // Write only if there are some changes
    if(!ok || content != oldContent) {
        // Write make file content
        wxFile file(makefile.GetFullPath(), wxFile::write);

        if(!file.Write(content)) {
            CL_ERROR("Unable to write custom makefile (CMakePlugin): " + makefile.GetFullPath());
        }
    }
}

/* ************************************************************************ */

void CMakePlugin::OnWorkspaceLoaded(wxCommandEvent& event)
{
    // Allow others to do something
    event.Skip();

    // Load everything into memory
    m_settingsManager->Load();
}

/* ************************************************************************ */

void CMakePlugin::ProcessBuildEvent(clBuildEvent& event, wxString param)
{
    wxString project = event.GetProjectName();
    const wxString config = event.GetConfigurationName();

    // Get settings
    const CMakeProjectSettings* settings = GetSettingsManager()->GetProjectSettings(project, config);

    // Doesn't exists or not enabled
    if(!settings || !settings->enabled) {
        // Unable to export makefile
        event.Skip();
        return;
    }

    // Project has parent project
    if(!settings->parentProject.IsEmpty()) {
        // Add project name as target
        param = project + " " + param;
        // Build parent project
        project = settings->parentProject;
    }

    // Workspace directory
    const wxFileName workspaceDir = GetWorkspaceDirectory();

    // Use relative path
    wxFileName projectDir = GetProjectDirectory(project);
    projectDir.MakeRelativeTo(workspaceDir.GetFullPath());

    const wxString projectDirEsc = projectDir.GetPath(wxPATH_NO_SEPARATOR, wxPATH_UNIX);

    // Build command
    wxString cmd = "$(MAKE)";

    if(!projectDirEsc.IsEmpty()) cmd += " -C \"" + projectDirEsc + "\"";

    // Add makefile
    cmd += " -f \"" + project + ".mk\"";

    // Add optional parameters
    if(!param.IsEmpty()) cmd += " " + param;

    // The build command is simple make call with different makefile
    event.SetCommand(cmd);
}

/* ************************************************************************ */
