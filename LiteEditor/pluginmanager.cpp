//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : pluginmanager.cpp
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
#include "pluginmanager.h"

#include "BuildTab.hpp"
#include "Keyboard/clKeyboardManager.h"
#include "SideBar.hpp"
#include "StdToWX.h"
#include "app.h"
#include "bitmap_loader.h"
#include "build_settings_config.h"
#include "buildmanager.h"
#include "clEditorBar.h"
#include "clInfoBar.h"
#include "clStrings.h"
#include "cl_config.h"
#include "cl_standard_paths.h"
#include "ctags_manager.h"
#include "debugger.h"
#include "detachedpanesinfo.h"
#include "editor_config.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileexplorer.h"
#include "fileview.h"
#include "findinfilesdlg.h"
#include "frame.h"
#include "language.h"
#include "macromanager.h"
#include "manager.h"
#include "optionsconfig.h"
#include "plugin_version.h"
#include "procutils.h"
#include "sessionmanager.h"
#include "workspacetab.h"

#include <memory>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/tokenzr.h>
#include <wx/toolbook.h>
#include <wx/xrc/xmlres.h>

namespace
{
const wxString SIDEBAR = PANE_LEFT_SIDEBAR;
const wxString SECONDARY_SIDEBAR = PANE_RIGHT_SIDEBAR;
const wxString BOTTOM_BAR = PANE_OUTPUT;
} // namespace

PluginManager* PluginManager::Get()
{
    static PluginManager theManager;
    ::clSetManager(&theManager);
    return &theManager;
}

void PluginManager::UnLoad()
{
    {
        // Before we unload the plugins, store the list of visible workspace tabs
        auto sidebar_book = clMainFrame::Get()->GetWorkspacePane()->GetNotebook();
        wxArrayString visibleTabs;
        for (size_t i = 0; i < sidebar_book->GetPageCount(); ++i) {
            visibleTabs.Add(sidebar_book->GetPageText(i));
        }
        clConfig::Get().Write("VisibleWorkspaceTabs", visibleTabs);
    }

    {
        // Now do the same for the output view
        auto bottom_book = clMainFrame::Get()->GetOutputPane()->GetNotebook();
        wxArrayString visibleTabs;
        for (size_t i = 0; i < bottom_book->GetPageCount(); ++i) {
            visibleTabs.Add(bottom_book->GetPageText(i));
        }
        clConfig::Get().Write("VisibleOutputTabs", visibleTabs);
    }

    for (auto [__, plugin] : m_plugins) {
        plugin->UnPlug();
        delete plugin;
    }
    m_plugins.clear();
    m_dl.clear();
}

PluginManager::~PluginManager() {}

PluginManager::PluginManager()
    : m_bmpLoader(NULL)
{
    m_menusToBeHooked.insert(MenuTypeFileExplorer);
    m_menusToBeHooked.insert(MenuTypeFileView_Workspace);
    m_menusToBeHooked.insert(MenuTypeFileView_Project);
    m_menusToBeHooked.insert(MenuTypeFileView_Folder);
    m_menusToBeHooked.insert(MenuTypeFileView_File);
    m_menusToBeHooked.insert(MenuTypeEditor);
}

void PluginManager::Load()
{
    wxString ext;
#if defined(__WXGTK__)
    ext = wxT("so");

#elif defined(__WXMAC__)
    ext = wxT("dylib");

#else
    ext = wxT("dll");
#endif

    wxString fileSpec(wxT("*.") + ext);
    clConfig conf("plugins.conf");

    conf.ReadItem(&m_pluginsData);

    // set the managers
    // this code assures us that the shared objects will see the same instances as the application
    // does
    LanguageST::Get()->SetTagsManager(GetTagsManager());
    TagsManagerST::Get()->SetLanguage(LanguageST::Get());

    // Plugin loading policy
    CodeLiteApp* app = static_cast<CodeLiteApp*>(GetTheApp());
    CodeLiteApp::PluginPolicy pp = app->GetPluginLoadPolicy();
    wxArrayString allowedPlugins;
    if (pp == CodeLiteApp::PP_None) {
        return;
    }

    else if (pp == CodeLiteApp::PP_FromList) {
        allowedPlugins = app->GetAllowedPlugins();
    }

    wxString pluginsDir = clStandardPaths::Get().GetPluginsDirectory();
    if (wxDir::Exists(pluginsDir)) {
        // get list of dlls
        wxArrayString files;
        wxDir::GetAllFiles(pluginsDir, &files, fileSpec, wxDIR_FILES);

        // Sort the plugins by A-Z
        std::sort(files.begin(), files.end());
        for (size_t i = 0; i < files.GetCount(); i++) {

            wxString fileName(files.Item(i));
#ifdef __WXGTK__
            wxFileName fnDLL(fileName);
            if (fnDLL.GetFullName().StartsWith("lib")) {
                // don't attempt to load a library
                continue;
            }
#endif

            clDynamicLibrary* dl = new clDynamicLibrary();
            if (!dl->Load(fileName)) {
                clERROR() << "Failed to load plugin's dll" << fileName << endl;
                if (!dl->GetError().IsEmpty()) {
                    clERROR() << dl->GetError() << endl;
                }
                wxDELETE(dl);
                continue;
            }

            bool success(false);
            GET_PLUGIN_INFO_FUNC pfnGetPluginInfo = (GET_PLUGIN_INFO_FUNC)dl->GetSymbol(wxT("GetPluginInfo"), &success);
            if (!success) {
                wxDELETE(dl);
                continue;
            }

            // load the plugin version method
            // if the methods does not exist, handle it as if it has value of 100 (lowest version API)
            int interface_version(100);
            GET_PLUGIN_INTERFACE_VERSION_FUNC pfnInterfaceVersion =
                (GET_PLUGIN_INTERFACE_VERSION_FUNC)dl->GetSymbol(wxT("GetPluginInterfaceVersion"), &success);
            if (success) {
                interface_version = pfnInterfaceVersion();
            } else {
                clWARNING() << "Failed to find GetPluginInterfaceVersion() in dll" << fileName << endl;
                if (!dl->GetError().IsEmpty()) {
                    clWARNING() << dl->GetError() << endl;
                }
            }

            if (interface_version != PLUGIN_INTERFACE_VERSION) {
                clWARNING() << "Version interface mismatch error for plugin:" << fileName
                            << ". Found:" << interface_version << "Expected:" << PLUGIN_INTERFACE_VERSION << endl;
                wxDELETE(dl);
                continue;
            }

            // Check if this dll can be loaded
            PluginInfo* pluginInfo = pfnGetPluginInfo();

            wxString pname = pluginInfo->GetName();
            m_installedPlugins.insert({ pname, *pluginInfo });

            pname.MakeLower().Trim().Trim(false);

            // Check the policy
            if (pp == CodeLiteApp::PP_FromList && allowedPlugins.Index(pname) == wxNOT_FOUND) {
                // Policy is set to 'from list' and this plugin does not match any plugins from
                // the list, don't allow it to be loaded
                wxDELETE(dl);
                continue;
            }

            // If the plugin does not exist in the m_pluginsData, assume its the first time we see it
            bool firstTimeLoading = (m_pluginsData.GetPlugins().count(pluginInfo->GetName()) == 0);
            if (firstTimeLoading && pluginInfo->HasFlag(PluginInfo::kDisabledByDefault)) {
                m_pluginsData.DisablePlugin(pluginInfo->GetName());
                wxDELETE(dl);
                continue;
            }

            // Can we load it?
            if (!m_pluginsData.CanLoad(*pluginInfo)) {
                clWARNING() << "Plugin:" << pluginInfo->GetName() << " is not enabled" << endl;
                wxDELETE(dl);
                continue;
            }

            // try and load the plugin
            GET_PLUGIN_CREATE_FUNC pfn = (GET_PLUGIN_CREATE_FUNC)dl->GetSymbol(wxT("CreatePlugin"), &success);
            if (!success) {
                clWARNING() << "Failed to find CreatePlugin() in dll:" << fileName << endl;
                if (!dl->GetError().IsEmpty()) {
                    clWARNING() << dl->GetError() << endl;
                }

                m_pluginsData.DisablePlugin(pluginInfo->GetName());
                continue;
            }

            // Construct the plugin
            IPlugin* plugin = pfn((IManager*)this);
            clDEBUG() << "Loaded plugin:" << plugin->GetLongName() << endl;
            m_plugins[plugin->GetShortName()] = plugin;

            // Load the toolbar
            plugin->CreateToolBar(clMainFrame::Get()->GetPluginsToolBar());

            // Keep the dynamic load library
            m_dl.push_back(dl);
        }
        clMainFrame::Get()->GetDockingManager().Update();

        // Let the plugins plug their menu in the 'Plugins' menu at the menu bar
        // the create menu will be placed as a sub menu of the 'Plugin' menu
        wxMenu* pluginsMenu = NULL;
        wxMenuItem* menuitem = clMainFrame::Get()->GetMainMenuBar()->FindItem(XRCID("manage_plugins"), &pluginsMenu);
        if (pluginsMenu && menuitem) {
            for (auto& vt : m_plugins) {
                IPlugin* plugin = vt.second;
                plugin->SetPluginsMenu(pluginsMenu);
                plugin->CreatePluginMenu(pluginsMenu);
            }
        }

        // save the plugins data
        conf.WriteItem(&m_pluginsData);
    }

    // Now that all the plugins are loaded, load from the configuration file
    // list of visible tabs
    static const wxArrayString DefaultArray = StdToWX::ToArrayString({ "NOT-FOUND" });

    DetachedPanesInfo dpi;
    GetConfigTool()->ReadObject(wxT("DetachedPanesList"), &dpi);
    const wxArrayString& detachedPanes = dpi.GetPanes();

    {
        // Hide workspace tabs
        const wxArrayString& tabs = GetWorkspaceTabs();
        wxArrayString visibleTabs = clConfig::Get().Read("VisibleWorkspaceTabs", DefaultArray);
        if (!((visibleTabs.size() == 1) && (visibleTabs.Item(0) == "NOT-FOUND"))) {
            for (size_t i = 0; i < tabs.size(); ++i) {
                if ((visibleTabs.Index(tabs.Item(i)) == wxNOT_FOUND) &&
                    (detachedPanes.Index(tabs.Item(i)) == wxNOT_FOUND)) {
                    // hidden tab - post an event
                    clCommandEvent eventHide(wxEVT_SHOW_WORKSPACE_TAB);
                    eventHide.SetSelected(false).SetString(tabs.Item(i));
                    EventNotifier::Get()->AddPendingEvent(eventHide);
                }
            }
        }
    }

    {
        // Hide output tabs
        const wxArrayString& tabs = GetOutputTabs();
        wxArrayString visibleTabs = clConfig::Get().Read("VisibleOutputTabs", DefaultArray);
        if (!((visibleTabs.size() == 1) && (visibleTabs.Item(0) == "NOT-FOUND"))) {
            for (size_t i = 0; i < tabs.size(); ++i) {
                if ((visibleTabs.Index(tabs.Item(i)) == wxNOT_FOUND) &&
                    (detachedPanes.Index(tabs.Item(i)) == wxNOT_FOUND)) {
                    // hidden tab - post an event
                    clCommandEvent eventHide(wxEVT_SHOW_OUTPUT_TAB);
                    eventHide.SetSelected(false).SetString(tabs.Item(i));
                    EventNotifier::Get()->AddPendingEvent(eventHide);
                }
            }
        }
    }
}

IEditor* PluginManager::GetActiveEditor()
{
    if (clMainFrame::Get() && clMainFrame::Get()->GetMainBook()) {
        clEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
        if (!editor) {
            return nullptr;
        }
        return dynamic_cast<IEditor*>(editor);
    }
    return NULL;
}

IConfigTool* PluginManager::GetConfigTool() { return EditorConfigST::Get(); }

void PluginManager::HookPopupMenu(wxMenu* menu, MenuType type)
{
    std::map<wxString, IPlugin*>::iterator iter = m_plugins.begin();
    for (; iter != m_plugins.end(); iter++) {
        iter->second->HookPopupMenu(menu, type);
    }
}

TreeItemInfo PluginManager::GetSelectedTreeItemInfo(TreeType type)
{
    TreeItemInfo info;
    switch (type) {
    case TreeFileExplorer:
        return clMainFrame::Get()->GetFileExplorer()->GetItemInfo();
    case TreeFileView:
        return clMainFrame::Get()->GetWorkspaceTab()->GetFileView()->GetSelectedItemInfo();
    default:
        return info;
    }
}

clTreeCtrl* PluginManager::GetWorkspaceTree() { return clMainFrame::Get()->GetWorkspaceTab()->GetFileView(); }

clTreeCtrl* PluginManager::GetFileExplorerTree() { return clMainFrame::Get()->GetFileExplorer()->GetTree(); }

MainNotebook* PluginManager::GetMainNotebook() { return clMainFrame::Get()->GetMainBook()->GetNotebook(); }

IEditor* PluginManager::OpenFile(const wxString& fileName, const wxString& bmpResourceName, const wxString& tooltip)
{
    int bmp_index = clMainFrame::Get()->GetMainBook()->GetBitmapIndexOrAdd(bmpResourceName);
    IEditor* editor = clMainFrame::Get()->GetMainBook()->OpenFile(fileName, bmp_index, tooltip);
    if (editor) {
        editor->SetActive();
    }
    return editor;
}

IEditor* PluginManager::OpenRemoteFile(const wxString& local_path, const wxString& remote_path,
                                       const wxString& ssh_account, const wxString& tooltip)
{
    return clMainFrame::Get()->GetMainBook()->OpenRemoteFile(local_path, remote_path, ssh_account, tooltip);
}

IEditor* PluginManager::OpenFile(const wxString& fileName, const wxString& projectName, int lineno, OF_extra flags)
{
    IEditor* editor = clMainFrame::Get()->GetMainBook()->OpenFile(fileName, projectName, lineno, wxNOT_FOUND, flags);
    if (editor) {
        editor->SetActive();
    }
    return editor;
}

wxString PluginManager::GetStartupDirectory() const { return ManagerST::Get()->GetStartupDirectory(); }

void PluginManager::AddProject(const wxString& path) { ManagerST::Get()->AddProject(path); }

bool PluginManager::IsWorkspaceOpen() const { return ManagerST::Get()->IsWorkspaceOpen(); }

TagsManager* PluginManager::GetTagsManager() { return TagsManagerST::Get(); }

clCxxWorkspace* PluginManager::GetWorkspace() { return clCxxWorkspaceST::Get(); }

bool PluginManager::AddFilesToVirtualFolder(wxTreeItemId& item, wxArrayString& paths)
{
    return clMainFrame::Get()->GetWorkspaceTab()->GetFileView()->AddFilesToVirtualFolder(item, paths);
}

bool PluginManager::AddFilesToVirtualFolder(const wxString& vdFullPath, wxArrayString& paths)
{
    return clMainFrame::Get()->GetWorkspaceTab()->GetFileView()->AddFilesToVirtualFolder(vdFullPath, paths);
}

bool PluginManager::AddFilesToVirtualFolderIntelligently(const wxString& vdFullPath, wxArrayString& paths)
{
    return clMainFrame::Get()->GetWorkspaceTab()->GetFileView()->AddFilesToVirtualFolderIntelligently(vdFullPath,
                                                                                                      paths);
}

void PluginManager::RedefineProjFiles(ProjectPtr proj, const wxString& path, std::vector<wxString>& files)
{
    return clMainFrame::Get()->GetWorkspaceTab()->GetFileView()->RedefineProjFiles(proj, path, files);
}

int PluginManager::GetToolbarIconSize()
{
    // for now return 24 by default
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    if (options) {
        return options->GetIconsSize();
    }
    return 24;
}

wxAuiManager* PluginManager::GetDockingManager() { return m_dockingManager; }

EnvironmentConfig* PluginManager::GetEnv() { return EnvironmentConfig::Instance(); }

wxString PluginManager::GetProjectExecutionCommand(const wxString& projectName, wxString& wd)
{
    return ManagerST::Get()->GetProjectExecutionCommand(projectName, wd, false);
}

wxApp* PluginManager::GetTheApp() { return wxTheApp; }

void PluginManager::ReloadWorkspace()
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("reload_workspace"));
    clMainFrame::Get()->GetEventHandler()->AddPendingEvent(evt);
}

IPlugin* PluginManager::GetPlugin(const wxString& pluginName)
{
    std::map<wxString, IPlugin*>::iterator iter = m_plugins.find(pluginName);
    if (iter != m_plugins.end()) {
        return iter->second;
    }
    return NULL;
}

wxEvtHandler* PluginManager::GetOutputWindow() { return clMainFrame::Get()->GetOutputPane()->GetOutputWindow(); }

bool PluginManager::SaveAll(bool prompt) { return clMainFrame::Get()->GetMainBook()->SaveAll(prompt, false); }

wxString PluginManager::GetInstallDirectory() const { return ManagerST::Get()->GetInstallDir(); }

bool PluginManager::CreateVirtualDirectory(const wxString& parentPath, const wxString& vdName)
{
    return clMainFrame::Get()->GetWorkspaceTab()->GetFileView()->CreateVirtualDirectory(parentPath, vdName);
}

OptionsConfigPtr PluginManager::GetEditorSettings()
{
    // First try to use clEditor::GetOptions, as it takes account of local preferences
    clEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    if (editor) {
        return editor->GetOptions();
    }
    // Failing that...
    return EditorConfigST::Get()->GetOptions();
}

void PluginManager::FindAndSelect(const wxString& pattern, const wxString& name, int pos)
{
    clEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    if (editor) {
        editor->FindAndSelectV(pattern, name, pos, NavMgr::Get());
        editor->SetActive();
    }
}

TagEntryPtr PluginManager::GetTagAtCaret(bool scoped, bool impl)
{
    clEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    if (!editor)
        return NULL;
    return editor->GetContext()->GetTagAtCaret(scoped, impl);
}

bool PluginManager::AllowToolbar()
{
    long v = EditorConfigST::Get()->GetInteger("UseSingleToolbar", 1);
    return (v == 1 ? false : true);
}

void PluginManager::EnableToolbars()
{
    // In case, plugins are now allowed to insert toolbars, disable the toolbars_menu item
    if (AllowToolbar() == false) {
        int ii = clMainFrame::Get()->GetMainMenuBar()->FindMenu(wxT("View"));
        if (ii != wxNOT_FOUND) {
            wxMenu* viewMenu = clMainFrame::Get()->GetMainMenuBar()->GetMenu(ii);
            wxMenuItem* item = viewMenu->FindItem(XRCID("toolbars_menu"));
            if (item) {
                item->Enable(false);
            }
        }
    }
}

void PluginManager::SetStatusMessage(const wxString& msg, int seconds_to_live)
{
    if (GetStatusBar()) {
        GetStatusBar()->SetMessage(msg, seconds_to_live);
    }
}

void PluginManager::ProcessCommandQueue() { ManagerST::Get()->ProcessCommandQueue(); }

void PluginManager::PushQueueCommand(const QueueCommand& cmd) { ManagerST::Get()->PushQueueCommand(cmd); }

void PluginManager::StopAndClearQueue() { ManagerST::Get()->StopBuild(); }

bool PluginManager::IsBuildInProgress() const { return ManagerST::Get()->IsBuildInProgress(); }

bool PluginManager::IsBuildEndedSuccessfully() const { return ManagerST::Get()->IsBuildEndedSuccessfully(); }

wxString PluginManager::GetProjectNameByFile(wxString& fullPathFileName)
{
    return ManagerST::Get()->GetProjectNameByFile(fullPathFileName);
}

wxString PluginManager::GetProjectNameByFile(const wxString& fullPathFileName)
{
    return ManagerST::Get()->GetProjectNameByFile(fullPathFileName);
}

BuildManager* PluginManager::GetBuildManager() { return BuildManagerST::Get(); }

BuildSettingsConfig* PluginManager::GetBuildSettingsConfigManager() { return BuildSettingsConfigST::Get(); }

bool PluginManager::ClosePage(const wxString& title) { return clMainFrame::Get()->GetMainBook()->ClosePage(title); }
bool PluginManager::ClosePage(const wxFileName& filename)
{
    MainBook* book = clMainFrame::Get()->GetMainBook();
    clEditor* editor = book->FindEditor(filename.GetFullPath());
    return clMainFrame::Get()->GetMainBook()->ClosePage(editor);
}

wxWindow* PluginManager::FindPage(const wxString& text) { return clMainFrame::Get()->GetMainBook()->FindPage(text); }

bool PluginManager::AddPage(wxWindow* win, const wxString& text, const wxString& tooltip,
                            const wxString& bmpResourceName, bool selected)
{
    int bmp_index = clMainFrame::Get()->GetMainBook()->GetBitmapIndexOrAdd(bmpResourceName);
    return clMainFrame::Get()->GetMainBook()->AddBookPage(win, text, tooltip, bmp_index, selected, wxNOT_FOUND);
}

bool PluginManager::SelectPage(wxWindow* win) { return clMainFrame::Get()->GetMainBook()->SelectPage(win); }

IEditor* PluginManager::OpenFile(const BrowseRecord& rec) { return clMainFrame::Get()->GetMainBook()->OpenFile(rec); }

NavMgr* PluginManager::GetNavigationMgr() { return NavMgr::Get(); }

void PluginManager::HookProjectSettingsTab(wxBookCtrlBase* book, const wxString& projectName,
                                           const wxString& configName)
{
    std::map<wxString, IPlugin*>::iterator iter = m_plugins.begin();
    for (; iter != m_plugins.end(); iter++) {
        iter->second->HookProjectSettingsTab(book, projectName, configName);
    }
}

void PluginManager::UnHookProjectSettingsTab(wxBookCtrlBase* book, const wxString& projectName,
                                             const wxString& configName)
{
    std::map<wxString, IPlugin*>::iterator iter = m_plugins.begin();
    for (; iter != m_plugins.end(); iter++) {
        iter->second->UnHookProjectSettingsTab(book, projectName, configName);
    }
}

IEditor* PluginManager::NewEditor() { return clMainFrame::Get()->GetMainBook()->NewEditor(); }

bool PluginManager::IsShutdownInProgress() const { return ManagerST::Get()->IsShutdownInProgress(); }

BitmapLoader* PluginManager::GetStdIcons()
{
    // return the current bitmap loader
    return clBitmaps::Get().GetLoader();
}

wxArrayString PluginManager::GetProjectCompileFlags(const wxString& projectName, bool isCppFile)
{
    if (IsWorkspaceOpen() == false)
        return wxArrayString();

    wxArrayString args;

    // Next apppend the user include paths
    wxString errMsg;

    // First, we need to find the currently active workspace configuration
    BuildMatrixPtr matrix = GetWorkspace()->GetBuildMatrix();
    if (!matrix) {
        return wxArrayString();
    }

    wxString workspaceSelConf = matrix->GetSelectedConfigurationName();

    // Now that we got the selected workspace configuration, extract the related project configuration
    ProjectPtr proj = GetWorkspace()->FindProjectByName(projectName, errMsg);
    if (!proj) {
        return args;
    }

    wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, proj->GetName());
    BuildConfigPtr dependProjbldConf = GetWorkspace()->GetProjBuildConf(proj->GetName(), projectSelConf);
    if (dependProjbldConf && dependProjbldConf->IsCustomBuild() == false) {
        // Get the include paths and add them
        wxString projectIncludePaths = dependProjbldConf->GetIncludePath();
        wxArrayString projectIncludePathsArr = wxStringTokenize(projectIncludePaths, wxT(";"), wxTOKEN_STRTOK);
        for (size_t i = 0; i < projectIncludePathsArr.GetCount(); i++) {
            args.Add(wxString::Format(wxT("-I%s"), projectIncludePathsArr[i].c_str()));
        }
        // get the compiler options and add them
        wxString projectCompileOptions = dependProjbldConf->GetCompileOptions();
        wxArrayString projectCompileOptionsArr = wxStringTokenize(projectCompileOptions, wxT(";"), wxTOKEN_STRTOK);
        for (size_t i = 0; i < projectCompileOptionsArr.GetCount(); i++) {
            wxString cmpOption(projectCompileOptionsArr.Item(i));
            cmpOption.Trim().Trim(false);
            wxString tmp;
            // Expand backticks / $(shell ...) syntax supported by codelite
            if (cmpOption.StartsWith(wxT("$(shell "), &tmp) || cmpOption.StartsWith(wxT("`"), &tmp)) {
                cmpOption = tmp;
                tmp.Clear();
                if (cmpOption.EndsWith(wxT(")"), &tmp) || cmpOption.EndsWith(wxT("`"), &tmp)) {
                    cmpOption = tmp;
                }
                if (m_backticks.find(cmpOption) == m_backticks.end()) {
                    // Expand the backticks into their value
                    wxArrayString outArr;
                    // Apply the environment before executing the command
                    EnvSetter setter(EnvironmentConfig::Instance(), NULL, projectName, dependProjbldConf->GetName());
                    ProcUtils::SafeExecuteCommand(cmpOption, outArr);
                    wxString expandedValue;
                    for (size_t j = 0; j < outArr.size(); j++) {
                        expandedValue << outArr.Item(j) << wxT(" ");
                    }
                    m_backticks[cmpOption] = expandedValue;
                    cmpOption = expandedValue;
                } else {
                    cmpOption = m_backticks.find(cmpOption)->second;
                }
            }
            args.Add(cmpOption);
        }
        // get the compiler preprocessor and add them as well
        wxString projectPreps = dependProjbldConf->GetPreprocessor();
        wxArrayString projectPrepsArr = wxStringTokenize(projectPreps, wxT(";"), wxTOKEN_STRTOK);
        for (size_t i = 0; i < projectPrepsArr.GetCount(); i++) {
            args.Add(wxString::Format(wxT("-D%s"), projectPrepsArr[i].c_str()));
        }
    }
    return args;
}

void PluginManager::AddEditorPage(wxWindow* page, const wxString& name, const wxString& tooltip)
{
    clMainFrame::Get()->GetMainBook()->AddBookPage(page, name, tooltip, wxNOT_FOUND, true, wxNOT_FOUND);
}

wxPanel* PluginManager::GetEditorPaneNotebook() { return clMainFrame::Get()->GetMainBook(); }

wxWindow* PluginManager::GetActivePage() { return clMainFrame::Get()->GetMainBook()->GetCurrentPage(); }

wxWindow* PluginManager::GetPage(size_t page) { return clMainFrame::Get()->GetMainBook()->GetPage(page); }

wxString PluginManager::GetPageTitle(wxWindow* win) const
{
    return clMainFrame::Get()->GetMainBook()->GetPageTitle(win);
}

void PluginManager::SetPageTitle(wxWindow* win, const wxString& title)
{
    clMainFrame::Get()->GetMainBook()->SetPageTitle(win, title);
}

ProjectPtr PluginManager::GetSelectedProject() const
{
    return clMainFrame::Get()->GetWorkspaceTab()->GetFileView()->GetSelectedProject();
}

IEditor* PluginManager::FindEditor(const wxString& filename) const
{
    return clMainFrame::Get()->GetMainBook()->FindEditor(filename);
}

void PluginManager::EnableClangCodeCompletion(bool b) { wxUnusedVar(b); }

size_t PluginManager::GetPageCount() const { return clMainFrame::Get()->GetMainBook()->GetPageCount(); }

size_t PluginManager::GetAllEditors(IEditor::List_t& editors, bool inOrder)
{
    clEditor::Vec_t tmpEditors;
    size_t flags = MainBook::kGetAll_IncludeDetached;
    if (inOrder) {
        flags |= MainBook::kGetAll_RetainOrder;
    }

    clMainFrame::Get()->GetMainBook()->GetAllEditors(tmpEditors, flags);
    editors.insert(editors.end(), tmpEditors.begin(), tmpEditors.end());
    return editors.size();
}

size_t PluginManager::GetAllBreakpoints(clDebuggerBreakpoint::Vec_t& breakpoints)
{
    breakpoints.clear();
    ManagerST::Get()->GetBreakpointsMgr()->GetBreakpoints(breakpoints);
    return breakpoints.size();
}

clDebuggerBreakpoint PluginManager::CreateBreakpoint(const wxString& filepath, int line_number)
{
    clDebuggerBreakpoint bp;
    bp.file = filepath;
    bp.lineno = line_number;
    bp.internal_id = ManagerST::Get()->GetBreakpointsMgr()->GetNextID();
    return bp;
}

void PluginManager::DeleteAllBreakpoints() { ManagerST::Get()->GetBreakpointsMgr()->DelAllBreakpoints(); }

void PluginManager::SetBreakpoints(const clDebuggerBreakpoint::Vec_t& breakpoints)
{
    ManagerST::Get()->GetBreakpointsMgr()->DelAllBreakpoints();
    for (size_t i = 0; i < breakpoints.size(); ++i) {
        ManagerST::Get()->GetBreakpointsMgr()->AddBreakpoint(breakpoints.at(i));
    }
}

void PluginManager::LoadPerspective(const wxString& perspectiveName)
{
    ManagerST::Get()->GetPerspectiveManager().LoadPerspective(perspectiveName);
}

void PluginManager::SavePerspective(const wxString& perspectiveName)
{
    ManagerST::Get()->GetPerspectiveManager().SavePerspective(perspectiveName, true);
}

void PluginManager::ProcessEditEvent(wxCommandEvent& e, IEditor* editor)
{
    clEditor* lEditor = dynamic_cast<clEditor*>(editor);
    if (lEditor) {
        lEditor->OnMenuCommand(e);
    }
}

void PluginManager::AppendOutputTabText(eOutputPaneTab tab, const wxString& text, bool toggle_view)
{
    switch (tab) {
    case kOutputTab_Build:
        clMainFrame::Get()->GetOutputPane()->GetBuildTab()->AppendLine(text);
        break;
    case kOutputTab_Output:
        clMainFrame::Get()->GetOutputPane()->GetOutputWindow()->AppendText(text, toggle_view);
        break;
    }
}

void PluginManager::ClearOutputTab(eOutputPaneTab tab)
{
    switch (tab) {
    case kOutputTab_Build:
        clMainFrame::Get()->GetOutputPane()->GetBuildTab()->Clear();
        break;
    case kOutputTab_Output:
        clMainFrame::Get()->GetOutputPane()->GetOutputWindow()->Clear();
        break;
    }
}

void PluginManager::AddWorkspaceToRecentlyUsedList(const wxFileName& filename)
{
    if (filename.Exists()) {
        ManagerST::Get()->AddToRecentlyOpenedWorkspaces(filename.GetFullPath());
    }
}

void PluginManager::StoreWorkspaceSession(const wxFileName& workspaceFile)
{
    if (workspaceFile.Exists()) {
        SessionEntry session;
        clMainFrame::Get()->GetMainBook()->CreateSession(session);
        session.SetWorkspaceName(workspaceFile.GetFullPath());
        ManagerST::Get()->GetBreakpointsMgr()->SaveSession(session);
        SessionManager::Get().Save(session.GetWorkspaceName(), session);
    }
}

void PluginManager::LoadWorkspaceSession(const wxFileName& workspaceFile)
{
    SessionEntry session;
    if (SessionManager::Get().GetSession(workspaceFile.GetFullPath(), session)) {
        // notify about session loading starting
        clCommandEvent event_loading{ wxEVT_SESSION_LOADING };
        EventNotifier::Get()->ProcessEvent(event_loading);

        // Undo any workspace/editor link while loading
        clMainFrame::Get()->GetWorkspaceTab()->FreezeThaw(true);
        clMainFrame::Get()->GetMainBook()->RestoreSession(session);
        clMainFrame::Get()->GetWorkspaceTab()->FreezeThaw(false);

        // Set this session as the 'Last' session
        ManagerST::Get()->GetBreakpointsMgr()->LoadSession(session);
        SessionManager::Get().SetLastSession(workspaceFile.GetFullPath());

        // and we are done
        clCommandEvent event_loaded{ wxEVT_SESSION_LOADED };
        EventNotifier::Get()->AddPendingEvent(event_loaded);
    }
}

void PluginManager::OpenFindInFileForPath(const wxString& path)
{
    wxArrayString paths;
    paths.Add(path);
    OpenFindInFileForPaths(paths);
}

void PluginManager::OpenFindInFileForPaths(const wxArrayString& paths)
{
    // Fire the wxEVT_CMD_FIND_IN_FILES_SHOWING showing event
    clFindInFilesEvent eventFifShowing(wxEVT_FINDINFILES_DLG_SHOWING);
    if (EventNotifier::Get()->ProcessEvent(eventFifShowing))
        return;

    // Prepare the fif dialog
    FindInFilesDialog dlg(EventNotifier::Get()->TopFrame());
    if (!paths.empty()) {
        dlg.SetSearchPaths(wxJoin(paths, ';'), true);
    }
    // Show it
    if (dlg.ShowDialog() == wxID_OK) {
        // Notify about the dialog dismissal
        clFindInFilesEvent eventDismiss(wxEVT_FINDINFILES_DLG_DISMISSED);
        EventNotifier::Get()->ProcessEvent(eventDismiss);
    }
}

void PluginManager::ShowOutputPane(const wxString& selectedWindow)
{
    ManagerST::Get()->ShowOutputPane(selectedWindow, true, false);
}

void PluginManager::ShowManagementWindow(const wxString& selectedWindow, bool show)
{
    // locate the window
    wxWindow* page = BookGetPage(PaneId::SIDE_BAR, selectedWindow);
    if (page) {
        if (show) {
            ManagerST::Get()->ShowWorkspacePane(selectedWindow, true);
        } else {
            ManagerST::Get()->HidePane(SIDEBAR);
        }
    } else {
        page = BookGetPage(PaneId::BOTTOM_BAR, selectedWindow);
        if (page) {
            if (show) {
                ShowOutputPane(selectedWindow);
            } else {
                ManagerST::Get()->HidePane(BOTTOM_BAR);
            }
        } else {
            page = BookGetPage(PaneId::SECONDARY_SIDE_BAR, selectedWindow);
            if (page) {
                if (show) {
                    ManagerST::Get()->ShowSecondarySideBarPane(selectedWindow);
                } else {
                    ManagerST::Get()->HidePane(SECONDARY_SIDEBAR);
                }
            }
        }
    }
}

size_t PluginManager::GetAllTabs(clTab::Vec_t& tabs)
{
    clMainFrame::Get()->GetMainBook()->GetAllTabs(tabs);
    return tabs.size();
}

clStatusBar* PluginManager::GetStatusBar()
{
    if (clMainFrame::m_initCompleted) {
        return clMainFrame::Get()->GetStatusBar();
    }
    return NULL;
}

void PluginManager::ToggleSidebarPane(const wxString& selectedWindow)
{
    if (ManagerST::Get()->IsPaneVisible(SIDEBAR)) {
        ManagerST::Get()->HidePane(SIDEBAR);
    } else {
        ManagerST::Get()->ShowWorkspacePane(selectedWindow, true);
    }
}

void PluginManager::ShowPane(const wxString& pane_name, bool show)
{
    if (show) {
        if (!ManagerST::Get()->IsPaneVisible(pane_name)) {
            ManagerST::Get()->ShowPane(pane_name);
        }
    } else {
        // hide
        if (ManagerST::Get()->IsPaneVisible(pane_name)) {
            ManagerST::Get()->HidePane(pane_name);
        }
    }
}

void PluginManager::ToggleSecondarySidebarPane(const wxString& selectedWindow)
{
    if (ManagerST::Get()->IsPaneVisible(PANE_RIGHT_SIDEBAR)) {
        ManagerST::Get()->HidePane(PANE_RIGHT_SIDEBAR);
    } else {
        ManagerST::Get()->ShowWorkspacePane(selectedWindow, true);
    }
}

void PluginManager::ToggleOutputPane(const wxString& selectedWindow)
{
    if (ManagerST::Get()->IsPaneVisible(PANE_OUTPUT)) {
        if (!selectedWindow.IsEmpty()) {
            wxString selectedTabName;
            Notebook* book = clMainFrame::Get()->GetOutputPane()->GetNotebook();
            int where = book->GetSelection();
            if (where != wxNOT_FOUND) {
                selectedTabName = book->GetPageText(where);
            }
            if (selectedTabName == selectedWindow) {
                // The requested tab is already selected, just hide the pane
                ManagerST::Get()->HidePane(PANE_OUTPUT);
            } else {
                // The output pane is visible, but the selected tab is not the one we wanted
                // Select it
                ManagerST::Get()->ShowOutputPane(selectedWindow, true, false);
            }
        } else {
            // The output pane is visible and the selected tab is the one we requested
            // So just hide it
            ManagerST::Get()->HidePane(PANE_OUTPUT);
        }
    } else {
        // The output pane is hidden, show it and select the requested tab
        ManagerST::Get()->ShowOutputPane(selectedWindow, true, false);
    }
}

clWorkspaceView* PluginManager::GetWorkspaceView() { return clMainFrame::Get()->GetWorkspaceTab()->GetView(); }

void PluginManager::ShowToolBar(bool show)
{
    wxCommandEvent event(wxEVT_MENU, XRCID("hide_tool_bar"));
    event.SetInt(show ? 1 : 0);
    event.SetEventObject(clMainFrame::Get());
    clMainFrame::Get()->GetEventHandler()->AddPendingEvent(event);
}

bool PluginManager::IsToolBarShown() const
{
    if (clMainFrame::Get()->GetPluginsToolBar()) {
        // we have native toolbar
        return clMainFrame::Get()->GetPluginsToolBar()->IsShown();
    }
    return false;
}

bool PluginManager::CloseEditor(IEditor* editor, bool prompt)
{
    return clMainFrame::Get()->GetMainBook()->ClosePage(editor, prompt);
}

clEditorBar* PluginManager::GetNavigationBar() { return clMainFrame::Get()->GetMainBook()->GetEditorBar(); }

clToolBarGeneric* PluginManager::GetToolBar() { return clMainFrame::Get()->GetPluginsToolBar(); }

clInfoBar* PluginManager::GetInfoBar() { return clMainFrame::Get()->GetMessageBar(); }

void PluginManager::DisplayMessage(const wxString& message, int flags,
                                   const std::vector<std::pair<wxWindowID, wxString>>& buttons)
{
    return clMainFrame::Get()->GetMessageBar()->DisplayMessage(message, flags, buttons);
}

void PluginManager::GetBreakpoints(std::vector<clDebuggerBreakpoint>& bpList)
{
    ManagerST::Get()->GetBreakpointsMgr()->GetBreakpoints(bpList);
}

void PluginManager::ShowBuildMenu(clToolBar* toolbar, wxWindowID buttonId)
{
    clMainFrame::Get()->ShowBuildMenu(toolbar, buttonId);
}

wxMenuBar* PluginManager::GetMenuBar() { return clMainFrame::Get()->GetMainMenuBar(); }

void PluginManager::OpenFileAndAsyncExecute(const wxString& fileName, std::function<void(IEditor*)>&& func)
{
    clMainFrame::Get()->GetMainBook()->OpenFileAsync(fileName, std::move(func));
}

bool PluginManager::SelectEditor(IEditor* editor)
{
    // find editor and select it
    auto mainbook = clMainFrame::Get()->GetMainBook();
    return mainbook->SelectPage(editor->GetCtrl());
}

void PluginManager::BookAddPage(PaneId pane_id, wxWindow* page, const wxString& label, const wxString& bmpname)
{
    switch (pane_id) {
    case PaneId::BOTTOM_BAR:
        clMainFrame::Get()->GetOutputPane()->GetNotebook()->AddPage(page, label, true);
        break;
    case PaneId::SIDE_BAR:
        clMainFrame::Get()->GetWorkspacePane()->GetNotebook()->AddPage(page, label, bmpname, true);
        break;
    case PaneId::DEBUG_BAR:
        clMainFrame::Get()->GetDebuggerPane()->GetNotebook()->AddPage(page, label, true);
        break;
    case PaneId::SECONDARY_SIDE_BAR:
        clMainFrame::Get()->GetSecondarySideBar()->GetNotebook()->AddPage(page, label, bmpname, true);
        break;
    }
}

namespace
{
template <typename BookT>
wxWindow* find_page(BookT* book, const wxString& label)
{
    for (size_t i = 0; i < book->GetPageCount(); ++i) {
        if (book->GetPageText(i) == label) {
            return book->GetPage(i);
        }
    }
    return nullptr;
}

template <typename BookT>
bool find_page_label(BookT* book, wxWindow* page, wxString* label)
{
    for (size_t i = 0; i < book->GetPageCount(); ++i) {
        if (book->GetPage(i) == page) {
            *label = book->GetPageText(i);
            return true;
        }
    }
    return false;
}

template <typename BookT>
int find_page_index(BookT* book, wxWindow* page)
{
    for (size_t i = 0; i < book->GetPageCount(); ++i) {
        if (book->GetPage(i) == page) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

template <typename BookT>
int find_page_index(BookT* book, const wxString& label)
{
    for (size_t i = 0; i < book->GetPageCount(); ++i) {
        if (book->GetPageText(i) == label) {
            return i;
        }
    }
    return wxNOT_FOUND;
}
} // namespace

wxWindow* PluginManager::BookGetPage(PaneId pane_id, const wxString& label)
{
    switch (pane_id) {
    case PaneId::BOTTOM_BAR:
        return find_page(clMainFrame::Get()->GetOutputPane()->GetNotebook(), label);
    case PaneId::SIDE_BAR:
        return find_page(clMainFrame::Get()->GetWorkspacePane()->GetNotebook(), label);
    case PaneId::DEBUG_BAR:
        return find_page(clMainFrame::Get()->GetDebuggerPane()->GetNotebook(), label);
    case PaneId::SECONDARY_SIDE_BAR:
        return find_page(clMainFrame::Get()->GetSecondarySideBar()->GetNotebook(), label);
    }
    return nullptr;
}

wxWindow* PluginManager::BookRemovePage(PaneId pane_id, const wxString& label)
{
    switch (pane_id) {
    case PaneId::BOTTOM_BAR: {
        auto book = clMainFrame::Get()->GetOutputPane()->GetNotebook();
        int index = find_page_index(book, label);
        CHECK_COND_RET_NULL(index != wxNOT_FOUND);
        auto page = book->GetPage(index);
        book->RemovePage(index);
        page->Hide();
        return page;
    } break;
    case PaneId::SECONDARY_SIDE_BAR:
    case PaneId::SIDE_BAR: {
        auto book = pane_id == PaneId::SIDE_BAR ? clMainFrame::Get()->GetWorkspacePane()->GetNotebook()
                                                : clMainFrame::Get()->GetSecondarySideBar()->GetNotebook();
        int index = find_page_index(book, label);
        CHECK_COND_RET_NULL(index != wxNOT_FOUND);
        auto page = book->GetPage(index);
        book->RemovePage(index);
        page->Hide();
        return page;
    } break;
    case PaneId::DEBUG_BAR: {
        auto book = clMainFrame::Get()->GetDebuggerPane()->GetNotebook();
        int index = find_page_index(book, label);
        CHECK_COND_RET_NULL(index != wxNOT_FOUND);
        auto page = book->GetPage(index);
        book->RemovePage(index);
        page->Hide();
        return page;
    } break;
    }
    return nullptr;
}

wxWindow* PluginManager::BookRemovePage(PaneId pane_id, wxWindow* page)
{
    wxString label;
    switch (pane_id) {
    case PaneId::BOTTOM_BAR:
        find_page_label(clMainFrame::Get()->GetOutputPane()->GetNotebook(), page, &label);
        break;
    case PaneId::SIDE_BAR:
        find_page_label(clMainFrame::Get()->GetWorkspacePane()->GetNotebook(), page, &label);
        break;
    case PaneId::SECONDARY_SIDE_BAR:
        find_page_label(clMainFrame::Get()->GetSecondarySideBar()->GetNotebook(), page, &label);
        break;
    case PaneId::DEBUG_BAR:
        find_page_label(clMainFrame::Get()->GetDebuggerPane()->GetNotebook(), page, &label);
        break;
    }

    if (label.empty()) {
        return nullptr;
    }
    return BookRemovePage(pane_id, label);
}

wxWindow* PluginManager::BookGet(PaneId pane_id)
{
    switch (pane_id) {
    case PaneId::BOTTOM_BAR:
        return clMainFrame::Get()->GetOutputPane()->GetNotebook();
    case PaneId::SIDE_BAR:
        return clMainFrame::Get()->GetWorkspacePane()->GetNotebook();
    case PaneId::SECONDARY_SIDE_BAR:
        return clMainFrame::Get()->GetSecondarySideBar()->GetNotebook();
    case PaneId::DEBUG_BAR:
        return clMainFrame::Get()->GetDebuggerPane()->GetNotebook();
    }
    return nullptr;
}

bool PluginManager::BookDeletePage(PaneId pane_id, wxWindow* page)
{
    wxString label;
    switch (pane_id) {
    case PaneId::BOTTOM_BAR:
        find_page_label(clMainFrame::Get()->GetOutputPane()->GetNotebook(), page, &label);
        break;
    case PaneId::SIDE_BAR:
        find_page_label(clMainFrame::Get()->GetWorkspacePane()->GetNotebook(), page, &label);
        break;
    case PaneId::SECONDARY_SIDE_BAR:
        find_page_label(clMainFrame::Get()->GetSecondarySideBar()->GetNotebook(), page, &label);
        break;
    case PaneId::DEBUG_BAR:
        find_page_label(clMainFrame::Get()->GetDebuggerPane()->GetNotebook(), page, &label);
        break;
    }

    if (label.empty()) {
        return false;
    }
    return BookDeletePage(pane_id, label);
}

bool PluginManager::BookDeletePage(PaneId pane_id, const wxString& label)
{
    switch (pane_id) {
    case PaneId::BOTTOM_BAR: {
        auto book = clMainFrame::Get()->GetOutputPane()->GetNotebook();
        int index = find_page_index(book, label);
        CHECK_COND_RET_FALSE(index != wxNOT_FOUND);
        auto page = book->GetPage(index);
        book->DeletePage(index, false);
        return true;
    } break;
    case PaneId::SIDE_BAR:
    case PaneId::SECONDARY_SIDE_BAR: {
        auto book = pane_id == PaneId::SIDE_BAR ? clMainFrame::Get()->GetWorkspacePane()->GetNotebook()
                                                : clMainFrame::Get()->GetSecondarySideBar()->GetNotebook();
        int index = find_page_index(book, label);
        CHECK_COND_RET_FALSE(index != wxNOT_FOUND);
        auto page = book->GetPage(index);
        book->DeletePage(index);
        return true;
    } break;
    case PaneId::DEBUG_BAR: {
        auto book = clMainFrame::Get()->GetDebuggerPane()->GetNotebook();
        int index = find_page_index(book, label);
        CHECK_COND_RET_FALSE(index != wxNOT_FOUND);
        auto page = book->GetPage(index);
        book->DeletePage(index, false);
        return true;
    } break;
    }
    return false;
}

void PluginManager::BookSelectPage(PaneId pane_id, const wxString& label)
{
    int index = wxNOT_FOUND;
    switch (pane_id) {
    case PaneId::BOTTOM_BAR: {
        auto book = clMainFrame::Get()->GetOutputPane()->GetNotebook();
        index = find_page_index(book, label);
        CHECK_COND_RET(index != wxNOT_FOUND);
        book->SetSelection(index);
    } break;
    case PaneId::SIDE_BAR:
    case PaneId::SECONDARY_SIDE_BAR: {
        auto book = pane_id == PaneId::SIDE_BAR ? clMainFrame::Get()->GetWorkspacePane()->GetNotebook()
                                                : clMainFrame::Get()->GetSecondarySideBar()->GetNotebook();
        index = find_page_index(book, label);
        CHECK_COND_RET(index != wxNOT_FOUND);
        book->SetSelection(index);
    } break;
    case PaneId::DEBUG_BAR: {
        auto book = clMainFrame::Get()->GetDebuggerPane()->GetNotebook();
        index = find_page_index(book, label);
        CHECK_COND_RET(index != wxNOT_FOUND);
        book->SetSelection(index);
    } break;
    }
}

void PluginManager::BookSelectPage(PaneId pane_id, wxWindow* page)
{
    wxString label;
    switch (pane_id) {
    case PaneId::BOTTOM_BAR:
        find_page_label(clMainFrame::Get()->GetOutputPane()->GetNotebook(), page, &label);
        break;
    case PaneId::SIDE_BAR:
        find_page_label(clMainFrame::Get()->GetWorkspacePane()->GetNotebook(), page, &label);
        break;
    case PaneId::SECONDARY_SIDE_BAR:
        find_page_label(clMainFrame::Get()->GetSecondarySideBar()->GetNotebook(), page, &label);
        break;
    case PaneId::DEBUG_BAR:
        find_page_label(clMainFrame::Get()->GetDebuggerPane()->GetNotebook(), page, &label);
        break;
    }
    CHECK_COND_RET(!label.empty());
    BookSelectPage(pane_id, label);
}

wxPanel* PluginManager::GetMainPanel() { return clMainFrame::Get()->GetMainPanel(); }
