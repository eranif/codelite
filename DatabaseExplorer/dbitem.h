//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : dbitem.h
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

#ifndef DBITEM_H
#define DBITEM_H
#include "database.h"
#include "table.h"
#include <wx/treebase.h> // Base class: wxTreeItemData
/*! \brief Class for saving pointer to database objects into tree control in DbViewerPanel */
class DbItem : public wxTreeItemData {
protected:
	Database* m_pDatabase;
	Table* m_pTable;	
	xsSerializable* m_pData;
public:
	DbItem(Database* pDatabase, Table* pTable);
	DbItem(xsSerializable* data);
	virtual ~DbItem() = default;

	Database* GetDatabase(){ return this->m_pDatabase; }
	Table* GetTable(){ return this->m_pTable; }
	xsSerializable* GetData() { return this->m_pData; }
};

#endif // DBITEM_H
