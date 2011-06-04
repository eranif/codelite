#include "../include/PostgresInterface.h"

bool PostgresInterface::Init()
{
#ifdef __WIN32__
  bool bLoaded = m_PostgresDLL.Load(wxT("libpq.dll"), wxDL_VERBATIM);
#else
  bool bLoaded = m_PostgresDLL.Load(wxDynamicLibrary::CanonicalizeName(wxT("pq")));
#endif
  if (!bLoaded)
  {
    return false;
  }

  wxString symbol = wxT("PQstatus");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQstatus = (PQstatusType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQsetdbLogin");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQsetdbLogin = (PQsetdbLoginType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQerrorMessage");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQerrorMessage = (PQerrorMessageType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQclientEncoding");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQclientEncoding = (PQclientEncodingType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQsetClientEncoding");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQsetClientEncoding = (PQsetClientEncodingType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("pg_encoding_to_char");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQencodingToChar = (PQencodingToCharType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQfinish");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQfinish = (PQfinishType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQexec");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQexec = (PQexecType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQresultStatus");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQresultStatus = (PQresultStatusType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQclear");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQclear = (PQclearType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQcmdTuples");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQcmdTuples = (PQcmdTuplesType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQprepare");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQprepare = (PQprepareType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQresultStatus");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQresultStatus = (PQresultStatusType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQexecPrepared");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQexecPrepared = (PQexecPreparedType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQresultErrorMessage");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQresultErrorMessage = (PQresultErrorMessageType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQntuples");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQntuples = (PQntuplesType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQbinaryTuples");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQbinaryTuples = (PQbinaryTuplesType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQfname");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQfname = (PQfnameType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQgetisnull");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQgetisnull = (PQgetisnullType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQgetvalue");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQgetvalue = (PQgetvalueType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQgetlength");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQgetlength = (PQgetlengthType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQunescapeBytea");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQunescapeBytea = (PQunescapeByteaType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQnfields");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQnfields = (PQnfieldsType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQfreemem");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQfreemem = (PQfreememType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("PQftype");
  if (m_PostgresDLL.HasSymbol(symbol))
  {
    m_pPQftype = (PQftypeType)m_PostgresDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  return true;
}
