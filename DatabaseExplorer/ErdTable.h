//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ErdTable.h
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

#ifndef ERDTABLE_H
#define ERDTABLE_H

#include "ErdForeignKey.h"
#include "IDbType.h"
#include "NArrow.h"
#include "constraint.h"
#include "table.h"

#include <wx/wx.h>
#include <wx/wxsf/BitmapShape.h>
#include <wx/wxsf/DiagramManager.h>
#include <wx/wxsf/FlexGridShape.h>
#include <wx/wxsf/RoundRectShape.h>
#include <wx/wxsf/TextShape.h>
#include <wx/wxxmlserializer/XmlSerializer.h>

/*! \brief Class representig table graphics in ERD diagram. For more info see wxSF manual. */
class ErdTable : public wxSFRoundRectShape {

public:
	XS_DECLARE_CLONABLE_CLASS(ErdTable);

	ErdTable();
	ErdTable(const ErdTable& obj);
	ErdTable(Table* tab);
	
	virtual ~ErdTable();
	
	void AddColumn(const wxString& colName, IDbType* type);
	void UpdateColumns();
	
	Table* GetTable() { return (Table*) this->GetUserData(); }

protected:	
	void Initialize();
	
	wxSFTextShape *m_pLabel;
	wxSFFlexGridShape* m_pGrid;
	
	virtual void DrawHighlighted(wxDC& dc);
	virtual void DrawHover(wxDC& dc);
	virtual void DrawNormal(wxDC& dc);
	
	void DrawDetails(wxDC& dc);
	
	void ClearGrid();
	void ClearConnections();
	void AddColumnShape(const wxString& colName, int id, Constraint::constraintType type);
	void SetCommonProps(wxSFShapeBase* shape);
};

#endif // ERDTABLE_H
