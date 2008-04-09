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

class Notebook;
class wxFlatNotebook;

class PluginManager : public IManager
{
	std::map<wxString, IPlugin*> m_plugins;
	std::list< clDynamicLibrary* > m_dl;
	std::map<wxString, PluginInfo> m_pluginsInfo;
	
private:
	PluginManager(){}
	virtual ~PluginManager();

public:
	
	static PluginManager *Get();
	
	virtual void Load();
	virtual void UnLoad();
	
	/**
	 * \brief return a map of all loaded plugins
	 */
	const std::map<wxString, PluginInfo>& GetPluginsInfo() const {return m_pluginsInfo;}
	
	//------------------------------------
	//Implementation of IManager interface
	//------------------------------------

	virtual IEditor *GetActiveEditor();
	virtual IConfigTool *GetConfigTool();
	virtual TreeItemInfo GetSelectedTreeItemInfo(TreeType type);
	virtual wxTreeCtrl *GetTree(TreeType type);
	virtual Notebook *GetOutputPaneNotebook();
	virtual void OpenFile(const wxString &fileName, const wxString &projectName, int lineno = wxNOT_FOUND);
	virtual wxString GetStartupDirectory() const;
	virtual void AddProject(const wxString & path);
	virtual bool IsWorkspaceOpen() const;
	virtual TagsManager *GetTagsManager();
	virtual Workspace *GetWorkspace();
	virtual bool AddFilesToVirtualFodler(wxTreeItemId &item, wxArrayString &paths);
	virtual int GetToolbarIconSize();
	virtual wxFlatNotebook *GetMainNotebook();
	
	//------------------------------------
	//End of IManager interface
	//------------------------------------

	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
	
};

#endif //PLUGINMANAGER_H
