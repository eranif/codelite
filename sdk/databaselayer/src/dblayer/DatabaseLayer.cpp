#include "../include/DatabaseLayer.h"
#include "../include/DatabaseErrorCodes.h"
#include "../include/DatabaseLayerException.h"

// ctor()
DatabaseLayer::DatabaseLayer()
 : DatabaseErrorReporter()
{
}

// dtor()
DatabaseLayer::~DatabaseLayer()
{
  CloseResultSets();
  CloseStatements();
}

int DatabaseLayer::RunQuery(const wxString& strQuery)
{
  return RunQuery(strQuery, true);
}

void DatabaseLayer::CloseResultSets()
{
  // Iterate through all of the result sets and close them all
  DatabaseResultSetHashSet::iterator start = m_ResultSets.begin();
  DatabaseResultSetHashSet::iterator stop = m_ResultSets.end();
  while (start != stop)
  {
    wxLogDebug(_("ResultSet NOT closed and cleaned up by the DatabaseLayer dtor"));
    delete (*start);
    start++;
  }
  m_ResultSets.clear();
}

void DatabaseLayer::CloseStatements()
{
  // Iterate through all of the statements and close them all
  DatabaseStatementHashSet::iterator start = m_Statements.begin();
  DatabaseStatementHashSet::iterator stop = m_Statements.end();
  while (start != stop)
  {
    wxLogDebug(_("PreparedStatement NOT closed and cleaned up by the DatabaseLayer dtor"));
    delete (*start);
    start++;
  }
  m_Statements.clear();
}

bool DatabaseLayer::CloseResultSet(DatabaseResultSet* pResultSet)
{
  if (pResultSet != NULL)
  {
    // Check if we have this result set in our list
    if (m_ResultSets.find(pResultSet) != m_ResultSets.end())
    {
      // Remove the result set pointer from the list and delete the pointer
      delete pResultSet;
      m_ResultSets.erase(pResultSet);
      return true;
    }

    // If not then iterate through all of the statements and see
    //  if any of them have the result set in their lists
    DatabaseStatementHashSet::iterator it;
    for( it = m_Statements.begin(); it != m_Statements.end(); ++it )
    {
      // If the statement knows about the result set then it will close the 
      //  result set and return true, otherwise it will return false
      PreparedStatement* pStatement = *it;
      if (pStatement != NULL)
      {
        if (pStatement->CloseResultSet(pResultSet))
        {
          return true;
        }
      }
    }

    // If we don't know about the result set and the statements don't
    //  know about it, the just delete it
    delete pResultSet;
    return true;
  }
  else
  {
    // Return false on NULL pointer
    return false;
  }

}

bool DatabaseLayer::CloseStatement(PreparedStatement* pStatement)
{
  if (pStatement != NULL)
  {
    // See if we know about this pointer, if so then remove it from the list
    if (m_Statements.find(pStatement) != m_Statements.end())
    {
      // Remove the statement pointer from the list and delete the pointer
      delete pStatement;
      m_Statements.erase(pStatement);
      return true;
    }

    // Otherwise just delete it
    delete pStatement;
    return true;
  }
  else
  {
    // Return false on NULL pointer
    return false;
  }
}


int DatabaseLayer::GetSingleResultInt(const wxString& strSQL, const wxString& strField, bool bRequireUniqueResult /*= true*/)
{
  wxVariant variant(strField);
  return GetSingleResultInt(strSQL, &variant, bRequireUniqueResult);
}

int DatabaseLayer::GetSingleResultInt(const wxString& strSQL, int nField, bool bRequireUniqueResult /*= true*/)
{
  wxVariant variant((long)nField);
  return GetSingleResultInt(strSQL, &variant, bRequireUniqueResult);
}

int DatabaseLayer::GetSingleResultInt(const wxString& strSQL, const wxVariant* field, bool bRequireUniqueResult /*= true*/)
{
  bool valueRetrievedFlag = false;
  int value = -1;

  DatabaseResultSet* pResult = NULL;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    pResult = ExecuteQuery(strSQL);

    while (pResult->Next())
    {
      if (valueRetrievedFlag)
      {
        // Close the result set, reset the value and throw an exception
        CloseResultSet(pResult);
        pResult = NULL;
        value = -1;
        SetErrorCode(DATABASE_LAYER_NON_UNIQUE_RESULTSET);
        SetErrorMessage(wxT("A non-unique result was returned."));
        ThrowDatabaseException();
        return value;
      }
      else
      {
        if (field->IsType(_("string")))
          value = pResult->GetResultInt(field->GetString());
        else
          value = pResult->GetResultInt(field->GetLong());
        valueRetrievedFlag = true;

        // If the user isn't concerned about returning a unique result,
        //  then just exit after the first record is found
        if (!bRequireUniqueResult)
          break;
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

    throw e;
  }
#endif

  if (pResult != NULL)
  {
    CloseResultSet(pResult);
    pResult = NULL;
  }

  // Make sure that a value was retrieved from the database
  if (!valueRetrievedFlag)
  {
    value = -1;
    SetErrorCode(DATABASE_LAYER_NO_ROWS_FOUND);
    SetErrorMessage(wxT("No result was returned."));
    ThrowDatabaseException();
    return value;
  }
  
  return value;
}

wxString DatabaseLayer::GetSingleResultString(const wxString& strSQL, int nField, bool bRequireUniqueResult /*= true*/)
{
  wxVariant variant((long)nField);
  return GetSingleResultString(strSQL, &variant, bRequireUniqueResult);
}

wxString DatabaseLayer::GetSingleResultString(const wxString& strSQL, const wxString& strField, bool bRequireUniqueResult /*= true*/)
{
  wxVariant variant(strField);
  return GetSingleResultString(strSQL, &variant, bRequireUniqueResult);
}

wxString DatabaseLayer::GetSingleResultString(const wxString& strSQL, const wxVariant* field, bool bRequireUniqueResult /*= true*/)
{
  bool valueRetrievedFlag = false;
  wxString value = wxEmptyString;

  DatabaseResultSet* pResult = NULL;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    pResult = ExecuteQuery(strSQL);

    while (pResult->Next())
    {
      if (valueRetrievedFlag)
      {
        // Close the result set, reset the value and throw an exception
        CloseResultSet(pResult);
        pResult = NULL;
        value = wxEmptyString;
        SetErrorCode(DATABASE_LAYER_NON_UNIQUE_RESULTSET);
        SetErrorMessage(wxT("A non-unique result was returned."));
        ThrowDatabaseException();
        return value;
      }
      else
      {
        if (field->IsType(_("string")))
          value = pResult->GetResultString(field->GetString());
        else
          value = pResult->GetResultString(field->GetLong());
        valueRetrievedFlag = true;

        // If the user isn't concerned about returning a unique result,
        //  then just exit after the first record is found
        if (!bRequireUniqueResult)
          break;
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

    throw e;
  }
#endif

  if (pResult != NULL)
  {
    CloseResultSet(pResult);
    pResult = NULL;
  }

  // Make sure that a value was retrieved from the database
  if (!valueRetrievedFlag)
  {
    value = wxEmptyString;
    SetErrorCode(DATABASE_LAYER_NO_ROWS_FOUND);
    SetErrorMessage(wxT("No result was returned."));
    ThrowDatabaseException();
    return value;
  }
  
  return value;
}

long DatabaseLayer::GetSingleResultLong(const wxString& strSQL, int nField, bool bRequireUniqueResult /*= true*/)
{
  wxVariant variant((long)nField);
  return GetSingleResultLong(strSQL, &variant, bRequireUniqueResult);
}

long DatabaseLayer::GetSingleResultLong(const wxString& strSQL, const wxString& strField, bool bRequireUniqueResult /*= true*/)
{
  wxVariant variant(strField);
  return GetSingleResultLong(strSQL, &variant, bRequireUniqueResult);
}

long DatabaseLayer::GetSingleResultLong(const wxString& strSQL, const wxVariant* field, bool bRequireUniqueResult /*= true*/)
{
  bool valueRetrievedFlag = false;
  long value = -1;

  DatabaseResultSet* pResult = NULL;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    pResult = ExecuteQuery(strSQL);

    while (pResult->Next())
    {
      if (valueRetrievedFlag)
      {
        // Close the result set, reset the value and throw an exception
        CloseResultSet(pResult);
        pResult = NULL;
        value = -1;
        SetErrorCode(DATABASE_LAYER_NON_UNIQUE_RESULTSET);
        SetErrorMessage(wxT("A non-unique result was returned."));
        ThrowDatabaseException();
        return value;
      }
      else
      {
        if (field->IsType(_("string")))
          value = pResult->GetResultLong(field->GetString());
        else
          value = pResult->GetResultLong(field->GetLong());
        valueRetrievedFlag = true;

        // If the user isn't concerned about returning a unique result,
        //  then just exit after the first record is found
        if (!bRequireUniqueResult)
          break;
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

    throw e;
  }
#endif

  if (pResult != NULL)
  {
    CloseResultSet(pResult);
    pResult = NULL;
  }

  // Make sure that a value was retrieved from the database
  if (!valueRetrievedFlag)
  {
    value = -1;
    SetErrorCode(DATABASE_LAYER_NO_ROWS_FOUND);
    SetErrorMessage(wxT("No result was returned."));
    ThrowDatabaseException();
    return value;
  }
  
  return value;
}

bool DatabaseLayer::GetSingleResultBool(const wxString& strSQL, int nField, bool bRequireUniqueResult /*= true*/)
{
  wxVariant variant((long)nField);
  return GetSingleResultBool(strSQL, &variant, bRequireUniqueResult);
}

bool DatabaseLayer::GetSingleResultBool(const wxString& strSQL, const wxString& strField, bool bRequireUniqueResult /*= true*/)
{
  wxVariant variant(strField);
  return GetSingleResultBool(strSQL, &variant, bRequireUniqueResult);
}

bool DatabaseLayer::GetSingleResultBool(const wxString& strSQL, const wxVariant* field, bool bRequireUniqueResult /*= true*/)
{
  bool valueRetrievedFlag = false;
  bool value = false;

  DatabaseResultSet* pResult = NULL;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    pResult = ExecuteQuery(strSQL);

    while (pResult->Next())
    {
      if (valueRetrievedFlag)
      {
        // Close the result set, reset the value and throw an exception
        CloseResultSet(pResult);
        pResult = NULL;
        value = false;
        SetErrorCode(DATABASE_LAYER_NON_UNIQUE_RESULTSET);
        SetErrorMessage(wxT("A non-unique result was returned."));
        ThrowDatabaseException();
        return value;
      }
      else
      {
        if (field->IsType(_("string")))
          value = pResult->GetResultBool(field->GetString());
        else
          value = pResult->GetResultBool(field->GetLong());
        valueRetrievedFlag = true;

        // If the user isn't concerned about returning a unique result,
        //  then just exit after the first record is found
        if (!bRequireUniqueResult)
          break;
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

    throw e;
  }
#endif

  if (pResult != NULL)
  {
    CloseResultSet(pResult);
    pResult = NULL;
  }

  // Make sure that a value was retrieved from the database
  if (!valueRetrievedFlag)
  {
    value = false;
    SetErrorCode(DATABASE_LAYER_NO_ROWS_FOUND);
    SetErrorMessage(wxT("No result was returned."));
    ThrowDatabaseException();
    return value;
  }
  
  return value;
}
 
wxDateTime DatabaseLayer::GetSingleResultDate(const wxString& strSQL, int nField, bool bRequireUniqueResult /*= true*/)
{
  wxVariant variant((long)nField);
  return GetSingleResultDate(strSQL, &variant, bRequireUniqueResult);
}

wxDateTime DatabaseLayer::GetSingleResultDate(const wxString& strSQL, const wxString& strField, bool bRequireUniqueResult /*= true*/)
{
  wxVariant variant(strField);
  return GetSingleResultDate(strSQL, &variant, bRequireUniqueResult);
}

wxDateTime DatabaseLayer::GetSingleResultDate(const wxString& strSQL, const wxVariant* field, bool bRequireUniqueResult /*= true*/)
{
  bool valueRetrievedFlag = false;
  wxDateTime value = wxInvalidDateTime;

  DatabaseResultSet* pResult = NULL;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    pResult = ExecuteQuery(strSQL);

    while (pResult->Next())
    {
      if (valueRetrievedFlag)
      {
        // Close the result set, reset the value and throw an exception
        CloseResultSet(pResult);
        pResult = NULL;
        value = wxInvalidDateTime;
        SetErrorCode(DATABASE_LAYER_NON_UNIQUE_RESULTSET);
        SetErrorMessage(wxT("A non-unique result was returned."));
        ThrowDatabaseException();
        return value;
      }
      else
      {
        if (field->IsType(_("string")))
          value = pResult->GetResultDate(field->GetString());
        else
          value = pResult->GetResultDate(field->GetLong());
        valueRetrievedFlag = true;

        // If the user isn't concerned about returning a unique result,
        //  then just exit after the first record is found
        if (!bRequireUniqueResult)
          break;
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

    throw e;
  }
#endif

  if (pResult != NULL)
  {
    CloseResultSet(pResult);
    pResult = NULL;
  }

  // Make sure that a value was retrieved from the database
  if (!valueRetrievedFlag)
  {
    value = wxInvalidDateTime;
    SetErrorCode(DATABASE_LAYER_NO_ROWS_FOUND);
    SetErrorMessage(wxT("No result was returned."));
    ThrowDatabaseException();
    return value;
  }
  
  return value;
}

void* DatabaseLayer::GetSingleResultBlob(const wxString& strSQL, int nField, wxMemoryBuffer& Buffer, bool bRequireUniqueResult /*= true*/)
{
  wxVariant variant((long)nField);
  return GetSingleResultBlob(strSQL, &variant, Buffer, bRequireUniqueResult);
}

void* DatabaseLayer::GetSingleResultBlob(const wxString& strSQL, const wxString& strField, wxMemoryBuffer& Buffer, bool bRequireUniqueResult /*= true*/)
{
  wxVariant variant(strField);
  return GetSingleResultBlob(strSQL, &variant, Buffer, bRequireUniqueResult);
}

void* DatabaseLayer::GetSingleResultBlob(const wxString& strSQL, const wxVariant* field, wxMemoryBuffer& Buffer, bool bRequireUniqueResult /*= true*/)
{
  bool valueRetrievedFlag = false;
  void* value = NULL;

  DatabaseResultSet* pResult = NULL;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    pResult = ExecuteQuery(strSQL);

    while (pResult->Next())
    {
      if (valueRetrievedFlag)
      {
        // Close the result set, reset the value and throw an exception
        CloseResultSet(pResult);
        pResult = NULL;
        value = NULL;
        SetErrorCode(DATABASE_LAYER_NON_UNIQUE_RESULTSET);
        SetErrorMessage(wxT("A non-unique result was returned."));
        ThrowDatabaseException();
        return value;
      }
      else
      {
        if (field->IsType(_("string")))
          value = pResult->GetResultBlob(field->GetString(), Buffer);
        else
          value = pResult->GetResultBlob(field->GetLong(), Buffer);
        valueRetrievedFlag = true;

        // If the user isn't concerned about returning a unique result,
        //  then just exit after the first record is found
        if (!bRequireUniqueResult)
          break;
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

    throw e;
  }
#endif

  if (pResult != NULL)
  {
    CloseResultSet(pResult);
    pResult = NULL;
  }

  // Make sure that a value was retrieved from the database
  if (!valueRetrievedFlag)
  {
    value = NULL;
    SetErrorCode(DATABASE_LAYER_NO_ROWS_FOUND);
    SetErrorMessage(wxT("No result was returned."));
    ThrowDatabaseException();
    return value;
  }
  
  return value;
}
 
double DatabaseLayer::GetSingleResultDouble(const wxString& strSQL, int nField, bool bRequireUniqueResult /*= true*/)
{
  wxVariant variant((long)nField);
  return GetSingleResultDouble(strSQL, &variant, bRequireUniqueResult);
}

double DatabaseLayer::GetSingleResultDouble(const wxString& strSQL, const wxString& strField, bool bRequireUniqueResult /*= true*/)
{
  wxVariant variant(strField);
  return GetSingleResultDouble(strSQL, &variant, bRequireUniqueResult);
}

double DatabaseLayer::GetSingleResultDouble(const wxString& strSQL, const wxVariant* field, bool bRequireUniqueResult /*= true*/)
{
  bool valueRetrievedFlag = false;
  double value = -1;

  DatabaseResultSet* pResult = NULL;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    pResult = ExecuteQuery(strSQL);

    while (pResult->Next())
    {
      if (valueRetrievedFlag)
      {
        // Close the result set, reset the value and throw an exception
        CloseResultSet(pResult);
        pResult = NULL;
        value = -1;
        SetErrorCode(DATABASE_LAYER_NON_UNIQUE_RESULTSET);
        SetErrorMessage(wxT("A non-unique result was returned."));
        ThrowDatabaseException();
        return value;
      }
      else
      {
        if (field->IsType(_("string")))
          value = pResult->GetResultDouble(field->GetString());
        else
          value = pResult->GetResultDouble(field->GetLong());
        valueRetrievedFlag = true;

        // If the user isn't concerned about returning a unique result,
        //  then just exit after the first record is found
        if (!bRequireUniqueResult)
          break;
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

    throw e;
  }
#endif

  if (pResult != NULL)
  {
    CloseResultSet(pResult);
    pResult = NULL;
  }

  // Make sure that a value was retrieved from the database
  if (!valueRetrievedFlag)
  {
    value = -1;
    SetErrorCode(DATABASE_LAYER_NO_ROWS_FOUND);
    SetErrorMessage(wxT("No result was returned."));
    ThrowDatabaseException();
    return value;
  }
  
  return value;
}

wxArrayInt DatabaseLayer::GetResultsArrayInt(const wxString& strSQL, int nField)
{
  wxVariant variant((long)nField);
  return GetResultsArrayInt(strSQL, &variant);
}

wxArrayInt DatabaseLayer::GetResultsArrayInt(const wxString& strSQL, const wxString& strField)
{
  wxVariant variant(strField);
  return GetResultsArrayInt(strSQL, &variant);
}

wxArrayInt DatabaseLayer::GetResultsArrayInt(const wxString& strSQL, const wxVariant* field)
{
  wxArrayInt returnArray;
  
  DatabaseResultSet* pResult = NULL;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    pResult = ExecuteQuery(strSQL);

    while (pResult->Next())
    {
      if (field->IsType(_("string")))
        returnArray.Add(pResult->GetResultInt(field->GetString()));
      else
        returnArray.Add(pResult->GetResultInt(field->GetLong()));
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

wxArrayString DatabaseLayer::GetResultsArrayString(const wxString& strSQL, int nField)
{
  wxVariant variant((long)nField);
  return GetResultsArrayString(strSQL, &variant);
}

wxArrayString DatabaseLayer::GetResultsArrayString(const wxString& strSQL, const wxString& strField)
{
  wxVariant variant(strField);
  return GetResultsArrayString(strSQL, &variant);
}

wxArrayString DatabaseLayer::GetResultsArrayString(const wxString& strSQL, const wxVariant* field)
{
  wxArrayString returnArray;
  
  DatabaseResultSet* pResult = NULL;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    pResult = ExecuteQuery(strSQL);

    while (pResult->Next())
    {
      if (field->IsType(_("string")))
        returnArray.Add(pResult->GetResultString(field->GetString()));
      else
        returnArray.Add(pResult->GetResultString(field->GetLong()));
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

wxArrayLong DatabaseLayer::GetResultsArrayLong(const wxString& strSQL, int nField)
{
  wxVariant variant((long)nField);
  return GetResultsArrayLong(strSQL, &variant);
}

wxArrayLong DatabaseLayer::GetResultsArrayLong(const wxString& strSQL, const wxString& strField)
{
  wxVariant variant(strField);
  return GetResultsArrayLong(strSQL, &variant);
}

wxArrayLong DatabaseLayer::GetResultsArrayLong(const wxString& strSQL, const wxVariant* field)
{
  wxArrayLong returnArray;
  
  DatabaseResultSet* pResult = NULL;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    pResult = ExecuteQuery(strSQL);

    while (pResult->Next())
    {
      if (field->IsType(_("string")))
        returnArray.Add(pResult->GetResultLong(field->GetString()));
      else
        returnArray.Add(pResult->GetResultLong(field->GetLong()));
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

#if wxCHECK_VERSION(2, 7, 0)
wxArrayDouble DatabaseLayer::GetResultsArrayDouble(const wxString& strSQL, int nField)
{
  wxVariant variant((long)nField);
  return GetResultsArrayDouble(strSQL, &variant);
}

wxArrayDouble DatabaseLayer::GetResultsArrayDouble(const wxString& strSQL, const wxString& strField)
{
  wxVariant variant(strField);
  return GetResultsArrayDouble(strSQL, &variant);
}

wxArrayDouble DatabaseLayer::GetResultsArrayDouble(const wxString& strSQL, const wxVariant* field)
{
  wxArrayDouble returnArray;
  
  DatabaseResultSet* pResult = NULL;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  try
  {
#endif
    pResult = ExecuteQuery(strSQL);

    while (pResult->Next())
    {
      if (field->IsType(_("string")))
        returnArray.Add(pResult->GetResultDouble(field->GetString()));
      else
        returnArray.Add(pResult->GetResultDouble(field->GetLong()));
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
#endif

