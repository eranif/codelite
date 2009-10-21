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
#include "keyboardmanager.h"

class Notebook;
class EnvironmentConfig;
class JobQueue;
class BuildSettingsConfig;
class BuildManager;

class PluginManager : public IManager
{
	std::map<wxString, IPlugin*> m_plugins;
	std::list< clDynamicLibrary* > m_dl;
	std::map<wxString, PluginInfo> m_pluginsInfo;
	KeyboardManager m_keyboardMgr;

private:
	PluginManager() {}
	virtual ~PluginManager();

public:

	static PluginManager *Get();

	virtual void Load();
	virtual void UnLoad();
	virtual void EnableToolbars();

	/**
	 * \brief return a map of all loaded plugins
	 */
	const std::map<wxString, PluginInfo>& GetPluginsInfo() const {
		return m_pluginsInfo;
	}

	//------------------------------------
	//Implementation of IManager interface
	//------------------------------------

	virtual IEditor *              GetActiveEditor();
	virtual IConfigTool *          GetConfigTool();
	virtual TreeItemInfo           GetSelectedTreeItemInfo(TreeType type);
	virtual wxTreeCtrl *           GetTree(TreeType type);
	virtual Notebook *             GetOutputPaneNotebook();
    virtual Notebook *             GetWorkspacePaneNotebook();
	virtual bool                   OpenFile(const wxString &fileName, const wxString &projectName = wxEmptyString, int lineno = wxNOT_FOUND);
	virtual bool                   OpenFile(const BrowseRecord &rec);
	virtual wxString               GetStartupDirectory() const;
	virtual void                   AddProject(const wxString & path);
	virtual bool                   IsWorkspaceOpen() const;
	virtual TagsManager *          GetTagsManager();
	virtual Workspace *            GetWorkspace();
	virtual bool                   AddFilesToVirtualFolder(wxTreeItemId &item, wxArrayString &paths);
	virtual bool                   AddFilesToVirtualFolder(const wxString &vdFullPath, wxArrayString &paths);
	virtual bool                   AddFilesToVirtualFolderIntelligently(const wxString &vdFullPath, wxArrayString &paths);
	virtual int                    GetToolbarIconSize();
	virtual wxAuiManager*          GetDockingManager();
	virtual EnvironmentConfig*     GetEnv();
	virtual JobQueue *             GetJobQueue();
	virtual wxString               GetProjectExecutionCommand(const wxString &projectName, wxString &wd);
	virtual wxApp *                GetTheApp();
	virtual void                   ReloadWorkspace();
	virtual IPlugin*               GetPlugin(const wxString &pluginName);
	virtual wxEvtHandler *         GetOutputWindow();
	virtual bool                   SaveAll();
	virtual wxString               GetInstallDirectory() const;
	virtual IKeyboard *            GetKeyboardManager();
	virtual bool                   CreateVirtualDirectory(const wxString& parentPath, const wxString& vdName);
	virtual OptionsConfigPtr       GetEditorSettings();
    virtual void                   FindAndSelect(const wxString& pattern, const wxString& name);
    virtual TagEntryPtr            GetTagAtCaret(bool scoped, bool impl);
	virtual bool                   AllowToolbar();
	virtual void                   SetStatusMessage(const wxString &msg, int col, int id);
	virtual void                   PushQueueCommand(const QueueCommand &cmd);
	virtual void                   ProcessCommandQueue();
	virtual void                   StopAndClearQueue();
	virtual bool                   IsBuildInProgress() const;
	virtual bool                   IsBuildEndedSuccessfully() const;
	virtual wxString               GetProjectNameByFile( const wxString &fullPathFileName );
	virtual BuildManager *         GetBuildManager();
	virtual BuildSettingsConfig *  GetBuildSettingsConfigManager();
    virtual bool                   ClosePage(const wxString &text);
    virtual wxWindow *             FindPage(const wxString &text);
    virtual bool                   AddPage(wxWindow *win, const wxString &text, const wxBitmap &bmp = wxNullBitmap, bool selected = false);
    virtual bool                   SelectPage(wxWindow *win);
	virtual NavMgr *               GetNavigationMgr();

	//------------------------------------
	//End of IManager interface
	//------------------------------------

	// (Un)Hook the contect menus
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);

	// (Un)Hook the project settings tab
	virtual void HookProjectSettingsTab  (wxNotebook *book, const wxString &projectName, const wxString &configName);
	virtual void UnHookProjectSettingsTab(wxNotebook *book, const wxString &projectName, const wxString &configName);
};

#endif //PLUGINMANAGER_H
