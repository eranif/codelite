#ifndef __POSTGRESQL_INTERFACES_H__
#define __POSTGRESQL_INTERFACES_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/dynlib.h"

#include "libpq-fe.h"

typedef ConnStatusType (*PQstatusType)(const PGconn*);
typedef PGconn* (*PQsetdbLoginType)(const char*, const char*,
  const char*, const char*, const char*, const char*, const char*);
typedef char* (*PQerrorMessageType)(const PGconn*);
typedef int (*PQclientEncodingType)(const PGconn*);
typedef int (*PQsetClientEncodingType)(const PGconn*, const char*);
typedef const char* (*PQencodingToCharType)(int);
typedef void (*PQfinishType)(const PGconn*);
typedef PGresult* (*PQexecType)(const PGconn*, const char*);
typedef ExecStatusType (*PQresultStatusType)(const PGresult*);
typedef void (*PQclearType)(PGresult*);
typedef char* (*PQcmdTuplesType)(const PGresult*);
typedef PGresult* (*PQprepareType)(PGconn*, const char*, const char*, int, const Oid*);
typedef PGresult* (*PQexecPreparedType)(PGconn*, const char*, int, const char *const*,
  const int*, const int*, int resultFormat);
typedef char* (*PQresultErrorMessageType)(const PGresult*);
typedef int (*PQntuplesType)(const PGresult*);
typedef int (*PQbinaryTuplesType)(const PGresult*);
typedef char* (*PQfnameType)(const PGresult*, int);
typedef int (*PQgetisnullType)(const PGresult*, int, int);
typedef char* (*PQgetvalueType)(const PGresult*, int, int);
typedef int (*PQgetlengthType)(const PGconn*);
typedef unsigned char* (*PQunescapeByteaType)(const unsigned char*, size_t*);
typedef void (*PQfreememType)(void*);
typedef Oid (*PQftypeType)(const PGresult*, int);
typedef int (*PQfsizeType)(const PGresult*, int);
typedef int (*PQnfieldsType)(const PGresult*);


class PostgresInterface
{
public:
  PostgresInterface() { }
  bool Init();

  PQstatusType GetPQstatus() { return m_pPQstatus; }
  PQsetdbLoginType GetPQsetdbLogin() { return m_pPQsetdbLogin; }
  PQerrorMessageType GetPQerrorMessage() { return m_pPQerrorMessage; }
  PQclientEncodingType GetPQclientEncoding() { return m_pPQclientEncoding; }
  PQsetClientEncodingType GetPQsetClientEncoding() { return m_pPQsetClientEncoding; }
  PQencodingToCharType GetPQencodingToChar() { return m_pPQencodingToChar; }
  PQfinishType GetPQfinish() { return m_pPQfinish; }
  PQexecType GetPQexec() { return m_pPQexec; }
  PQresultStatusType GetPQresultStatus() { return m_pPQresultStatus; }
  PQclearType GetPQclear() { return m_pPQclear; }
  PQcmdTuplesType GetPQcmdTuples() { return m_pPQcmdTuples; }
  PQprepareType GetPQprepare() { return m_pPQprepare; }
  PQexecPreparedType GetPQexecPrepared() { return m_pPQexecPrepared; }
  PQresultErrorMessageType GetPQresultErrorMessage() { return m_pPQresultErrorMessage; }
  PQntuplesType GetPQntuples() { return m_pPQntuples; }
  PQbinaryTuplesType GetPQbinaryTuples() { return m_pPQbinaryTuples; }
  PQfnameType GetPQfname() { return m_pPQfname; }
  PQgetisnullType GetPQgetisnull() { return m_pPQgetisnull; }
  PQgetvalueType GetPQgetvalue() { return m_pPQgetvalue; }
  PQgetlengthType GetPQgetlength() { return m_pPQgetlength; }
  PQunescapeByteaType GetPQunescapeBytea() { return m_pPQunescapeBytea; }
  PQfreememType GetPQfreemem() { return m_pPQfreemem; }
  PQftypeType GetPQftype() { return m_pPQftype; }
  PQfsizeType GetPQfsize() { return m_pPQfsize; }
  PQnfieldsType GetPQnfields() { return m_pPQnfields; }


private:
  wxDynamicLibrary m_PostgresDLL;

  PQstatusType m_pPQstatus;
  PQsetdbLoginType m_pPQsetdbLogin;
  PQerrorMessageType m_pPQerrorMessage;
  PQclientEncodingType m_pPQclientEncoding;
  PQsetClientEncodingType m_pPQsetClientEncoding;
  PQencodingToCharType m_pPQencodingToChar;
  PQfinishType m_pPQfinish;
  PQexecType m_pPQexec;
  PQresultStatusType m_pPQresultStatus;
  PQclearType m_pPQclear;
  PQcmdTuplesType m_pPQcmdTuples;
  PQprepareType m_pPQprepare;
  PQexecPreparedType m_pPQexecPrepared;
  PQresultErrorMessageType m_pPQresultErrorMessage;
  PQntuplesType m_pPQntuples;
  PQbinaryTuplesType m_pPQbinaryTuples;
  PQfnameType m_pPQfname;
  PQgetisnullType m_pPQgetisnull;
  PQgetvalueType m_pPQgetvalue;
  PQgetlengthType m_pPQgetlength;
  PQunescapeByteaType m_pPQunescapeBytea;
  PQfreememType m_pPQfreemem;
  PQftypeType m_pPQftype;
  PQfsizeType m_pPQfsize;
  PQnfieldsType m_pPQnfields;
};

#endif // __POSTGRESQL_INTERFACES_H__

