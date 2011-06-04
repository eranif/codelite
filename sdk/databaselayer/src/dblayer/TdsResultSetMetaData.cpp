#include "../include/TdsResultSetMetaData.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(ArrayOfTdsColumns);

// ctor
TdsResultSetMetaData::TdsResultSetMetaData(TDSRESULTINFO* pInfo)
{
  for (int i=0; i<pInfo->num_cols; i++)
  {
    TdsColumnData column(pInfo->columns[i]);
    m_Columns.push_back(column);
  }
}

int TdsResultSetMetaData::GetColumnType(int i)
{
  return m_Columns[i-1].GetColumnType();
}

int TdsResultSetMetaData::GetColumnSize(int i)
{
  return m_Columns[i-1].GetColumnSize();
}

wxString TdsResultSetMetaData::GetColumnName(int i)
{
  return m_Columns[i-1].GetColumnName();
}

int TdsResultSetMetaData::GetColumnCount()
{
  return m_Columns.size();
}


