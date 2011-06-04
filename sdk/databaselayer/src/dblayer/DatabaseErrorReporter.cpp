#include "../include/DatabaseErrorReporter.h"
#include "../include/DatabaseErrorCodes.h"
#include "../include/DatabaseLayerException.h"

DatabaseErrorReporter::DatabaseErrorReporter()
{
  ResetErrorCodes();
}

DatabaseErrorReporter::~DatabaseErrorReporter()
{
}

const wxString& DatabaseErrorReporter::GetErrorMessage()
{
  return m_strErrorMessage;
}

int DatabaseErrorReporter::GetErrorCode()
{
  return m_nErrorCode;
}

void DatabaseErrorReporter::SetErrorMessage(const wxString& strErrorMessage)
{
  m_strErrorMessage = strErrorMessage;
}

void DatabaseErrorReporter::SetErrorCode(int nErrorCode)
{
  m_nErrorCode = nErrorCode;
}

void DatabaseErrorReporter::ResetErrorCodes()
{
  m_strErrorMessage = _("");
  m_nErrorCode = DATABASE_LAYER_OK;
}

void DatabaseErrorReporter::ThrowDatabaseException()
{
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
  DatabaseLayerException error(GetErrorCode(), GetErrorMessage());
  throw error;
#endif
}

