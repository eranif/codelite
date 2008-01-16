#ifndef FILEEXPLORERTREE_H
#define FILEEXPLORERTREE_H

#include "virtualdirtreectrl.h"
#include "imanager.h"

class wxMenu;

class FileExplorerTree : public wxVirtualDirTreeCtrl
{
	wxMenu *m_rclickMenu;
private:
	void DoOpenItem(const wxTreeItemId& item);
	void DoOpenItemInTextEditor(const wxTreeItemId& item);
	void DoItemActivated(const wxTreeItemId &item);
	
public:
	FileExplorerTree(wxWindow *parent, wxWindowID id = wxID_ANY);
	virtual ~FileExplorerTree();
	TreeItemInfo GetSelectedItemInfo();

protected:
	DECLARE_EVENT_TABLE()
	virtual void OnContextMenu(wxTreeEvent &event);
	virtual void OnItemActivated(wxTreeEvent &event);
	virtual void OnOpenFile(wxCommandEvent &event);
	virtual void OnOpenFileInTextEditor(wxCommandEvent &event);
	virtual void OnRefreshNode(wxCommandEvent &event);
	virtual void OnDeleteNode(wxCommandEvent &event);
	virtual void OnMouseDblClick( wxMouseEvent &event );
	virtual void OnKeyDown( wxTreeEvent &event );
	virtual void OnOpenShell(wxCommandEvent &event);
};

#endif //FILEEXPLORERTREE_H


