#include "../include/SqliteDatabaseLayer.h"

#include "sqlite3.h"

#include "../include/SqliteResultSet.h"
#include "../include/SqlitePreparedStatement.h"
#include "../include/DatabaseErrorCodes.h"
#include "../include/DatabaseLayerException.h"

#include <wx/tokenzr.h>
#include <wx/filename.h>

// ctor()
SqliteDatabaseLayer::SqliteDatabaseLayer()
 : DatabaseLayer()
{
  m_pDatabase = NULL; //&m_Database; //new sqlite3;
  wxCSConv conv(_("UTF-8"));
  SetEncoding(&conv);
}

SqliteDatabaseLayer::SqliteDatabaseLayer(const wxString& strDatabase, bool mustExist /*= false*/)
 : DatabaseLayer()
{
  m_pDatabase = NULL; //new sqlite3;
  wxCSConv conv(_("UTF-8"));
  SetEncoding(&conv);
  Open(strDatabase, mustExist);
}

// dtor()
SqliteDatabaseLayer::~SqliteDatabaseLayer()
{
  //wxPrintf(_("~SqliteDatabaseLayer()\n"));
  Close();
  //wxDELETE(m_pDatabase);
}

// open database
bool SqliteDatabaseLayer::Open(const wxString& strDatabase, bool mustExist)
{
  if (strDatabase!= _(":memory:") && // :memory: is a special SQLite in-memory database
    mustExist && !(wxFileName::FileExists(strDatabase)))
  {
    SetErrorCode(DATABASE_LAYER_ERROR);
    SetErrorMessage(_("The specified database file '") + strDatabase + _("' does not exist."));
    ThrowDatabaseException();
    return false;
  }
  return Open(strDatabase);
}

bool SqliteDatabaseLayer::Open(const wxString& strDatabase)
{
  ResetErrorCodes();

  //if (m_pDatabase == NULL)
  //  m_pDatabase = new sqlite3;

  wxCharBuffer databaseNameBuffer = ConvertToUnicodeStream(strDatabase);
  sqlite3* pDbPtr = (sqlite3*)m_pDatabase;
  int nReturn = sqlite3_open(databaseNameBuffer, &pDbPtr);
  m_pDatabase = pDbPtr;
  if (nReturn != SQLITE_OK)
  {
    SetErrorCode(SqliteDatabaseLayer::TranslateErrorCode(sqlite3_errcode((sqlite3*)m_pDatabase)));
    SetErrorMessage(ConvertFromUnicodeStream(sqlite3_errmsg((sqlite3*)m_pDatabase)));
    ThrowDatabaseException();
    return false;
  }
  return true;
}

// close database  
bool SqliteDatabaseLayer::Close()
{
  ResetErrorCodes();

  CloseResultSets();
  CloseStatements();

  if (m_pDatabase != NULL)
  {
    int nReturn = sqlite3_close((sqlite3*)m_pDatabase);
    if (nReturn != SQLITE_OK)
    {
      SetErrorCode(SqliteDatabaseLayer::TranslateErrorCode(sqlite3_errcode((sqlite3*)m_pDatabase)));
      SetErrorMessage(ConvertFromUnicodeStream(sqlite3_errmsg((sqlite3*)m_pDatabase)));
      ThrowDatabaseException();
      return false;
    }
    m_pDatabase = NULL;
  }

  return true;
}

bool SqliteDatabaseLayer::IsOpen()
{
  return (m_pDatabase != NULL);
}

void SqliteDatabaseLayer::BeginTransaction()
{
  wxLogDebug(_("Beginning transaction"));
  RunQuery(_("begin transaction;"), false);
}

void SqliteDatabaseLayer::Commit()
{
  wxLogDebug(_("Commiting transaction"));
  RunQuery(_("commit transaction;"), false);
}

void SqliteDatabaseLayer::RollBack()
{
  wxLogDebug(_("Rolling back transaction"));
  RunQuery(_("rollback transaction;"), false);
}

// query database
int SqliteDatabaseLayer::RunQuery(const wxString& strQuery, bool bParseQuery)
{
  ResetErrorCodes();

  if (m_pDatabase == NULL)
    return false;

  wxArrayString QueryArray;
  if (bParseQuery)
    QueryArray = ParseQueries(strQuery);
  else
    QueryArray.push_back(strQuery);

  wxArrayString::iterator start = QueryArray.begin();
  wxArrayString::iterator stop = QueryArray.end();

  while (start != stop)
  {
    char* szErrorMessage = NULL;
    wxString strErrorMessage = _("");
    wxCharBuffer sqlBuffer = ConvertToUnicodeStream(*start);
    int nReturn = sqlite3_exec((sqlite3*)m_pDatabase, sqlBuffer, 0, 0, &szErrorMessage);
  
    if (szErrorMessage != NULL)
    {
      strErrorMessage = ConvertFromUnicodeStream(szErrorMessage);
      sqlite3_free(szErrorMessage);
    }

    if (nReturn != SQLITE_OK)
    {
      SetErrorCode(SqliteDatabaseLayer::TranslateErrorCode(sqlite3_errcode((sqlite3*)m_pDatabase)));
      SetErrorMessage(strErrorMessage);
      ThrowDatabaseException();
      return DATABASE_LAYER_QUERY_RESULT_ERROR;
    }

    start++;
  }
  return (sqlite3_changes((sqlite3*)m_pDatabase));
}

DatabaseResultSet* SqliteDatabaseLayer::RunQueryWithResults(const wxString& strQuery)
{
  ResetErrorCodes();

  if (m_pDatabase != NULL)
  {
    wxArrayString QueryArray = ParseQueries(strQuery);
     
    for (unsigned int i=0; i<(QueryArray.size()-1); i++)
    {
      char* szErrorMessage = NULL;
      wxString strErrorMessage = _("");
      wxCharBuffer sqlBuffer = ConvertToUnicodeStream(QueryArray[i]);
      int nReturn = sqlite3_exec((sqlite3*)m_pDatabase, sqlBuffer, 0, 0, &szErrorMessage);
  
      if (szErrorMessage != NULL)
      {
        SetErrorCode(SqliteDatabaseLayer::TranslateErrorCode(sqlite3_errcode((sqlite3*)m_pDatabase)));
        strErrorMessage = ConvertFromUnicodeStream(szErrorMessage);
        sqlite3_free(szErrorMessage);
        return NULL;
      }

      if (nReturn != SQLITE_OK)
      {
        SetErrorCode(SqliteDatabaseLayer::TranslateErrorCode(sqlite3_errcode((sqlite3*)m_pDatabase)));
        SetErrorMessage(strErrorMessage);
        ThrowDatabaseException();
        return NULL;
      }
    }

    // Create a Prepared statement for the last SQL statement and get a result set from it
    SqlitePreparedStatement* pStatement = (SqlitePreparedStatement*)PrepareStatement(QueryArray[QueryArray.size()-1], false);
    SqliteResultSet* pResultSet = new SqliteResultSet(pStatement, true);
    if (pResultSet)
      pResultSet->SetEncoding(GetEncoding());

    LogResultSetForCleanup(pResultSet);
    return pResultSet;
  }
  else
  {
    return NULL;
  }
}

PreparedStatement* SqliteDatabaseLayer::PrepareStatement(const wxString& strQuery)
{
  return PrepareStatement(strQuery, true);
}

PreparedStatement* SqliteDatabaseLayer::PrepareStatement(const wxString& strQuery, bool bLogForCleanup)
{
  ResetErrorCodes();

  if (m_pDatabase != NULL)
  {
    SqlitePreparedStatement* pReturnStatement = new SqlitePreparedStatement((sqlite3*)m_pDatabase);
    if (pReturnStatement)
      pReturnStatement->SetEncoding(GetEncoding());
    
    wxArrayString QueryArray = ParseQueries(strQuery);

    wxArrayString::iterator start = QueryArray.begin();
    wxArrayString::iterator stop = QueryArray.end();

    while (start != stop)
    {
      const char* szTail=0;
      wxCharBuffer sqlBuffer;
      do
      {
        sqlite3_stmt* pStatement;
        wxString strSQL;
        if (szTail != 0)
        {
          strSQL = (wxChar*)szTail;
        }
        else
        {
          strSQL = (*start);
        }
        sqlBuffer = ConvertToUnicodeStream(strSQL);
#if SQLITE_VERSION_NUMBER>=3003009
        int nReturn = sqlite3_prepare_v2((sqlite3*)m_pDatabase, sqlBuffer, -1, &pStatement, &szTail);
#else
        int nReturn = sqlite3_prepare((sqlite3*)m_pDatabase, sqlBuffer, -1, &pStatement, &szTail);
#endif
   
        if (nReturn != SQLITE_OK)
        {
          SetErrorCode(SqliteDatabaseLayer::TranslateErrorCode(nReturn));
          SetErrorMessage(ConvertFromUnicodeStream(sqlite3_errmsg((sqlite3*)m_pDatabase)));
          wxDELETE(pReturnStatement);
          ThrowDatabaseException();
          return NULL;
        }
        pReturnStatement->AddPreparedStatement(pStatement);

#if wxUSE_UNICODE
      } while (strlen(szTail) > 0);
#else
      } while (wxStrlen(szTail) > 0);
#endif    
      
      start++;
    }

    if (bLogForCleanup)
      LogStatementForCleanup(pReturnStatement);
    return pReturnStatement;
  }
  else
  {
    return NULL;
  }
}

bool SqliteDatabaseLayer::TableExists(const wxString& table)
{
  // Initialize variables
  bool bReturn = false;
  // Keep these variables outside of scope so that we can clean them up
  //  in case of an error
  PreparedStatement* pStatement = NULL;
  DatabaseResultSet* pResult = NULL;

#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    wxString query = _("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name=?;");
    pStatement = PrepareStatement(query);
    if (pStatement)
    {
      pStatement->SetParamString(1, table);
      pResult = pStatement->ExecuteQuery();
      if (pResult)
      {
        if (pResult->Next())
        {
          if(pResult->GetResultInt(1) != 0)
          {
            bReturn = true;
          }
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

bool SqliteDatabaseLayer::ViewExists(const wxString& view)
{
  // Initialize variables
  bool bReturn = false;
  // Keep these variables outside of scope so that we can clean them up
  //  in case of an error
  PreparedStatement* pStatement = NULL;
  DatabaseResultSet* pResult = NULL;

#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    wxString query = _("SELECT COUNT(*) FROM sqlite_master WHERE type='view' AND name=?;");
    pStatement = PrepareStatement(query);
    if (pStatement)
    {
      pStatement->SetParamString(1, view);
      pResult = pStatement->ExecuteQuery();
      if (pResult)
      {
        if (pResult->Next())
        {
          if(pResult->GetResultInt(1) != 0)
          {
            bReturn = true;
          }
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

wxArrayString SqliteDatabaseLayer::GetTables()
{
  wxArrayString returnArray;

  DatabaseResultSet* pResult = NULL;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    wxString query = _("SELECT name FROM sqlite_master WHERE type='table';");
    pResult = ExecuteQuery(query);

    while (pResult->Next())
    {
      returnArray.Add(pResult->GetResultString(1));
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

wxArrayString SqliteDatabaseLayer::GetViews()
{
  wxArrayString returnArray;

  DatabaseResultSet* pResult = NULL;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    wxString query = _("SELECT name FROM sqlite_master WHERE type='view';");
    pResult = ExecuteQuery(query);

    while (pResult->Next())
    {
      returnArray.Add(pResult->GetResultString(1));
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

wxArrayString SqliteDatabaseLayer::GetColumns(const wxString& table)
{
    wxArrayString returnArray;

  // Keep these variables outside of scope so that we can clean them up
  //  in case of an error
  DatabaseResultSet* pResult = NULL;
  ResultSetMetaData* pMetaData = NULL;

#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    wxCharBuffer tableNameBuffer = ConvertToUnicodeStream(table);
    wxString query = wxString::Format(_("SELECT * FROM '%s' LIMIT 0;"), table.c_str());
    pResult = ExecuteQuery(query);
    pResult->Next();
    pMetaData = pResult->GetMetaData();

    // 1-based
    for(int i=1; i<=pMetaData->GetColumnCount(); i++)
    {
      returnArray.Add(pMetaData->GetColumnName(i));
    }

#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  }
  catch (DatabaseLayerException& e)
  {
    if (pMetaData != NULL)
    {
      pResult->CloseMetaData(pMetaData);
      pMetaData = NULL;
    }

    if (pResult != NULL)
    {
      CloseResultSet(pResult);
      pResult = NULL;
    }

    throw e;
  }
#endif

  if (pMetaData != NULL)
  {
    pResult->CloseMetaData(pMetaData);
    pMetaData = NULL;
  }

  if (pResult != NULL)
  {
    CloseResultSet(pResult);
    pResult = NULL;
  }

  return returnArray;
}

int SqliteDatabaseLayer::TranslateErrorCode(int nCode)
{
  // Ultimately, this will probably be a map of SQLite database error code values to DatabaseLayer values
  // For now though, we'll just return error
  int nReturn = nCode;
  /*
  switch (nCode)
  {
    case SQLITE_ERROR:
      nReturn = DATABASE_LAYER_SQL_SYNTAX_ERROR;
      break;
    case SQLITE_INTERNAL:
      nReturn = DATABASE_LAYER_ERROR;
      break;
    case SQLITE_PERM:
      nReturn = DATABASE_LAYER_ERROR;
      break;
    case SQLITE_ABORT:
      nReturn = DATABASE_LAYER_ERROR;
      break;
    case SQLITE_BUSY:
      nReturn = DATABASE_LAYER_ERROR;
      break;
    case SQLITE_LOCKED:
      nReturn = DATABASE_LAYER_ERROR;
      break;
    case SQLITE_NOMEM:
      nReturn = DATABASE_LAYER_ALLOCATION_ERROR;
      break;
    case SQLITE_READONLY:
      nReturn = DATABASE_LAYER_ERROR;
      break;
    case SQLITE_INTERRUPT:
      nReturn = DATABASE_LAYER_ERROR;
      break;
    case SQLITE_IOERR:
      nReturn = DATABASE_LAYER_ERROR;
      break;
    case SQLITE_CORRUPT:
      nReturn = DATABASE_LAYER_ERROR;
      break;
    case SQLITE_NOTFOUND:
      nReturn = DATABASE_LAYER_ERROR;
      break;
    case SQLITE_FULL:
      nReturn = DATABASE_LAYER_ERROR;
      break;
    case SQLITE_CANTOPEN:
      nReturn = DATABASE_LAYER_ERROR;
      break;
    case SQLITE_PROTOCOL:
      nReturn = DATABASE_LAYER_ERROR;
      break;
    case SQLITE_SCHEMA:
      nReturn = DATABASE_LAYER_ERROR;
      break;
    case SQLITE_TOOBIG:
      nReturn = DATABASE_LAYER_ERROR;
      break;
    case SQLITE_CONSTRAINT:
      nReturn = DATABASE_LAYER_CONSTRAINT_VIOLATION;
      break;
    case SQLITE_MISMATCH:
      nReturn = DATABASE_LAYER_INCOMPATIBLE_FIELD_TYPE;
      break;
    case SQLITE_MISUSE:
      nReturn = DATABASE_LAYER_ERROR;
      break;
    case SQLITE_NOLFS:
      nReturn = DATABASE_LAYER_ERROR;
      break;
    case SQLITE_AUTH:
      nReturn = DATABASE_LAYER_ERROR;
      break;
    default:
      nReturn = DATABASE_LAYER_ERROR;
      break;
  }
  */
  return nReturn;
}

