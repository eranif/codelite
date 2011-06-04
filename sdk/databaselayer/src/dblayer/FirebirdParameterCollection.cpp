#include "../include/FirebirdParameterCollection.h"

FirebirdParameterCollection::FirebirdParameterCollection(FirebirdInterface* pInterface, XSQLDA* pParameters)
{
  m_pInterface = pInterface;
  m_FirebirdParameters = pParameters;
  AllocateParameterSpace();
}

FirebirdParameterCollection::~FirebirdParameterCollection()
{
  if (m_FirebirdParameters)
  {
    FreeParameterSpace();
  }
  
  FirebirdParameterArray::iterator start = m_Parameters.begin();
  FirebirdParameterArray::iterator stop = m_Parameters.end();
    
  while (start != stop)
  {
    FirebirdParameter* pParameter = (FirebirdParameter*)(*start);
    wxDELETE(pParameter);
    (*start) = NULL;
    start++;
  }
  m_Parameters.clear();
}

// set field
void FirebirdParameterCollection::SetParam(int nPosition, int nValue)
{
  FirebirdParameter* pParameter = new FirebirdParameter(m_pInterface, &m_FirebirdParameters->sqlvar[nPosition-1], nValue);
  SetParam(nPosition, pParameter);
}

void FirebirdParameterCollection::SetParam(int nPosition, double dblValue)
{
  FirebirdParameter* pParameter = new FirebirdParameter(m_pInterface, &m_FirebirdParameters->sqlvar[nPosition-1], dblValue);
  SetParam(nPosition, pParameter);
}

void FirebirdParameterCollection::SetParam(int nPosition, const wxString& strValue)
{
  FirebirdParameter* pParameter = new FirebirdParameter(m_pInterface, &m_FirebirdParameters->sqlvar[nPosition-1], strValue, GetEncoding());
  SetParam(nPosition, pParameter);
}

void FirebirdParameterCollection::SetParam(int nPosition)
{
  FirebirdParameter* pParameter = new FirebirdParameter(m_pInterface, &m_FirebirdParameters->sqlvar[nPosition-1]);
  SetParam(nPosition, pParameter);
}

void FirebirdParameterCollection::SetParam(int nPosition, isc_db_handle pDatabase, isc_tr_handle pTransaction, const void* pData, long nDataLength)
{
  FirebirdParameter* pParameter = new FirebirdParameter(m_pInterface, &m_FirebirdParameters->sqlvar[nPosition-1], pDatabase, pTransaction, pData, nDataLength);
  SetParam(nPosition, pParameter);
}

void FirebirdParameterCollection::SetParam(int nPosition, const wxDateTime& dateValue)
{
  FirebirdParameter* pParameter = new FirebirdParameter(m_pInterface, &m_FirebirdParameters->sqlvar[nPosition-1], dateValue);
  SetParam(nPosition, pParameter);
}

void FirebirdParameterCollection::SetParam(int nPosition, bool bValue)
{
  FirebirdParameter* pParameter = new FirebirdParameter(m_pInterface, &m_FirebirdParameters->sqlvar[nPosition-1], bValue);
  SetParam(nPosition, pParameter);
}

void FirebirdParameterCollection::SetParam(int nPosition, FirebirdParameter* pParameter)
{
  // First make sure that there are enough elements in the collection
  while (m_Parameters.size() < (unsigned int)(nPosition))
  {
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

void FirebirdParameterCollection::ResetBlobParameters()
{
  if (m_FirebirdParameters == NULL)
    return;
  
  FirebirdParameterArray::iterator start = m_Parameters.begin();
  FirebirdParameterArray::iterator stop = m_Parameters.end();
    
  while (start != stop)
  {
    const XSQLVAR* pVar = ((FirebirdParameter*)(*start))->GetFirebirdSqlVarPtr();
    if ((pVar->sqltype & ~1) == SQL_BLOB)
    {
      ((FirebirdParameter*)(*start))->ResetBlob();
    }
    start++;
  }
}

void FirebirdParameterCollection::AllocateParameterSpace()
{
  if (m_FirebirdParameters == NULL)
    return;
  
  for (int i = 0; i < m_FirebirdParameters->sqld; i++)
  {
    XSQLVAR* pVar = &(m_FirebirdParameters->sqlvar[i]);
    switch (pVar->sqltype & ~1)
    {
      case SQL_ARRAY:
      case SQL_BLOB:
        //pVar->sqldata = (char*)new ISC_QUAD;
        //memset(pVar->sqldata, 0, sizeof(ISC_QUAD));
        break;
      case SQL_TIMESTAMP:
        //pVar->sqldata = (char*)new ISC_TIMESTAMP;
        //memset(pVar->sqldata, 0, sizeof(ISC_TIMESTAMP));
        break;
      case SQL_TYPE_TIME:
        pVar->sqldata = (char*)new ISC_TIME;
        memset(pVar->sqldata, 0, sizeof(ISC_TIME));
        break;
      case SQL_TYPE_DATE:
        pVar->sqldata = (char*)new ISC_DATE;
        memset(pVar->sqldata, 0, sizeof(ISC_DATE));
        break;
      case SQL_TEXT:
        pVar->sqldata = (char*)new wxChar[pVar->sqllen+1];
        memset(pVar->sqldata, '\0', pVar->sqllen);
        pVar->sqldata[pVar->sqllen] = '\0';
        break;
      case SQL_VARYING:
        pVar->sqldata = (char*)new wxChar[pVar->sqllen+3];
        memset(pVar->sqldata, 0, 2);
        memset(pVar->sqldata+2, '\0', pVar->sqllen);
        pVar->sqldata[pVar->sqllen+2] = '\0';
        break;
      case SQL_SHORT:
        //pVar->sqldata = (char*)new short(0);
        break;
      case SQL_LONG:
        //pVar->sqldata = (char*)new long(0);
        break;
      case SQL_INT64:
        pVar->sqldata = (char*)new ISC_INT64(0);
        break;
      case SQL_FLOAT:
        //pVar->sqldata = (char*)new float(0.0);
        break;
      case SQL_DOUBLE:
        //pVar->sqldata = (char*)new double(0.0);
        break;
      default: 
        wxLogError(_("Error allocating space for unknown parameter type\n")); 
        break;
    }
    //if (pVar->sqltype & 1)
      //pVar->sqlind = new short(-1);	// 0 indicator
  }
}  

void FirebirdParameterCollection::FreeParameterSpace()
{
  //return;
  if (m_FirebirdParameters)
  {
    for (int i = 0; i < m_FirebirdParameters->sqld; i++)
    {
      XSQLVAR* pVar = &(m_FirebirdParameters->sqlvar[i]);
      if (pVar->sqldata != 0)
      {
        switch (pVar->sqltype & ~1)
        {
          case SQL_ARRAY:
          case SQL_BLOB:
            //wxDELETE(pVar->sqldata);
            break;
          case SQL_TIMESTAMP:
            //wxDELETE(pVar->sqldata);
            break;
          case SQL_TYPE_TIME:
            wxDELETE(pVar->sqldata);
            break;
          case SQL_TYPE_DATE:
            //wxDELETE(pVar->sqldata);
            break;
          case SQL_TEXT:
          case SQL_VARYING:
            wxDELETEA(pVar->sqldata);
            break;
          case SQL_SHORT:
            //wxDELETE(pVar->sqldata);
            break;
          case SQL_LONG:
            //wxDELETE(pVar->sqldata);
            break;
          case SQL_INT64:
            //wxDELETE(pVar->sqldata);
            break;
          case SQL_FLOAT:
            //wxDELETE(pVar->sqldata);
            break;
          case SQL_DOUBLE:
            //wxDELETE(pVar->sqldata);
            break;
          default:
            wxLogError(_("Error deleting unknown parameter type\n"));
            break;
        }
      }
      //if (pVar->sqlind != 0)
        //delete pVar->sqlind;
      //wxDELETE(pVar->sqlind);
    }
    wxDELETEA(m_FirebirdParameters);
  }
}

