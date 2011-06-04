#ifndef __ORACLE_DATABASE_LAYER_H__
#define __ORACLE_DATABASE_LAYER_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/arrstr.h"

#include "DatabaseLayer.h"

#include "occi.h"

class OracleDatabaseLayer : public DatabaseLayer
{
public:
  // ctor()
  OracleDatabaseLayer();
  OracleDatabaseLayer(const wxString& strUser, const wxString& strPassword, bool bOpenDatabase = false);
  OracleDatabaseLayer(const wxString& strDatabase, const wxString& strUser, const wxString& strPassword);
  OracleDatabaseLayer(const wxString& strServer, const wxString& strDatabase, const wxString& strUser, const wxString& strPassword);
  OracleDatabaseLayer(oracle::occi::Environment* pEnvironment, oracle::occi::Connection* pDatabase);

  // dtor()
  virtual ~OracleDatabaseLayer();

  // open database
  virtual bool Open();
  virtual bool Open(const wxString& strDatabase);
  virtual bool Open(const wxString& strServer, const wxString& strDatabase);
  virtual bool Open(const wxString& strDatabase, const wxString& strUser, const wxString& strPassword);
  virtual bool Open(const wxString& strServer, const wxString& strDatabase, const wxString& strUser, const wxString& strPassword);

  // close database
  virtual bool Close();
  
  // Is the connection to the database open?
  virtual bool IsOpen();

  // transaction support
  virtual void BeginTransaction();
  virtual void Commit();
  virtual void RollBack();
  
  // query database
  virtual bool RunQuery(const wxString& strQuery, bool bParseQuery);
  virtual DatabaseResultSet* RunQueryWithResults(const wxString& strQuery);
  
  // PreparedStatement support
  virtual PreparedStatement* PrepareStatement(const wxString& strQuery);
  
  // Database schema API contributed by M. Szeftel (author of wxActiveRecordGenerator)
  virtual bool TableExists(const wxString& table);
  virtual bool ViewExists(const wxString& view);
  virtual wxArrayString GetTables();
  virtual wxArrayString GetViews();
  virtual wxArrayString GetColumns(const wxString& table);

  static int TranslateErrorCode(int nCode);

private:
  oracle::occi::Environment* m_pEnvironment;
  oracle::occi::Connection* m_pDatabase;

  bool m_bAutoCommit;
  wxString m_strServer;
  wxString m_strDatabase;
  wxString m_strUser;
  wxString m_strPassword;
  wxString m_strPort;
};

#endif // __ORACLE_DATABASE_LAYER_H__
