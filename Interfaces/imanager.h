#ifndef IMANAGER_H
#define IMANAGER_H

#include "ieditor.h"
#include "iconfigtool.h"
#include "wx/treectrl.h"
#include "custom_notebook.h"

class TagsManager;
class Workspace;

//--------------------------
//Auxulary class
//--------------------------
class TreeItemInfo {
public:
	wxTreeItemId m_item;
	wxFileName m_fileName;	//< FileName where available (FileView & File Explorer trees) 
	wxString m_text;		//< Tree item text (all)
	int	m_itemType;			//< For FileView items (FileView only)
};

//---------------------------
// List of availabla trees 
//---------------------------
enum TreeType {
	TreeFileView = 0,
	TreeFileExplorer,
};

//------------------------------------------------------------------
// Defines the interface of the manager
//------------------------------------------------------------------
class IManager {
public:
	IManager(){}
	virtual ~IManager(){}
	
	//return the current editor
	virtual IEditor *GetActiveEditor() = 0; 
	virtual void OpenFile(const wxString &fileName, const wxString &projectName, int lineno = wxNOT_FOUND) = 0;
	virtual IConfigTool *GetConfigTool() = 0;
	virtual TreeItemInfo GetSelectedTreeItemInfo(TreeType type) = 0;
	virtual wxTreeCtrl *GetTree(TreeType type) = 0;
	virtual Notebook *GetOutputPaneNotebook() = 0;
	virtual wxString GetStartupDirectory() const = 0;
	virtual void AddProject(const wxString & path) = 0;
	virtual bool IsWorkspaceOpen() const = 0;
	virtual TagsManager *GetTagsManager() = 0;
	virtual Workspace *GetWorkspace() = 0;
	virtual bool AddFilesToVirtualFodler(wxTreeItemId &item, wxArrayString &paths) = 0;
	virtual int GetToolbarIconSize() = 0;
	virtual Notebook *GetMainNotebook() = 0;
	virtual wxAuiManager* GetDockingManager() = 0;
};

#endif //IMANAGER_H

