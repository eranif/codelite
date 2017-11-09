//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : pluginmanager.h
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
#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "plugin.h"
#include "map"
#include "list"
#include "vector"
#include "wx/string.h"
#include "wx/treectrl.h"
#include "dynamiclibrary.h"
#include "plugindata.h"
#include "project.h"
#include <set>
#include <map>
#include "plugindata.h"

class clEditorBar;
class wxBookCtrlBase;
class EnvironmentConfig;
class JobQueue;
class BuildSettingsConfig;
class BuildManager;
class BitmapLoader;
class clWorkspaceView;

class PluginManager : public IManager
{
    std::map<wxString, IPlugin*> m_plugins;
    std::list<clDynamicLibrary*> m_dl;
    PluginInfoArray m_pluginsData;
    BitmapLoader* m_bmpLoader;
    std::set<MenuType> m_menusToBeHooked;
    std::map<wxString, wxString> m_backticks;
    wxAuiManager* m_dockingManager;

private:
    PluginManager();
    virtual ~PluginManager();

public:
    static PluginManager* Get();

    virtual void Load();
    virtual void UnLoad();
    virtual void EnableToolbars();

    /**
     * \brief return a map of all loaded plugins
     */
    const PluginInfoArray& GetPluginsInfo() const { return m_pluginsData; }
    void SetDockingManager(wxAuiManager* dockingManager) { this->m_dockingManager = dockingManager; }

    //------------------------------------
    // Implementation of IManager interface
    //------------------------------------
    void EnableClangCodeCompletion(bool b);
    IEditor* GetActiveEditor();
    IConfigTool* GetConfigTool();
    TreeItemInfo GetSelectedTreeItemInfo(TreeType type);
    wxTreeCtrl* GetTree(TreeType type);
    Notebook* GetOutputPaneNotebook();
    Notebook* GetWorkspacePaneNotebook();
    IEditor* OpenFile(const wxString& fileName, const wxString& projectName = wxEmptyString, int lineno = wxNOT_FOUND);
    IEditor* OpenFile(
        const wxString& fileName, const wxBitmap& bmp, const wxString& tooltip = wxEmptyString);
    IEditor* OpenFile(const BrowseRecord& rec);
    wxString GetStartupDirectory() const;
    void AddProject(const wxString& path);
    bool IsWorkspaceOpen() const;
    TagsManager* GetTagsManager();
    clCxxWorkspace* GetWorkspace();
    bool AddFilesToVirtualFolder(wxTreeItemId& item, wxArrayString& paths);
    bool AddFilesToVirtualFolder(const wxString& vdFullPath, wxArrayString& paths);
    bool AddFilesToVirtualFolderIntelligently(const wxString& vdFullPath, wxArrayString& paths);
    int GetToolbarIconSize();
    wxAuiManager* GetDockingManager();
    EnvironmentConfig* GetEnv();
    JobQueue* GetJobQueue();
    wxString GetProjectExecutionCommand(const wxString& projectName, wxString& wd);
    wxApp* GetTheApp();
    void ReloadWorkspace();
    IPlugin* GetPlugin(const wxString& pluginName);
    wxEvtHandler* GetOutputWindow();
    bool SaveAll();
    wxString GetInstallDirectory() const;
    bool CreateVirtualDirectory(const wxString& parentPath, const wxString& vdName);
    OptionsConfigPtr GetEditorSettings();
    void FindAndSelect(const wxString& pattern, const wxString& name, int pos = 0);
    TagEntryPtr GetTagAtCaret(bool scoped, bool impl);
    bool AllowToolbar();
    void SetStatusMessage(const wxString& msg, int seconds_to_live = wxID_ANY);
    void PushQueueCommand(const QueueCommand& cmd);
    void ProcessCommandQueue();
    void StopAndClearQueue();
    bool IsBuildInProgress() const;
    bool IsBuildEndedSuccessfully() const;
    wxString GetProjectNameByFile(const wxString& fullPathFileName);
    BuildManager* GetBuildManager();
    BuildSettingsConfig* GetBuildSettingsConfigManager();
    bool ClosePage(const wxString& title);
    bool ClosePage(const wxFileName& filename);
    wxWindow* FindPage(const wxString& text);
    bool AddPage(wxWindow* win, const wxString& text, const wxString& tooltip = wxEmptyString,
        const wxBitmap& bmp = wxNullBitmap, bool selected = false);
    bool SelectPage(wxWindow* win);
    NavMgr* GetNavigationMgr();
    IEditor* NewEditor();
    bool CloseEditor(IEditor* editor, bool prompt = true);
    bool IsShutdownInProgress() const;
    BitmapLoader* GetStdIcons();
    wxArrayString GetProjectCompileFlags(const wxString& projectName, bool isCppFile);
    void AddEditorPage(wxWindow* page, const wxString& name, const wxString& tooltip = wxEmptyString);
    wxPanel* GetEditorPaneNotebook();
    wxWindow* GetActivePage();
    wxWindow* GetPage(size_t page);
    size_t GetPageCount() const;
    wxString GetPageTitle(wxWindow* win) const;
    void SetPageTitle(wxWindow* win, const wxString& title);
    ProjectPtr GetSelectedProject() const;
    void RedefineProjFiles(ProjectPtr proj, const wxString& path, std::vector<wxString>& files);
    IEditor* FindEditor(const wxString& filename) const;
    size_t GetAllEditors(IEditor::List_t& editors, bool inOrder = false);
    size_t GetAllTabs(clTab::Vec_t& tabs);
    size_t GetAllBreakpoints(BreakpointInfo::Vec_t& breakpoints);
    void DeleteAllBreakpoints();
    void SetBreakpoints(const BreakpointInfo::Vec_t& breakpoints);
    void LoadPerspective(const wxString& perspectiveName);
    void SavePerspective(const wxString& perspectiveName);
    void ProcessEditEvent(wxCommandEvent& e, IEditor* editor);
    void AppendOutputTabText(eOutputPaneTab tab, const wxString& text);
    void ClearOutputTab(eOutputPaneTab tab);
    void AddWorkspaceToRecentlyUsedList(const wxFileName& filename);
    void StoreWorkspaceSession(const wxFileName& workspaceFile);
    void LoadWorkspaceSession(const wxFileName& workspaceFile);
    void OpenFindInFileForPath(const wxString& path);
    void OpenFindInFileForPaths(const wxArrayString& paths);
    void ShowOutputPane(const wxString& selectedWindow = "");
    void ToggleOutputPane(const wxString& selectedWindow = "");
    clStatusBar* GetStatusBar();
    clEditorBar* GetNavigationBar();
    clWorkspaceView* GetWorkspaceView();
    bool IsToolBarShown() const;
    void ShowToolBar(bool show = true);
    //------------------------------------
    // End of IManager interface
    //------------------------------------

    // (Un)Hook the contect menus
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);

    // (Un)Hook the project settings tab
    virtual void HookProjectSettingsTab(wxBookCtrlBase* book, const wxString& projectName, const wxString& configName);
    virtual void UnHookProjectSettingsTab(
        wxBookCtrlBase* book, const wxString& projectName, const wxString& configName);
};

#endif // PLUGINMANAGER_H
