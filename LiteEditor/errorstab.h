//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : buidltab.h
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
#ifndef __errorstab__
#define __errorstab__

#include <vector>
#include "buidltab.h"
#include "cl_treelistctrl.h"

class ErrorsTab : public OutputTabWindow
{
	friend class BuildTab;
	clTreeListCtrl* m_treeListCtrl;
	BuildTab*       m_bt;
	int             m_errorCount;
	int             m_warningCount;
	
private:
	wxTreeItemId DoFindFile(const wxString& filename);
	bool         IsMessageExists(const wxString& msg, long line, const wxTreeItemId& item);
	
protected:
	void ClearLines();
	void AddError( const BuildTab::LineInfo &lineInfo);
	void OnClearAll        (wxCommandEvent   &e);
	void OnClearAllUI      (wxUpdateUIEvent  &e);
	void OnHoldOpenUpdateUI(wxUpdateUIEvent& e);
	void OnItemDClick      (wxTreeEvent &event);
	void OnBuildEnded      (wxCommandEvent &event);
public:
	ErrorsTab(BuildTab *bt, wxWindow *parent, wxWindowID id, const wxString &name);
	~ErrorsTab();
};

#endif // __errorstab__
