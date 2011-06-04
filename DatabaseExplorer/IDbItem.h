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
