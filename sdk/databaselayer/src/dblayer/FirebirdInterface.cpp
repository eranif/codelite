#include "../include/FirebirdInterface.h"

bool FirebirdInterface::Init()
{
  bool bLoaded = m_FirebirdDLL.Load(wxDynamicLibrary::CanonicalizeName(wxT("fbclient")));
  if (!bLoaded)
  {
    return false;
  }

  wxString symbol = wxT("fb_interpret");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pFbInterpret = (fb_interpretType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_expand_dpb");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscExpandDpb = (isc_expand_dpbType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_attach_database");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscAttachDatabase = (isc_attach_databaseType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_detach_database");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscDetachDatabase = (isc_detach_databaseType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_start_transaction");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscStartTransaction = (isc_start_transactionType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_commit_transaction");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscCommitTransaction = (isc_commit_transactionType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_rollback_transaction");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscRollbackTransaction = (isc_rollback_transactionType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_dsql_execute_immediate");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscDsqlExecuteImmediate = (isc_dsql_execute_immediateType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_dsql_allocate_statement");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscDsqlAllocateStatement = (isc_dsql_allocate_statementType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_dsql_prepare");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscDsqlPrepare = (isc_dsql_prepareType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_dsql_describe");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscDsqlDescribe = (isc_dsql_describeType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_sql_interprete");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscSqlInterprete = (isc_sql_interpreteType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_sqlcode");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscSqlcode = (isc_sqlcodeType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_encode_timestamp");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscEncodeTimestamp = (isc_encode_timestampType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_create_blob2");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscCreateBlob2 = (isc_create_blob2Type)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_open_blob2");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscOpenBlob2 = (isc_open_blob2Type)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_put_segment");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscPutSegment = (isc_put_segmentType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_close_blob");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscCloseBlob = (isc_close_blobType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_commit_retaining");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscCommitRetaining = (isc_commit_retainingType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_dsql_free_statement");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscDsqlFreeStatement = (isc_dsql_free_statementType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_dsql_describe_bind");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscDsqlDescribeBind = (isc_dsql_describe_bindType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_dsql_execute");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscDsqlExecute = (isc_dsql_executeType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_dsql_sql_info");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscDsqlSqlInfo = (isc_dsql_sql_infoType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_vax_integer");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscVaxInteger = (isc_vax_integerType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_dsql_execute2");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscDsqlExecute2 = (isc_dsql_execute2Type)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_dsql_fetch");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscDsqlFetch = (isc_dsql_fetchType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_decode_timestamp");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscDecodeTimestamp = (isc_decode_timestampType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_decode_sql_date");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscDecodeSqlDate = (isc_decode_sql_dateType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_decode_sql_time");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscDecodeSqlTime = (isc_decode_sql_timeType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("isc_get_segment");
  if (m_FirebirdDLL.HasSymbol(symbol))
  {
    m_pIscGetSegment = (isc_get_segmentType)m_FirebirdDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  return true;
}

