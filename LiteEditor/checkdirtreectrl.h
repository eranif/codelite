//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : checkdirtreectrl.h
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
 #ifndef CHECKDIRCTRL_H
#define CHECKDIRCTRL_H

#include "checktreectrl.h"
#include "wx/dir.h"
#include "wx/filename.h"

class DirTreeData : public wxTreeItemData
{
	wxFileName m_dirname;
public:
	DirTreeData(const wxFileName &dir) : m_dirname(dir){}
	virtual ~DirTreeData(){}

	const wxFileName& GetDir() const {return m_dirname;}
};


class CheckDirTreeCtrl : public wxCheckTreeCtrl
{
	wxString m_root;
public:
	CheckDirTreeCtrl(wxWindow *parent, const wxString &rootPath, wxWindowID id = wxID_ANY);
	virtual ~CheckDirTreeCtrl();
	void GetUnselectedDirs(wxArrayString &arr);
	void GetSelectedDirs(wxArrayString &arr);
	void BuildTree(const wxString &rootPath);
	wxString GetRootDir(){return m_root;}
	DECLARE_EVENT_TABLE();
	void OnItemExpading(wxTreeEvent &event);
	void OnItemUnchecked(wxCheckTreeCtrlEvent &event);
	void OnItemChecked(wxCheckTreeCtrlEvent &event);

private:
	void GetChildren(const wxTreeItemId &parent, const wxDir &dir, const wxString &path);
	void AddChildren(const wxTreeItemId &item);
};

#endif //CHECKDIRCTRL_H
