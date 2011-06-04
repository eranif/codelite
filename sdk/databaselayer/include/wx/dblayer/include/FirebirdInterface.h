#ifndef __FIREBIRD_INTERFACES_H__
#define __FIREBIRD_INTERFACES_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/dynlib.h"

#include "ibase.h"

typedef ISC_STATUS (ISC_EXPORT *fb_interpretType)(ISC_SCHAR*, unsigned int, const ISC_STATUS**);
typedef void (ISC_EXPORT *isc_expand_dpbType)(ISC_SCHAR**, short*, ...);
typedef ISC_STATUS (ISC_EXPORT *isc_attach_databaseType)(ISC_STATUS*, short, const ISC_SCHAR*,
  isc_db_handle*, short, const ISC_SCHAR*);
typedef ISC_STATUS (ISC_EXPORT *isc_detach_databaseType)(ISC_STATUS*, isc_db_handle*);
typedef ISC_STATUS (ISC_EXPORT *isc_start_transactionType)(ISC_STATUS*, isc_tr_handle*, short, ...);
typedef ISC_STATUS (ISC_EXPORT *isc_commit_transactionType)(ISC_STATUS*, isc_tr_handle*);
typedef ISC_STATUS (ISC_EXPORT *isc_rollback_transactionType)(ISC_STATUS*, isc_tr_handle*);
typedef ISC_STATUS (ISC_EXPORT *isc_dsql_execute_immediateType)(ISC_STATUS*, isc_db_handle*,
  isc_tr_handle*, unsigned short, const ISC_SCHAR*, unsigned short, XSQLDA*);
typedef ISC_STATUS (ISC_EXPORT *isc_dsql_allocate_statementType)(ISC_STATUS*, isc_db_handle*, isc_stmt_handle*);
typedef ISC_STATUS(ISC_EXPORT *isc_dsql_prepareType)(ISC_STATUS*, isc_tr_handle*, isc_stmt_handle*,
  unsigned short, const ISC_SCHAR*, unsigned short, XSQLDA*);
typedef ISC_STATUS (ISC_EXPORT *isc_dsql_describeType)(ISC_STATUS*, isc_stmt_handle*,
  unsigned short, XSQLDA*);
typedef void (ISC_EXPORT *isc_sql_interpreteType)(short, ISC_SCHAR*, short);
typedef ISC_LONG (ISC_EXPORT *isc_sqlcodeType)(const ISC_STATUS*);
typedef void (ISC_EXPORT *isc_encode_timestampType)(const void*, ISC_TIMESTAMP*);
typedef ISC_STATUS (ISC_EXPORT *isc_create_blob2Type)(ISC_STATUS*, isc_db_handle*,
  isc_tr_handle*, isc_blob_handle*, ISC_QUAD*, short, const ISC_SCHAR*);
typedef ISC_STATUS (ISC_EXPORT *isc_open_blob2Type)(ISC_STATUS*, isc_db_handle*,
  isc_tr_handle*, isc_blob_handle*, ISC_QUAD*, ISC_USHORT, const ISC_UCHAR*);
typedef ISC_STATUS (ISC_EXPORT *isc_put_segmentType)(ISC_STATUS*, isc_blob_handle*, 
  unsigned short, const ISC_SCHAR*);
typedef ISC_STATUS (ISC_EXPORT *isc_close_blobType)(ISC_STATUS*, isc_blob_handle*);
typedef ISC_STATUS (ISC_EXPORT *isc_commit_retainingType)(ISC_STATUS*, isc_tr_handle*);
typedef ISC_STATUS (ISC_EXPORT *isc_dsql_free_statementType)(ISC_STATUS*, isc_stmt_handle*, unsigned short);
typedef ISC_STATUS (ISC_EXPORT *isc_dsql_describe_bindType)(ISC_STATUS*, isc_stmt_handle*,
  unsigned short, XSQLDA*);
typedef ISC_STATUS (ISC_EXPORT *isc_dsql_executeType)(ISC_STATUS*, isc_tr_handle*, 
  isc_stmt_handle*, unsigned short, XSQLDA*);
typedef ISC_STATUS (ISC_EXPORT *isc_dsql_sql_infoType)(ISC_STATUS*, isc_stmt_handle*,
  short, const ISC_SCHAR*, short, ISC_SCHAR*);
typedef ISC_LONG (ISC_EXPORT *isc_vax_integerType)(const ISC_SCHAR*, short);
typedef ISC_STATUS (ISC_EXPORT *isc_dsql_execute2Type)(ISC_STATUS*, isc_tr_handle*,
  isc_stmt_handle*, unsigned short, XSQLDA*, XSQLDA*);
typedef ISC_STATUS (ISC_EXPORT *isc_dsql_fetchType)(ISC_STATUS*, isc_stmt_handle*,
  unsigned short, XSQLDA*);
typedef void (ISC_EXPORT *isc_decode_timestampType)(const ISC_TIMESTAMP*, void*);
typedef void (ISC_EXPORT *isc_decode_sql_dateType)(const ISC_DATE*, void*);
typedef void (ISC_EXPORT *isc_decode_sql_timeType)(const ISC_TIME*, void*);
typedef ISC_STATUS (ISC_EXPORT *isc_get_segmentType)(ISC_STATUS*, isc_blob_handle*,
  unsigned short*, unsigned short, ISC_SCHAR*);

class FirebirdInterface
{
public:
  FirebirdInterface() { }
  bool Init();

  fb_interpretType GetFbInterpret() { return m_pFbInterpret; }
  isc_expand_dpbType GetIscExpandDpb() { return m_pIscExpandDpb; }
  isc_attach_databaseType GetIscAttachDatabase() { return m_pIscAttachDatabase; }
  isc_detach_databaseType GetIscDetachDatabase() { return m_pIscDetachDatabase; }
  isc_start_transactionType GetIscStartTransaction() { return m_pIscStartTransaction; }
  isc_commit_transactionType GetIscCommitTransaction() { return m_pIscCommitTransaction; }
  isc_rollback_transactionType GetIscRollbackTransaction() { return m_pIscRollbackTransaction; }
  isc_dsql_execute_immediateType GetIscDsqlExecuteImmediate() { return m_pIscDsqlExecuteImmediate; }
  isc_dsql_allocate_statementType GetIscDsqlAllocateStatement() { return m_pIscDsqlAllocateStatement; }
  isc_dsql_prepareType GetIscDsqlPrepare() { return m_pIscDsqlPrepare; }
  isc_dsql_describeType GetIscDsqlDescribe() { return m_pIscDsqlDescribe; }
  isc_sql_interpreteType GetIscSqlInterprete() { return m_pIscSqlInterprete; }
  isc_sqlcodeType GetIscSqlcode() { return m_pIscSqlcode; }
  isc_encode_timestampType GetIscEncodeTimestamp() { return m_pIscEncodeTimestamp; }
  isc_create_blob2Type GetIscCreateBlob2() { return m_pIscCreateBlob2; }
  isc_open_blob2Type GetIscOpenBlob2() { return m_pIscOpenBlob2; }
  isc_put_segmentType GetIscPutSegment() { return m_pIscPutSegment; }
  isc_close_blobType GetIscCloseBlob() { return m_pIscCloseBlob; }
  isc_commit_retainingType GetIscCommitRetaining() { return m_pIscCommitRetaining; }
  isc_dsql_free_statementType GetIscDsqlFreeStatement() { return m_pIscDsqlFreeStatement; }
  isc_dsql_describe_bindType GetIscDsqlDescribeBind() { return m_pIscDsqlDescribeBind; }
  isc_dsql_executeType GetIscDsqlExecute() { return m_pIscDsqlExecute; }
  isc_dsql_sql_infoType GetIscDsqlSqlInfo() { return m_pIscDsqlSqlInfo; }
  isc_vax_integerType GetIscVaxInteger() { return m_pIscVaxInteger; }
  isc_dsql_execute2Type GetIscDsqlExecute2() { return m_pIscDsqlExecute2; }
  isc_dsql_fetchType GetIscDsqlFetch() { return m_pIscDsqlFetch; }
  isc_decode_timestampType GetIscDecodeTimestamp() { return m_pIscDecodeTimestamp; }
  isc_decode_sql_dateType GetIscDecodeSqlDate() { return m_pIscDecodeSqlDate; }
  isc_decode_sql_timeType GetIscDecodeSqlTime() { return m_pIscDecodeSqlTime; }
  isc_get_segmentType GetIscGetSegment() { return m_pIscGetSegment; }

private:
  wxDynamicLibrary m_FirebirdDLL;

  fb_interpretType m_pFbInterpret;
  isc_expand_dpbType m_pIscExpandDpb;
  isc_attach_databaseType m_pIscAttachDatabase;
  isc_detach_databaseType m_pIscDetachDatabase;
  isc_start_transactionType m_pIscStartTransaction;
  isc_commit_transactionType m_pIscCommitTransaction;
  isc_rollback_transactionType m_pIscRollbackTransaction;
  isc_dsql_execute_immediateType m_pIscDsqlExecuteImmediate;
  isc_dsql_allocate_statementType m_pIscDsqlAllocateStatement;
  isc_dsql_prepareType m_pIscDsqlPrepare;
  isc_dsql_describeType m_pIscDsqlDescribe;
  isc_sql_interpreteType m_pIscSqlInterprete;
  isc_sqlcodeType m_pIscSqlcode;
  isc_encode_timestampType m_pIscEncodeTimestamp;
  isc_create_blob2Type m_pIscCreateBlob2;
  isc_open_blob2Type m_pIscOpenBlob2;
  isc_put_segmentType m_pIscPutSegment;
  isc_close_blobType m_pIscCloseBlob;
  isc_commit_retainingType m_pIscCommitRetaining;
  isc_dsql_free_statementType m_pIscDsqlFreeStatement;
  isc_dsql_describe_bindType m_pIscDsqlDescribeBind;
  isc_dsql_executeType m_pIscDsqlExecute;
  isc_dsql_sql_infoType m_pIscDsqlSqlInfo;
  isc_vax_integerType m_pIscVaxInteger;
  isc_dsql_execute2Type m_pIscDsqlExecute2;
  isc_dsql_fetchType m_pIscDsqlFetch;
  isc_decode_timestampType m_pIscDecodeTimestamp;
  isc_decode_sql_dateType m_pIscDecodeSqlDate;
  isc_decode_sql_timeType m_pIscDecodeSqlTime;
  isc_get_segmentType m_pIscGetSegment;
};

#endif // __FIREBIRD_INTERFACES_H__
