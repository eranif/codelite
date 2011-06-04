#include "../include/MysqlPreparedStatementParameterCollection.h"

MysqlPreparedStatementParameterCollection::MysqlPreparedStatementParameterCollection()
{
}

MysqlPreparedStatementParameterCollection::~MysqlPreparedStatementParameterCollection()
{
  MysqlParameterArray::iterator start = m_Parameters.begin();
  MysqlParameterArray::iterator stop = m_Parameters.end();

  while (start != stop)
  {
    if ((*start) != NULL)
    {
      MysqlParameter* pParameter = (MysqlParameter*)(*start);
      wxDELETE(pParameter);
      (*start) = NULL;
    }
    start++;
  }
}

int MysqlPreparedStatementParameterCollection::GetSize()
{
  return m_Parameters.size();
}

MYSQL_BIND* MysqlPreparedStatementParameterCollection::GetMysqlParameterBindings()
{
  MYSQL_BIND* pBindings = new MYSQL_BIND[m_Parameters.size()];
  
  memset(pBindings, 0, sizeof(MYSQL_BIND)*m_Parameters.size());

  for (unsigned int i=0; i<m_Parameters.size(); i++)
  {
    pBindings[i].buffer_type=m_Parameters[i]->GetBufferType();
    pBindings[i].buffer=(void*)m_Parameters[i]->GetDataPtr();
    pBindings[i].buffer_length=m_Parameters[i]->GetDataLength();
    pBindings[i].length=m_Parameters[i]->GetDataLengthPtr();
  }

  return pBindings;
}

void MysqlPreparedStatementParameterCollection::SetParam(int nPosition, int nValue)
{
  //MysqlParameter Parameter(nValue);
  //SetParam(nPosition, Parameter);
  MysqlParameter* pParameter = new MysqlParameter(nValue);
  pParameter->SetEncoding(GetEncoding());
  SetParam(nPosition, pParameter);
}

void MysqlPreparedStatementParameterCollection::SetParam(int nPosition, double dblValue)
{
  //MysqlParameter Parameter(dblValue);
  //SetParam(nPosition, Parameter);
  MysqlParameter* pParameter = new MysqlParameter(dblValue);
  pParameter->SetEncoding(GetEncoding());
  SetParam(nPosition, pParameter);
}

void MysqlPreparedStatementParameterCollection::SetParam(int nPosition, const wxString& strValue)
{
  //MysqlParameter Parameter(strValue);
  //SetParam(nPosition, Parameter);
  MysqlParameter* pParameter = new MysqlParameter(strValue);
  pParameter->SetEncoding(GetEncoding());
  SetParam(nPosition, pParameter);
}

void MysqlPreparedStatementParameterCollection::SetParam(int nPosition)
{
  //MysqlParameter Parameter;
  //SetParam(nPosition, Parameter);
  MysqlParameter* pParameter = new MysqlParameter();
  pParameter->SetEncoding(GetEncoding());
  SetParam(nPosition, pParameter);
}

void MysqlPreparedStatementParameterCollection::SetParam(int nPosition, const void* pData, long nDataLength)
{
  //MysqlParameter Parameter(pData, nDataLength);
  //SetParam(nPosition, Parameter);
  MysqlParameter* pParameter = new MysqlParameter(pData, nDataLength);
  pParameter->SetEncoding(GetEncoding());
  SetParam(nPosition, pParameter);
}

void MysqlPreparedStatementParameterCollection::SetParam(int nPosition, const wxDateTime& dateValue)
{
  //MysqlParameter Parameter(dateValue);
  //SetParam(nPosition, Parameter);
  MysqlParameter* pParameter = new MysqlParameter(dateValue);
  pParameter->SetEncoding(GetEncoding());
  SetParam(nPosition, pParameter);
}

void MysqlPreparedStatementParameterCollection::SetParam(int nPosition, bool bValue)
{
  //MysqlParameter Parameter(bValue);
  //SetParam(nPosition, Parameter);
  MysqlParameter* pParameter = new MysqlParameter(bValue);
  pParameter->SetEncoding(GetEncoding());
  SetParam(nPosition, pParameter);
}

void MysqlPreparedStatementParameterCollection::SetParam(int nPosition, MysqlParameter* pParameter)
{
  // First make sure that there are enough elements in the collection
  while (m_Parameters.size() < (unsigned int)(nPosition))
  {
    //MysqlParameter EmptyParameter;
    m_Parameters.push_back(NULL);//EmptyParameter);
  }
  // Free up any data that is being replaced so the allocated memory isn't lost
  if (m_Parameters[nPosition-1] != NULL)
  {
    delete (m_Parameters[nPosition-1]);
  }
  // Now set the new data
  m_Parameters[nPosition-1] = pParameter;
}

