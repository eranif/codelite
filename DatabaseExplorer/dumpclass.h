//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : dumpclass.h
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

#ifndef DUMPCLASS_H
#define DUMPCLASS_H

#include "IDbAdapter.h"
#include "table.h"
#include "wx/wxxmlserializer/XmlSerializer.h"

#include <wx/textfile.h>

/*! \brief DumpClass can save items content (database table data) into sql file. */
class DumpClass {

public:
	DumpClass(IDbAdapter* pDbAdapter, xsSerializable* pItems, const wxString& fileName);
	virtual ~DumpClass() = default;
	
	/*! \brief start dumping data. Return finally status */
	wxString DumpData();

	
protected:
	int DumpTable(wxTextFile* pFile, Table* pTab);
	wxString m_fileName;
	xsSerializable* m_pItems;
	IDbAdapter* m_pDbAdapter;

};

#endif // DUMPCLASS_H
