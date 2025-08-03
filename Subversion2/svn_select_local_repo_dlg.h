//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : svn_select_local_repo_dlg.h
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

#ifndef __svn_select_local_repo_dlg__
#define __svn_select_local_repo_dlg__

#include "subversion2_ui.h"

class Subversion2;
class SvnSelectLocalRepoDlg : public SvnSelectLocalRepoBase
{
	Subversion2 *m_svn;
	
protected:
	// Handlers for SvnSelectLocalRepoBase events.
	void OnPathSelected( wxCommandEvent& event );
	void OnPathActivated( wxCommandEvent& event );
	void OnMenu( wxMouseEvent& event );
	
	void OnRemoveEntry(wxCommandEvent &e);
	
public:
	/** Constructor */
	SvnSelectLocalRepoDlg( wxWindow* parent, Subversion2* plugin, const wxString &curpath );
	virtual ~SvnSelectLocalRepoDlg() = default;
	wxString GetPath() const;
};

#endif // __svn_select_local_repo_dlg__
