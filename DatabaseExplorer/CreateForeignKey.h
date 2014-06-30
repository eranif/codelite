//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CreateForeignKey.h
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

#ifndef CREATEFOREIGNKEY_H
#define CREATEFOREIGNKEY_H
#include <wx/wx.h>
#include <wx/wxsf/ShapeBase.h>

#include "ErdTable.h"
#include "table.h"
#include "column.h"
#include "constraint.h"

#include "GUI.h" // Base class: _CreateForeignKey
/*! \brief  Dialog for foreign key creating. If N:M connection type is selected, dialog create new connection table. */
class CreateForeignKey : public _CreateForeignKey {

public:
	CreateForeignKey(wxWindow* parent, ErdTable* pSourceTable, ErdTable* pDestTable, const wxString& srcColName, const wxString& dstColName);
	virtual ~CreateForeignKey();
	virtual void OnCancelClick(wxCommandEvent& event);
	virtual void OnOKClick(wxCommandEvent& event);
	virtual void OnOKUI(wxUpdateUIEvent& event);
protected:
	ErdTable* m_pSrcTable;
	ErdTable* m_pDstTable;
	wxString m_srcColName;
	wxString m_dstColName;
};

#endif // CREATEFOREIGNKEY_H
