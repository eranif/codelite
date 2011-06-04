#include "../include/MysqlDatabaseLayer.h"

#include "mysql.h"

#include "../include/MysqlInterface.h"
#include "../include/MysqlPreparedStatement.h"
#include "../include/MysqlPreparedStatementResultSet.h"
#include "../include/DatabaseErrorCodes.h"
#include "../include/DatabaseLayerException.h"

#include <wx/tokenzr.h>

// ctor
MysqlDatabaseLayer::MysqlDatabaseLayer()
 : DatabaseLayer()
{
#ifndef DONT_USE_DYNAMIC_DATABASE_LAYER_LINKING
  m_pInterface = new MysqlInterface();
  if (!m_pInterface->Init())
  {
    SetErrorCode(DATABASE_LAYER_ERROR_LOADING_LIBRARY);
    SetErrorMessage(wxT("Error loading MySQL library"));
    ThrowDatabaseException();
    return;
  }
#endif
  InitDatabase();
  m_strServer = _("localhost");
  m_iPort = 3306; // default
  m_strDatabase = _("");
  m_strUser = _("");
  m_strPassword = _("");
}

MysqlDatabaseLayer::MysqlDatabaseLayer(const wxString& strDatabase)
 : DatabaseLayer()
{
#ifndef DONT_USE_DYNAMIC_DATABASE_LAYER_LINKING
  m_pInterface = new MysqlInterface();
  if (!m_pInterface->Init())
  {
    SetErrorCode(DATABASE_LAYER_ERROR_LOADING_LIBRARY);
    SetErrorMessage(wxT("Error loading MySQL library"));
    ThrowDatabaseException();
    return;
  }
#endif
  InitDatabase();
  m_strServer = _("localhost");
  m_iPort = 3306; // default
  m_strUser = _("");
  m_strPassword = _("");
  Open(strDatabase);
}

MysqlDatabaseLayer::MysqlDatabaseLayer(const wxString& strServer, const wxString& strDatabase)
 : DatabaseLayer()
{
#ifndef DONT_USE_DYNAMIC_DATABASE_LAYER_LINKING
  m_pInterface = new MysqlInterface();
  if (!m_pInterface->Init())
  {
    SetErrorCode(DATABASE_LAYER_ERROR_LOADING_LIBRARY);
    SetErrorMessage(wxT("Error loading MySQL library"));
    ThrowDatabaseException();
    return;
  }
#endif
  InitDatabase();
  ParseServerAndPort(strServer);
  m_strUser = _("");
  m_strPassword = _("");
  Open(strDatabase);
}

MysqlDatabaseLayer::MysqlDatabaseLayer(const wxString& strDatabase, const wxString& strUser, const wxString& strPassword)
 : DatabaseLayer()
{
#ifndef DONT_USE_DYNAMIC_DATABASE_LAYER_LINKING
  m_pInterface = new MysqlInterface();
  if (!m_pInterface->Init())
  {
    SetErrorCode(DATABASE_LAYER_ERROR_LOADING_LIBRARY);
    SetErrorMessage(wxT("Error loading MySQL library"));
    ThrowDatabaseException();
    return;
  }
#endif
  InitDatabase();
  m_strServer = _("localhost");
  m_iPort = 3306; // default
  m_strUser = strUser;
  m_strPassword = strPassword;
  Open(strDatabase);
}

MysqlDatabaseLayer::MysqlDatabaseLayer(const wxString& strServer, const wxString& strDatabase, const wxString& strUser, const wxString& strPassword)
 : DatabaseLayer()
{
#ifndef DONT_USE_DYNAMIC_DATABASE_LAYER_LINKING
  m_pInterface = new MysqlInterface();
  if (!m_pInterface->Init())
  {
    SetErrorCode(DATABASE_LAYER_ERROR_LOADING_LIBRARY);
    SetErrorMessage(wxT("Error loading MySQL library5"));
    ThrowDatabaseException();
    return;
  }
#endif
  InitDatabase();
  ParseServerAndPort(strServer);
  m_strUser = strUser;
  m_strPassword = strPassword;
  Open(strDatabase);
}

// dtor
MysqlDatabaseLayer::~MysqlDatabaseLayer()
{
  Close();
  //m_pInterface->GetMysqlClose()(m_pDatabase);
  //delete m_pDatabase;
  //m_pInterface->GetMysqlLibraryEnd()();
  m_pInterface->GetMysqlServerEnd()();
  wxDELETE(m_pInterface);
}

// open database
void MysqlDatabaseLayer::InitDatabase()
{
  //char *server_options[] = { "mysql_test", "--defaults-file=my.cnf" };
  //int num_elements = sizeof(server_options)/ sizeof(char *);

  //char *server_groups[] = { "libmysqld_server", "libmysqld_client" };
  //m_pInterface->GetMysqlServerInit()(num_elements, server_options, server_groups);
  //m_pDatabase = new MYSQL();
  //m_pInterface->GetMysqlLibraryInit()();
  //m_pInterface->GetMysqlInit()(m_pDatabase);
//  m_pInterface->GetMysqlServerInit()( 0, NULL, NULL );
  m_pDatabase = m_pInterface->GetMysqlInit()(NULL);
}

// open database
bool MysqlDatabaseLayer::Open(const wxString& strServer, const wxString& strDatabase)
{
  ParseServerAndPort(strServer);
  return Open(strDatabase);
}

bool MysqlDatabaseLayer::Open(const wxString& strDatabase, const wxString& strUser, const wxString& strPassword)
{
  m_strUser = strUser;
  m_strPassword = strPassword;
  return Open(strDatabase);
}

bool MysqlDatabaseLayer::Open(const wxString& strServer, const wxString& strDatabase, const wxString& strUser, const wxString& strPassword)
{
  ParseServerAndPort(strServer);
  m_strUser = strUser;
  m_strPassword = strPassword;
  return Open(strDatabase);
}

bool MysqlDatabaseLayer::Open(const wxString& strDatabase)
{
  m_strDatabase = strDatabase;
  
  wxCharBuffer serverCharBuffer = ConvertToUnicodeStream(m_strServer);
  wxCharBuffer userCharBuffer = ConvertToUnicodeStream(m_strUser);
  wxCharBuffer passwordCharBuffer = ConvertToUnicodeStream(m_strPassword);
  wxCharBuffer databaseNameCharBuffer = ConvertToUnicodeStream(m_strDatabase);
  long connectFlags = 0; 
#if MYSQL_VERSION_ID >= 40100 
  #if !defined ulong 
    #define ulong unsigned long 
  #endif 
  connectFlags |= CLIENT_MULTI_RESULTS; 
  connectFlags |= CLIENT_MULTI_STATEMENTS; 
#endif 
   if (m_pInterface->GetMysqlRealConnect()((MYSQL*)m_pDatabase, serverCharBuffer, userCharBuffer, passwordCharBuffer, databaseNameCharBuffer, m_iPort, NULL/*socket*/, connectFlags) != NULL)   
  {
#if wxUSE_UNICODE
    const char* sqlStatement = "SET CHARACTER_SET_CLIENT=utf8, "
                 "CHARACTER_SET_CONNECTION=utf8, "
                 "CHARACTER_SET_RESULTS=utf8;";

    m_pInterface->GetMysqlRealQuery()((MYSQL*)m_pDatabase, sqlStatement, strlen(sqlStatement));
    wxCSConv conv(_("UTF-8"));
    SetEncoding(&conv);
#endif

    return true;
  }
  else
  {
    SetErrorCode(MysqlDatabaseLayer::TranslateErrorCode(m_pInterface->GetMysqlErrno()((MYSQL*)m_pDatabase)));
    SetErrorMessage(ConvertFromUnicodeStream(m_pInterface->GetMysqlError()((MYSQL*)m_pDatabase)));
    ThrowDatabaseException();
    return false;
  }
}

void MysqlDatabaseLayer::ParseServerAndPort(const wxString& strServer)
{
  int portIndicator = strServer.Find(_(":"));
  if (portIndicator > -1)
  {
    m_strServer = strServer.SubString(0, portIndicator-1);
    m_iPort = wxAtoi(strServer.SubString(portIndicator+1, strServer.Length()-1));
  }
  else
  {
    m_strServer = strServer;
    m_iPort = 3306; // default
  }
}

// close database
bool MysqlDatabaseLayer::Close()
{
  CloseResultSets();
  CloseStatements();

  ResetErrorCodes();
  if (m_pDatabase)
  {
    m_pInterface->GetMysqlClose()((MYSQL*)m_pDatabase);
    m_pDatabase = NULL;
  }
//  m_pInterface->GetMysqlServerEnd()();
  //delete m_pDatabase;
  //m_pDatabase = NULL;
  return true;
}

  
bool MysqlDatabaseLayer::IsOpen()
{
  return (m_pDatabase != NULL);
}

// transaction support
void MysqlDatabaseLayer::BeginTransaction()
{
  ResetErrorCodes();

  int nReturn = m_pInterface->GetMysqlAutoCommit()((MYSQL*)m_pDatabase, 0);
  if (nReturn != 0)
  {
    SetErrorCode(MysqlDatabaseLayer::TranslateErrorCode(m_pInterface->GetMysqlErrno()((MYSQL*)m_pDatabase)));
    SetErrorMessage(ConvertFromUnicodeStream(m_pInterface->GetMysqlError()((MYSQL*)m_pDatabase)));
    ThrowDatabaseException();
  }
}

void MysqlDatabaseLayer::Commit()
{
  ResetErrorCodes();

  int nReturn = m_pInterface->GetMysqlCommit()((MYSQL*)m_pDatabase);
  if (nReturn != 0)
  {
    SetErrorCode(MysqlDatabaseLayer::TranslateErrorCode(m_pInterface->GetMysqlErrno()((MYSQL*)m_pDatabase)));
    SetErrorMessage(ConvertFromUnicodeStream(m_pInterface->GetMysqlError()((MYSQL*)m_pDatabase)));
    ThrowDatabaseException();
  }
  nReturn = m_pInterface->GetMysqlAutoCommit()((MYSQL*)m_pDatabase, 1);
  if (nReturn != 0)
  {
    SetErrorCode(MysqlDatabaseLayer::TranslateErrorCode(m_pInterface->GetMysqlErrno()((MYSQL*)m_pDatabase)));
    SetErrorMessage(ConvertFromUnicodeStream(m_pInterface->GetMysqlError()((MYSQL*)m_pDatabase)));
    ThrowDatabaseException();
  }
}

void MysqlDatabaseLayer::RollBack()
{
  ResetErrorCodes();

  int nReturn = m_pInterface->GetMysqlRollback()((MYSQL*)m_pDatabase);
  if (nReturn != 0)
  {
    SetErrorCode(MysqlDatabaseLayer::TranslateErrorCode(m_pInterface->GetMysqlErrno()((MYSQL*)m_pDatabase)));
    SetErrorMessage(ConvertFromUnicodeStream(m_pInterface->GetMysqlError()((MYSQL*)m_pDatabase)));
    ThrowDatabaseException();
  }
  nReturn = m_pInterface->GetMysqlAutoCommit()((MYSQL*)m_pDatabase, 1);
  if (nReturn != 0)
  {
    SetErrorCode(MysqlDatabaseLayer::TranslateErrorCode(m_pInterface->GetMysqlErrno()((MYSQL*)m_pDatabase)));
    SetErrorMessage(ConvertFromUnicodeStream(m_pInterface->GetMysqlError()((MYSQL*)m_pDatabase)));
    ThrowDatabaseException();
  }
}

  
// query database
int MysqlDatabaseLayer::RunQuery(const wxString& strQuery, bool bParseQuery)
{
  ResetErrorCodes();

  wxArrayString QueryArray;
  if (bParseQuery)
    QueryArray = ParseQueries(strQuery);
  else
    QueryArray.push_back(strQuery);

  wxArrayString::iterator start = QueryArray.begin();
  wxArrayString::iterator stop = QueryArray.end();

  while (start != stop)
  {
    wxCharBuffer sqlBuffer = ConvertToUnicodeStream(*start);
    //puts(sqlBuffer);
    int nReturn = m_pInterface->GetMysqlQuery()((MYSQL*)m_pDatabase, sqlBuffer);
    if (nReturn != 0)
    {
      SetErrorCode(MysqlDatabaseLayer::TranslateErrorCode(m_pInterface->GetMysqlErrno()((MYSQL*)m_pDatabase)));
      SetErrorMessage(ConvertFromUnicodeStream(m_pInterface->GetMysqlError()((MYSQL*)m_pDatabase)));
      ThrowDatabaseException();
      return DATABASE_LAYER_QUERY_RESULT_ERROR;
    }
    start++;
  }
  return m_pInterface->GetMysqlAffectedRows()((MYSQL*)m_pDatabase);
}

DatabaseResultSet* MysqlDatabaseLayer::RunQueryWithResults(const wxString& strQuery)
{
  ResetErrorCodes();

  wxArrayString QueryArray = ParseQueries(strQuery);

  int nArraySize = QueryArray.size();
  MysqlPreparedStatementResultSet* pResultSet = NULL;
  for (int i=0; i<nArraySize; i++)
  {
    wxString strCurrentQuery = QueryArray[i];
    MYSQL_STMT* pMysqlStatement = m_pInterface->GetMysqlStmtInit()((MYSQL*)m_pDatabase);
    if (pMysqlStatement != NULL)
    {
      wxCharBuffer sqlBuffer = ConvertToUnicodeStream(strCurrentQuery);
      //puts(sqlBuffer);
      wxString sqlUTF8((const char*)sqlBuffer, wxConvUTF8);
      if (m_pInterface->GetMysqlStmtPrepare()(pMysqlStatement, sqlBuffer, sqlUTF8.Length()) == 0)
      {
        int nReturn = m_pInterface->GetMysqlStmtExecute()(pMysqlStatement);
        if (nReturn != 0)
        {
          SetErrorCode(MysqlDatabaseLayer::TranslateErrorCode(m_pInterface->GetMysqlStmtErrno()(pMysqlStatement)));
          SetErrorMessage(ConvertFromUnicodeStream(m_pInterface->GetMysqlStmtError()(pMysqlStatement)));

          // Clean up after ourselves
          m_pInterface->GetMysqlStmtFreeResult()(pMysqlStatement);
          m_pInterface->GetMysqlStmtClose()(pMysqlStatement);

          ThrowDatabaseException();
          return NULL;
        }
      }
      else
      {
        SetErrorCode(MysqlDatabaseLayer::TranslateErrorCode(m_pInterface->GetMysqlErrno()((MYSQL*)m_pDatabase)));
        SetErrorMessage(ConvertFromUnicodeStream(m_pInterface->GetMysqlError()((MYSQL*)m_pDatabase)));
        ThrowDatabaseException();
      }
      if (i == nArraySize-1)
      {
        pResultSet = new MysqlPreparedStatementResultSet(m_pInterface, pMysqlStatement, true);
        if (pResultSet)
          pResultSet->SetEncoding(GetEncoding());
#if wxUSE_UNICODE
        //wxPrintf(_("Allocating statement at %d\n"), pMysqlStatement);
       // m_ResultSets[pResultSet] = pMysqlStatement;
#endif
        LogResultSetForCleanup(pResultSet);
        return pResultSet;
      }

      m_pInterface->GetMysqlStmtFreeResult()(pMysqlStatement);
      m_pInterface->GetMysqlStmtClose()(pMysqlStatement);
    }
    else
    {
      SetErrorCode(MysqlDatabaseLayer::TranslateErrorCode(m_pInterface->GetMysqlErrno()((MYSQL*)m_pDatabase)));
      SetErrorMessage(ConvertFromUnicodeStream(m_pInterface->GetMysqlError()((MYSQL*)m_pDatabase)));
      ThrowDatabaseException();
      return NULL;
    }
  }
  LogResultSetForCleanup(pResultSet);
  return pResultSet;
}

// PreparedStatement support
PreparedStatement* MysqlDatabaseLayer::PrepareStatement(const wxString& strQuery)
{
  ResetErrorCodes();

  wxArrayString QueryArray = ParseQueries(strQuery);

  wxArrayString::iterator start = QueryArray.begin();
  wxArrayString::iterator stop = QueryArray.end();

  MysqlPreparedStatement* pStatement = new MysqlPreparedStatement(m_pInterface);
  if (pStatement)
    pStatement->SetEncoding(GetEncoding());
  while (start != stop)
  {
    MYSQL_STMT* pMysqlStatement = m_pInterface->GetMysqlStmtInit()((MYSQL*)m_pDatabase);
    if (pMysqlStatement != NULL)
    {
      wxCharBuffer sqlBuffer = ConvertToUnicodeStream((*start));
      //puts(sqlBuffer);
      if (m_pInterface->GetMysqlStmtPrepare()(pMysqlStatement, sqlBuffer, GetEncodedStreamLength((*start))) == 0)
      {
        pStatement->AddPreparedStatement(pMysqlStatement);
      }
      else
      {
        SetErrorCode(MysqlDatabaseLayer::TranslateErrorCode(m_pInterface->GetMysqlErrno()((MYSQL*)m_pDatabase)));
        SetErrorMessage(ConvertFromUnicodeStream(m_pInterface->GetMysqlError()((MYSQL*)m_pDatabase)));
        ThrowDatabaseException();
      }
    }
    else
    {
      SetErrorCode(MysqlDatabaseLayer::TranslateErrorCode(m_pInterface->GetMysqlErrno()((MYSQL*)m_pDatabase)));
      SetErrorMessage(ConvertFromUnicodeStream(m_pInterface->GetMysqlError()((MYSQL*)m_pDatabase)));
      ThrowDatabaseException();
      return NULL;
    }
    start++;
  }
  LogStatementForCleanup(pStatement);
  return pStatement;
}

bool MysqlDatabaseLayer::TableExists(const wxString& table)
{
  bool bReturn = false;
/*  
  // This is the way that I'd prefer to retrieve the list of tables
  // Unfortunately MySQL returns both tables and view together
  // So we have to try a SQL call (which may be MySQL version dependent)
  wxCharBuffer tableBuffer = ConvertToUnicodeStream(table);
  MYSQL_RES* pResults = m_pInterface->GetMysqlListTables()(m_pDatabase, tableBuffer);
  if (pResults != NULL)
  {
    MYSQL_ROW currentRow = NULL;
    while ((currentRow = m_Interfce.GetMysqlFetchRow()(pResults)) != NULL)
    {
      wxString strTable = ConvertFromUnicodeStream(currentRow[0]);
      if (strTable == table)
        bReturn = true;
    }
    m_pInterface->GetMysqlFreeResult()(pResults);
  }
*/
  // Keep these variables outside of scope so that we can clean them up
  //  in case of an error
  PreparedStatement* pStatement = NULL;
  DatabaseResultSet* pResult = NULL;

#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    wxString query = _("SHOW TABLE STATUS WHERE Comment != 'VIEW' AND Name=?;");
    pStatement = PrepareStatement(query);
    if (pStatement)
    {
      pStatement->SetParamString(1, table);
      pResult = pStatement->ExecuteQuery();
      if (pResult)
      {
        if (pResult->Next())
        {
          wxString strTable = pResult->GetResultString(1);
          if (table == strTable)
            bReturn = true;
        }
      }
    }
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  }
  catch (DatabaseLayerException& e)
  {
    if (pResult != NULL)
    {
      CloseResultSet(pResult);
      pResult = NULL;
    }

    if (pStatement != NULL)
    {
      CloseStatement(pStatement);
      pStatement = NULL;
    }

    throw e;
  }
#endif

  if (pResult != NULL)
  {
    CloseResultSet(pResult);
    pResult = NULL;
  }

  if (pStatement != NULL)
  {
    CloseStatement(pStatement);
    pStatement = NULL;
  }

  return bReturn;
}

bool MysqlDatabaseLayer::ViewExists(const wxString& view)
{
  bool bReturn = false;
/*  
  // This is the way that I'd prefer to retrieve the list of tables
  // Unfortunately MySQL returns both tables and view together
  // So we have to try a SQL call (which may be MySQL version dependent)
  wxCharBuffer viewBuffer = ConvertToUnicodeStream(view);
  MYSQL_RES* pResults = m_pInterface->GetMysqlListTables()(m_pDatabase, viewBuffer);
  if (pResults != NULL)
  {
    MYSQL_ROW currentRow = NULL;
    while ((currentRow = m_pInterface->GetMysqlFetchRow()(pResults)) != NULL)
    {
      wxString strView = ConvertFromUnicodeStream(currentRow[0]);
      if (strView == view)
        bReturn = true;
    }
    m_pInterface->GetMysqlFreeResult()(pResults);
  }
*/
  // Keep these variables outside of scope so that we can clean them up
  //  in case of an error
  PreparedStatement* pStatement = NULL;
  DatabaseResultSet* pResult = NULL;

#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    wxString query = _("SHOW TABLE STATUS WHERE Comment = 'VIEW' AND Name=?;");
    pStatement = PrepareStatement(query);
    if (pStatement)
    {
      pStatement->SetParamString(1, view);
      pResult = pStatement->ExecuteQuery();
      if (pResult)
      {
        if (pResult->Next())
        {
          wxString strView = pResult->GetResultString(1);
          if (view == strView)
            bReturn = true;
        }
      }
    }
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  }
  catch (DatabaseLayerException& e)
  {
    if (pResult != NULL)
    {
      CloseResultSet(pResult);
      pResult = NULL;
    }

    if (pStatement != NULL)
    {
      CloseStatement(pStatement);
      pStatement = NULL;
    }

    throw e;
  }
#endif

  if (pResult != NULL)
  {
    CloseResultSet(pResult);
    pResult = NULL;
  }

  if (pStatement != NULL)
  {
    CloseStatement(pStatement);
    pStatement = NULL;
  }

  return bReturn;
}

wxArrayString MysqlDatabaseLayer::GetTables()
{
  wxArrayString returnArray;

  if (m_pInterface->GetMysqlGetServerVersion()((MYSQL*)m_pDatabase) >= 50010)
  {
    DatabaseResultSet* pResult = NULL;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
    try
    {
#endif
      wxString query = _("SHOW TABLE STATUS WHERE Comment != 'VIEW';");
      pResult = ExecuteQuery(query);

      while (pResult->Next())
      {
        wxString table = pResult->GetResultString(1).Trim();
        if (!table.IsEmpty())
          returnArray.Add(table);
      }
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
    }
    catch (DatabaseLayerException& e)
    {
      if (pResult != NULL)
      {
        CloseResultSet(pResult);
        pResult = NULL;
      }

      throw e;
    }
#endif

    if (pResult != NULL)
    {
      CloseResultSet(pResult);
      pResult = NULL;
    }
  }

  // If no tables have been found, then try the MySQL API directly
  //  This may pick up VIEWS as well as tables unfortunately
  if (returnArray.Count() == 0)
  {
    MYSQL_RES* pResults = m_pInterface->GetMysqlListTables()((MYSQL*)m_pDatabase, NULL);
    if (pResults != NULL)
    {
      MYSQL_ROW currentRow = NULL;
      while ((currentRow = m_pInterface->GetMysqlFetchRow()(pResults)) != NULL)
      {
        wxString strTable = ConvertFromUnicodeStream(currentRow[0]);
        returnArray.Add(strTable);
      }
      m_pInterface->GetMysqlFreeResult()(pResults);
    }
  }

  return returnArray;
}

wxArrayString MysqlDatabaseLayer::GetViews()
{
  wxArrayString returnArray;

  if (m_pInterface->GetMysqlGetServerVersion()((MYSQL*)m_pDatabase) >= 50010)
  {
    DatabaseResultSet* pResult = NULL;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
    try
    {
#endif
      wxString query = _("SHOW TABLE STATUS WHERE Comment = 'VIEW';");
      pResult = ExecuteQuery(query);

      while (pResult->Next())
      {
        returnArray.Add(pResult->GetResultString(1).Trim());
      }
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
    }
    catch (DatabaseLayerException& e)
    {
      if (pResult != NULL)
      {
        CloseResultSet(pResult);
        pResult = NULL;
      }

      throw e;
    }
#endif

    if (pResult != NULL)
    {
      CloseResultSet(pResult);
      pResult = NULL;
    }
  }

  return returnArray;
}

wxArrayString MysqlDatabaseLayer::GetColumns(const wxString& table)
{
  wxArrayString returnArray;
  // Keep these variables outside of scope so that we can clean them up
  //  in case of an error
  DatabaseResultSet* pResult = NULL;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    wxString query = wxString::Format(_("SHOW COLUMNS FROM %s;"), table.c_str());
    pResult = ExecuteQuery(query);

    while (pResult->Next())
    {
      returnArray.Add(pResult->GetResultString(1).Trim());
    }
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  }
  catch (DatabaseLayerException& e)
  {
    if (pResult != NULL)
    {
      CloseResultSet(pResult);
      pResult = NULL;
    }

    throw e;
  }
#endif

  if (pResult != NULL)
  {
    CloseResultSet(pResult);
    pResult = NULL;
  }

 
  return returnArray;
}

int MysqlDatabaseLayer::TranslateErrorCode(int nCode)
{
  // Ultimately, this will probably be a map of Mysql database error code values to DatabaseLayer values
  // For now though, we'll just return error
  return nCode;
  //return DATABASE_LAYER_ERROR;
}

bool MysqlDatabaseLayer::IsAvailable()
{
  bool bAvailable = false;
  MysqlInterface* pInterface = new MysqlInterface();
  bAvailable = pInterface && pInterface->Init();
  wxDELETE(pInterface);
  return bAvailable;
}

