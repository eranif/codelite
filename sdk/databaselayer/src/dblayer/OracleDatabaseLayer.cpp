#include "../include/OracleDatabaseLayer.h"
#include "../include/OracleResultSet.h"
#include "../include/OraclePreparedStatement.h"
#include "../include/OraclePreparedStatementWrapper.h"
#include "../include/DatabaseErrorCodes.h"

// ctor()
OracleDatabaseLayer::OracleDatabaseLayer()
{
  m_bAutoCommit = true;
  m_strPort = _("1521");
  m_pEnvironment = oracle::occi::Environment::createEnvironment();
  m_pDatabase = NULL;
}

OracleDatabaseLayer::OracleDatabaseLayer(const wxString& strUser, const wxString& strPassword, bool bOpenDatabase /*=false*/)
{
  m_bAutoCommit = true;
  m_strPort = _("1521");
  m_strUser = strUser;
  m_strPassword = strPassword;
  m_pEnvironment = oracle::occi::Environment::createEnvironment();
  if (bOpenDatabase)
    Open();
}

OracleDatabaseLayer::OracleDatabaseLayer(const wxString& strDatabase, const wxString& strUser, const wxString& strPassword)
{
  m_bAutoCommit = true;
  m_strPort = _("1521");
  m_pEnvironment = oracle::occi::Environment::createEnvironment();
  m_pDatabase = NULL;
  Open(strDatabase, strUser, strPassword);
}

OracleDatabaseLayer::OracleDatabaseLayer(oracle::occi::Environment* pEnvironment, oracle::occi::Connection* pDatabase)
{
  m_bAutoCommit = true;
  m_strPort = _("1521");
  m_pEnvironment = pEnvironment;
  m_pDatabase = pDatabase;
}

OracleDatabaseLayer::OracleDatabaseLayer(const wxString& strServer, const wxString& strDatabase, const wxString& strUser, const wxString& strPassword)
{
  m_bAutoCommit = true;
  m_strPort = _("1521");
  m_pEnvironment = oracle::occi::Environment::createEnvironment();
  m_pDatabase = NULL;
  Open(strServer, strDatabase, strUser, strPassword);
}

// dtor()
OracleDatabaseLayer::~OracleDatabaseLayer()
{
  Close();
}


// open database
bool OracleDatabaseLayer::Open()
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
    
    if (m_pEnvironment == NULL)
      m_pEnvironment = oracle::occi::Environment::createEnvironment();

    wxCharBuffer userCharBuffer = ConvertToUnicodeStream(m_strUser);
    std::string strUser = (const char*)userCharBuffer;
    wxCharBuffer passwordCharBuffer = ConvertToUnicodeStream(m_strPassword);
    std::string strPassword = (const char*)passwordCharBuffer;
    m_pDatabase = m_pEnvironment->createConnection(strUser, strPassword, strTnsString);

    if (m_pDatabase == NULL)
    {
      // If the database didn't set an error indicator, then make sure that is gets set
      if (GetErrorCode() == DATABASE_LAYER_OK)
      {
        SetErrorCode(DATABASE_LAYER_ERROR);
        SetErrorMessage(_("Error creating database connection"));
      }
      ThrowDatabaseException();
      return false;
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
    return false;
  }
  return true;
}

bool OracleDatabaseLayer::Open(const wxString& strDatabase)
{
  m_strDatabase = strDatabase;
  return Open();
}

bool OracleDatabaseLayer::Open(const wxString& strServer, const wxString& strDatabase)
{
  m_strServer = strServer;
  return Open(strDatabase);
}

bool OracleDatabaseLayer::Open(const wxString& strDatabase, const wxString& strUser, const wxString& strPassword)
{
  m_strUser = strUser;
  m_strPassword = strPassword;
  return Open(strDatabase);
}

bool OracleDatabaseLayer::Open(const wxString& strServer, const wxString& strDatabase, const wxString& strUser, const wxString& strPassword)
{
  m_strServer = strServer;
  return Open(strDatabase, strUser, strPassword);
}


  // close database
bool OracleDatabaseLayer::Close()
{
   CloseResultSets();
   CloseStatements();

  try
  {
    if (m_pEnvironment)
    {
      if (m_pDatabase)
        m_pEnvironment->terminateConnection(m_pDatabase);

      oracle::occi::Environment::terminateEnvironment(m_pEnvironment);
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
    return false;
  }
  return true;
}

bool OracleDatabaseLayer::IsOpen()
{
  return (m_pDatabase != NULL);
}

  // transaction support
void OracleDatabaseLayer::BeginTransaction()
{
  try
  {
    m_bAutoCommit = false;
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
}

void OracleDatabaseLayer::Commit()
{
  try
  {
    m_pDatabase->commit();
    m_bAutoCommit = true;
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
}

void OracleDatabaseLayer::RollBack()
{
  try
  {
    m_pDatabase->rollback();
    m_bAutoCommit = true;
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
}

  
  // query database
bool OracleDatabaseLayer::RunQuery(const wxString& strQuery, bool bParseQuery)
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
      //std::string strSQL(sqlBuffer);
      wxString strTemp = *start;
      if (strTemp.Last() == ';')
        strTemp.RemoveLast();
      puts(strTemp.c_str());
      std::string strSQL(strTemp);
      oracle::occi::Statement* pOracleStatement = m_pDatabase->createStatement(strSQL);
      if (pOracleStatement)
      {
        pOracleStatement->setAutoCommit(m_bAutoCommit);
        pOracleStatement->execute();
        m_pDatabase->terminateStatement(pOracleStatement);
      }
      start++;
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
    return false;
  }
  return true;
}

DatabaseResultSet* OracleDatabaseLayer::RunQueryWithResults(const wxString& strQuery)
{
  OracleResultSet* pResultSet = NULL;
  try
  {
    wxArrayString QueryArray = ParseQueries(strQuery);

    if (QueryArray.size() > 0)
    {
      for (unsigned int i=0; i<(QueryArray.size()-1); i++)
      {
        wxCharBuffer sqlBuffer = ConvertToUnicodeStream(QueryArray[i]);
        //std::string strSQL(sqlBuffer);
      wxString strTemp = QueryArray[i];
      if (strTemp.Last() == ';')
        strTemp.RemoveLast();
      puts(strTemp.c_str());
      std::string strSQL(strTemp);
        oracle::occi::Statement* pOracleStatement = m_pDatabase->createStatement(strSQL);
        if (pOracleStatement)
        {
          pOracleStatement->setAutoCommit(m_bAutoCommit);
          pOracleStatement->execute();
          m_pDatabase->terminateStatement(pOracleStatement);
        }
      }

      // Deal with the last query separately
      wxCharBuffer sqlBuffer = ConvertToUnicodeStream(QueryArray[QueryArray.size()-1]);
      //std::string strSQL(sqlBuffer);
      wxString strTemp = QueryArray[QueryArray.size()-1];
      if (strTemp.Last() == ';')
        strTemp.RemoveLast();
      puts(strTemp.c_str());
      std::string strSQL(strTemp);
      oracle::occi::Statement* pOracleStatement = m_pDatabase->createStatement(strSQL);
      if (pOracleStatement)
      {
        pOracleStatement->setAutoCommit(m_bAutoCommit);
        oracle::occi::ResultSet* pOracleResultSet = pOracleStatement->executeQuery();
        pResultSet = new OracleResultSet(pOracleResultSet, true);
      }
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
    return NULL;
  }
  return pResultSet;
}

  
// PreparedStatement support
PreparedStatement* OracleDatabaseLayer::PrepareStatement(const wxString& strQuery)
{
  OraclePreparedStatement* pStatement = new OraclePreparedStatement(m_pEnvironment);
  try
  {
    wxArrayString QueryArray = ParseQueries(strQuery);

    wxArrayString::iterator start = QueryArray.begin();
    wxArrayString::iterator stop = QueryArray.end();

    while (start != stop)
    {
      /*
      wxCharBuffer sqlBuffer = ConvertToUnicodeStream((*start));
      std::string strSQL(sqlBuffer);
      oracle::occi::Statement* pOracleStatement = m_pDatabase->createStatement(strSQL);
      if (pOracleStatement)
      {
        pOracleStatement->setAutoCommit(m_bAutoCommit);
        pStatement->AddStatement(pOracleStatement);
      }
      */
      OraclePreparedStatementWrapper* pStatementWrapper = new OraclePreparedStatementWrapper(m_pDatabase, (*start));
      if (pStatementWrapper)
      {
        oracle::occi::Statement* pOracleStatement = pStatementWrapper->GetStatement();
        if (pOracleStatement)
        {
          pOracleStatement->setAutoCommit(m_bAutoCommit);
          pStatement->AddStatement(pStatementWrapper);
        }
      }

      start++;
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    wxDELETE(pStatement);
    ThrowDatabaseException();
    return NULL;
  }

  return pStatement;
}

bool OracleDatabaseLayer::TableExists(const wxString& table)
{
  // Placeholder code stub
  return false;
}

bool OracleDatabaseLayer::ViewExists(const wxString& view)
{
  // Placeholder code stub
  return false;
}

wxArrayString OracleDatabaseLayer::GetTables()
{
  // Placeholder code stub
  wxArrayString returnArray;
  return returnArray;
}

wxArrayString OracleDatabaseLayer::GetViews()
{
  // Placeholder code stub
  wxArrayString returnArray;
  return returnArray;
}

wxArrayString OracleDatabaseLayer::GetColumns(const wxString& table)
{
  // Placeholder code stub
  wxArrayString returnArray;
  return returnArray;
}

int OracleDatabaseLayer::TranslateErrorCode(int nCode)
{
  // Ultimately, this will probably be a map of SQLite database error code values to DatabaseLayer values
  // For now though, we'll just return error
  int nReturn = nCode;
  return nReturn;
}

