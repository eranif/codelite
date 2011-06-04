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
	virtual ~DbItem();
	
	Database* GetDatabase(){ return this->m_pDatabase; }
	Table* GetTable(){ return this->m_pTable; }
	xsSerializable* GetData() { return this->m_pData; }
};

#endif // DBITEM_H
