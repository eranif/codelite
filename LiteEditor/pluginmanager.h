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

    //------------------------------------
    // Implementation of IManager interface
    //------------------------------------
    virtual void EnableClangCodeCompletion(bool b);
    virtual IEditor* GetActiveEditor();
    virtual IConfigTool* GetConfigTool();
    virtual TreeItemInfo GetSelectedTreeItemInfo(TreeType type);
    virtual wxTreeCtrl* GetTree(TreeType type);
    virtual Notebook* GetOutputPaneNotebook();
    virtual Notebook* GetWorkspacePaneNotebook();
    virtual IEditor* OpenFile(
        const wxString& fileName, const wxString& projectName = wxEmptyString, int lineno = wxNOT_FOUND);
    virtual IEditor* OpenFile(const BrowseRecord& rec);
    virtual wxString GetStartupDirectory() const;
    virtual void AddProject(const wxString& path);
    virtual bool IsWorkspaceOpen() const;
    virtual TagsManager* GetTagsManager();
    virtual clCxxWorkspace* GetWorkspace();
    virtual bool AddFilesToVirtualFolder(wxTreeItemId& item, wxArrayString& paths);
    virtual bool AddFilesToVirtualFolder(const wxString& vdFullPath, wxArrayString& paths);
    virtual bool AddFilesToVirtualFolderIntelligently(const wxString& vdFullPath, wxArrayString& paths);
    virtual int GetToolbarIconSize();
    virtual wxAuiManager* GetDockingManager();
    virtual EnvironmentConfig* GetEnv();
    virtual JobQueue* GetJobQueue();
    virtual wxString GetProjectExecutionCommand(const wxString& projectName, wxString& wd);
    virtual wxApp* GetTheApp();
    virtual void ReloadWorkspace();
    virtual IPlugin* GetPlugin(const wxString& pluginName);
    virtual wxEvtHandler* GetOutputWindow();
    virtual bool SaveAll();
    virtual wxString GetInstallDirectory() const;
    virtual bool CreateVirtualDirectory(const wxString& parentPath, const wxString& vdName);
    virtual OptionsConfigPtr GetEditorSettings();
    virtual void FindAndSelect(const wxString& pattern, const wxString& name, int pos = 0);
    virtual TagEntryPtr GetTagAtCaret(bool scoped, bool impl);
    virtual bool AllowToolbar();
    virtual void SetStatusMessage(const wxString& msg, int seconds_to_live = wxID_ANY);
    virtual void PushQueueCommand(const QueueCommand& cmd);
    virtual void ProcessCommandQueue();
    virtual void StopAndClearQueue();
    virtual bool IsBuildInProgress() const;
    virtual bool IsBuildEndedSuccessfully() const;
    virtual wxString GetProjectNameByFile(const wxString& fullPathFileName);
    virtual BuildManager* GetBuildManager();
    virtual BuildSettingsConfig* GetBuildSettingsConfigManager();
    virtual bool ClosePage(const wxString& title);
    virtual bool ClosePage(const wxFileName& filename);
    virtual wxWindow* FindPage(const wxString& text);
    virtual bool AddPage(wxWindow* win,
        const wxString& text,
        const wxString& tooltip = wxEmptyString,
        const wxBitmap& bmp = wxNullBitmap,
        bool selected = false);
    virtual bool SelectPage(wxWindow* win);
    virtual NavMgr* GetNavigationMgr();
    virtual IEditor* NewEditor();
    virtual bool IsShutdownInProgress() const;
    virtual BitmapLoader* GetStdIcons();
    virtual wxArrayString GetProjectCompileFlags(const wxString& projectName, bool isCppFile);
    virtual void AddEditorPage(wxWindow* page, const wxString& name, const wxString& tooltip = wxEmptyString);
    virtual wxPanel* GetEditorPaneNotebook();
    virtual wxWindow* GetActivePage();
    virtual wxWindow* GetPage(size_t page);
    virtual size_t GetPageCount() const;
    virtual wxString GetPageTitle(wxWindow* win) const;
    virtual void SetPageTitle(wxWindow* win, const wxString& title);
    virtual ProjectPtr GetSelectedProject() const;
    virtual void RedefineProjFiles(ProjectPtr proj, const wxString& path, std::vector<wxString>& files);
    virtual IEditor* FindEditor(const wxString& filename) const;
    virtual size_t GetAllEditors(IEditor::List_t& editors, bool inOrder = false);
    virtual size_t GetAllTabs(clTab::Vec_t& tabs);
    virtual size_t GetAllBreakpoints(BreakpointInfo::Vec_t& breakpoints);
    virtual void DeleteAllBreakpoints();
    virtual void SetBreakpoints(const BreakpointInfo::Vec_t& breakpoints);
    virtual void LoadPerspective(const wxString& perspectiveName);
    virtual void SavePerspective(const wxString& perspectiveName);
    virtual void ProcessEditEvent(wxCommandEvent& e, IEditor* editor);
    virtual void AppendOutputTabText(eOutputPaneTab tab, const wxString& text);
    virtual void ClearOutputTab(eOutputPaneTab tab);
    virtual void AddWorkspaceToRecentlyUsedList(const wxFileName& filename);
    virtual void StoreWorkspaceSession(const wxFileName& workspaceFile);
    virtual void LoadWorkspaceSession(const wxFileName& workspaceFile);
    virtual void OpenFindInFileForPath(const wxString& path);
    virtual void OpenFindInFileForPaths(const wxArrayString& paths);
    virtual void ShowOutputPane(const wxString& selectedWindow = "");
    virtual void ToggleOutputPane(const wxString& selectedWindow = "");
    virtual clStatusBar* GetStatusBar();
    virtual clWorkspaceView* GetWorkspaceView();
    virtual bool IsToolBarShown() const;
    virtual void ShowToolBar(bool show = true);

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
