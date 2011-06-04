#include "../include/PostgresPreparedStatementParameterCollection.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(ArrayOfPostgresParameters);

PostgresPreparedStatementParameterCollection::~PostgresPreparedStatementParameterCollection()
{
  m_Parameters.Clear();
}

int PostgresPreparedStatementParameterCollection::GetSize()
{
  return m_Parameters.size();
}

char** PostgresPreparedStatementParameterCollection::GetParamValues()
{
  char** paramValues = new char*[m_Parameters.size()];

  for (unsigned int i=0; i<m_Parameters.size(); i++)
  {
    // Get a pointer to the appropriate data member variable for this parameter
    paramValues[i] = (char*)(m_Parameters[i].GetDataPtr());
  }

  return paramValues;
}

int* PostgresPreparedStatementParameterCollection::GetParamLengths()
{
  int* paramLengths = new int[m_Parameters.size()];
  
  for (unsigned int i=0; i<m_Parameters.size(); i++)
  {
    // Get a pointer to the m_nBufferLength member variable for this parameter
    paramLengths[i] = m_Parameters[i].GetDataLength();
  }

  return paramLengths;
}

int* PostgresPreparedStatementParameterCollection::GetParamFormats()
{
  int* paramFormats = new int[m_Parameters.size()];
  
  for (unsigned int i=0; i<m_Parameters.size(); i++)
  {
    paramFormats[i] = (m_Parameters[i].IsBinary()) ? 1 : 0;
  }

  return paramFormats;
}

void PostgresPreparedStatementParameterCollection::SetParam(int nPosition, int nValue)
{
  PostgresParameter Parameter(nValue);
  SetParam(nPosition, Parameter);
}

void PostgresPreparedStatementParameterCollection::SetParam(int nPosition, double dblValue)
{
  PostgresParameter Parameter(dblValue);
  SetParam(nPosition, Parameter);
}

void PostgresPreparedStatementParameterCollection::SetParam(int nPosition, const wxString& strValue)
{
  PostgresParameter Parameter(strValue);
  SetParam(nPosition, Parameter);
}

void PostgresPreparedStatementParameterCollection::SetParam(int nPosition)
{
  PostgresParameter Parameter;
  SetParam(nPosition, Parameter);
}

void PostgresPreparedStatementParameterCollection::SetParam(int nPosition, const void* pData, long nDataLength)
{
  PostgresParameter Parameter(pData, nDataLength);
  SetParam(nPosition, Parameter);
}

void PostgresPreparedStatementParameterCollection::SetParam(int nPosition, const wxDateTime& dateValue)
{
  PostgresParameter Parameter(dateValue);
  SetParam(nPosition, Parameter);
}

void PostgresPreparedStatementParameterCollection::SetParam(int nPosition, bool bValue)
{
  PostgresParameter Parameter(bValue);
  SetParam(nPosition, Parameter);
}

void PostgresPreparedStatementParameterCollection::SetParam(int nPosition, PostgresParameter& Parameter)
{
  // First make sure that there are enough elements in the collection
  while (m_Parameters.size() < (unsigned int)(nPosition))
  {
    PostgresParameter EmptyParameter;
    m_Parameters.push_back(EmptyParameter);
  }
  m_Parameters[nPosition-1] = Parameter;
}

