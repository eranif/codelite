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
#include "app.h"
#include <wx/toolbook.h>
#include "environmentconfig.h"
#include "macromanager.h"
#include "build_settings_config.h"
#include "buildmanager.h"
#include "jobqueue.h"
#include "pluginmanager.h"
#include "bitmap_loader.h"
#include <wx/tokenzr.h>
#include "optionsconfig.h"
#include "language.h"
#include "manager.h"
#include "wx/filename.h"
#include <wx/log.h>
#include <wx/dir.h>
#include "frame.h"
#include "generalinfo.h"
#include "editor_config.h"
#include "workspace_pane.h"
#include "fileview.h"
#include "wx/xrc/xmlres.h"
#include "ctags_manager.h"
#include "fileexplorer.h"
#include "plugin_version.h"
#include "workspacetab.h"
#include "file_logger.h"
#include "cl_config.h"
#include "FileExplorerTab.h"
#include "clang_code_completion.h"
#include "debugger.h"
#include "cl_standard_paths.h"
#include "new_build_tab.h"

PluginManager* PluginManager::Get()
{
    static PluginManager theManager;
    return &theManager;
}

void PluginManager::UnLoad()
{
    std::map<wxString, IPlugin*>::iterator plugIter = m_plugins.begin();
    for(; plugIter != m_plugins.end(); plugIter++) {
        IPlugin* plugin = plugIter->second;
        plugin->UnPlug();
        delete plugin;
    }

#if wxVERSION_NUMBER < 2900
    std::list<clDynamicLibrary*>::iterator iter = m_dl.begin();
    for(; iter != m_dl.end(); iter++) {
        (*iter)->Detach();
        delete (*iter);
    }
#endif

    m_dl.clear();
    m_plugins.clear();
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
    if(pp == CodeLiteApp::PP_None)
        return;

    else if(pp == CodeLiteApp::PP_FromList)
        allowedPlugins = app->GetAllowedPlugins();

    wxString pluginsDir = clStandardPaths::Get().GetPluginsDirectory();
    if(wxDir::Exists(pluginsDir)) {
        // get list of dlls
        wxArrayString files;
        wxDir::GetAllFiles(pluginsDir, &files, fileSpec, wxDIR_FILES);

        // Sort the plugins by A-Z
        std::sort(files.begin(), files.end());
        for(size_t i = 0; i < files.GetCount(); i++) {

            wxString fileName(files.Item(i));
#if defined(__WXMSW__) && !defined(NDEBUG)

            // Under MSW loading a release plugin while in debug mode will cause a crash
            if(!fileName.EndsWith("-dbg.dll")) {
                continue;
            }
#elif defined(__WXMSW__)

            // filter debug plugins
            if(fileName.EndsWith("-dbg.dll")) {
                continue;
            }
#endif

            clDynamicLibrary* dl = new clDynamicLibrary();
            if(!dl->Load(fileName)) {
                CL_ERROR(wxT("Failed to load plugin's dll: ") + fileName);
                if(!dl->GetError().IsEmpty()) {
                    CL_ERROR(dl->GetError());
                }
                continue;
            }

            bool success(false);
            GET_PLUGIN_INFO_FUNC pfnGetPluginInfo = (GET_PLUGIN_INFO_FUNC)dl->GetSymbol(wxT("GetPluginInfo"), &success);
            if(!success) {
                continue;
            }

            // load the plugin version method
            // if the methods does not exist, handle it as if it has value of 100 (lowest version API)
            int interface_version(100);
            GET_PLUGIN_INTERFACE_VERSION_FUNC pfnInterfaceVersion =
                (GET_PLUGIN_INTERFACE_VERSION_FUNC)dl->GetSymbol(wxT("GetPluginInterfaceVersion"), &success);
            if(success) {
                interface_version = pfnInterfaceVersion();
            } else {
                CL_WARNING(wxT("Failed to find GetPluginInterfaceVersion() in dll: ") + fileName);
                if(!dl->GetError().IsEmpty()) {
                    CL_WARNING(dl->GetError());
                }
            }

            if(interface_version != PLUGIN_INTERFACE_VERSION) {
                CL_WARNING(wxString::Format(wxT("Version interface mismatch error for plugin '%s'. Plugin's interface "
                                                "version is '%d', CodeLite interface version is '%d'"),
                                            fileName.c_str(),
                                            interface_version,
                                            PLUGIN_INTERFACE_VERSION));
                continue;
            }

            // Check if this dll can be loaded
            PluginInfo pluginInfo = pfnGetPluginInfo();

            wxString pname = pluginInfo.GetName();
            pname.MakeLower().Trim().Trim(false);

            // Check the policy
            if(pp == CodeLiteApp::PP_FromList && allowedPlugins.Index(pname) == wxNOT_FOUND) {
                // Policy is set to 'from list' and this plugin does not match any plugins from
                // the list, don't allow it to be loaded
                continue;
            }

            // Add the plugin information
            m_pluginsData.AddPlugin(pluginInfo);

            // Can we load it?
            if(!m_pluginsData.CanLoad(pluginInfo.GetName())) {
                CL_WARNING(wxT("Plugin ") + pluginInfo.GetName() + wxT(" is not enabled"));
                continue;
            }

            // try and load the plugin
            GET_PLUGIN_CREATE_FUNC pfn = (GET_PLUGIN_CREATE_FUNC)dl->GetSymbol(wxT("CreatePlugin"), &success);
            if(!success) {
                CL_WARNING(wxT("Failed to find CreatePlugin() in dll: ") + fileName);
                if(!dl->GetError().IsEmpty()) {
                    CL_WARNING(dl->GetError());
                }

                m_pluginsData.DisablePlugin(pluginInfo.GetName());
                continue;
            }

            // Construct the plugin
            IPlugin* plugin = pfn((IManager*)this);
            CL_DEBUG(wxT("Loaded plugin: ") + plugin->GetLongName());
            m_plugins[plugin->GetShortName()] = plugin;

            // Load the toolbar
            clToolBar* tb = plugin->CreateToolBar((wxWindow*)clMainFrame::Get());
            if(tb) {
#if USE_AUI_TOOLBAR
                // When using AUI toolbars, use our own custom art-provider
                tb->SetArtProvider(new CLMainAuiTBArt());
#endif
                clMainFrame::Get()->GetDockingManager().AddPane(tb,
                                                                wxAuiPaneInfo()
                                                                    .Name(plugin->GetShortName())
                                                                    .LeftDockable(true)
                                                                    .RightDockable(true)
                                                                    .Caption(plugin->GetShortName())
                                                                    .ToolbarPane()
                                                                    .Top()
                                                                    .Row(0));

                // Add menu entry at the 'View->Toolbars' menu for this toolbar
                wxMenuItem* item = clMainFrame::Get()->GetMenuBar()->FindItem(XRCID("toolbars_menu"));
                if(item) {
                    wxMenu* submenu = NULL;
                    submenu = item->GetSubMenu();
                    // add the new toolbar entry at the end of this menu

                    int id = wxNewId();
                    wxString text(plugin->GetShortName());
                    text << _(" ToolBar");
                    wxMenuItem* newItem = new wxMenuItem(submenu, id, text, wxEmptyString, wxITEM_CHECK);
                    submenu->Append(newItem);
                    clMainFrame::Get()->RegisterToolbar(id, plugin->GetShortName());
                }
            }

            // Let the plugin plug its menu in the 'Plugins' menu at the menu bar
            // the create menu will be placed as a sub menu of the 'Plugin' menu
            wxMenu* pluginsMenu = NULL;
            wxMenuItem* menuitem = clMainFrame::Get()->GetMenuBar()->FindItem(XRCID("manage_plugins"), &pluginsMenu);
            if(menuitem && pluginsMenu) {
                plugin->CreatePluginMenu(pluginsMenu);
            }

            // Keep the dynamic load library
            m_dl.push_back(dl);
        }
        clMainFrame::Get()->GetDockingManager().Update();

        // save the plugins data
        conf.WriteItem(&m_pluginsData);
    }
}

IEditor* PluginManager::GetActiveEditor() { return (IEditor*)clMainFrame::Get()->GetMainBook()->GetActiveEditor(true); }

IConfigTool* PluginManager::GetConfigTool() { return EditorConfigST::Get(); }

void PluginManager::HookPopupMenu(wxMenu* menu, MenuType type)
{
    // Hook each menu once!
    std::set<MenuType>::iterator it = m_menusToBeHooked.find(type);
    if(it != m_menusToBeHooked.end()) {
        // Call 'HookPopupMenu'
        std::map<wxString, IPlugin*>::iterator iter = m_plugins.begin();
        for(; iter != m_plugins.end(); iter++) {
            iter->second->HookPopupMenu(menu, type);
        }

        // remove this menu entry from the set to avoid
        // further hooking
        if(type != MenuTypeEditor) {

            // Dont remove the MenuTypeEditor from the set, since there can be multiple
            // instances of that menu (each editor holds a copy of the menu)
            // LEditor itself will make sure to call this method only once
            m_menusToBeHooked.erase(it);
        }
    }
}

TreeItemInfo PluginManager::GetSelectedTreeItemInfo(TreeType type)
{
    TreeItemInfo info;
    switch(type) {
    case TreeFileExplorer:
        return clMainFrame::Get()->GetFileExplorer()->GetFileTree()->GetSelectedItemInfo();
    case TreeFileView:
        return clMainFrame::Get()->GetWorkspaceTab()->GetFileView()->GetSelectedItemInfo();
    default:
        return info;
    }
}

wxTreeCtrl* PluginManager::GetTree(TreeType type)
{
    switch(type) {
    case TreeFileExplorer:
        return clMainFrame::Get()->GetFileExplorer()->GetFileTree()->Tree()->GetTreeCtrl();
    case TreeFileView:
        return clMainFrame::Get()->GetWorkspaceTab()->GetFileView();
    default:
        return NULL;
    }
}

Notebook* PluginManager::GetOutputPaneNotebook() { return clMainFrame::Get()->GetOutputPane()->GetNotebook(); }

Notebook* PluginManager::GetWorkspacePaneNotebook() { return clMainFrame::Get()->GetWorkspacePane()->GetNotebook(); }

bool PluginManager::OpenFile(const wxString& fileName, const wxString& projectName, int lineno)
{
    if(clMainFrame::Get()->GetMainBook()->OpenFile(fileName, projectName, lineno)) {
        LEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
        if(editor) {
            editor->SetActive();
        }
        return true;
    }
    return false;
}

wxString PluginManager::GetStartupDirectory() const { return ManagerST::Get()->GetStartupDirectory(); }

void PluginManager::AddProject(const wxString& path) { ManagerST::Get()->AddProject(path); }

bool PluginManager::IsWorkspaceOpen() const { return ManagerST::Get()->IsWorkspaceOpen(); }

TagsManager* PluginManager::GetTagsManager() { return TagsManagerST::Get(); }

Workspace* PluginManager::GetWorkspace() { return WorkspaceST::Get(); }

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
    if(options) {
        return options->GetIconsSize();
    }
    return 24;
}

wxAuiManager* PluginManager::GetDockingManager() { return &clMainFrame::Get()->GetDockingManager(); }

EnvironmentConfig* PluginManager::GetEnv() { return EnvironmentConfig::Instance(); }

JobQueue* PluginManager::GetJobQueue() { return JobQueueSingleton::Instance(); }
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
    if(iter != m_plugins.end()) {
        return iter->second;
    }
    return NULL;
}

wxEvtHandler* PluginManager::GetOutputWindow() { return clMainFrame::Get()->GetOutputPane()->GetOutputWindow(); }

bool PluginManager::SaveAll() { return clMainFrame::Get()->GetMainBook()->SaveAll(true, false); }

wxString PluginManager::GetInstallDirectory() const { return ManagerST::Get()->GetInstallDir(); }

IKeyboard* PluginManager::GetKeyboardManager() { return &m_keyboardMgr; }

bool PluginManager::CreateVirtualDirectory(const wxString& parentPath, const wxString& vdName)
{
    return clMainFrame::Get()->GetWorkspaceTab()->GetFileView()->CreateVirtualDirectory(parentPath, vdName);
}

OptionsConfigPtr PluginManager::GetEditorSettings()
{
    // First try to use LEditor::GetOptions, as it takes account of local preferences
    LEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    if(editor) {
        return editor->GetOptions();
    }
    // Failing that...
    return EditorConfigST::Get()->GetOptions();
}

void PluginManager::FindAndSelect(const wxString& pattern, const wxString& name, int pos)
{
    LEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    if(editor) {
        editor->FindAndSelectV(pattern, name, pos, NavMgr::Get());
        editor->SetActive();
    }
}

TagEntryPtr PluginManager::GetTagAtCaret(bool scoped, bool impl)
{
    LEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    if(!editor)
        return NULL;
    return editor->GetContext()->GetTagAtCaret(scoped, impl);
}

bool PluginManager::AllowToolbar()
{
    long v;
    if(EditorConfigST::Get()->GetLongValue(wxT("UseSingleToolbar"), v)) {
        return v ? false : true;
    } else {
// entry does not exist
#ifdef __WXMAC__
        return false;
#else
        return true;
#endif
    }
}

void PluginManager::EnableToolbars()
{
    // In case, plugins are now allowed to insert toolbars, disable the toolbars_menu item
    if(AllowToolbar() == false) {
        int ii = clMainFrame::Get()->GetMenuBar()->FindMenu(wxT("View"));
        if(ii != wxNOT_FOUND) {
            wxMenu* viewMenu = clMainFrame::Get()->GetMenuBar()->GetMenu(ii);
            wxMenuItem* item = viewMenu->FindItem(XRCID("toolbars_menu"));
            if(item) {
                item->Enable(false);
            }
        }
    }
}

void PluginManager::SetStatusMessage(const wxString& msg, int col, int seconds_to_live /*=wxID_ANY*/)
{
    clMainFrame::Get()->SetStatusMessage(msg, col, seconds_to_live);
}

void PluginManager::ProcessCommandQueue() { ManagerST::Get()->ProcessCommandQueue(); }

void PluginManager::PushQueueCommand(const QueueCommand& cmd) { ManagerST::Get()->PushQueueCommand(cmd); }

void PluginManager::StopAndClearQueue() { ManagerST::Get()->StopBuild(); }

bool PluginManager::IsBuildInProgress() const { return ManagerST::Get()->IsBuildInProgress(); }

bool PluginManager::IsBuildEndedSuccessfully() const { return ManagerST::Get()->IsBuildEndedSuccessfully(); }

wxString PluginManager::GetProjectNameByFile(const wxString& fullPathFileName)
{
    return ManagerST::Get()->GetProjectNameByFile(fullPathFileName);
}

BuildManager* PluginManager::GetBuildManager() { return BuildManagerST::Get(); }

BuildSettingsConfig* PluginManager::GetBuildSettingsConfigManager() { return BuildSettingsConfigST::Get(); }

bool PluginManager::ClosePage(const wxString& text) { return clMainFrame::Get()->GetMainBook()->ClosePage(text); }

wxWindow* PluginManager::FindPage(const wxString& text) { return clMainFrame::Get()->GetMainBook()->FindPage(text); }

bool PluginManager::AddPage(wxWindow* win, const wxString& text, const wxBitmap& bmp, bool selected)
{
    return clMainFrame::Get()->GetMainBook()->AddPage(win, text, bmp, selected);
}

bool PluginManager::SelectPage(wxWindow* win) { return clMainFrame::Get()->GetMainBook()->SelectPage(win); }

bool PluginManager::OpenFile(const BrowseRecord& rec) { return clMainFrame::Get()->GetMainBook()->OpenFile(rec); }

NavMgr* PluginManager::GetNavigationMgr() { return NavMgr::Get(); }

void
    PluginManager::HookProjectSettingsTab(wxBookCtrlBase* book, const wxString& projectName, const wxString& configName)
{
    std::map<wxString, IPlugin*>::iterator iter = m_plugins.begin();
    for(; iter != m_plugins.end(); iter++) {
        iter->second->HookProjectSettingsTab(book, projectName, configName);
    }
}

void PluginManager::UnHookProjectSettingsTab(wxBookCtrlBase* book,
                                             const wxString& projectName,
                                             const wxString& configName)
{
    std::map<wxString, IPlugin*>::iterator iter = m_plugins.begin();
    for(; iter != m_plugins.end(); iter++) {
        iter->second->UnHookProjectSettingsTab(book, projectName, configName);
    }
}

IEditor* PluginManager::NewEditor() { return clMainFrame::Get()->GetMainBook()->NewEditor(); }

bool PluginManager::IsShutdownInProgress() const { return ManagerST::Get()->IsShutdownInProgress(); }

BitmapLoader* PluginManager::GetStdIcons()
{
    if(!m_bmpLoader) {
        m_bmpLoader = new BitmapLoader();
    }
    return m_bmpLoader;
}

wxArrayString PluginManager::GetProjectCompileFlags(const wxString& projectName, bool isCppFile)
{
    if(IsWorkspaceOpen() == false)
        return wxArrayString();

    wxArrayString args;

    // Next apppend the user include paths
    wxString errMsg;

    // First, we need to find the currently active workspace configuration
    BuildMatrixPtr matrix = GetWorkspace()->GetBuildMatrix();
    if(!matrix) {
        return wxArrayString();
    }

    wxString workspaceSelConf = matrix->GetSelectedConfigurationName();

    // Now that we got the selected workspace configuration, extract the related project configuration
    ProjectPtr proj = GetWorkspace()->FindProjectByName(projectName, errMsg);
    if(!proj) {
        return args;
    }

    wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, proj->GetName());
    BuildConfigPtr dependProjbldConf = GetWorkspace()->GetProjBuildConf(proj->GetName(), projectSelConf);
    if(dependProjbldConf && dependProjbldConf->IsCustomBuild() == false) {
        // Get the include paths and add them
        wxString projectIncludePaths = dependProjbldConf->GetIncludePath();
        wxArrayString projectIncludePathsArr = wxStringTokenize(projectIncludePaths, wxT(";"), wxTOKEN_STRTOK);
        for(size_t i = 0; i < projectIncludePathsArr.GetCount(); i++) {
            args.Add(wxString::Format(wxT("-I%s"), projectIncludePathsArr[i].c_str()));
        }
        // get the compiler options and add them
        wxString projectCompileOptions = dependProjbldConf->GetCompileOptions();
        wxArrayString projectCompileOptionsArr = wxStringTokenize(projectCompileOptions, wxT(";"), wxTOKEN_STRTOK);
        for(size_t i = 0; i < projectCompileOptionsArr.GetCount(); i++) {
            wxString cmpOption(projectCompileOptionsArr.Item(i));
            cmpOption.Trim().Trim(false);
            wxString tmp;
            // Expand backticks / $(shell ...) syntax supported by codelite
            if(cmpOption.StartsWith(wxT("$(shell "), &tmp) || cmpOption.StartsWith(wxT("`"), &tmp)) {
                cmpOption = tmp;
                tmp.Clear();
                if(cmpOption.EndsWith(wxT(")"), &tmp) || cmpOption.EndsWith(wxT("`"), &tmp)) {
                    cmpOption = tmp;
                }
                if(m_backticks.find(cmpOption) == m_backticks.end()) {
                    // Expand the backticks into their value
                    wxArrayString outArr;
                    // Apply the environment before executing the command
                    EnvSetter setter(EnvironmentConfig::Instance(), NULL, projectName);
                    ProcUtils::SafeExecuteCommand(cmpOption, outArr);
                    wxString expandedValue;
                    for(size_t j = 0; j < outArr.size(); j++) {
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
        for(size_t i = 0; i < projectPrepsArr.GetCount(); i++) {
            args.Add(wxString::Format(wxT("-D%s"), projectPrepsArr[i].c_str()));
        }
    }
    return args;
}

void PluginManager::AddEditorPage(wxWindow* page, const wxString& name)
{
    clMainFrame::Get()->GetMainBook()->AddPage(page, name, wxNullBitmap, true);
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

void PluginManager::EnableClangCodeCompletion(bool b)
{
#if HAS_LIBCLANG
    ClangCodeCompletion::Instance()->ClearCache();
    TagsOptionsData& options = clMainFrame::Get()->GetTagsOptions();
    size_t clang_flags = options.GetClangOptions();

    if(b) {
        clang_flags |= CC_CLANG_ENABLED;
    } else {
        clang_flags &= ~CC_CLANG_ENABLED;
    }

    options.SetClangOptions(clang_flags);
    TagsManagerST::Get()->SetCtagsOptions(options);

#else
    wxUnusedVar(b);
#endif
}

size_t PluginManager::GetPageCount() const { return clMainFrame::Get()->GetMainBook()->GetPageCount(); }

size_t PluginManager::GetAllEditors(IEditor::List_t& editors, bool inOrder)
{
    LEditor::Vec_t tmpEditors;
    size_t flags = MainBook::kGetAll_IncludeDetached;
    if(inOrder) {
        flags |= MainBook::kGetAll_RetainOrder;
    }

    clMainFrame::Get()->GetMainBook()->GetAllEditors(tmpEditors, flags);
    editors.insert(editors.end(), tmpEditors.begin(), tmpEditors.end());
    return editors.size();
}

size_t PluginManager::GetAllBreakpoints(BreakpointInfo::Vec_t& breakpoints)
{
    breakpoints.clear();
    ManagerST::Get()->GetBreakpointsMgr()->GetBreakpoints(breakpoints);
    return breakpoints.size();
}

void PluginManager::DeleteAllBreakpoints() { ManagerST::Get()->GetBreakpointsMgr()->DelAllBreakpoints(); }

void PluginManager::SetBreakpoints(const BreakpointInfo::Vec_t& breakpoints)
{
    ManagerST::Get()->GetBreakpointsMgr()->DelAllBreakpoints();
    for(size_t i = 0; i < breakpoints.size(); ++i) {
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
    LEditor* lEditor = dynamic_cast<LEditor*>(editor);
    if(lEditor) {
        lEditor->OnMenuCommand(e);
    }
}

void PluginManager::AppendOutputTabText(eOutputPaneTab tab, const wxString& text)
{
    switch(tab) {
    case kOutputTab_Build:
        clMainFrame::Get()->GetOutputPane()->GetBuildTab()->AppendLine(text);
        break;
    case kOutputTab_Output:
        clMainFrame::Get()->GetOutputPane()->GetOutputWindow()->AppendText(text);
        break;
    }
}

void PluginManager::ClearOutputTab(eOutputPaneTab tab)
{
    switch(tab) {
    case kOutputTab_Build:
        clMainFrame::Get()->GetOutputPane()->GetBuildTab()->Clear();
        break;
    case kOutputTab_Output:
        clMainFrame::Get()->GetOutputPane()->GetOutputWindow()->Clear();
        break;
    }
}
