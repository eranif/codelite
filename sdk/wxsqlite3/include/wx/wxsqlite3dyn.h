///////////////////////////////////////////////////////////////////////////////
// Name:        wxsqlite3dyn.h
// Purpose:     wxWidgets wrapper around the SQLite3 embedded database library.
// Author:      Ulrich Telle
// Modified by:
// Created:     2005-11-12
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/// \file wxsqlite3dyn.h Definition of the SQLite3 API functions

#ifdef DYNFUNC

DYNFUNC(return, void *,                sqlite3_aggregate_context,     (sqlite3_context *p, int nBytes), (p, nBytes));
DYNFUNC(return, int,                   sqlite3_aggregate_count,       (sqlite3_context *p), (p));
DYNFUNC(return, int,                   sqlite3_bind_blob,             (sqlite3_stmt *pStmt, int i, const void *zData, int nData, void (*xDel)(void*)), (pStmt, i, zData, nData, xDel));
DYNFUNC(return, int,                   sqlite3_bind_double,           (sqlite3_stmt *pStmt, int i, double rValue), (pStmt, i, rValue));
DYNFUNC(return, int,                   sqlite3_bind_int,              (sqlite3_stmt *pStmt, int i, int iValue), (pStmt, i, iValue));
DYNFUNC(return, int,                   sqlite3_bind_int64,            (sqlite3_stmt *pStmt, int i, sqlite_int64 iValue), (pStmt, i, iValue));
DYNFUNC(return, int,                   sqlite3_bind_null,             (sqlite3_stmt *pStmt, int i), (pStmt, i));
DYNFUNC(return, int,                   sqlite3_bind_parameter_count,  (sqlite3_stmt *pStmt), (pStmt));
DYNFUNC(return, int,                   sqlite3_bind_parameter_index,  (sqlite3_stmt *pStmt, const char *zName), (pStmt, zName));
DYNFUNC(return, const char *,          sqlite3_bind_parameter_name,   (sqlite3_stmt *pStmt, int i), (pStmt, i));
DYNFUNC(return, int,                   sqlite3_bind_text,             (sqlite3_stmt *pStmt, int i, const char *zData, int nData, void (*xDel)(void*)), (pStmt, i, zData, nData, xDel));
// DYNFUNC(return, int,                   sqlite3_bind_text16,           (sqlite3_stmt *pStmt, int i, const void *zData, int nData, void (*xDel)(void*)), (pStmt, i, zData, nData, xDel));
// DYNFUNC(return, int,                   sqlite3_bind_value,            (sqlite3_stmt *pStmt, int i, const sqlite3_value *pValue), (pStmt, i, pValue));
// DYNFUNC(return, int,                   sqlite3_busy_handler,          (sqlite3 *db, int (*xBusy)(void*,int), void *pArg), (db, xBusy, pArg));
DYNFUNC(return, int,                   sqlite3_busy_timeout,          (sqlite3 *db, int ms), (db, ms));
DYNFUNC(return, int,                   sqlite3_changes,               (sqlite3 *db), (db));
DYNFUNC(return, int,                   sqlite3_clear_bindings,        (sqlite3_stmt *pStmt), (pStmt));
DYNFUNC(return, int,                   sqlite3_close,                 (sqlite3 *db), (db));
DYNFUNC(return, int,                   sqlite3_collation_needed,      (sqlite3 *db, void *pCollNeededArg, void(*xCollNeeded)(void*,sqlite3*,int eTextRep,const char*)), (db, pCollNeededArg, xCollNeeded));
// DYNFUNC(return, int,                   sqlite3_collation_needed16,    (sqlite3 *db, void *pCollNeededArg, void(*xCollNeeded16)(void*,sqlite3*,int eTextRep,const void*)), (db, pCollNeededArg, xCollNeeded16));
DYNFUNC(return, const void *,          sqlite3_column_blob,           (sqlite3_stmt *pStmt, int iCol), (pStmt, iCol));
DYNFUNC(return, int,                   sqlite3_column_bytes,          (sqlite3_stmt *pStmt, int iCol), (pStmt, iCol));
// DYNFUNC(return, int,                   sqlite3_column_bytes16,        (sqlite3_stmt *pStmt, int iCol), (pStmt, iCol));
DYNFUNC(return, int,                   sqlite3_column_count,          (sqlite3_stmt *pStmt), (pStmt));
DYNFUNC(return, const char *,          sqlite3_column_decltype,       (sqlite3_stmt *pStmt, int i), (pStmt, i));
// DYNFUNC(return, const void *,          sqlite3_column_decltype16,     (sqlite3_stmt *pStmt, int i), (pStmt, i));
DYNFUNC(return, double,                sqlite3_column_double,         (sqlite3_stmt *pStmt, int iCol), (pStmt, iCol));
DYNFUNC(return, int,                   sqlite3_column_int,            (sqlite3_stmt *pStmt, int iCol), (pStmt, iCol));
DYNFUNC(return, sqlite_int64,          sqlite3_column_int64,          (sqlite3_stmt *pStmt, int iCol), (pStmt, iCol));
DYNFUNC(return, const char *,          sqlite3_column_name,           (sqlite3_stmt *pStmt, int iCol), (pStmt, iCol));
// DYNFUNC(return, const void *,          sqlite3_column_name16,         (sqlite3_stmt *pStmt, int iCol), (pStmt, iCol));
DYNFUNC(return, const unsigned char *, sqlite3_column_text,           (sqlite3_stmt *pStmt, int iCol), (pStmt, iCol));
// DYNFUNC(return, const void *,          sqlite3_column_text16,         (sqlite3_stmt *pStmt, int iCol), (pStmt, iCol));
DYNFUNC(return, int,                   sqlite3_column_type,           (sqlite3_stmt *pStmt, int iCol), (pStmt, iCol));
DYNFUNC(return, void *,                sqlite3_commit_hook,           (sqlite3 *db, int (*xCallback)(void*), void *pArg), (db, xCallback, pArg));
DYNFUNC(return, int,                   sqlite3_complete,              (const char *sql), (sql));
// DYNFUNC(return, int,                   sqlite3_complete16,            (const void *sql), (sql));
DYNFUNC(return, int,                   sqlite3_create_collation,      (sqlite3 *db, const char *zName, int eTextRep, void*v, int(*xCompare)(void*,int,const void*,int,const void*)), (db, zName, eTextRep, v, xCompare));
// DYNFUNC(return, int,                   sqlite3_create_collation16,    (sqlite3 *db, const char *zName, int eTextRep, void*v, int(*xCompare)(void*,int,const void*,int,const void*)), (db, zName, eTextRep, v, xCompare));
DYNFUNC(return, int,                   sqlite3_create_function,       (sqlite3 *db, const char *zFunctionName, int nArg, int eTextRep, void*v, void (*xFunc)(sqlite3_context*,int,sqlite3_value**), void (*xStep)(sqlite3_context*,int,sqlite3_value**), void (*xFinal)(sqlite3_context*)), (db, zFunctionName, nArg, eTextRep, v, xFunc, xStep, xFinal));
// DYNFUNC(return, int,                   sqlite3_create_function16,     (sqlite3 *db, const void *zFunctionName, int nArg, int eTextRep, void*v, void (*xFunc)(sqlite3_context*,int,sqlite3_value**), void (*xStep)(sqlite3_context*,int,sqlite3_value**), void (*xFinal)(sqlite3_context*)), (db, zFunctionName, nArg, eTextRep, v, xFunc, xStep, xFinal));
// DYNFUNC(return, int,                   sqlite3_data_count,            (sqlite3_stmt *pStmt), (pStmt));
// DYNFUNC(return, sqlite3 *,             sqlite3_db_handle,             (sqlite3_stmt *pStmt), (pStmt));
DYNFUNC(return, int,                   sqlite3_enable_load_extension, (sqlite3 *db, int onoff), (db, onoff));
// DYNFUNC(return, int,                   sqlite3_errcode,               (sqlite3 *db), (db));
DYNFUNC(return, const char *,          sqlite3_errmsg,                (sqlite3 *db), (db));
// DYNFUNC(return, const void *,          sqlite3_errmsg16,              (sqlite3 *db), (db));
DYNFUNC(return, int,                   sqlite3_exec,                  (sqlite3 *db, const char *sql, sqlite3_callback c, void *v, char **errmsg), (db, sql, c, v, errmsg));
// DYNFUNC(return, int,                   sqlite3_expired,               (sqlite3_stmt *pStmt), (pStmt));
DYNFUNC(return, int,                   sqlite3_extended_result_codes, (sqlite3 *db, int onoff), (db, onoff));
DYNFUNC(return, int,                   sqlite3_finalize,              (sqlite3_stmt *pStmt), (pStmt));
DYNFUNC(;, void,                       sqlite3_free,                  (char *z), (z));
DYNFUNC(;, void,                       sqlite3_free_table,            (char **result), (result));
DYNFUNC(return, int,                   sqlite3_get_autocommit,        (sqlite3 *db), (db));
// DYNFUNC(return, void *,                sqlite3_get_auxdata,           (sqlite3_context *pCtx, int iArg), (pCtx, iArg));
DYNFUNC(return, int,                   sqlite3_get_table,             (sqlite3 *db, const char *sql, char ***resultp, int *nrow, int *ncolumn, char **errmsg), (db, sql, resultp, nrow, ncolumn, errmsg));
// DYNFUNC(return, int,                   sqlite3_global_recover,        (), ());
DYNFUNC(;, void,                       sqlite3_interrupt,             (sqlite3 *db), (db));
DYNFUNC(return, sqlite_int64,          sqlite3_last_insert_rowid,     (sqlite3 *db), (db));
DYNFUNC(return, int,                   sqlite3_load_extension,        (sqlite3 *db, const char *zFile, const char *zProc, char **pzErrMsg), (db, zFile, zProc, pzErrMsg));
DYNFUNC(return, const char *,          sqlite3_libversion,            (void), ());
// DYNFUNC(return, int,                   sqlite3_libversion_number,     (void), ());
// DYNFUNC(return, char *,                sqlite3_mprintf,               (const char *zFormat,...), (zFormat,...));
DYNFUNC(return, int,                   sqlite3_open,                  (const char *filename, sqlite3 **ppDb), (filename, ppDb));
// DYNFUNC(return, int,                   sqlite3_open16,                (const void *filename, sqlite3 **ppDb), (filename, ppDb));
DYNFUNC(return, int,                   sqlite3_prepare_v2,            (sqlite3 *db, const char *zSql, int nBytes, sqlite3_stmt **ppStmt, const char **pzTail), (db, zSql, nBytes, ppStmt, pzTail));
// DYNFUNC(return, int,                   sqlite3_prepare16_v2,          (sqlite3 *db, const void *zSql, int nBytes, sqlite3_stmt **ppStmt, const void **pzTail), (db, zSql, nBytes, ppStmt, pzTail));
// DYNFUNC(;, void,                       sqlite3_progress_handler,      (sqlite3 *db, int nOps, int (*xProgress)(void*), void *pArg), (db, nOps, xProgress, pArg));
DYNFUNC(return, int,                   sqlite3_reset,                 (sqlite3_stmt *pStmt), (pStmt));
DYNFUNC(;, void,                       sqlite3_result_blob,           (sqlite3_context *pCtx, const void *z, int n, void (*xDel)(void *)), (pCtx, z, n, xDel));
DYNFUNC(;, void,                       sqlite3_result_double,         (sqlite3_context *pCtx, double rVal), (pCtx, rVal));
DYNFUNC(;, void,                       sqlite3_result_error,          (sqlite3_context *pCtx, const char *z, int n), (pCtx, z, n));
// DYNFUNC(;, void,                       sqlite3_result_error16,        (sqlite3_context *pCtx, const void *z, int n), (pCtx, z, n));
DYNFUNC(;, void,                       sqlite3_result_int,            (sqlite3_context *pCtx, int iVal), (pCtx, iVal));
DYNFUNC(;, void,                       sqlite3_result_int64,          (sqlite3_context *pCtx, sqlite_int64 iVal), (pCtx, iVal));
DYNFUNC(;, void,                       sqlite3_result_null,           (sqlite3_context *pCtx), (pCtx));
DYNFUNC(;, void,                       sqlite3_result_text,           (sqlite3_context *pCtx, const char *z, int n, void (*xDel)(void *)), (pCtx, z, n, xDel));
// DYNFUNC(;, void,                       sqlite3_result_text16,         (sqlite3_context *pCtx, const void *z, int n, void (*xDel)(void *)), (pCtx, z, n, xDel));
// DYNFUNC(;, void,                       sqlite3_result_text16be,       (sqlite3_context *pCtx, const void *z, int n, void (*xDel)(void *)), (pCtx, z, n, xDel));
// DYNFUNC(;, void,                       sqlite3_result_text16le,       (sqlite3_context *pCtx, const void *z, int n, void (*xDel)(void *)), (pCtx, z, n, xDel));
DYNFUNC(;, void,                       sqlite3_result_value,          (sqlite3_context *pCtx, sqlite3_value *pValue), (pCtx, pValue));
DYNFUNC(return, void *,                sqlite3_rollback_hook,         (sqlite3 *db, void (*xCallback)(void*), void *pArg), (db, xCallback, pArg));
DYNFUNC(return, int,                   sqlite3_set_authorizer,        (sqlite3 *db, int (*xAuth)(void*,int,const char*,const char*,const char*,const char*), void *pArg), (db, xAuth, pArg));
// DYNFUNC(;, void,                       sqlite3_set_auxdata,           (sqlite3_context *pCtx, int iArg, void *pAux, void (*xDelete)(void*)), (pCtx, iArg, pAux, xDelete));
// DYNFUNC(return, char *,                sqlite3_snprintf,              (int n, char *zBuf, const char *zFormat, ...), (n, zBuf, zFormat, ...));
DYNFUNC(return, int,                   sqlite3_step,                  (sqlite3_stmt *pStmt), (pStmt));
// DYNFUNC(return, int,                   sqlite3_total_changes,         (sqlite3 *db), (db));
// DYNFUNC(return, void *,                sqlite3_trace,                 (sqlite3 *db, void(*xTrace)(void*,const char*), void *pArg), (db, xTrace, pArg));
// DYNFUNC(return, int,                   sqlite3_transfer_bindings,     (sqlite3_stmt *pStmt, sqlite3_stmt *pStmt), (pStmt, pStmt));
DYNFUNC(return, void *,                sqlite3_update_hook,           (sqlite3 *db, void (*xCallback)(void *, int, char const *, char const *, wxsqlite_int64), void *pArg), (db, xCallback, pArg));
DYNFUNC(return, void *,                sqlite3_user_data,             (sqlite3_context *pCtx), (pCtx));
DYNFUNC(return, const void *,          sqlite3_value_blob,            (sqlite3_value *pVal), (pVal));
DYNFUNC(return, int,                   sqlite3_value_bytes,           (sqlite3_value *pVal), (pVal));
// DYNFUNC(return, int,                   sqlite3_value_bytes16,         (sqlite3_value *pVal), (pVal));
DYNFUNC(return, double,                sqlite3_value_double,          (sqlite3_value *pVal), (pVal));
DYNFUNC(return, int,                   sqlite3_value_int,             (sqlite3_value *pVal), (pVal));
DYNFUNC(return, sqlite_int64,          sqlite3_value_int64,           (sqlite3_value *pVal), (pVal));
DYNFUNC(return, const unsigned char *, sqlite3_value_text,            (sqlite3_value *pVal), (pVal));
// DYNFUNC(return, const void *,          sqlite3_value_text16,          (sqlite3_value *pVal), (pVal));
// DYNFUNC(return, const void *,          sqlite3_value_text16be,        (sqlite3_value *pVal), (pVal));
// DYNFUNC(return, const void *,          sqlite3_value_text16le,        (sqlite3_value *pVal), (pVal));
DYNFUNC(return, int,                   sqlite3_value_type,            (sqlite3_value *pVal), (pVal));
DYNFUNC(return, char *,                sqlite3_vmprintf,              (const char* p, va_list ap), (p, ap));

#if WXSQLITE3_HAVE_METADATA
DYNFUNC(return, const char *,          sqlite3_column_database_name,  (sqlite3_stmt *pStmt, int iCol), (pStmt, iCol));
// DYNFUNC(return, const void *,          sqlite3_column_database_name16, (sqlite3_stmt *pStmt, int iCol), (pStmt, iCol));
DYNFUNC(return, const char *,          sqlite3_column_table_name,     (sqlite3_stmt *pStmt, int iCol), (pStmt, iCol));
// DYNFUNC(return, const void *,          sqlite3_column_table_name16,   (sqlite3_stmt *pStmt, int iCol), (pStmt, iCol));
DYNFUNC(return, const char *,          sqlite3_column_origin_name,    (sqlite3_stmt *pStmt, int iCol), (pStmt, iCol));
// DYNFUNC(return, const void *,          sqlite3_column_origin_name16,  (sqlite3_stmt *pStmt, int iCol), (pStmt, iCol));
DYNFUNC(return, int,                   sqlite3_table_column_metadata, (sqlite3 *db, const char *zDbName, const char *zTableName, const char *zColumnName, char const **pzDataType, char const **pzCollSeq, int *pNotNull, int *pPrimaryKey, int *pAutoinc), (db, zDbName, zTableName, zColumnName, pzDataType, pzCollSeq, pNotNull, pPrimaryKey, pAutoinc));
#endif

// SQLcrypt API
// Additional error codes: SQLCRYPT3_TOOSHORT, SQLCRYPT3_TOOLONG, SQLCRYPT3_BADLIC
// DYNFUNC(return, int,                  sqlcrypt3_passphrase,           (sqlite3 *db, const char *key, int codec, char **errmsg), (db, key, codec, errmsg));
// DYNFUNC(return, int,                  sqlcrypt3_license_key,          (const char *lic), (lic));

// SQLite-Crypt API
// ATTN: different open call
// DYNFUNC(return, int,                   sqlite3_open,                  (const char *filename, const char *strPass, sqlite3 **ppDb), (filename, strPass, ppDb));
// DYNFUNC(;, void,                       sqlite3_changepassword,        (sqlite3* db,const char* strNewPass), (db, strNewPass));

#endif
