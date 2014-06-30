//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : IDbItem.h
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

#ifndef IDBITEM_H
#define IDBITEM_H

#include <wx/treebase.h> // Base class: wxTreeItemData


enum DbItemType {
	DbDatabaseType,
	DbTableType,
	DbStoredProcedureType,
	DbViewType,
	DbColumnType
	};


class IDbItem : public wxTreeItemData {

public:
	virtual wxString GetName() = 0;
	virtual void SetName(wxString& name) = 0;
	
	virtual DbItemType GetType() = 0;
	
	virtual wxString GetParentName() = 0;
	virtual void SetParentName(wxString& databaseName) = 0;


};

#endif // IDBITEM_H
