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

#include "clToolBar.h"
#include "debugger.h"
#include "dynamiclibrary.h"
#include "plugin.h"
#include "plugindata.h"
#include "project.h"

#include <list>
#include <map>
#include <set>
#include <vector>
#include <wx/string.h>
#include <wx/treectrl.h>

class clEditorBar;
class wxBookCtrlBase;
class EnvironmentConfig;
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
    PluginInfo::PluginMap_t m_installedPlugins;

private:
    PluginManager();
    virtual ~PluginManager();

public:
    static PluginManager* Get();

    virtual void Load();
    virtual void UnLoad();
    virtual void EnableToolbars();

    void SetInstalledPlugins(const PluginInfo::PluginMap_t& installedPlugins)
    {
        this->m_installedPlugins = installedPlugins;
    }
    const PluginInfo::PluginMap_t& GetInstalledPlugins() const { return m_installedPlugins; }
    /**
     * \brief return a map of all loaded plugins
     */
    const PluginInfoArray& GetPluginsInfo() const { return m_pluginsData; }
    void SetDockingManager(wxAuiManager* dockingManager) { this->m_dockingManager = dockingManager; }

    //------------------------------------
    // Implementation of IManager interface
    //------------------------------------
    void EnableClangCodeCompletion(bool b) override;
    IEditor* GetActiveEditor() override;
    clToolBarGeneric* GetToolBar() override;
    wxMenuBar* GetMenuBar() override;
    IConfigTool* GetConfigTool() override;
    TreeItemInfo GetSelectedTreeItemInfo(TreeType type) override;
    clTreeCtrl* GetFileExplorerTree() override;
    clTreeCtrl* GetWorkspaceTree() override;
    Notebook* GetOutputPaneNotebook() override;
    Notebook* GetWorkspacePaneNotebook() override;
    Notebook* GetMainNotebook() override;
    IEditor* OpenFile(const wxString& fileName, const wxString& projectName = wxEmptyString, int lineno = wxNOT_FOUND,
                      OF_extra flags = OF_AddJump) override;
    IEditor* OpenFile(const wxString& fileName, const wxString& bmpResourceName,
                      const wxString& tooltip = wxEmptyString) override;
    IEditor* OpenFile(const BrowseRecord& rec) override;
    wxString GetStartupDirectory() const override;
    void AddProject(const wxString& path) override;
    bool IsWorkspaceOpen() const override;
    TagsManager* GetTagsManager() override;
    clCxxWorkspace* GetWorkspace() override;
    bool AddFilesToVirtualFolder(wxTreeItemId& item, wxArrayString& paths) override;
    bool AddFilesToVirtualFolder(const wxString& vdFullPath, wxArrayString& paths) override;
    bool AddFilesToVirtualFolderIntelligently(const wxString& vdFullPath, wxArrayString& paths) override;
    int GetToolbarIconSize() override;
    wxAuiManager* GetDockingManager() override;
    EnvironmentConfig* GetEnv() override;
    wxString GetProjectExecutionCommand(const wxString& projectName, wxString& wd) override;
    wxApp* GetTheApp() override;
    void ReloadWorkspace() override;
    IPlugin* GetPlugin(const wxString& pluginName) override;
    wxEvtHandler* GetOutputWindow() override;
    bool SaveAll(bool prompt = true) override;
    wxString GetInstallDirectory() const override;
    bool CreateVirtualDirectory(const wxString& parentPath, const wxString& vdName) override;
    OptionsConfigPtr GetEditorSettings() override;
    void FindAndSelect(const wxString& pattern, const wxString& name, int pos = 0) override;
    TagEntryPtr GetTagAtCaret(bool scoped, bool impl) override;
    bool AllowToolbar() override;
    void SetStatusMessage(const wxString& msg, int seconds_to_live = wxID_ANY) override;
    void PushQueueCommand(const QueueCommand& cmd) override;
    void ProcessCommandQueue() override;
    void StopAndClearQueue() override;
    bool IsBuildInProgress() const override;
    bool IsBuildEndedSuccessfully() const override;
    wxString GetProjectNameByFile(wxString& fullPathFileName) override;
    wxString GetProjectNameByFile(const wxString& fullPathFileName) override;
    BuildManager* GetBuildManager() override;
    BuildSettingsConfig* GetBuildSettingsConfigManager() override;
    bool ClosePage(const wxString& title) override;
    bool ClosePage(const wxFileName& filename) override;
    wxWindow* FindPage(const wxString& text) override;
    bool AddPage(wxWindow* win, const wxString& text, const wxString& tooltip = wxEmptyString,
                 const wxString& bmpResourceName = wxEmptyString, bool selected = false) override;
    bool SelectPage(wxWindow* win) override;
    NavMgr* GetNavigationMgr() override;
    IEditor* NewEditor() override;
    bool CloseEditor(IEditor* editor, bool prompt = true) override;
    bool IsShutdownInProgress() const override;
    BitmapLoader* GetStdIcons() override;
    wxArrayString GetProjectCompileFlags(const wxString& projectName, bool isCppFile) override;
    void AddEditorPage(wxWindow* page, const wxString& name, const wxString& tooltip = wxEmptyString) override;
    wxPanel* GetEditorPaneNotebook() override;
    wxWindow* GetActivePage() override;
    wxWindow* GetPage(size_t page) override;
    size_t GetPageCount() const override;
    wxString GetPageTitle(wxWindow* win) const override;
    void SetPageTitle(wxWindow* win, const wxString& title) override;
    ProjectPtr GetSelectedProject() const override;
    void RedefineProjFiles(ProjectPtr proj, const wxString& path, std::vector<wxString>& files) override;
    IEditor* FindEditor(const wxString& filename) const override;
    size_t GetAllEditors(IEditor::List_t& editors, bool inOrder = false) override;
    size_t GetAllTabs(clTab::Vec_t& tabs) override;
    size_t GetAllBreakpoints(clDebuggerBreakpoint::Vec_t& breakpoints) override;
    clDebuggerBreakpoint CreateBreakpoint(const wxString& filepath, int line_number) override;
    void DeleteAllBreakpoints() override;
    void SetBreakpoints(const clDebuggerBreakpoint::Vec_t& breakpoints) override;
    void LoadPerspective(const wxString& perspectiveName) override;
    void SavePerspective(const wxString& perspectiveName) override;
    void ProcessEditEvent(wxCommandEvent& e, IEditor* editor) override;
    void AppendOutputTabText(eOutputPaneTab tab, const wxString& text) override;
    void ClearOutputTab(eOutputPaneTab tab) override;
    void AddWorkspaceToRecentlyUsedList(const wxFileName& filename) override;
    void StoreWorkspaceSession(const wxFileName& workspaceFile) override;
    void LoadWorkspaceSession(const wxFileName& workspaceFile) override;
    void OpenFindInFileForPath(const wxString& path) override;
    void OpenFindInFileForPaths(const wxArrayString& paths) override;
    void ShowOutputPane(const wxString& selectedWindow = "") override;
    void ToggleOutputPane(const wxString& selectedWindow = "") override;
    clStatusBar* GetStatusBar() override;
    clEditorBar* GetNavigationBar() override;
    clWorkspaceView* GetWorkspaceView() override;
    bool IsToolBarShown() const override;
    void ShowToolBar(bool show = true) override;
    void ShowBuildMenu(clToolBar* toolbar, wxWindowID buttonId) override;
    void OpenFileAndAsyncExecute(const wxString& fileName, std::function<void(IEditor*)>&& func) override;
    /**
     * @brief return list of all breakpoints
     */
    void GetBreakpoints(std::vector<clDebuggerBreakpoint>& bpList) override;

    /**
     * @brief display message to the user using the info bar
     */
    void DisplayMessage(const wxString& message, int flags = wxICON_INFORMATION,
                        const std::vector<std::pair<wxWindowID, wxString>>& buttons = {}) override;

    //------------------------------------
    // End of IManager interface
    //------------------------------------

    // (Un)Hook the contect menus
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);

    // (Un)Hook the project settings tab
    virtual void HookProjectSettingsTab(wxBookCtrlBase* book, const wxString& projectName, const wxString& configName);
    virtual void UnHookProjectSettingsTab(wxBookCtrlBase* book, const wxString& projectName,
                                          const wxString& configName);
};

#endif // PLUGINMANAGER_H
