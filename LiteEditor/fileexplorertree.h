//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : fileexplorertree.h
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
    virtual void OnSearchNode(wxCommandEvent &event);
    virtual void OnTagNode(wxCommandEvent &event);
	virtual void OnMouseDblClick( wxMouseEvent &event );
	virtual void OnKeyDown( wxTreeEvent &event );
	virtual void OnOpenShell(wxCommandEvent &event);
	virtual void OnOpenWidthDefaultApp(wxCommandEvent &e);
};

#endif //FILEEXPLORERTREE_H


