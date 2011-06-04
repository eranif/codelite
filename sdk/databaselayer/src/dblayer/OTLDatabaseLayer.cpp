#include "OTLDatabaseLayer.h"
#include "OTLResultSet.h"
#include "OTLPreparedStatement.h"
#include <DatabaseErrorCodes.h>
#include <string>

std::string RemoveLastSemiColon(std::string strvalue)
{
  std::string result = strvalue;

  if (result.size() == 0)
    return result;

  if (result[result.size() -1] == ';' )
  {
    result.assign(strvalue,0,strvalue.size()-1);
  }
  return result;
}


// ctor()
OTLDatabaseLayer::OTLDatabaseLayer()
{
  m_bAutoCommit = true;
  m_strPort = _("1521");
  otl_connect::otl_initialize();
}

OTLDatabaseLayer::OTLDatabaseLayer(const wxString& strUser, const wxString& strPassword, bool bOpenDatabase /*=false*/)
{
  m_bAutoCommit = true;
  m_strPort = _("1521");
  m_strUser = strUser;
  m_strPassword = strPassword;
  otl_connect::otl_initialize();
  if (bOpenDatabase)
    Open();
}

OTLDatabaseLayer::OTLDatabaseLayer(const wxString& strUser, const wxString& strPassword, const wxString& strDatabase)
{
  m_bAutoCommit = true;
  m_strPort = _("1521");
  otl_connect::otl_initialize();
  Open(strDatabase, strUser, strPassword);
}

// dtor()
OTLDatabaseLayer::~OTLDatabaseLayer()
{
  Close();
}

void OTLDatabaseLayer::DeleteOTLStuff(void)
{
}

// open database
bool OTLDatabaseLayer::Open()
{
  try
  {
    wxString tnsString = _("");

    if (m_strServer != wxEmptyString)
    {
      tnsString += _("Host=") + m_strServer;
      if (m_strPort != wxEmptyString)
      {
        // Add the parameter separater (colon) if needed
        if (tnsString.length() > 0)
          tnsString += _(";");
        tnsString += _("Port=") + m_strPort;
      }
    }
    if (m_strDatabase != wxEmptyString)
    {
      // Add the parameter separater (colon) if needed
      if (tnsString.length() > 0)
        tnsString += _(";");
      tnsString += _("Service Name=") + m_strDatabase;
    }

    wxCharBuffer tnsStringBuffer = ConvertToUnicodeStream(tnsString);
    std::string strTnsString(tnsStringBuffer);

    wxString connectString = m_strUser + _("/") + m_strPassword;
    if (m_strDatabase != wxEmptyString)
      connectString += _("@") + m_strDatabase;

    try
    {
      m_database.rlogon(connectString.mb_str());
    }
    catch (otl_exception& e)
    {
      SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
      SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
      ThrowDatabaseException();
      return false;
    }
    catch (...)
    {
      if (GetErrorCode() == DATABASE_LAYER_OK)
      {
        SetErrorCode(DATABASE_LAYER_ERROR);
        SetErrorMessage(_("Error creating database connection"));
      }
      ThrowDatabaseException();
      return false;
    }
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
    return false;
  }
  return true;
}

bool OTLDatabaseLayer::Open(const wxString& strDatabase)
{
  m_strDatabase = strDatabase;
  return Open();
}

bool OTLDatabaseLayer::Open(const wxString& strServer, const wxString& strDatabase)
{
  m_strServer = strServer;
  return Open(strDatabase);
}

bool OTLDatabaseLayer::Open(const wxString& strDatabase, const wxString& strUser, const wxString& strPassword)
{
  m_strUser = strUser;
  m_strPassword = strPassword;
  return Open(strDatabase);
}

bool OTLDatabaseLayer::Open(const wxString& strServer, const wxString& strDatabase, const wxString& strUser, const wxString& strPassword)
{
  m_strServer = strServer;
  return Open(strDatabase, strUser, strPassword);
}


// close database
bool OTLDatabaseLayer::Close()
{
  try
  {
    CloseResultSets();
    CloseStatements();

    //DeleteOTLStuff();
    m_database.logoff();
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
    return false;
  }
  return true;
}

bool OTLDatabaseLayer::IsOpen()
{
  return m_database.connected;
}

// transaction support
void OTLDatabaseLayer::BeginTransaction()
{
  try
  {
    m_bAutoCommit = false;
    m_database.auto_commit_off();
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
}

void OTLDatabaseLayer::Commit()
{
  try
  {
    m_database.commit();
    m_bAutoCommit = true;
    m_database.auto_commit_on();
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
}

void OTLDatabaseLayer::RollBack()
{
  try
  {
    m_database.rollback();
    m_bAutoCommit = true;
    m_database.auto_commit_on();
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
}


// query database
int OTLDatabaseLayer::RunQuery(const wxString& strQuery, bool bParseQuery)
{
  try
  {
    wxArrayString QueryArray;
    if (bParseQuery)
      QueryArray = ParseQueries(strQuery);
    else
      QueryArray.push_back(strQuery);

    wxArrayString::iterator start = QueryArray.begin();
    wxArrayString::iterator stop = QueryArray.end();

    while (start != stop)
    {
      wxCharBuffer sqlBuffer = ConvertToUnicodeStream((*start));
      std::string strSQL(sqlBuffer);
      strSQL = RemoveLastSemiColon(strSQL);
      //wxPrintf(_("RunQuery: '%s'\n"), strSQL.c_str());
      otl_stream otlStream(1,strSQL.c_str(),m_database);
      start++;
    }
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
    return false;
  }
  return true;
}

DatabaseResultSet* OTLDatabaseLayer::RunQueryWithResults(const wxString& strQuery)
{
  OTLResultSet* pResultSet = NULL;
  try
  {
    wxArrayString QueryArray = ParseQueries(strQuery);

    if (QueryArray.size() > 0)
    {
      for (unsigned int i=0; i<(QueryArray.size()-1); i++)
      {
        wxCharBuffer sqlBuffer = ConvertToUnicodeStream(QueryArray[i]);
        std::string strSQL(sqlBuffer);
        strSQL = RemoveLastSemiColon(strSQL);
        //wxPrintf(_("RunQuery: '%s'\n"), strSQL.c_str());
        otl_stream otlStream(1,strSQL.c_str(),m_database);
      }

      // Deal with the last query separately
      wxCharBuffer sqlBuffer = ConvertToUnicodeStream(QueryArray[QueryArray.size()-1]);
      std::string strSQL(sqlBuffer);
      strSQL = RemoveLastSemiColon(strSQL);

      //wxPrintf(_("RunQuery: '%s'\n"), strSQL.c_str());
      otl_stream *otlInputStream = new otl_stream(1,strSQL.c_str(),m_database);
      {
        //fixme
        //pOTLStatement->setAutoCommit(m_bAutoCommit);
        pResultSet = new OTLResultSet(otlInputStream, true);
      }
    }
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
    return NULL;
  }

  if (pResultSet)
    LogResultSetForCleanup(pResultSet);

  return pResultSet;
}


// PreparedStatement support
PreparedStatement* OTLDatabaseLayer::PrepareStatement(const wxString& strQuery)
{
  OTLPreparedStatement* pStatement = NULL;
  try
  {
    wxArrayString QueryArray = ParseQueries(strQuery);

    wxArrayString::iterator start = QueryArray.begin();
    wxArrayString::iterator stop = QueryArray.end();

    while (start != stop)
    {
      wxCharBuffer sqlBuffer = ConvertToUnicodeStream((*start));
      std::string strSQL(sqlBuffer);
      //fixme
      /*
         OTL::occi::Statement* pOTLStatement = m_pDatabase->createStatement(strSQL);
         if (pOTLStatement)
         {
           pOTLStatement->setAutoCommit(m_bAutoCommit);
           pStatement->AddStatement(pOTLStatement);
         }
      */
      start++;
    }
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    wxDELETE(pStatement);
    ThrowDatabaseException();
    return NULL;
  }

  if (pStatement)
    LogStatementForCleanup(pStatement);

  return pStatement;
}

bool OTLDatabaseLayer::TableExists(const wxString& table)
{
  // Placeholder code stub
  return false;
}

bool OTLDatabaseLayer::ViewExists(const wxString& view)
{
  // Placeholder code stub
  return false;
}

wxArrayString OTLDatabaseLayer::GetTables()
{
  // Placeholder code stub
  wxArrayString returnArray;
  return returnArray;
}

wxArrayString OTLDatabaseLayer::GetViews()
{
  // Placeholder code stub
  wxArrayString returnArray;
  return returnArray;
}

wxArrayString OTLDatabaseLayer::GetColumns(const wxString& table)
{
  // Placeholder code stub
  wxArrayString returnArray;
  return returnArray;
}

int OTLDatabaseLayer::TranslateErrorCode(int nCode)
{
  // Ultimately, this will probably be a map of SQLite database error code values to DatabaseLayer values
  // For now though, we'll just return error
  int nReturn = nCode;
  return nReturn;
}
