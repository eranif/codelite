#include "dbitem.h"


DbItem::~DbItem()
{
}

DbItem::DbItem(Database* pDatabase, Table* pTable)
{
	this->m_pDatabase = pDatabase;
	this->m_pTable = pTable;
	this->m_pData = NULL;
}

DbItem::DbItem(xsSerializable* data)
{
	this->m_pData = data;
	this->m_pDatabase = NULL;
	this->m_pTable = NULL;
}

