///////////////////////////////////////////////////////////////////////////////
// Name:        wxsqlite3.cpp
// Purpose:     Implementation of wxSQLite3 classes
// Author:      Ulrich Telle
// Modified by:
// Created:     2005-07-06
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/// \file wxsqlite3.cpp Implementation of the wxSQLite3 class

#if defined(__GNUG__) && !defined(__APPLE__) && !defined(_WIN32)
#pragma implementation "wxsqlite3.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/wxsqlite3.h"
#include "wx/wxsqlite3opt.h"

// Suppress some Visual C++ warnings regarding the default constructor
// for a C struct used only in SQLite modules
#ifdef __VISUALC__
#pragma warning(disable : 4510)
#pragma warning(disable : 4610)
#endif

#include "sqlite3.h"

// Dynamic loading of the SQLite library

#if wxUSE_DYNAMIC_SQLITE3_LOAD

#include "wx/dynlib.h"

#define DYNFUNC(rcode, rtype, fname, farg, farguse) \
    typedef rtype(*p##fname) farg;                  \
    static p##fname s_##fname = NULL;
#include "wx/wxsqlite3dyn.h"
#undef DYNFUNC

static wxDynamicLibrary s_dll;

static void InitSQLite3DLL()
{
    if(s_dll.IsLoaded()) {
        return;
    }

#ifdef __WIN32__
    if(!s_dll.Load(wxT("sqlite3")))
#else
    if(!s_dll.Load(wxT("libsqlite3")))
#endif
    {
        throw wxSQLite3Exception(-1, wxT("error loading dynamic library"));
    }

#define DYNFUNC(rcode, rtype, fname, farg, farguse)                                       \
    s_##fname = (p##fname)s_dll.GetSymbol(wxT(#fname));                                   \
    if(!s_##fname) {                                                                      \
        s_dll.Unload();                                                                   \
        throw wxSQLite3Exception(-1, wxT("error getting symbol <") wxT(#fname) wxT(">")); \
    }
#include "wx/wxsqlite3dyn.h"
#undef DYNFUNC
};

#define DYNFUNC(rcode, rtype, fname, farg, farguse) \
    rtype fname farg                                \
    {                                               \
        InitSQLite3DLL();                           \
        rcode s_##fname farguse;                    \
    };
#include "wx/wxsqlite3dyn.h"
#undef DYNFUNC

#endif // wxUSE_DYNAMIC_SQLITE3_LOAD

// Error messages. Pre-2.9 wxTRANSLATE returned  a wxChar*; since then it's char*
#if wxVERSION_NUMBER < 2900
#define wxSomething wxChar
#else
#define wxSomething char
#endif
const wxSomething* wxERRMSG_NODB = wxTRANSLATE("No Database opened");
const wxSomething* wxERRMSG_NOSTMT = wxTRANSLATE("Statement not accessible");
const wxSomething* wxERRMSG_NOMEM = wxTRANSLATE("Out of memory");
const wxSomething* wxERRMSG_DECODE = wxTRANSLATE("Cannot decode binary");
const wxSomething* wxERRMSG_INVALID_INDEX = wxTRANSLATE("Invalid field index");
const wxSomething* wxERRMSG_INVALID_NAME = wxTRANSLATE("Invalid field name");
const wxSomething* wxERRMSG_INVALID_ROW = wxTRANSLATE("Invalid row index");
const wxSomething* wxERRMSG_INVALID_QUERY = wxTRANSLATE("Invalid scalar query");

const wxSomething* wxERRMSG_NORESULT = wxTRANSLATE("Null Results pointer");
const wxSomething* wxERRMSG_BIND_STR = wxTRANSLATE("Error binding string param");
const wxSomething* wxERRMSG_BIND_INT = wxTRANSLATE("Error binding int param");
const wxSomething* wxERRMSG_BIND_INT64 = wxTRANSLATE("Error binding int64 param");
const wxSomething* wxERRMSG_BIND_DBL = wxTRANSLATE("Error binding double param");
const wxSomething* wxERRMSG_BIND_BLOB = wxTRANSLATE("Error binding blob param");
const wxSomething* wxERRMSG_BIND_DATETIME = wxTRANSLATE("Error binding date/time param");
const wxSomething* wxERRMSG_BIND_NULL = wxTRANSLATE("Error binding NULL param");
const wxSomething* wxERRMSG_BIND_CLEAR = wxTRANSLATE("Error clearing bindings");

const wxSomething* wxERRMSG_NOMETADATA = wxTRANSLATE("Meta data support not available");
const wxSomething* wxERRMSG_NOCODEC = wxTRANSLATE("Encryption support not available");
const wxSomething* wxERRMSG_NOLOADEXT = wxTRANSLATE("Loadable extension support not available");

// ----------------------------------------------------------------------------
// inline conversion from UTF8 strings to wxStringe
// ----------------------------------------------------------------------------

inline wxString UTF8toWxString(const char* localValue)
{
#if wxUSE_UNICODE
    return wxString(localValue, wxConvUTF8);
#else
    return wxString(wxConvUTF8.cMB2WC(localValue), *wxConvCurrent);
#endif
}

// ----------------------------------------------------------------------------
// inline conversion from wxString to wxLongLong
// ----------------------------------------------------------------------------

inline wxLongLong ConvertStringToLongLong(const wxString& str, wxLongLong defValue /*=0*/)
{
    size_t n = str.Length();
    size_t j = 0;
    wxLongLong value = 0;
    bool negative = false;

    if(str[j] == '-') {
        negative = true;
        j++;
    }

    while(j < n) {
        if(str[j] < '0' || str[j] > '9') {
            return defValue;
        }
        value *= 10;
        value += (str[j] - '0');
        j++;
    }

    return negative ? -value : value;
}

// ----------------------------------------------------------------------------
// wxSQLite3Exception: class
// ----------------------------------------------------------------------------

wxSQLite3Exception::wxSQLite3Exception(int errorCode, const wxString& errorMsg)
    : m_errorCode(errorCode)
{
    m_errorMessage = ErrorCodeAsString(errorCode) + _T("[") + wxString::Format(_T("%d"), errorCode) + _T("]: ") +
                     wxGetTranslation(errorMsg);
}

wxSQLite3Exception::wxSQLite3Exception(const wxSQLite3Exception& e)
    : m_errorCode(e.m_errorCode)
    , m_errorMessage(e.m_errorMessage)
{
}

const wxString wxSQLite3Exception::ErrorCodeAsString(int errorCode)
{
    switch(errorCode) {
    case SQLITE_OK:
        return _T("SQLITE_OK");
    case SQLITE_ERROR:
        return _T("SQLITE_ERROR");
    case SQLITE_INTERNAL:
        return _T("SQLITE_INTERNAL");
    case SQLITE_PERM:
        return _T("SQLITE_PERM");
    case SQLITE_ABORT:
        return _T("SQLITE_ABORT");
    case SQLITE_BUSY:
        return _T("SQLITE_BUSY");
    case SQLITE_LOCKED:
        return _T("SQLITE_LOCKED");
    case SQLITE_NOMEM:
        return _T("SQLITE_NOMEM");
    case SQLITE_READONLY:
        return _T("SQLITE_READONLY");
    case SQLITE_INTERRUPT:
        return _T("SQLITE_INTERRUPT");
    case SQLITE_IOERR:
        return _T("SQLITE_IOERR");
    case SQLITE_CORRUPT:
        return _T("SQLITE_CORRUPT");
    case SQLITE_NOTFOUND:
        return _T("SQLITE_NOTFOUND");
    case SQLITE_FULL:
        return _T("SQLITE_FULL");
    case SQLITE_CANTOPEN:
        return _T("SQLITE_CANTOPEN");
    case SQLITE_PROTOCOL:
        return _T("SQLITE_PROTOCOL");
    case SQLITE_EMPTY:
        return _T("SQLITE_EMPTY");
    case SQLITE_SCHEMA:
        return _T("SQLITE_SCHEMA");
    case SQLITE_TOOBIG:
        return _T("SQLITE_TOOBIG");
    case SQLITE_CONSTRAINT:
        return _T("SQLITE_CONSTRAINT");
    case SQLITE_MISMATCH:
        return _T("SQLITE_MISMATCH");
    case SQLITE_MISUSE:
        return _T("SQLITE_MISUSE");
    case SQLITE_NOLFS:
        return _T("SQLITE_NOLFS");
    case SQLITE_AUTH:
        return _T("SQLITE_AUTH");
    case SQLITE_FORMAT:
        return _T("SQLITE_FORMAT");
    case SQLITE_RANGE:
        return _T("SQLITE_RANGE");
    case SQLITE_NOTADB:
        return _T("SQLITE_NOTADB");
    case SQLITE_ROW:
        return _T("SQLITE_ROW");
    case SQLITE_DONE:
        return _T("SQLITE_DONE");
    // Extended error codes
    case SQLITE_IOERR_READ:
        return _T("SQLITE_IOERR_READ");
    case SQLITE_IOERR_SHORT_READ:
        return _T("SQLITE_IOERR_SHORT_READ");
    case SQLITE_IOERR_WRITE:
        return _T("SQLITE_IOERR_WRITE");
    case SQLITE_IOERR_FSYNC:
        return _T("SQLITE_IOERR_FSYNC");
    case SQLITE_IOERR_DIR_FSYNC:
        return _T("SQLITE_IOERR_DIR_FSYNC");
    case SQLITE_IOERR_TRUNCATE:
        return _T("SQLITE_IOERR_TRUNCATE");
    case SQLITE_IOERR_FSTAT:
        return _T("SQLITE_IOERR_FSTAT");
    case SQLITE_IOERR_UNLOCK:
        return _T("SQLITE_IOERR_UNLOCK");
    case SQLITE_IOERR_RDLOCK:
        return _T("SQLITE_IOERR_RDLOCK");

    case WXSQLITE_ERROR:
        return _T("WXSQLITE_ERROR");
    default:
        return _T("UNKNOWN_ERROR");
    }
}

wxSQLite3Exception::~wxSQLite3Exception() {}

// ----------------------------------------------------------------------------
// wxSQLite3StatementBuffer: class providing a statement buffer
//                           for use with the SQLite3 vmprintf function
// ----------------------------------------------------------------------------

wxSQLite3StatementBuffer::wxSQLite3StatementBuffer() { m_buffer = 0; }

wxSQLite3StatementBuffer::~wxSQLite3StatementBuffer() { Clear(); }

void wxSQLite3StatementBuffer::Clear()
{
    if(m_buffer) {
        sqlite3_free(m_buffer);
        m_buffer = 0;
    }
}

const char* wxSQLite3StatementBuffer::Format(const char* format, ...)
{
    Clear();
    va_list va;
    va_start(va, format);
    m_buffer = sqlite3_vmprintf(format, va);
    va_end(va);
    return m_buffer;
}

// ----------------------------------------------------------------------------
// wxSQLite3ResultSet: class providing access to the result set of a query
// ----------------------------------------------------------------------------

wxSQLite3ResultSet::wxSQLite3ResultSet()
{
    m_stmt = 0;
    m_eof = true;
    m_first = true;
    m_cols = 0;
    m_ownStmt = false;
}

wxSQLite3ResultSet::wxSQLite3ResultSet(const wxSQLite3ResultSet& resultSet)
{
    m_stmt = resultSet.m_stmt;
    // Only one object can own the statement
    const_cast<wxSQLite3ResultSet&>(resultSet).m_stmt = 0;
    m_eof = resultSet.m_eof;
    m_first = resultSet.m_first;
    m_cols = resultSet.m_cols;
    m_ownStmt = resultSet.m_ownStmt;
}

wxSQLite3ResultSet::wxSQLite3ResultSet(void* db, void* stmt, bool eof, bool first, bool ownStmt /*=true*/)
{
    m_db = db;
    m_stmt = stmt;
    m_eof = eof;
    m_first = first;
    m_cols = sqlite3_column_count((sqlite3_stmt*)m_stmt);
    m_ownStmt = ownStmt;
}

wxSQLite3ResultSet::~wxSQLite3ResultSet()
{
    try {
        Finalize();
    } catch(...) {
    }
}

wxSQLite3ResultSet& wxSQLite3ResultSet::operator=(const wxSQLite3ResultSet& resultSet)
{
    try {
        Finalize();
    } catch(...) {
    }
    m_stmt = resultSet.m_stmt;
    // Only one object can own the statement
    const_cast<wxSQLite3ResultSet&>(resultSet).m_stmt = 0;
    m_eof = resultSet.m_eof;
    m_first = resultSet.m_first;
    m_cols = resultSet.m_cols;
    m_ownStmt = resultSet.m_ownStmt;
    return *this;
}

int wxSQLite3ResultSet::GetColumnCount()
{
    CheckStmt();
    return m_cols;
}

wxString wxSQLite3ResultSet::GetAsString(int columnIndex)
{
    CheckStmt();

    if(columnIndex < 0 || columnIndex > m_cols - 1) {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_INVALID_INDEX);
    }

    const char* localValue = (const char*)sqlite3_column_text((sqlite3_stmt*)m_stmt, columnIndex);
    return UTF8toWxString(localValue);
}

wxString wxSQLite3ResultSet::GetAsString(const wxString& columnName)
{
    int columnIndex = FindColumnIndex(columnName);
    const char* localValue = (const char*)sqlite3_column_text((sqlite3_stmt*)m_stmt, columnIndex);
    return UTF8toWxString(localValue);
}

int wxSQLite3ResultSet::GetInt(int columnIndex, int nullValue /* = 0 */)
{
    if(GetColumnType(columnIndex) == SQLITE_NULL) {
        return nullValue;
    } else {
        return sqlite3_column_int((sqlite3_stmt*)m_stmt, columnIndex);
    }
}

int wxSQLite3ResultSet::GetInt(const wxString& columnName, int nullValue /* = 0 */)
{
    int columnIndex = FindColumnIndex(columnName);
    return GetInt(columnIndex, nullValue);
}

wxLongLong wxSQLite3ResultSet::GetInt64(int columnIndex, wxLongLong nullValue /* = 0 */)
{
    if(GetColumnType(columnIndex) == SQLITE_NULL) {
        return nullValue;
    } else {
        return wxLongLong(sqlite3_column_int64((sqlite3_stmt*)m_stmt, columnIndex));
    }
}

wxLongLong wxSQLite3ResultSet::GetInt64(const wxString& columnName, wxLongLong nullValue /* = 0 */)
{
    int columnIndex = FindColumnIndex(columnName);
    return GetInt64(columnIndex, nullValue);
}

double wxSQLite3ResultSet::GetDouble(int columnIndex, double nullValue /* = 0.0 */)
{
    if(GetColumnType(columnIndex) == SQLITE_NULL) {
        return nullValue;
    } else {
        return sqlite3_column_double((sqlite3_stmt*)m_stmt, columnIndex);
    }
}

double wxSQLite3ResultSet::GetDouble(const wxString& columnName, double nullValue /* = 0.0 */)
{
    int columnIndex = FindColumnIndex(columnName);
    return GetDouble(columnIndex, nullValue);
}

wxString wxSQLite3ResultSet::GetString(int columnIndex, const wxString& nullValue /* = wxEmptyString */)
{
    if(GetColumnType(columnIndex) == SQLITE_NULL) {
        return nullValue;
    } else {
        const char* localValue = (const char*)sqlite3_column_text((sqlite3_stmt*)m_stmt, columnIndex);
        return UTF8toWxString(localValue);
    }
}

wxString wxSQLite3ResultSet::GetString(const wxString& columnName, const wxString& nullValue /* = wxEmptyString */)
{
    int columnIndex = FindColumnIndex(columnName);
    return GetString(columnIndex, nullValue);
}

const unsigned char* wxSQLite3ResultSet::GetBlob(int columnIndex, int& len)
{
    CheckStmt();

    if(columnIndex < 0 || columnIndex > m_cols - 1) {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_INVALID_INDEX);
    }

    len = sqlite3_column_bytes((sqlite3_stmt*)m_stmt, columnIndex);
    return (const unsigned char*)sqlite3_column_blob((sqlite3_stmt*)m_stmt, columnIndex);
}

const unsigned char* wxSQLite3ResultSet::GetBlob(const wxString& columnName, int& len)
{
    int columnIndex = FindColumnIndex(columnName);
    return GetBlob(columnIndex, len);
}

wxMemoryBuffer& wxSQLite3ResultSet::GetBlob(int columnIndex, wxMemoryBuffer& buffer)
{
    CheckStmt();

    if(columnIndex < 0 || columnIndex > m_cols - 1) {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_INVALID_INDEX);
    }

    int len = sqlite3_column_bytes((sqlite3_stmt*)m_stmt, columnIndex);
    const void* blob = sqlite3_column_blob((sqlite3_stmt*)m_stmt, columnIndex);
    buffer.AppendData((void*)blob, len);
    return buffer;
}

wxMemoryBuffer& wxSQLite3ResultSet::GetBlob(const wxString& columnName, wxMemoryBuffer& buffer)
{
    int columnIndex = FindColumnIndex(columnName);
    return GetBlob(columnIndex, buffer);
}

wxDateTime wxSQLite3ResultSet::GetDate(int columnIndex)
{
    if(GetColumnType(columnIndex) == SQLITE_NULL) {
        return wxInvalidDateTime;
    } else {
        wxDateTime date;
        if(date.ParseDate(GetString(columnIndex))) {
            return date;
        } else {
            return wxInvalidDateTime;
        }
    }
}

wxDateTime wxSQLite3ResultSet::GetDate(const wxString& columnName)
{
    int columnIndex = FindColumnIndex(columnName);
    return GetDate(columnIndex);
}

wxDateTime wxSQLite3ResultSet::GetTime(int columnIndex)
{
    if(GetColumnType(columnIndex) == SQLITE_NULL) {
        return wxInvalidDateTime;
    } else {
        wxDateTime date;
        if(date.ParseTime(GetString(columnIndex))) {
            return date;
        } else {
            return wxInvalidDateTime;
        }
    }
}

wxDateTime wxSQLite3ResultSet::GetTime(const wxString& columnName)
{
    int columnIndex = FindColumnIndex(columnName);
    return GetTime(columnIndex);
}

wxDateTime wxSQLite3ResultSet::GetDateTime(int columnIndex)
{
    if(GetColumnType(columnIndex) == SQLITE_NULL) {
        return wxInvalidDateTime;
    } else {
        wxDateTime date;
        if(date.ParseDateTime(GetString(columnIndex))) {
            date.SetMillisecond(0);
            return date;
        } else {
            return wxInvalidDateTime;
        }
    }
}

wxDateTime wxSQLite3ResultSet::GetDateTime(const wxString& columnName)
{
    int columnIndex = FindColumnIndex(columnName);
    return GetDateTime(columnIndex);
}

wxDateTime wxSQLite3ResultSet::GetTimestamp(int columnIndex)
{
    if(GetColumnType(columnIndex) == SQLITE_NULL) {
        return wxInvalidDateTime;
    } else {
        wxDateTime date;
        if(date.ParseDateTime(GetString(columnIndex))) {
            return date;
        } else {
            return wxInvalidDateTime;
        }
    }
}

wxDateTime wxSQLite3ResultSet::GetTimestamp(const wxString& columnName)
{
    int columnIndex = FindColumnIndex(columnName);
    return GetTimestamp(columnIndex);
}

bool wxSQLite3ResultSet::GetBool(int columnIndex) { return GetInt(columnIndex) != 0; }

bool wxSQLite3ResultSet::GetBool(const wxString& columnName)
{
    int columnIndex = FindColumnIndex(columnName);
    return GetBool(columnIndex);
}

bool wxSQLite3ResultSet::IsNull(int columnIndex) { return (GetColumnType(columnIndex) == SQLITE_NULL); }

bool wxSQLite3ResultSet::IsNull(const wxString& columnName)
{
    int columnIndex = FindColumnIndex(columnName);
    return (GetColumnType(columnIndex) == SQLITE_NULL);
}

int wxSQLite3ResultSet::FindColumnIndex(const wxString& columnName)
{
    CheckStmt();

    wxCharBuffer strColumnName = wxConvUTF8.cWC2MB(columnName.wc_str(*wxConvCurrent));
    const char* localColumnName = strColumnName;

    if(columnName.Len() > 0) {
        for(int columnIndex = 0; columnIndex < m_cols; columnIndex++) {
            const char* temp = sqlite3_column_name((sqlite3_stmt*)m_stmt, columnIndex);

            if(strcmp(localColumnName, temp) == 0) {
                return columnIndex;
            }
        }
    }

    throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_INVALID_INDEX);
}

wxString wxSQLite3ResultSet::GetColumnName(int columnIndex)
{
    CheckStmt();

    if(columnIndex < 0 || columnIndex > m_cols - 1) {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_INVALID_INDEX);
    }

    const char* localValue = sqlite3_column_name((sqlite3_stmt*)m_stmt, columnIndex);
    return UTF8toWxString(localValue);
}

wxString wxSQLite3ResultSet::GetDeclaredColumnType(int columnIndex)
{
    CheckStmt();

    if(columnIndex < 0 || columnIndex > m_cols - 1) {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_INVALID_INDEX);
    }

    const char* localValue = sqlite3_column_decltype((sqlite3_stmt*)m_stmt, columnIndex);
    return UTF8toWxString(localValue);
}

int wxSQLite3ResultSet::GetColumnType(int columnIndex)
{
    CheckStmt();

    if(columnIndex < 0 || columnIndex > m_cols - 1) {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_INVALID_INDEX);
    }

    return sqlite3_column_type((sqlite3_stmt*)m_stmt, columnIndex);
}

bool wxSQLite3ResultSet::Eof()
{
    CheckStmt();
    return m_eof;
}

bool wxSQLite3ResultSet::NextRow()
{
    CheckStmt();

    int rc;
    if(m_first) {
        m_first = false;
        rc = (m_eof) ? SQLITE_DONE : SQLITE_ROW;
    } else {
        rc = sqlite3_step((sqlite3_stmt*)m_stmt);
    }

    if(rc == SQLITE_DONE) // no more rows
    {
        m_eof = true;
        return false;
    } else if(rc == SQLITE_ROW) // more rows
    {
        return true;
    } else {
        rc = sqlite3_finalize((sqlite3_stmt*)m_stmt);
        m_stmt = 0;
        const char* localError = sqlite3_errmsg((sqlite3*)m_db);
        throw wxSQLite3Exception(rc, UTF8toWxString(localError));
    }
}

void wxSQLite3ResultSet::Finalize()
{
    if(m_stmt && m_ownStmt) {
        int rc = sqlite3_finalize((sqlite3_stmt*)m_stmt);
        m_stmt = 0;
        if(rc != SQLITE_OK) {
            const char* localError = sqlite3_errmsg((sqlite3*)m_db);
            throw wxSQLite3Exception(rc, UTF8toWxString(localError));
        }
    }
}

void wxSQLite3ResultSet::CheckStmt()
{
    if(m_stmt == 0) {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_NOSTMT);
    }
}

wxString wxSQLite3ResultSet::GetDatabaseName(int columnIndex)
{
#if WXSQLITE3_HAVE_METADATA
    CheckStmt();
    if(columnIndex < 0 || columnIndex > m_cols - 1) {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_INVALID_INDEX);
    }

    const char* localValue = sqlite3_column_database_name((sqlite3_stmt*)m_stmt, columnIndex);
    if(localValue != NULL)
        return UTF8toWxString(localValue);
    else
        return wxEmptyString;
#else
    throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_NOMETADATA);
    return wxEmptyString;
#endif
}

wxString wxSQLite3ResultSet::GetTableName(int columnIndex)
{
#if WXSQLITE3_HAVE_METADATA
    CheckStmt();
    if(columnIndex < 0 || columnIndex > m_cols - 1) {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_INVALID_INDEX);
    }

    const char* localValue = sqlite3_column_table_name((sqlite3_stmt*)m_stmt, columnIndex);
    if(localValue != NULL)
        return UTF8toWxString(localValue);
    else
        return wxEmptyString;
#else
    throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_NOMETADATA);
    return wxEmptyString;
#endif
}

wxString wxSQLite3ResultSet::GetOriginName(int columnIndex)
{
#if WXSQLITE3_HAVE_METADATA
    CheckStmt();
    if(columnIndex < 0 || columnIndex > m_cols - 1) {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_INVALID_INDEX);
    }

    const char* localValue = sqlite3_column_origin_name((sqlite3_stmt*)m_stmt, columnIndex);
    if(localValue != NULL)
        return UTF8toWxString(localValue);
    else
        return wxEmptyString;
#else
    throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_NOMETADATA);
    return wxEmptyString;
#endif
}

// ----------------------------------------------------------------------------
// wxSQLite3Table: class holding the complete result set of a query
// ----------------------------------------------------------------------------

wxSQLite3Table::wxSQLite3Table()
{
    m_results = 0;
    m_rows = 0;
    m_cols = 0;
    m_currentRow = 0;
}

wxSQLite3Table::wxSQLite3Table(const wxSQLite3Table& table)
{
    m_results = table.m_results;
    // Only one object can own the results
    const_cast<wxSQLite3Table&>(table).m_results = 0;
    m_rows = table.m_rows;
    m_cols = table.m_cols;
    m_currentRow = table.m_currentRow;
}

wxSQLite3Table::wxSQLite3Table(char** results, int rows, int cols)
{
    m_results = results;
    m_rows = rows;
    m_cols = cols;
    m_currentRow = 0;
}

wxSQLite3Table::~wxSQLite3Table()
{
    try {
        Finalize();
    } catch(...) {
    }
}

wxSQLite3Table& wxSQLite3Table::operator=(const wxSQLite3Table& table)
{
    try {
        Finalize();
    } catch(...) {
    }
    m_results = table.m_results;
    // Only one object can own the results
    const_cast<wxSQLite3Table&>(table).m_results = 0;
    m_rows = table.m_rows;
    m_cols = table.m_cols;
    m_currentRow = table.m_currentRow;
    return *this;
}

void wxSQLite3Table::Finalize()
{
    if(m_results) {
        sqlite3_free_table(m_results);
        m_results = 0;
    }
}

int wxSQLite3Table::GetColumnCount()
{
    CheckResults();
    return m_cols;
}

int wxSQLite3Table::GetRowCount()
{
    CheckResults();
    return m_rows;
}

int wxSQLite3Table::FindColumnIndex(const wxString& columnName)
{
    CheckResults();

    wxCharBuffer strColumnName = wxConvUTF8.cWC2MB(columnName.wc_str(*wxConvCurrent));
    const char* localColumnName = strColumnName;

    if(columnName.Len() > 0) {
        for(int columnIndex = 0; columnIndex < m_cols; columnIndex++) {
            if(strcmp(localColumnName, m_results[columnIndex]) == 0) {
                return columnIndex;
            }
        }
    }

    throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_INVALID_NAME);
}

wxString wxSQLite3Table::GetAsString(int columnIndex)
{
    if(columnIndex < 0 || columnIndex > m_cols - 1) {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_INVALID_INDEX);
    }

    int nIndex = (m_currentRow * m_cols) + m_cols + columnIndex;
    const char* localValue = m_results[nIndex];
    return UTF8toWxString(localValue);
}

wxString wxSQLite3Table::GetAsString(const wxString& columnName)
{
    int index = FindColumnIndex(columnName);
    return GetAsString(index);
}

int wxSQLite3Table::GetInt(int columnIndex, int nullValue /* = 0 */)
{
    if(IsNull(columnIndex)) {
        return nullValue;
    } else {
        long value = nullValue;
        GetAsString(columnIndex).ToLong(&value);
        return (int)value;
    }
}

int wxSQLite3Table::GetInt(const wxString& columnName, int nullValue /* = 0 */)
{
    if(IsNull(columnName)) {
        return nullValue;
    } else {
        long value = nullValue;
        GetAsString(columnName).ToLong(&value);
        return (int)value;
    }
}

wxLongLong wxSQLite3Table::GetInt64(int columnIndex, wxLongLong nullValue /* = 0 */)
{
    if(IsNull(columnIndex)) {
        return nullValue;
    } else {
        return ConvertStringToLongLong(GetAsString(columnIndex), nullValue);
    }
}

wxLongLong wxSQLite3Table::GetInt64(const wxString& columnName, wxLongLong nullValue /* = 0 */)
{
    if(IsNull(columnName)) {
        return nullValue;
    } else {
        return ConvertStringToLongLong(GetAsString(columnName), nullValue);
    }
}

double wxSQLite3Table::GetDouble(int columnIndex, double nullValue /* = 0.0 */)
{
    if(IsNull(columnIndex)) {
        return nullValue;
    } else {
        double value = nullValue;
        GetAsString(columnIndex).ToDouble(&value);
        return value;
    }
}

double wxSQLite3Table::GetDouble(const wxString& columnName, double nullValue /* = 0.0 */)
{
    if(IsNull(columnName)) {
        return nullValue;
    } else {
        double value = nullValue;
        GetAsString(columnName).ToDouble(&value);
        return value;
    }
}

wxString wxSQLite3Table::GetString(int columnIndex, const wxString& nullValue /* = wxEmptyString */)
{
    if(IsNull(columnIndex)) {
        return nullValue;
    } else {
        return GetAsString(columnIndex);
    }
}

wxString wxSQLite3Table::GetString(const wxString& columnName, const wxString& nullValue /* = wxEmptyString */)
{
    if(IsNull(columnName)) {
        return nullValue;
    } else {
        return GetAsString(columnName);
    }
}

wxDateTime wxSQLite3Table::GetDate(int columnIndex)
{
    wxDateTime date;
    if(date.ParseDate(GetString(columnIndex))) {
        return date;
    } else {
        return wxInvalidDateTime;
    }
}

wxDateTime wxSQLite3Table::GetDate(const wxString& columnName)
{
    int columnIndex = FindColumnIndex(columnName);
    return GetDate(columnIndex);
}

wxDateTime wxSQLite3Table::GetTime(int columnIndex)
{
    wxDateTime date;
    if(date.ParseTime(GetString(columnIndex))) {
        return date;
    } else {
        return wxInvalidDateTime;
    }
}

wxDateTime wxSQLite3Table::GetTime(const wxString& columnName)
{
    int columnIndex = FindColumnIndex(columnName);
    return GetTime(columnIndex);
}

wxDateTime wxSQLite3Table::GetDateTime(int columnIndex)
{
    wxDateTime date;
    if(date.ParseDateTime(GetString(columnIndex))) {
        return date;
    } else {
        return wxInvalidDateTime;
    }
}

wxDateTime wxSQLite3Table::GetDateTime(const wxString& columnName)
{
    int columnIndex = FindColumnIndex(columnName);
    return GetDateTime(columnIndex);
}

bool wxSQLite3Table::GetBool(int columnIndex) { return GetInt(columnIndex) != 0; }

bool wxSQLite3Table::GetBool(const wxString& columnName)
{
    int columnIndex = FindColumnIndex(columnName);
    return GetBool(columnIndex);
}

bool wxSQLite3Table::IsNull(int columnIndex)
{
    CheckResults();

    if(columnIndex < 0 || columnIndex > m_cols - 1) {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_INVALID_INDEX);
    }

    int index = (m_currentRow * m_cols) + m_cols + columnIndex;
    const char* localValue = m_results[index];
    return (localValue == 0);
}

bool wxSQLite3Table::IsNull(const wxString& columnName)
{
    int index = FindColumnIndex(columnName);
    return IsNull(index);
}

wxString wxSQLite3Table::GetColumnName(int columnIndex)
{
    CheckResults();

    if(columnIndex < 0 || columnIndex > m_cols - 1) {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_INVALID_INDEX);
    }

    const char* localValue = m_results[columnIndex];
    return UTF8toWxString(localValue);
}

void wxSQLite3Table::SetRow(int row)
{
    CheckResults();

    if(row < 0 || row > m_rows - 1) {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_INVALID_ROW);
    }

    m_currentRow = row;
}

void wxSQLite3Table::CheckResults()
{
    if(m_results == 0) {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_NORESULT);
    }
}

// ----------------------------------------------------------------------------
// wxSQLite3Statement: class holding a prepared statement
// ----------------------------------------------------------------------------

wxSQLite3Statement::wxSQLite3Statement()
{
    m_db = 0;
    m_stmt = 0;
}

wxSQLite3Statement::wxSQLite3Statement(const wxSQLite3Statement& statement)
{
    m_db = statement.m_db;
    m_stmt = statement.m_stmt;
    // Only one object can own prepared statement
    const_cast<wxSQLite3Statement&>(statement).m_stmt = 0;
}

wxSQLite3Statement::wxSQLite3Statement(void* db, void* stmt)
{
    m_db = db;
    m_stmt = stmt;
}

wxSQLite3Statement::~wxSQLite3Statement()
{
    try {
        Finalize();
    } catch(...) {
    }
}

wxSQLite3Statement& wxSQLite3Statement::operator=(const wxSQLite3Statement& statement)
{
    m_db = statement.m_db;
    m_stmt = statement.m_stmt;
    // Only one object can own prepared statement
    const_cast<wxSQLite3Statement&>(statement).m_stmt = 0;
    return *this;
}

int wxSQLite3Statement::ExecuteUpdate()
{
    CheckDatabase();
    CheckStmt();

    const char* localError = 0;

    int rc = sqlite3_step((sqlite3_stmt*)m_stmt);

    if(rc == SQLITE_DONE) {
        int rowsChanged = sqlite3_changes((sqlite3*)m_db);

        rc = sqlite3_reset((sqlite3_stmt*)m_stmt);

        if(rc != SQLITE_OK) {
            localError = sqlite3_errmsg((sqlite3*)m_db);
            throw wxSQLite3Exception(rc, UTF8toWxString(localError));
        }

        return rowsChanged;
    } else {
        rc = sqlite3_reset((sqlite3_stmt*)m_stmt);
        localError = sqlite3_errmsg((sqlite3*)m_db);
        throw wxSQLite3Exception(rc, UTF8toWxString(localError));
    }
}

wxSQLite3ResultSet wxSQLite3Statement::ExecuteQuery()
{
    CheckDatabase();
    CheckStmt();

    int rc = sqlite3_step((sqlite3_stmt*)m_stmt);

    if(rc == SQLITE_DONE) // no more rows
    {
        return wxSQLite3ResultSet(m_db, m_stmt, true /*eof*/, true /*first*/, false);
    } else if(rc == SQLITE_ROW) // one or more rows
    {
        return wxSQLite3ResultSet(m_db, m_stmt, false /*eof*/, true /*first*/, false);
    } else {
        rc = sqlite3_reset((sqlite3_stmt*)m_stmt);
        const char* localError = sqlite3_errmsg((sqlite3*)m_db);
        throw wxSQLite3Exception(rc, UTF8toWxString(localError));
    }
}

int wxSQLite3Statement::GetParamCount()
{
    CheckStmt();
    return sqlite3_bind_parameter_count((sqlite3_stmt*)m_stmt);
}

int wxSQLite3Statement::GetParamIndex(const wxString& paramName)
{
    CheckStmt();

    wxCharBuffer strParamName = wxConvUTF8.cWC2MB(paramName.wc_str(*wxConvCurrent));
    const char* localParamName = strParamName;

    return sqlite3_bind_parameter_index((sqlite3_stmt*)m_stmt, localParamName);
}

wxString wxSQLite3Statement::GetParamName(int paramIndex)
{
    CheckStmt();
    const char* localParamName = sqlite3_bind_parameter_name((sqlite3_stmt*)m_stmt, paramIndex);
    return UTF8toWxString(localParamName);
}

void wxSQLite3Statement::Bind(int paramIndex, const wxString& stringValue)
{
    CheckStmt();

    wxCharBuffer strStringValue = stringValue.mb_str(wxConvISO8859_1).data();
    const char* localStringValue = strStringValue;

    int rc = sqlite3_bind_text((sqlite3_stmt*)m_stmt, paramIndex, localStringValue, -1, SQLITE_TRANSIENT);

    if(rc != SQLITE_OK) {
        throw wxSQLite3Exception(rc, wxERRMSG_BIND_STR);
    }
}

void wxSQLite3Statement::Bind(int paramIndex, int intValue)
{
    CheckStmt();
    int rc = sqlite3_bind_int((sqlite3_stmt*)m_stmt, paramIndex, intValue);

    if(rc != SQLITE_OK) {
        throw wxSQLite3Exception(rc, wxERRMSG_BIND_INT);
    }
}

void wxSQLite3Statement::Bind(int paramIndex, wxLongLong int64Value)
{
    CheckStmt();
    int rc = sqlite3_bind_int64((sqlite3_stmt*)m_stmt, paramIndex, int64Value.GetValue());

    if(rc != SQLITE_OK) {
        throw wxSQLite3Exception(rc, wxERRMSG_BIND_INT64);
    }
}

void wxSQLite3Statement::Bind(int paramIndex, double doubleValue)
{
    CheckStmt();
    int rc = sqlite3_bind_double((sqlite3_stmt*)m_stmt, paramIndex, doubleValue);

    if(rc != SQLITE_OK) {
        throw wxSQLite3Exception(rc, wxERRMSG_BIND_DBL);
    }
}

void wxSQLite3Statement::Bind(int paramIndex, const char* charValue)
{
    CheckStmt();
    int rc = sqlite3_bind_text((sqlite3_stmt*)m_stmt, paramIndex, charValue, -1, SQLITE_TRANSIENT);

    if(rc != SQLITE_OK) {
        throw wxSQLite3Exception(rc, wxERRMSG_BIND_STR);
    }
}

void wxSQLite3Statement::Bind(int paramIndex, const unsigned char* blobValue, int blobLen)
{
    CheckStmt();
    int rc = sqlite3_bind_blob((sqlite3_stmt*)m_stmt, paramIndex, (const void*)blobValue, blobLen, SQLITE_TRANSIENT);

    if(rc != SQLITE_OK) {
        throw wxSQLite3Exception(rc, wxERRMSG_BIND_BLOB);
    }
}

void wxSQLite3Statement::Bind(int paramIndex, const wxMemoryBuffer& blobValue)
{
    CheckStmt();
    int blobLen = blobValue.GetDataLen();
    int rc = sqlite3_bind_blob((sqlite3_stmt*)m_stmt, paramIndex, (const void*)blobValue.GetData(), blobLen,
                               SQLITE_TRANSIENT);

    if(rc != SQLITE_OK) {
        throw wxSQLite3Exception(rc, wxERRMSG_BIND_BLOB);
    }
}

void wxSQLite3Statement::BindDate(int paramIndex, const wxDateTime& date)
{
    if(date.IsValid()) {
        Bind(paramIndex, date.FormatISODate());
    } else {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_BIND_DATETIME);
    }
}

void wxSQLite3Statement::BindTime(int paramIndex, const wxDateTime& time)
{
    if(time.IsValid()) {
        Bind(paramIndex, time.FormatISOTime());
    } else {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_BIND_DATETIME);
    }
}

void wxSQLite3Statement::BindDateTime(int paramIndex, const wxDateTime& datetime)
{
    if(datetime.IsValid()) {
        Bind(paramIndex, datetime.Format(_T("%Y-%m-%d %H:%M:%S")));
    } else {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_BIND_DATETIME);
    }
}

void wxSQLite3Statement::BindTimestamp(int paramIndex, const wxDateTime& timestamp)
{
    if(timestamp.IsValid()) {
        Bind(paramIndex, timestamp.Format(_T("%Y-%m-%d %H:%M:%S.%l")));
    } else {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_BIND_DATETIME);
    }
}

void wxSQLite3Statement::BindBool(int paramIndex, bool value) { Bind(paramIndex, value ? 1 : 0); }

void wxSQLite3Statement::BindNull(int paramIndex)
{
    CheckStmt();
    int rc = sqlite3_bind_null((sqlite3_stmt*)m_stmt, paramIndex);

    if(rc != SQLITE_OK) {
        throw wxSQLite3Exception(rc, wxERRMSG_BIND_NULL);
    }
}

void wxSQLite3Statement::ClearBindings()
{
    CheckStmt();
#if 0 // missing in SQLite DLL
  int rc = sqlite3_clear_bindings((sqlite3_stmt*) m_stmt);

  if (rc != SQLITE_OK)
  {
    throw wxSQLite3Exception(rc, wxERRMSG_BIND_CLEAR);
  }
#else
    for(int paramIndex = 1; paramIndex <= GetParamCount(); paramIndex++) {
        BindNull(paramIndex);
    }
#endif
}

void wxSQLite3Statement::Reset()
{
    if(m_stmt) {
        int rc = sqlite3_reset((sqlite3_stmt*)m_stmt);

        if(rc != SQLITE_OK) {
            const char* localError = sqlite3_errmsg((sqlite3*)m_db);
            throw wxSQLite3Exception(rc, UTF8toWxString(localError));
        }
    }
}

void wxSQLite3Statement::Finalize()
{
    if(m_stmt) {
        int rc = sqlite3_finalize((sqlite3_stmt*)m_stmt);
        m_stmt = 0;

        if(rc != SQLITE_OK) {
            const char* localError = sqlite3_errmsg((sqlite3*)m_db);
            throw wxSQLite3Exception(rc, UTF8toWxString(localError));
        }
    }
}

void wxSQLite3Statement::CheckDatabase()
{
    if(m_db == 0) {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_NODB);
    }
}

void wxSQLite3Statement::CheckStmt()
{
    if(m_stmt == 0) {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_NOSTMT);
    }
}

// ----------------------------------------------------------------------------
// wxSQLite3Database: class holding a SQLite3 database object
// ----------------------------------------------------------------------------

#if WXSQLITE3_HAVE_CODEC
bool wxSQLite3Database::ms_hasEncryptionSupport = true;
#else
bool wxSQLite3Database::ms_hasEncryptionSupport = false;
#endif

#if WXSQLITE3_HAVE_METADATA
bool wxSQLite3Database::ms_hasMetaDataSupport = true;
#else
bool wxSQLite3Database::ms_hasMetaDataSupport = false;
#endif

#if WXSQLITE3_HAVE_LOAD_EXTENSION
bool wxSQLite3Database::ms_hasLoadExtSupport = true;
#else
bool wxSQLite3Database::ms_hasLoadExtSupport = false;
#endif

bool wxSQLite3Database::HasEncryptionSupport() { return ms_hasEncryptionSupport; }

bool wxSQLite3Database::HasMetaDataSupport() { return ms_hasMetaDataSupport; }

bool wxSQLite3Database::HasLoadExtSupport() { return ms_hasLoadExtSupport; }

wxSQLite3Database::wxSQLite3Database()
{
    m_db = 0;
    m_busyTimeoutMs = 60000; // 60 seconds
    m_isEncrypted = false;
}

wxSQLite3Database::wxSQLite3Database(const wxSQLite3Database& db)
{
    m_db = db.m_db;
    m_busyTimeoutMs = 60000; // 60 seconds
    m_isEncrypted = false;
}

wxSQLite3Database::~wxSQLite3Database() { Close(); }

wxSQLite3Database& wxSQLite3Database::operator=(const wxSQLite3Database& db)
{
    m_db = db.m_db;
    m_busyTimeoutMs = 60000; // 60 seconds
    m_isEncrypted = db.m_isEncrypted;
    return *this;
}

void wxSQLite3Database::Open(const wxString& fileName, const wxString& key)
{
    wxCharBuffer strLocalKey = wxConvUTF8.cWC2MB(key.wc_str(*wxConvCurrent));
    const char* localKey = strLocalKey;
    wxMemoryBuffer binaryKey;
    if(key.Length() > 0) {
        binaryKey.AppendData((void*)localKey, strlen(localKey));
    }
    Open(fileName, binaryKey);
}

void wxSQLite3Database::Open(const wxString& fileName, const wxMemoryBuffer& key)
{
    wxCharBuffer strFileName = wxConvUTF8.cWC2MB(fileName.wc_str(*wxConvCurrent));
    const char* localFileName = strFileName;

    int rc = sqlite3_open((const char*)localFileName, (sqlite3**)&m_db);

    if(rc != SQLITE_OK) {
        Close();
        const char* localError = sqlite3_errmsg((sqlite3*)m_db);
        throw wxSQLite3Exception(rc, UTF8toWxString(localError));
    }
    rc = sqlite3_extended_result_codes((sqlite3*)m_db, 1);
    if(rc != SQLITE_OK) {
        Close();
        const char* localError = sqlite3_errmsg((sqlite3*)m_db);
        throw wxSQLite3Exception(rc, UTF8toWxString(localError));
    }

#if WXSQLITE3_HAVE_CODEC
    if(key.GetDataLen() > 0) {
        rc = sqlite3_key((sqlite3*)m_db, key.GetData(), key.GetDataLen());
        if(rc != SQLITE_OK) {
            Close();
            const char* localError = sqlite3_errmsg((sqlite3*)m_db);
            throw wxSQLite3Exception(rc, UTF8toWxString(localError));
        }
        m_isEncrypted = true;
    }
#endif

    SetBusyTimeout(m_busyTimeoutMs);
}

bool wxSQLite3Database::IsOpen() const { return (m_db != NULL); }

void wxSQLite3Database::Close()
{
    if(m_db) {
        sqlite3_close((sqlite3*)m_db);
        m_db = 0;
    }
}

void wxSQLite3Database::Begin(wxSQLite3TransactionType transactionType)
{
    wxString sql;
    switch(transactionType) {
    case WXSQLITE_TRANSACTION_DEFERRED:
        sql << _T("begin deferred transaction");
        break;
    case WXSQLITE_TRANSACTION_IMMEDIATE:
        sql << _T("begin immediate transaction");
        break;
    case WXSQLITE_TRANSACTION_EXCLUSIVE:
        sql << _T("begin immediate transaction");
        break;
    default:
        sql << _T("begin transaction");
        break;
    }
    ExecuteUpdate(sql);
}

void wxSQLite3Database::Commit() { ExecuteUpdate("commit transaction"); }

void wxSQLite3Database::Rollback() { ExecuteUpdate("rollback transaction"); }

bool wxSQLite3Database::GetAutoCommit()
{
    CheckDatabase();
    return sqlite3_get_autocommit((sqlite3*)m_db) != 0;
}

wxSQLite3Statement wxSQLite3Database::PrepareStatement(const wxString& sql)
{
    wxCharBuffer strSql = wxConvUTF8.cWC2MB(sql.wc_str(*wxConvCurrent));
    const char* localSql = strSql;
    return PrepareStatement(localSql);
}

wxSQLite3Statement wxSQLite3Database::PrepareStatement(const wxSQLite3StatementBuffer& sql)
{
    return PrepareStatement((const char*)sql);
}

wxSQLite3Statement wxSQLite3Database::PrepareStatement(const char* sql)
{
    CheckDatabase();
    sqlite3_stmt* stmt = (sqlite3_stmt*)Prepare(sql);
    return wxSQLite3Statement(m_db, stmt);
}

bool wxSQLite3Database::TableExists(const wxString& tableName)
{
    wxString sql;
    sql << _T("select count(*) from sqlite_master where type='table' and name='") << tableName << _T("'");
    int rc = ExecuteScalar(sql);
    return (rc > 0);
}

bool wxSQLite3Database::CheckSyntax(const wxString& sql)
{
    wxCharBuffer strSql = wxConvUTF8.cWC2MB(sql.wc_str(*wxConvCurrent));
    const char* localSql = strSql;
    return CheckSyntax(localSql);
}

bool wxSQLite3Database::CheckSyntax(const wxSQLite3StatementBuffer& sql) { return CheckSyntax((const char*)sql); }

bool wxSQLite3Database::CheckSyntax(const char* sql) { return sqlite3_complete(sql) != 0; }

int wxSQLite3Database::ExecuteUpdate(const wxString& sql)
{
    wxCharBuffer strSql = wxConvUTF8.cWC2MB(sql.wc_str(*wxConvCurrent));
    const char* localSql = strSql;
    return ExecuteUpdate(localSql);
}

int wxSQLite3Database::ExecuteUpdate(const wxSQLite3StatementBuffer& sql) { return ExecuteUpdate((const char*)sql); }

int wxSQLite3Database::ExecuteUpdate(const char* sql)
{
    CheckDatabase();

    char* localError = 0;

    int rc = sqlite3_exec((sqlite3*)m_db, sql, 0, 0, &localError);

    if(rc == SQLITE_OK) {
        return sqlite3_changes((sqlite3*)m_db);
    } else {
        throw wxSQLite3Exception(rc, UTF8toWxString(localError));
    }
}

wxSQLite3ResultSet wxSQLite3Database::ExecuteQuery(const wxString& sql)
{
    wxCharBuffer strSql = wxConvUTF8.cWC2MB(sql.wc_str(*wxConvCurrent));
    const char* localSql = strSql;
    return ExecuteQuery(localSql);
}

wxSQLite3ResultSet wxSQLite3Database::ExecuteQuery(const wxSQLite3StatementBuffer& sql)
{
    return ExecuteQuery((const char*)sql);
}

wxSQLite3ResultSet wxSQLite3Database::ExecuteQuery(const char* sql)
{
    CheckDatabase();

    sqlite3_stmt* stmt = (sqlite3_stmt*)Prepare(sql);

    int rc = sqlite3_step(stmt);

    if(rc == SQLITE_DONE) // no rows
    {
        return wxSQLite3ResultSet(m_db, stmt, true /* eof */);
    } else if(rc == SQLITE_ROW) // one or more rows
    {
        return wxSQLite3ResultSet(m_db, stmt, false /* eof */);
    } else {
        rc = sqlite3_finalize(stmt);
        const char* localError = sqlite3_errmsg((sqlite3*)m_db);
        throw wxSQLite3Exception(rc, UTF8toWxString(localError));
    }
}

int wxSQLite3Database::ExecuteScalar(const wxString& sql)
{
    wxCharBuffer strSql = wxConvUTF8.cWC2MB(sql.wc_str(*wxConvCurrent));
    const char* localSql = strSql;
    return ExecuteScalar(localSql);
}

int wxSQLite3Database::ExecuteScalar(const wxSQLite3StatementBuffer& sql) { return ExecuteScalar((const char*)sql); }

int wxSQLite3Database::ExecuteScalar(const char* sql)
{
    wxSQLite3ResultSet resultSet = ExecuteQuery(sql);

    if(resultSet.Eof() || resultSet.GetColumnCount() < 1) {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_INVALID_QUERY);
    }

    long value = 0;
    resultSet.GetAsString(0).ToLong(&value);
    return value;
}

wxSQLite3Table wxSQLite3Database::GetTable(const wxString& sql)
{
    wxCharBuffer strSql = wxConvUTF8.cWC2MB(sql.wc_str(*wxConvCurrent));
    const char* localSql = strSql;
    return GetTable(localSql);
}

wxSQLite3Table wxSQLite3Database::GetTable(const wxSQLite3StatementBuffer& sql) { return GetTable((const char*)sql); }

wxSQLite3Table wxSQLite3Database::GetTable(const char* sql)
{
    CheckDatabase();

    char* localError = 0;
    char** results = 0;
    int rc;
    int rows(0);
    int cols(0);

    rc = sqlite3_get_table((sqlite3*)m_db, sql, &results, &rows, &cols, &localError);

    if(rc == SQLITE_OK) {
        return wxSQLite3Table(results, rows, cols);
    } else {
        throw wxSQLite3Exception(rc, UTF8toWxString(localError));
    }
}

wxLongLong wxSQLite3Database::GetLastRowId() { return wxLongLong(sqlite3_last_insert_rowid((sqlite3*)m_db)); }

void wxSQLite3Database::Interrupt() { sqlite3_interrupt((sqlite3*)m_db); }

void wxSQLite3Database::SetBusyTimeout(int nMillisecs)
{
    m_busyTimeoutMs = nMillisecs;
    sqlite3_busy_timeout((sqlite3*)m_db, m_busyTimeoutMs);
}

wxString wxSQLite3Database::GetVersion() { return UTF8toWxString(sqlite3_libversion()); }

bool wxSQLite3Database::CreateFunction(const wxString& funcName, int argCount, wxSQLite3ScalarFunction& function)
{
    CheckDatabase();
    wxCharBuffer strFuncName = wxConvUTF8.cWC2MB(funcName.wc_str(*wxConvCurrent));
    const char* localFuncName = strFuncName;
    int rc = sqlite3_create_function(
        (sqlite3*)m_db, localFuncName, argCount, SQLITE_UTF8, &function,
        (void (*)(sqlite3_context*, int, sqlite3_value**))wxSQLite3FunctionContext::ExecScalarFunction, NULL, NULL);
    return rc == SQLITE_OK;
}

bool wxSQLite3Database::CreateFunction(const wxString& funcName, int argCount, wxSQLite3AggregateFunction& function)
{
    CheckDatabase();
    wxCharBuffer strFuncName = wxConvUTF8.cWC2MB(funcName.wc_str(*wxConvCurrent));
    const char* localFuncName = strFuncName;
    int rc = sqlite3_create_function(
        (sqlite3*)m_db, localFuncName, argCount, SQLITE_UTF8, &function, NULL,
        (void (*)(sqlite3_context*, int, sqlite3_value**))wxSQLite3FunctionContext::ExecAggregateStep,
        (void (*)(sqlite3_context*))wxSQLite3FunctionContext::ExecAggregateFinalize);
    return rc == SQLITE_OK;
}

bool wxSQLite3Database::SetAuthorizer(wxSQLite3Authorizer& authorizer)
{
    CheckDatabase();
    int rc = sqlite3_set_authorizer((sqlite3*)m_db, wxSQLite3FunctionContext::ExecAuthorizer, &authorizer);
    return rc == SQLITE_OK;
}

void wxSQLite3Database::SetCommitHook(wxSQLite3Hook* commitHook)
{
    CheckDatabase();
    if(commitHook) {
        sqlite3_commit_hook((sqlite3*)m_db, (int (*)(void*))wxSQLite3FunctionContext::ExecCommitHook, commitHook);
    } else {
        sqlite3_commit_hook((sqlite3*)m_db, (int (*)(void*))NULL, NULL);
    }
}

void wxSQLite3Database::SetRollbackHook(wxSQLite3Hook* rollbackHook)
{
    CheckDatabase();
    if(rollbackHook) {
        sqlite3_rollback_hook((sqlite3*)m_db, (void (*)(void*))wxSQLite3FunctionContext::ExecRollbackHook,
                              rollbackHook);
    } else {
        sqlite3_rollback_hook((sqlite3*)m_db, (void (*)(void*))NULL, NULL);
    }
}

void wxSQLite3Database::SetUpdateHook(wxSQLite3Hook* updateHook)
{
    CheckDatabase();
    if(updateHook) {
        sqlite3_update_hook(
            (sqlite3*)m_db,
            (void (*)(void*, int, const char*, const char*, wxsqlite_int64))wxSQLite3FunctionContext::ExecUpdateHook,
            updateHook);
    } else {
        sqlite3_update_hook((sqlite3*)m_db, (void (*)(void*, int, const char*, const char*, wxsqlite_int64))NULL, NULL);
    }
}

void wxSQLite3Database::SetCollation(const wxString& collationName, wxSQLite3Collation* collation)
{
    CheckDatabase();
    wxCharBuffer strCollationName = wxConvUTF8.cWC2MB(collationName.wc_str(*wxConvCurrent));
    const char* localCollationName = strCollationName;
    int rc;
    if(collation) {
        rc = sqlite3_create_collation(
            (sqlite3*)m_db, localCollationName, SQLITE_UTF8, collation,
            (int (*)(void*, int, const void*, int, const void*))wxSQLite3Database::ExecComparisonWithCollation);
    } else {
        rc = sqlite3_create_collation((sqlite3*)m_db, localCollationName, SQLITE_UTF8, NULL,
                                      (int (*)(void*, int, const void*, int, const void*))NULL);
    }
    wxUnusedVar(rc);
}

void wxSQLite3Database::SetCollationNeededCallback()
{
    CheckDatabase();
    int rc = sqlite3_collation_needed(
        (sqlite3*)m_db, this, (void (*)(void*, sqlite3*, int, const char*))wxSQLite3Database::ExecCollationNeeded);
    if(rc != SQLITE_OK) {
        const char* localError = sqlite3_errmsg((sqlite3*)m_db);
        throw wxSQLite3Exception(rc, UTF8toWxString(localError));
    }
}

void wxSQLite3Database::CheckDatabase()
{
    if(!m_db) {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_NODB);
    }
}

void* wxSQLite3Database::Prepare(const char* sql)
{
    CheckDatabase();

    const char* tail = 0;
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2((sqlite3*)m_db, sql, -1, &stmt, &tail);

    if(rc != SQLITE_OK) {
        const char* localError = sqlite3_errmsg((sqlite3*)m_db);
        throw wxSQLite3Exception(rc, UTF8toWxString(localError));
    }

    return stmt;
}

/* static */
int wxSQLite3Database::ExecComparisonWithCollation(void* collation, int len1, const void* text1, int len2,
                                                   const void* text2)
{
#if wxUSE_UNICODE
    wxString locText1((const char*)text1, wxConvUTF8, len1);
    wxString locText2((const char*)text2, wxConvUTF8, len2);
#else
    size_t len = (len1 > len2) ? len1 : len2;
    char* buffer = new char[len + 1];
    memcpy(buffer, text1, len1);
    buffer[len1] = '\0';
    wxString locText1(wxConvUTF8.cMB2WC((const char*)buffer), *wxConvCurrent);
    memcpy(buffer, text2, len2);
    buffer[len2] = '\0';
    wxString locText2(wxConvUTF8.cMB2WC((const char*)buffer), *wxConvCurrent);
    delete[] buffer;
#endif
    return ((wxSQLite3Collation*)collation)->Compare(locText1, locText2);
}

void wxSQLite3Database::ExecCollationNeeded(void* db, void*, int, const char* collationName)
{
#if wxUSE_UNICODE
    wxString locCollation((const char*)collationName, wxConvUTF8);
#else
    wxString locCollation(wxConvUTF8.cMB2WC((const char*)collationName), *wxConvCurrent);
#endif
    ((wxSQLite3Database*)db)->SetNeededCollation(locCollation);
}

void wxSQLite3Database::GetMetaData(const wxString& databaseName, const wxString& tableName, const wxString& columnName,
                                    wxString* dataType, wxString* collation, bool* notNull, bool* primaryKey,
                                    bool* autoIncrement)
{
#if WXSQLITE3_HAVE_METADATA
    wxCharBuffer strDatabaseName = wxConvUTF8.cWC2MB(databaseName.wc_str(*wxConvCurrent));
    const char* localDatabaseName = strDatabaseName;
    if(databaseName == wxEmptyString)
        localDatabaseName = NULL;
    wxCharBuffer strTableName = wxConvUTF8.cWC2MB(tableName.wc_str(*wxConvCurrent));
    const char* localTableName = strTableName;
    wxCharBuffer strColumnName = wxConvUTF8.cWC2MB(columnName.wc_str(*wxConvCurrent));
    const char* localColumnName = strColumnName;
    const char* localDataType;
    const char* localCollation;
    int localNotNull;
    int localPrimaryKey;
    int localAutoIncrement;
    int rc = sqlite3_table_column_metadata((sqlite3*)m_db, localDatabaseName, localTableName, localColumnName,
                                           &localDataType, &localCollation, &localNotNull, &localPrimaryKey,
                                           &localAutoIncrement);

    if(rc != SQLITE_OK) {
        const char* localError = sqlite3_errmsg((sqlite3*)m_db);
        throw wxSQLite3Exception(rc, UTF8toWxString(localError));
    }

    if(dataType != NULL)
        *dataType = UTF8toWxString(localDataType);
    if(collation != NULL)
        *collation = UTF8toWxString(localCollation);

    if(notNull != NULL)
        *notNull = (localNotNull != 0);
    if(primaryKey != NULL)
        *primaryKey = (localPrimaryKey != 0);
    if(autoIncrement != NULL)
        *autoIncrement = (localAutoIncrement != 0);
#else
    throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_NOMETADATA);
#endif
}

void wxSQLite3Database::LoadExtension(const wxString& fileName, const wxString& entryPoint)
{
#if WXSQLITE3_HAVE_LOAD_EXTENSION
    wxCharBuffer strFileName = wxConvUTF8.cWC2MB(fileName.wc_str(*wxConvCurrent));
    const char* localFileName = strFileName;
    wxCharBuffer strEntryPoint = wxConvUTF8.cWC2MB(entryPoint.wc_str(*wxConvCurrent));
    const char* localEntryPoint = strEntryPoint;

    int rc = sqlite3_load_extension((sqlite3*)m_db, localFileName, localEntryPoint, NULL);
    if(rc != SQLITE_OK) {
        const char* localError = sqlite3_errmsg((sqlite3*)m_db);
        throw wxSQLite3Exception(rc, UTF8toWxString(localError));
    }
#else
    throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_NOLOADEXT);
#endif
}

void wxSQLite3Database::EnableLoadExtension(bool enable)
{
#if WXSQLITE3_HAVE_LOAD_EXTENSION
    int onoff = (enable) ? 1 : 0;
    int rc = sqlite3_enable_load_extension((sqlite3*)m_db, onoff);
    if(rc != SQLITE_OK) {
        const char* localError = sqlite3_errmsg((sqlite3*)m_db);
        throw wxSQLite3Exception(rc, UTF8toWxString(localError));
    }
#else
    throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_NOLOADEXT);
#endif
}

void wxSQLite3Database::ReKey(const wxString& newKey)
{
#if WXSQLITE3_HAVE_CODEC
    wxCharBuffer strLocalNewKey = wxConvUTF8.cWC2MB(newKey.wc_str(*wxConvCurrent));
    const char* localNewKey = strLocalNewKey;
    wxMemoryBuffer binaryNewKey;
    if(newKey.Length() > 0) {
        binaryNewKey.AppendData((void*)localNewKey, strlen(localNewKey));
    }
    ReKey(binaryNewKey);
#else
    throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_NOCODEC);
#endif
}

void wxSQLite3Database::ReKey(const wxMemoryBuffer& newKey)
{
#if WXSQLITE3_HAVE_CODEC
    int rc = sqlite3_rekey((sqlite3*)m_db, newKey.GetData(), newKey.GetDataLen());
    if(rc != SQLITE_OK) {
        const char* localError = sqlite3_errmsg((sqlite3*)m_db);
        throw wxSQLite3Exception(rc, UTF8toWxString(localError));
    }
#else
    throw wxSQLite3Exception(WXSQLITE_ERROR, wxERRMSG_NOCODEC);
#endif
}

// ----------------------------------------------------------------------------
// wxSQLite3FunctionContext: class providing the function context
//                           for user defined functions
// ----------------------------------------------------------------------------

int wxSQLite3FunctionContext::GetArgCount() { return m_argc; }

int wxSQLite3FunctionContext::GetArgType(int argIndex)
{
    if(argIndex >= 0 && argIndex < m_argc) {
        return sqlite3_value_type((sqlite3_value*)m_argv[argIndex]);
    } else {
        return SQLITE_NULL;
    }
}

bool wxSQLite3FunctionContext::IsNull(int argIndex)
{
    if(argIndex >= 0 && argIndex < m_argc) {
        return sqlite3_value_type((sqlite3_value*)m_argv[argIndex]) == SQLITE_NULL;
    } else {
        return true;
    }
}

int wxSQLite3FunctionContext::GetInt(int argIndex, int nullValue)
{
    if(argIndex >= 0 && argIndex < m_argc) {
        if(!IsNull(argIndex)) {
            return sqlite3_value_int((sqlite3_value*)m_argv[argIndex]);
        } else {
            return nullValue;
        }
    } else {
        return nullValue;
    }
}

wxLongLong wxSQLite3FunctionContext::GetInt64(int argIndex, wxLongLong nullValue)
{
    if(argIndex >= 0 && argIndex < m_argc) {
        if(!IsNull(argIndex)) {
            return wxLongLong(sqlite3_value_int64((sqlite3_value*)m_argv[argIndex]));
        } else {
            return nullValue;
        }
    } else {
        return nullValue;
    }
}

double wxSQLite3FunctionContext::GetDouble(int argIndex, double nullValue)
{
    if(argIndex >= 0 && argIndex < m_argc) {
        if(!IsNull(argIndex)) {
            return sqlite3_value_double((sqlite3_value*)m_argv[argIndex]);
        } else {
            return nullValue;
        }
    } else {
        return nullValue;
    }
}

wxString wxSQLite3FunctionContext::GetString(int argIndex, const wxString& nullValue)
{
    if(argIndex >= 0 && argIndex < m_argc) {
        if(!IsNull(argIndex)) {
            const char* localValue = (const char*)sqlite3_value_text((sqlite3_value*)m_argv[argIndex]);
            return UTF8toWxString(localValue);
        } else {
            return nullValue;
        }
    } else {
        return nullValue;
    }
}

wxMemoryBuffer& wxSQLite3FunctionContext::GetBlob(int argIndex, wxMemoryBuffer& buffer)
{
    if(argIndex >= 0 && argIndex < m_argc) {
        if(!IsNull(argIndex)) {
            int len = sqlite3_value_bytes((sqlite3_value*)m_argv[argIndex]);
            const void* blob = sqlite3_value_blob((sqlite3_value*)m_argv[argIndex]);
            buffer.AppendData((void*)blob, len);
        }
    }
    return buffer;
}

void wxSQLite3FunctionContext::SetResult(int value) { sqlite3_result_int((sqlite3_context*)m_ctx, value); }

void wxSQLite3FunctionContext::SetResult(wxLongLong value)
{
    sqlite3_result_int64((sqlite3_context*)m_ctx, value.GetValue());
}

void wxSQLite3FunctionContext::SetResult(double value) { sqlite3_result_double((sqlite3_context*)m_ctx, value); }

void wxSQLite3FunctionContext::SetResult(const wxString& value)
{
    wxCharBuffer strValue = wxConvUTF8.cWC2MB(value.wc_str(*wxConvCurrent));
    const char* localValue = strValue;
    sqlite3_result_text((sqlite3_context*)m_ctx, localValue, -1, SQLITE_TRANSIENT);
}

void wxSQLite3FunctionContext::SetResult(unsigned char* value, int len)
{
    sqlite3_result_blob((sqlite3_context*)m_ctx, value, len, SQLITE_TRANSIENT);
}

void wxSQLite3FunctionContext::SetResult(const wxMemoryBuffer& buffer)
{
    sqlite3_result_blob((sqlite3_context*)m_ctx, buffer.GetData(), buffer.GetDataLen(), SQLITE_TRANSIENT);
}

void wxSQLite3FunctionContext::SetResultNull() { sqlite3_result_null((sqlite3_context*)m_ctx); }

void wxSQLite3FunctionContext::SetResultArg(int argIndex)
{
    if(argIndex >= 0 && argIndex < m_argc) {
        sqlite3_result_value((sqlite3_context*)m_ctx, (sqlite3_value*)m_argv[argIndex]);
    } else {
        sqlite3_result_null((sqlite3_context*)m_ctx);
    }
}

void wxSQLite3FunctionContext::SetResultError(const wxString& errmsg)
{
    wxCharBuffer strErrmsg = wxConvUTF8.cWC2MB(errmsg.wc_str(*wxConvCurrent));
    const char* localErrmsg = strErrmsg;
    sqlite3_result_error((sqlite3_context*)m_ctx, localErrmsg, -1);
}

int wxSQLite3FunctionContext::GetAggregateCount()
{
    if(m_isAggregate) {
        return sqlite3_aggregate_count((sqlite3_context*)m_ctx);
    } else {
        return 0;
    }
}

void* wxSQLite3FunctionContext::GetAggregateStruct(int len)
{
    if(m_isAggregate) {
        return sqlite3_aggregate_context((sqlite3_context*)m_ctx, len);
    } else {
        return NULL;
    }
}

/* static */
void wxSQLite3FunctionContext::ExecScalarFunction(void* ctx, int argc, void** argv)
{
    wxSQLite3FunctionContext context(ctx, false, argc, argv);
    wxSQLite3ScalarFunction* func = (wxSQLite3ScalarFunction*)sqlite3_user_data((sqlite3_context*)ctx);
    func->Execute(context);
}

/* static */
void wxSQLite3FunctionContext::ExecAggregateStep(void* ctx, int argc, void** argv)
{
    wxSQLite3FunctionContext context(ctx, true, argc, argv);
    wxSQLite3AggregateFunction* func = (wxSQLite3AggregateFunction*)sqlite3_user_data((sqlite3_context*)ctx);
    func->Aggregate(context);
}

/* static */
void wxSQLite3FunctionContext::ExecAggregateFinalize(void* ctx)
{
    wxSQLite3FunctionContext context(ctx, true, 0, NULL);
    wxSQLite3AggregateFunction* func = (wxSQLite3AggregateFunction*)sqlite3_user_data((sqlite3_context*)ctx);
    func->Finalize(context);
}

/* static */
int wxSQLite3FunctionContext::ExecAuthorizer(void* func, int type, const char* arg1, const char* arg2, const char* arg3,
                                             const char* arg4)
{
#if wxUSE_UNICODE
    wxString locArg1(arg1, wxConvUTF8);
    wxString locArg2(arg2, wxConvUTF8);
    wxString locArg3(arg3, wxConvUTF8);
    wxString locArg4(arg4, wxConvUTF8);
#else
    wxString locArg1(wxConvUTF8.cMB2WC(arg1), *wxConvCurrent);
    wxString locArg2(wxConvUTF8.cMB2WC(arg2), *wxConvCurrent);
    wxString locArg3(wxConvUTF8.cMB2WC(arg3), *wxConvCurrent);
    wxString locArg4(wxConvUTF8.cMB2WC(arg4), *wxConvCurrent);
#endif
    wxSQLite3Authorizer::wxAuthorizationCode localType = (wxSQLite3Authorizer::wxAuthorizationCode)type;
    return (int)((wxSQLite3Authorizer*)func)->Authorize(localType, locArg1, locArg2, locArg3, locArg3);
}

/* static */
int wxSQLite3FunctionContext::ExecCommitHook(void* hook) { return (int)((wxSQLite3Hook*)hook)->CommitCallback(); }

/* static */
void wxSQLite3FunctionContext::ExecRollbackHook(void* hook) { ((wxSQLite3Hook*)hook)->RollbackCallback(); }

/* static */
void wxSQLite3FunctionContext::ExecUpdateHook(void* hook, int type, const char* database, const char* table,
                                              wxsqlite_int64 rowid)
{
#if wxUSE_UNICODE
    wxString locDatabase(database, wxConvUTF8);
    wxString locTable(table, wxConvUTF8);
#else
    wxString locDatabase(wxConvUTF8.cMB2WC(database), *wxConvCurrent);
    wxString locTable(wxConvUTF8.cMB2WC(table), *wxConvCurrent);
#endif
    wxSQLite3Hook::wxUpdateType locType = (wxSQLite3Hook::wxUpdateType)type;
    wxLongLong locRowid = rowid;
    ((wxSQLite3Hook*)hook)->UpdateCallback(locType, locDatabase, locTable, locRowid);
}

wxSQLite3FunctionContext::wxSQLite3FunctionContext(void* ctx, bool isAggregate, int argc, void** argv)
    : m_ctx(ctx)
    , m_isAggregate(isAggregate)
    , m_argc(argc)
    , m_argv(argv)
{
}

static const wxChar* authCodeString[] = { _T("SQLITE_COPY"),
                                          _T("SQLITE_CREATE_INDEX"),
                                          _T("SQLITE_CREATE_TABLE"),
                                          _T("SQLITE_CREATE_TEMP_INDEX"),
                                          _T("SQLITE_CREATE_TEMP_TABLE"),
                                          _T("SQLITE_CREATE_TEMP_TRIGGER"),
                                          _T("SQLITE_CREATE_TEMP_VIEW"),
                                          _T("SQLITE_CREATE_TRIGGER"),
                                          _T("SQLITE_CREATE_VIEW"),
                                          _T("SQLITE_DELETE"),
                                          _T("SQLITE_DROP_INDEX"),
                                          _T("SQLITE_DROP_TABLE"),
                                          _T("SQLITE_DROP_TEMP_INDEX"),
                                          _T("SQLITE_DROP_TEMP_TABLE"),
                                          _T("SQLITE_DROP_TEMP_TRIGGER"),
                                          _T("SQLITE_DROP_TEMP_VIEW"),
                                          _T("SQLITE_DROP_TRIGGER"),
                                          _T("SQLITE_DROP_VIEW"),
                                          _T("SQLITE_INSERT"),
                                          _T("SQLITE_PRAGMA"),
                                          _T("SQLITE_READ"),
                                          _T("SQLITE_SELECT"),
                                          _T("SQLITE_TRANSACTION"),
                                          _T("SQLITE_UPDATE"),
                                          _T("SQLITE_ATTACH"),
                                          _T("SQLITE_DETACH"),
                                          _T("SQLITE_ALTER_TABLE"),
                                          _T("SQLITE_REINDEX"),
                                          _T("SQLITE_ANALYZE"),
                                          _T("SQLITE_CREATE_VTABLE"),
                                          _T("SQLITE_DROP_VTABLE"),
                                          _T("SQLITE_FUNCTION") };

/* static */
wxString wxSQLite3Authorizer::AuthorizationCodeToString(wxSQLite3Authorizer::wxAuthorizationCode type)
{
    const wxChar* authString = _T("Unknown");
    if(type >= SQLITE_COPY && type <= SQLITE_MAX_CODE) {
        authString = authCodeString[type];
    }
    return wxString(authString);
}
