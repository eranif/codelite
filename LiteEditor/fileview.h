//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : fileview.h
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
 #ifndef FILE_VIEW_TREE_H
#define FILE_VIEW_TREE_H

#include "wx/treectrl.h"
#include "project.h"
#include "pluginmanager.h"
#include "imanager.h"
#include "map"

class wxMenu;

struct FileViewItem {
	wxString virtualDir;
	wxString fullpath;
	wxString displayName;
};

class FileViewTree : public wxTreeCtrl
{
	DECLARE_DYNAMIC_CLASS()
	wxMenu *m_folderMenu;
	wxMenu *m_projectMenu;
	wxMenu *m_fileMenu;
	wxMenu *m_workspaceMenu;

	std::map<void*, bool> m_itemsToSort;
	wxArrayTreeItemIds m_draggedItems;

public:
	/**
	 * Default cosntructor.
	 */
	FileViewTree();

	/**
	 * Parameterized constructor.
	 * @param parent Tree parent window
	 * @param id Window id
	 * @param pos Window position
	 * @param size Window size
	 * @param style Window style
	 */
	FileViewTree(wxWindow *parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);

	/**
	 * Destructor .
	 */
	virtual ~FileViewTree(void);

	/**
	 * Create tree, usually called after constructing FileViewTree with default constructor.
	 * @param parent Tree parent window
	 * @param id Window id
	 * @param pos Window position
	 * @param size Window size
	 * @param style Window style
	 */
	virtual void Create(wxWindow *parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);

	// Build the actual tree from the workspace
	void BuildTree();

	/**
	 * @brief mark project with given name as active project
	 * @param projectName
	 */
	void MarkActive(const wxString &projectName);

	/**
	 * @brief return the current selected item information
	 */
	TreeItemInfo GetSelectedItemInfo();

	/**
	 * Make sure that fileName under project is visible
	 * @param &project
	 * @param &fileName
	 */
	void ExpandToPath(const wxString &project, const wxFileName &fileName);

	/**
	 * @brief create and add file to the tree
	 * @param filename new file full path
	 * @param vdFullPath files' virtual folder full path (format-> project:path:to:vd)
	 */
	bool CreateAndAddFile(const wxString &filename, const wxString &vdFullPath);

	bool AddFilesToVirtualFolder(wxTreeItemId &item, wxArrayString &paths);
	bool AddFilesToVirtualFolder(const wxString &vdFullPath, wxArrayString &paths);
	bool AddFilesToVirtualFolderIntelligently(const wxString& vdFullPath, wxArrayString& paths);

	bool CreateVirtualDirectory(const wxString &parentPath, const wxString &vdName);

protected:
	virtual void OnPopupMenu(wxTreeEvent &event);
	virtual void OnItemActivated(wxTreeEvent &event);
	virtual void OnMouseDblClick(wxMouseEvent &event);
	virtual void OnRemoveProject(wxCommandEvent &event);
	virtual void OnSetActive(wxCommandEvent &event);
	virtual void OnNewItem(wxCommandEvent &event);
	virtual void OnAddExistingItem(wxCommandEvent &event);
	virtual void OnNewVirtualFolder(wxCommandEvent &event);
	virtual void OnLocalPrefs(wxCommandEvent &event);
	virtual void OnProjectProperties(wxCommandEvent &event);
	virtual void OnSortItem(wxCommandEvent &event);
	virtual void OnRemoveVirtualFolder(wxCommandEvent &event);
	virtual void OnRemoveItem(wxCommandEvent &event);
	virtual void OnExportMakefile(wxCommandEvent &event);
	virtual void OnSaveAsTemplate(wxCommandEvent &event);
	virtual void OnBuildOrder(wxCommandEvent &event);
	virtual void OnClean(wxCommandEvent &event);
	virtual void OnBuild(wxCommandEvent &event);
	virtual void OnReBuild(wxCommandEvent &event);
	virtual void OnRunPremakeStep(wxCommandEvent &event);
	virtual void OnBuildProjectOnly(wxCommandEvent &event);
	virtual void OnCleanProjectOnly(wxCommandEvent &event);
	virtual void OnRebuildProjectOnly(wxCommandEvent &event);
	virtual void OnStopBuild(wxCommandEvent &event);
	virtual void OnRetagProject(wxCommandEvent &event);
	virtual void OnRetagWorkspace(wxCommandEvent &event);
	virtual void OnBuildInProgress(wxUpdateUIEvent &event);
	virtual void OnRetagInProgressUI(wxUpdateUIEvent &event);
	virtual void OnItemBeginDrag(wxTreeEvent &event);
	virtual void OnItemEndDrag(wxTreeEvent &event);
	virtual void OnImportDirectory(wxCommandEvent &e);
	virtual void OnRenameItem(wxCommandEvent &e);
	virtual void OnCompileItem(wxCommandEvent &e);
    virtual void OnPreprocessItem(wxCommandEvent &e);
	virtual void SortTree();
	virtual void SortItem(wxTreeItemId &item);
	virtual void OnRenameVirtualFolder(wxCommandEvent &e);
	virtual wxTreeItemId ItemByFullPath(const wxString &fullPath);
	virtual void OnLocalWorkspaceSettings(wxCommandEvent &e);



	// Tree sorting
	virtual int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);
	int OnCompareItems(const FilewViewTreeItemData *a, const FilewViewTreeItemData *b);

	void PopupContextMenu(wxMenu *menu, MenuType type, const wxString &projectName = wxEmptyString);

private:
	// Build project node
	void BuildProjectNode(const wxString &projectName);
	int GetIconIndex(const ProjectItem &item);
	void ConnectEvents();
	wxString GetItemPath(wxTreeItemId &item);

	bool DoAddNewItem(wxTreeItemId &item, const wxString &filename, const wxString &vdFullpath);
	void DoRemoveProject(const wxString &name);
	void DoSetProjectActive(wxTreeItemId &item);
	void DoAddVirtualFolder(wxTreeItemId &parent, const wxString &text);
	void DoRemoveVirtualFolder(wxTreeItemId &parent);
	void DoRemoveItems();
	void DoItemActivated(wxTreeItemId &item, wxEvent &event);
	void DoAddItem(ProjectPtr proj, const FileViewItem &item);
	wxTreeItemId DoGetItemByText(const wxTreeItemId &parent, const wxString &text);

	wxTreeItemId GetSingleSelection();
	size_t GetMultiSelection(wxArrayTreeItemIds &arr);
	wxTreeItemId FindItemByPath(wxTreeItemId &parent, const wxString &projectPath, const wxString &fileName);
	DECLARE_EVENT_TABLE();
};

#endif // FILE_VIEW_TREE_H

