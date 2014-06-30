//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : AdapterSelectDlg.h
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

#ifndef ADAPTERSELECTDLG_H
#define ADAPTERSELECTDLG_H

#include "GUI.h" // Base class: _AdapterSelectDlg
#include <wx/wx.h>
#include <wx/aui/aui.h>

#include "../Interfaces/imanager.h"

#ifdef DBL_USE_MYSQL
#include "MySqlDbAdapter.h"
#endif

#ifdef DBL_USE_SQLITE
#include "SqliteDbAdapter.h"
#endif

#ifdef DBL_USE_POSTGRES
#include "PostgreSqlDbAdapter.h"
#endif

class xsSerializable;
class AdapterSelectDlg : public _AdapterSelectDlg {

	public:
		AdapterSelectDlg(wxWindow* parent, wxWindow* pParentBook, IManager* pManager, xsSerializable* pConnections);
		virtual ~AdapterSelectDlg();

		virtual void OnMysqlClick(wxCommandEvent& event);
		virtual void OnSqliteClick(wxCommandEvent& event);
		virtual void OnPostgresClick(wxCommandEvent& event);

	protected:
		wxWindow* m_pParentBook;
		IManager* m_pManager;
		xsSerializable* m_pConnectons;
};

#endif // ADAPTERSELECTDLG_H
