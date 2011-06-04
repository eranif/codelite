#include "../include/TdsDatabaseLayer.h"
#include "../include/TdsResultSet.h"
#include "../include/TdsPreparedStatement.h"
#include "../include/DatabaseErrorCodes.h"
#include "../include/DatabaseLayerException.h"

#include "tdsconvert.h"
//#include <wx/tokenzr.h>

TdsContextToDatabaseLayerMap TdsDatabaseLayer::m_ContextLookupMap;

static int tsql_handle_message(const TDSCONTEXT * pContext, TDSSOCKET * pTdsSocket, TDSMESSAGE * pMessage)
{
  //fprintf(stderr, "In tsql_handle_message: %d\n", pMessage->msgno);
	if (pMessage->msgno != 5701 && pMessage->msgno != 5703 && pMessage->msgno != 20018)
  {
    TdsDatabaseLayer* pLayer = TdsDatabaseLayer::LookupTdsLayer(pContext);
    if (pLayer != NULL)
    {
      pLayer->SetError(pMessage->msgno, DatabaseStringConverter::ConvertFromUnicodeStream(pMessage->message, "UTF-8"));
    }
    else
    {
      //fprintf(stderr, "In tsql_handle_message (%d)\n", pMessage->msgno);
	    if (pMessage->msgno == 0)
      {
		    //fprintf(stderr, "%s\n", pMessage->message);
		    return 0;
	    }

		    fprintf(stderr, "Msg %d, Level %d, State %d, Server %s, Line %d\n%s\n",
			    pMessage->msgno, pMessage->severity, pMessage->state, pMessage->server, pMessage->line_number, pMessage->message);
    }
  }
	return 0;
}

static int tsql_handle_error(const TDSCONTEXT * pContext, TDSSOCKET * pTdsSocket, TDSMESSAGE * pMessage)
{
  //fprintf(stderr, "In tsql_handle_error: %d\n", pMessage->msgno);
	if (pMessage->msgno != 5701 && pMessage->msgno != 5703 && pMessage->msgno != 20018)
  {
    TdsDatabaseLayer* pLayer = TdsDatabaseLayer::LookupTdsLayer(pContext);
    if (pLayer != NULL)
    {
      pLayer->SetError(pMessage->msgno, DatabaseStringConverter::ConvertFromUnicodeStream(pMessage->message, "UTF-8"));
    }
    else
    {
      //fprintf(stderr, "In tsql_handle_error (%d)\n", pMessage->msgno);
	    if (pMessage->msgno == 0)
      {
		    //fprintf(stderr, "%s\n", pMessage->message);
		    return 0;
	    }
		  fprintf(stderr, "Msg %d, Level %d, State %d, Server %s, Line %d\n%s\n",
			  pMessage->msgno, pMessage->severity, pMessage->state, pMessage->server, pMessage->line_number, pMessage->message);
	  }
  }

	return 0;
}

wxString RemoveLastSemiColon(const wxString& strvalue)
{
	wxString result = strvalue;

	if (result.size() == 0)
		return result;	

	if (result[result.size()-1] == ';' )
	{
    result.RemoveLast();
	}	
	return result;
}


// ctor()
TdsDatabaseLayer::TdsDatabaseLayer()
 : DatabaseLayer()
{
  m_pDatabase = NULL;
  m_pLogin = NULL;
  m_pContext = NULL;
  m_nTdsVersion = TDS_80;
  wxCSConv conv(_("UTF-8"));
  SetEncoding(&conv);
}

TdsDatabaseLayer::TdsDatabaseLayer(const wxString& strServer, const wxString& strDatabase, const wxString& strUser, const wxString& strPassword, int nTdsVersion)
 : DatabaseLayer()
{
  m_pDatabase = NULL;
  m_pLogin = NULL;
  m_pContext = NULL;
  wxCSConv conv(_("UTF-8"));
  SetEncoding(&conv);
  m_strServer = strServer;
  m_strLogin = strUser;
  m_strPassword = strPassword;
  m_nTdsVersion = nTdsVersion;
  Open(strDatabase);
}

// dtor()
TdsDatabaseLayer::~TdsDatabaseLayer()
{
  //wxPrintf(_("~TdsDatabaseLayer()\n"));
  Close();
  //wxDELETE(m_pDatabase);
}

// open database
bool TdsDatabaseLayer::Open(const wxString& strDatabase)
{
  ResetErrorCodes();

  m_strDatabase = strDatabase;

  if (!Connect())
  {
    //SetErrorCode(TdsDatabaseLayer::TranslateErrorCode(???));
    //SetErrorMessage(ConvertFromUnicodeStream(???));
    return false;
  }
  
  // Change the database
  if (!RunQuery(_("USE ") + strDatabase, false))
  {
    return false;
  }

  return true;
}

// Connect to the server
bool TdsDatabaseLayer::Connect()
{
	m_pLogin = tds_alloc_login();
	if (m_pLogin == NULL)
  {
		//fprintf(stderr, "tds_alloc_login() failed.\n");
    SetError(DATABASE_LAYER_ERROR, wxT("Failed to allocate login"));
    // Reset the variables so there are not mistaken as initialized
    m_pDatabase = NULL;
    m_pLogin = NULL;
    if (m_pContext)
    {
      tds_free_context(m_pContext);
      m_pContext = NULL;
    }
    ThrowDatabaseException();
		return false;
	}
  wxCharBuffer serverBuffer = ConvertToUnicodeStream(m_strServer);
  tds_set_server(m_pLogin, serverBuffer);
  wxCharBuffer loginBuffer = ConvertToUnicodeStream(m_strLogin);
	tds_set_user(m_pLogin, loginBuffer);
  wxCharBuffer passwordBuffer = ConvertToUnicodeStream(m_strPassword);
	tds_set_passwd(m_pLogin, passwordBuffer);
	tds_set_app(m_pLogin, "DatabaseLayer");
	tds_set_library(m_pLogin, "TDS-Library");
	tds_set_client_charset(m_pLogin, "UTF-8");
  switch (m_nTdsVersion)
  {
    case TDS_42:
      tds_set_version(m_pLogin, 4, 2);
      break;
    case TDS_46:
      tds_set_version(m_pLogin, 4, 6);
      break;
    case TDS_50:
      tds_set_version(m_pLogin, 5, 0);
      break;
    case TDS_70:
      tds_set_version(m_pLogin, 7, 0);
      break;
    case TDS_80:
      tds_set_version(m_pLogin, 8, 0);
      break;
    default:
      tds_set_version(m_pLogin, 0, 0);
      break;
  };
  
	m_pContext = tds_alloc_context(NULL);
  if (m_pContext == NULL)
  {
    //fprintf(stderr, "tds_alloc_context() failed.\n");
    SetError(DATABASE_LAYER_ERROR, wxT("Failed to allocate context"));
    // Reset the variables so there are not mistaken as initialized
    m_pDatabase = NULL;
    if (m_pLogin)
    {
      tds_free_login(m_pLogin);
      m_pLogin = NULL;
    }
    if (m_pContext)
    {
      tds_free_context(m_pContext);
      m_pContext = NULL;
    }
    ThrowDatabaseException();
    return false;
  }
	m_pContext->msg_handler = tsql_handle_message;
	m_pContext->err_handler = tsql_handle_error;

  // Add the context (and this databaselayer) from the lookup map
  //  used by the error handler
  TdsDatabaseLayer::AddTdsLayer(m_pContext, this);

  m_pDatabase = tds_alloc_socket(m_pContext, 512);
  if (m_pDatabase == NULL)
  {
    //fprintf(stderr, "tds_alloc_socket() failed.\n");
    SetError(DATABASE_LAYER_ERROR, wxT("Failed to allocate socket"));
    // Reset the variables so there are not mistaken as initialized
    m_pDatabase = NULL;
    if (m_pLogin)
    {
      tds_free_login(m_pLogin);
      m_pLogin = NULL;
    }
    if (m_pContext)
    {
      tds_free_context(m_pContext);
      m_pContext = NULL;
    }
    ThrowDatabaseException();
    return false;
  }
	tds_set_parent(m_pDatabase, NULL);

	TDSCONNECTION* pConnection = tds_read_config_info(NULL, m_pLogin, m_pContext->locale);
	if (!pConnection || tds_connect(m_pDatabase, pConnection) == TDS_FAIL)
  {
		if (pConnection)
    {
			tds_free_socket(m_pDatabase);
			//m_pDatabase = NULL;
			tds_free_connection(pConnection);
		}
		//fprintf(stderr, "tds_connect() failed\n");
    if (GetErrorCode() == DATABASE_LAYER_OK)
    {
      SetError(DATABASE_LAYER_ERROR, wxT("Database connection failed"));
    }
    // Reset the variables so there are not mistaken as initialized
    m_pDatabase = NULL;
    if (m_pLogin)
    {
      tds_free_login(m_pLogin);
      m_pLogin = NULL;
    }
    if (m_pContext)
    {
      tds_free_context(m_pContext);
      m_pContext = NULL;
    }
    ThrowDatabaseException();
		return false;
	}
	tds_free_connection(pConnection);

  return true;
}

// close database  
bool TdsDatabaseLayer::Close()
{
  //puts("Resetting error codes");
  ResetErrorCodes();

  //puts("Closing result sets");
  CloseResultSets();
  //puts("Closing statements");
  CloseStatements();

  if (m_pDatabase != NULL)
  {
    //puts("Freeing socket");
	  tds_free_socket(m_pDatabase);
    m_pDatabase = NULL;
  }
  if (m_pLogin != NULL)
  {
    //puts("Freeing login");
  	tds_free_login(m_pLogin);
    m_pLogin = NULL;
  }
  if (m_pContext != NULL)
  {
    // Remove the context (and this databaselayer) from the lookup map
    //  used by the error handler
    //puts("Removing TDS layer from hash");
    TdsDatabaseLayer::RemoveTdsLayer(m_pContext);

    // Free the context
    //puts("Freeing context");
	  tds_free_context(m_pContext);
  	m_pContext = NULL;
  }

  return true;
}

bool TdsDatabaseLayer::IsOpen()
{
  return (m_pDatabase != NULL);
}

void TdsDatabaseLayer::FreeAllocatedResultSets()
{
  //fprintf(stderr, "In FreeAllocatedResultSets\n");
	int rc;
	int result_type;
  /* */
	//while ((rc = tds_process_tokens(m_pDatabase, &result_type, NULL, TDS_TOKEN_RESULTS)) == TDS_SUCCEED)
	//while ((rc = tds_process_tokens(m_pDatabase, &result_type, NULL, TDS_RETURN_ROW|TDS_TOKEN_RESULTS|TDS_RETURN_COMPUTE)) == TDS_SUCCEED)
	while ((rc = tds_process_tokens(m_pDatabase, &result_type, NULL, TDS_STOPAT_ROWFMT|TDS_RETURN_DONE|TDS_RETURN_ROW|TDS_RETURN_COMPUTE)) == TDS_SUCCEED)
  {
		switch (result_type)
    {
		case TDS_DONE_RESULT:
		case TDS_DONEPROC_RESULT:
		case TDS_DONEINPROC_RESULT:
		case TDS_STATUS_RESULT:
			break;
		case TDS_ROWFMT_RESULT:
		case TDS_COMPUTEFMT_RESULT:
		case TDS_DESCRIBE_RESULT:
			break;
    case TDS_ROW_RESULT:
			//fprintf(stderr, "Warning (%d):  TdsDatabaseLayer query should not return results.  Type: %d\n", result_type, result_type);
      if (m_pDatabase->current_results && m_pDatabase->current_results->num_cols > 0)
      {
  			while (tds_process_tokens(m_pDatabase, &result_type, NULL, TDS_STOPAT_ROWFMT|TDS_RETURN_DONE|TDS_RETURN_ROW) == TDS_SUCCEED)
        {
		  	  //fprintf(stderr, "Warning:  TdsDatabaseLayer TDS_ROW_RESULT query should not return results.  Type: %d\n", result_type);
			    if (result_type != TDS_ROW_RESULT)
			      break;
 
    			if (!m_pDatabase->current_results)
	      		continue;
		  	}
      }
      if (m_pDatabase != NULL)
        tds_free_all_results(m_pDatabase);
      return;
			break;
		default:
			//fprintf(stderr, "Error:  TdsDatabaseLayer query should not return results.  Type: %d\n", result_type);
			return;
      //break;
		}
	}
 /* 
			while (tds_process_tokens(m_pDatabase, &result_type, NULL, TDS_TOKEN_RESULTS) == TDS_SUCCEED) {
				switch (result_type) {
				case TDS_ROWFMT_RESULT:
					break;
				case TDS_ROW_RESULT:
					while (tds_process_tokens(m_pDatabase, &result_type, NULL, TDS_STOPAT_ROWFMT|TDS_RETURN_DONE|TDS_RETURN_ROW) == TDS_SUCCEED) {
						if (result_type != TDS_ROW_RESULT)
							break;
 
						if (!m_pDatabase->current_results)
							continue;
 
						TDSCOLUMN* col = m_pDatabase->current_results->columns[0];
						int ctype = tds_get_conversion_type(col->column_type, col->column_size);

						unsigned char* src = col->column_data;
						int srclen = col->column_cur_size;
 
            CONV_RESULT dres;
						tds_convert(m_pDatabase->tds_ctx, ctype, (TDS_CHAR *) src, srclen, SYBINT4, &dres);
						int optionval = dres.i;
					}
					break;
				default:
					break;
				}
			}
 */

  // Make sure to clean up after ourselves
  if (m_pDatabase != NULL)
    tds_free_all_results(m_pDatabase);

	if (rc == TDS_FAIL)
  {
    ThrowDatabaseException();
		//fprintf(stderr, "tds_process_tokens() returned TDS_FAIL\n");
		return;
	}
  else if (rc != TDS_NO_MORE_RESULTS)
  {
    ThrowDatabaseException();
		//fprintf(stderr, "tds_process_tokens() unexpected return\n");
		return;
	}
}

void TdsDatabaseLayer::BeginTransaction()
{
  wxLogDebug(_("Beginning transaction"));
  RunQuery(_("begin transaction;"), false);
}

void TdsDatabaseLayer::Commit()
{
  wxLogDebug(_("Commiting transaction"));
  RunQuery(_("commit transaction;"), false);
}

void TdsDatabaseLayer::RollBack()
{
  wxLogDebug(_("Rolling back transaction"));
  RunQuery(_("rollback transaction;"), false);
}

// query database
int TdsDatabaseLayer::RunQuery(const wxString& strQuery, bool bParseQuery)
{
  ResetErrorCodes();

  if (m_pDatabase == NULL)
    return false;

  FreeAllocatedResultSets();

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
    wxString sql = RemoveLastSemiColon(*start);
    wxCharBuffer sqlBuffer = ConvertToUnicodeStream(sql);

    //fprintf(stderr, "Running query '%s'\n", (const char*)sqlBuffer);
    int nReturn = tds_submit_query(m_pDatabase, sqlBuffer);
    if (nReturn != TDS_SUCCEED)
    {
      //fprintf(stderr, "tds_submit_query() failed for query '%s'\n", sqlBuffer);
      //fprintf(stderr, "tds_submit_query() failed for query '%s'\n", (sql).c_str());
      FreeAllocatedResultSets();
      ThrowDatabaseException();
      return false;
    }
    FreeAllocatedResultSets();

    start++;
  }
  return true;
}

DatabaseResultSet* TdsDatabaseLayer::RunQueryWithResults(const wxString& strQuery)
{
  ResetErrorCodes();

  if (m_pDatabase != NULL)
  {
    FreeAllocatedResultSets();

    wxArrayString QueryArray = ParseQueries(strQuery);
     
    for (unsigned int i=0; i<(QueryArray.size()-1); i++)
    {
      char* szErrorMessage = NULL;
      wxString strErrorMessage = _("");
      wxString sql = RemoveLastSemiColon(QueryArray[i]);
      wxCharBuffer sqlBuffer = ConvertToUnicodeStream(sql);

      //fprintf(stderr, "Running query '%s'\n", sqlBuffer);
      int nReturn = tds_submit_query(m_pDatabase, sqlBuffer);
      if (nReturn != TDS_SUCCEED)
      {
        //fprintf(stderr, "tds_submit_query() failed for query '%s'\n", sql.c_str());
        FreeAllocatedResultSets();
        ThrowDatabaseException();
        return NULL;
      }
      FreeAllocatedResultSets();
    }

    // Create a Prepared statement for the last SQL statement and get a result set from it
    wxString strQuery = RemoveLastSemiColon(QueryArray[QueryArray.size()-1]);
    wxCharBuffer sqlBuffer = ConvertToUnicodeStream(strQuery);
    //fprintf(stderr, "Running query (with results) '%s'\n", sqlBuffer);
    int nReturn = tds_submit_query(m_pDatabase, sqlBuffer);
    if (nReturn != TDS_SUCCEED)
    {
        //fprintf(stderr, "tds_submit_query() failed for query '%s'\n", sqlBuffer);
        //fprintf(stderr, "tds_submit_query() failed for query '%s'\n", strQuery.c_str());
        FreeAllocatedResultSets();
        ThrowDatabaseException();
        return NULL;
    }
    TdsResultSet* pResultSet = new TdsResultSet(m_pDatabase);
    if (pResultSet)
      pResultSet->SetEncoding(GetEncoding());

    LogResultSetForCleanup(pResultSet);
    //fprintf(stderr, "Returning result set\n");
    return pResultSet;
  }
  else
  {
    return NULL;
  }
}

PreparedStatement* TdsDatabaseLayer::PrepareStatement(const wxString& strQuery)
{
  ResetErrorCodes();

  if (m_pDatabase != NULL)
  {
    wxString sql = RemoveLastSemiColon(strQuery);
    /*
    wxCharBuffer sqlBuffer = ConvertToUnicodeStream(sql);
    TDSDYNAMIC* pStatement = NULL;
    TDSPARAMINFO* pParameters = NULL;
    int nReturn = tds_submit_prepare(m_pDatabase, sqlBuffer, NULL, &pStatement, pParameters);
    if (nReturn != TDS_SUCCEED)
    {
      //fprintf(stderr, "tds_submit_prepare() failed for query '%s'\n", strQuery.c_str());

      if (pStatement != NULL)
        tds_free_dynamic(m_pDatabase, pStatement);
      if (pParameters != NULL)
        tds_free_param_results(pParameters);

      FreeAllocatedResultSets();
      ThrowDatabaseException();
      return NULL;
    }
    FreeAllocatedResultSets();

    TdsPreparedStatement* pReturnStatement = new TdsPreparedStatement(m_pDatabase, pStatement, strQuery);
    if (pReturnStatement)
      pReturnStatement->SetEncoding(GetEncoding());
    */
    TdsPreparedStatement* pReturnStatement = new TdsPreparedStatement(m_pDatabase, sql);
    if (pReturnStatement)
      pReturnStatement->SetEncoding(GetEncoding());

    if (pReturnStatement != NULL)
      LogStatementForCleanup(pReturnStatement);
    return pReturnStatement;
  }
  else
  {
    return NULL;
  }
}

bool TdsDatabaseLayer::TableExists(const wxString& table)
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
    wxString query = _("exec sp_tables ?, NULL, NULL, '''TABLE'''");
    pStatement = PrepareStatement(query);
    if (pStatement)
    {
      pStatement->SetParamString(1, table);
      pResult = pStatement->ExecuteQuery();
      if (pResult)
      {
        if (pResult->Next())
        {
          //wxPrintf(_("Table found: '%s'\n"), pResult->GetResultString(_("TABLE_NAME")));
          if(table.CmpNoCase(pResult->GetResultString(_("TABLE_NAME"))) == 0)
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

bool TdsDatabaseLayer::ViewExists(const wxString& view)
{
  bool bReturn = false;
  // Keep these variables outside of scope so that we can clean them up
  //  in case of an error
  PreparedStatement* pStatement = NULL;
  DatabaseResultSet* pResult = NULL;

#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    wxString query = _("exec sp_tables ?, NULL, NULL, '''VIEW'''");
    pStatement = PrepareStatement(query);
    if (pStatement)
    {
      pStatement->SetParamString(1, view);
      pResult = pStatement->ExecuteQuery();
      if (pResult)
      {
        if (pResult->Next())
        {
          //wxPrintf(_("View found: '%s'\n"), pResult->GetResultString(_("TABLE_NAME")));
          if(view.CmpNoCase(pResult->GetResultString(_("TABLE_NAME"))) == 0)
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

wxArrayString TdsDatabaseLayer::GetTables()
{
  wxArrayString returnArray;

  DatabaseResultSet* pResult = NULL;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    wxString query = _("sp_tables NULL, NULL, NULL, '''TABLE'''");
    pResult = ExecuteQuery(query);

    while (pResult->Next())
    {
      //wxPrintf(_("Adding table: '%s'\n"), pResult->GetResultString(_("TABLE_NAME")).Trim());
      returnArray.Add(pResult->GetResultString(_("TABLE_NAME")).Trim());
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

wxArrayString TdsDatabaseLayer::GetViews()
{
  wxArrayString returnArray;

  DatabaseResultSet* pResult = NULL;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    wxString query = _("sp_tables NULL, NULL, NULL, '''VIEW'''");
    pResult = ExecuteQuery(query);

    while (pResult->Next())
    {
      //wxPrintf(_("Adding view: '%s'\n"), pResult->GetResultString(_("TABLE_NAME")).Trim());
      returnArray.Add(pResult->GetResultString(_("TABLE_NAME")).Trim());
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

wxArrayString TdsDatabaseLayer::GetColumns(const wxString& table)
{
  wxArrayString returnArray;

  // Keep these variables outside of scope so that we can clean them up
  //  in case of an error
/* -- Prepared statement support isn't working as well as it should so use concatenated wxStrings
  PreparedStatement* pStatement = NULL;
  DatabaseResultSet* pResult = NULL;

#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    wxString query = _("sp_columns ?;");
    pStatement = PrepareStatement(query);
    if (pStatement)
    {
      pStatement->SetParamString(1, table);
      pResult = pStatement->ExecuteQuery();
      if (pResult)
      {
        while (pResult->Next())
        {
          //wxPrintf(_("Adding table: '%s'\n"), pResult->GetResultString(_("TABLE_NAME")).Trim());
          returnArray.Add(pResult->GetResultString(_("TABLE_NAME")).Trim());
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
*/
  DatabaseResultSet* pResult = NULL;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    wxString query = wxString::Format(_("sp_columns %s;"), table.c_str());
    pResult = ExecuteQuery(query);

    while (pResult->Next())
    {
      //wxPrintf(_("Adding column: '%s'\n"), pResult->GetResultString(_("COLUMN_NAME")).Trim());
      returnArray.Add(pResult->GetResultString(_("COLUMN_NAME")).Trim());
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

void TdsDatabaseLayer::SetError(int nCode, const wxString& strMessage)
{
  SetErrorCode(TranslateErrorCode(nCode));
  SetErrorMessage(strMessage);

  // We really don't want to throw the exception here since it removes
  //  the ability of the calling code to cleanup any memory usage
  //wxPrintf(_("TdsDatabaseLayer SetError() throwing exception\n"));
  //ThrowDatabaseException();
}

int TdsDatabaseLayer::TranslateErrorCode(int nCode)
{
  // Ultimately, this will probably be a map of TDS database error code values to DatabaseLayer values
  // For now though, we'll just return error
  int nReturn = nCode;
  return nReturn;
}

TdsDatabaseLayer* TdsDatabaseLayer::LookupTdsLayer(const TDSCONTEXT* pContext)
{
  TdsDatabaseLayer* pLayer = NULL;
  TdsContextToDatabaseLayerMap::iterator finder = TdsDatabaseLayer::m_ContextLookupMap.find(pContext);
  if (finder != TdsDatabaseLayer::m_ContextLookupMap.end())
    pLayer = finder->second;

  return pLayer;
}

void TdsDatabaseLayer::AddTdsLayer(TDSCONTEXT* pContext, TdsDatabaseLayer* pLayer)
{
  TdsDatabaseLayer::m_ContextLookupMap[pContext] = pLayer;
}

void TdsDatabaseLayer::RemoveTdsLayer(TDSCONTEXT* pContext)
{
  TdsDatabaseLayer::m_ContextLookupMap.erase(pContext);
}

