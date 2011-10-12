#ifndef DUMPCLASS_H
#define DUMPCLASS_H

#include <wx/wx.h>
#include <wx/textfile.h>
#include "IDbAdapter.h"
#include "table.h"
#include <wx/wxxmlserializer/XmlSerializer.h>


/*! \brief DumpClass can save items content (database table data) into sql file. */
class DumpClass {

public:
	DumpClass(IDbAdapter* pDbAdapter, xsSerializable* pItems, const wxString& fileName);
	virtual ~DumpClass();
	
	/*! \brief start dumping data. Return finally status */
	wxString DumpData();

	
protected:
	int DumpTable(wxTextFile* pFile, DBETable* pTab);
	wxString m_fileName;
	xsSerializable* m_pItems;
	IDbAdapter* m_pDbAdapter;

};

#endif // DUMPCLASS_H
