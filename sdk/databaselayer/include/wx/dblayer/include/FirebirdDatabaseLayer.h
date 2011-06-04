#ifndef __FIREBIRD_DATABASE_LAYER_H__
#define __FIREBIRD_DATABASE_LAYER_H__

#include "DatabaseLayerDef.h"
#include "DatabaseLayer.h"

#ifndef DONT_USE_DYNAMIC_DATABASE_LAYER_LINKING
class FirebirdInterface;
#endif

class WXDLLIMPEXP_DATABASELAYER FirebirdDatabaseLayer : public DatabaseLayer
{
public:
  // ctor()
  FirebirdDatabaseLayer();
  FirebirdDatabaseLayer(const wxString& strDatabase);
  FirebirdDatabaseLayer(const wxString& strDatabase, const wxString& strUser, const wxString& strPassword);
  FirebirdDatabaseLayer(const wxString& strServer , const wxString& strDatabase, const wxString& strUser, const wxString& strPassword);
  FirebirdDatabaseLayer(const wxString& strServer , const wxString& strDatabase, const wxString& strUser, const wxString& strPassword, const wxString& strRole);
  //FirebirdDatabaseLayer(isc_db_handle pDatabase) { m_pDatabase = pDatabase; }
  FirebirdDatabaseLayer(void* pDatabase) { m_pDatabase = pDatabase; }
  
  // dtor()
  virtual ~FirebirdDatabaseLayer();
  
  // open database
  virtual bool Open(const wxString& strDatabase);
  virtual bool Open();
  virtual bool Open(const wxString& strDatabase, const wxString& strUser, const wxString& strPassword);
  virtual bool Open(const wxString& strServer, const wxString& strDatabase, const wxString& strUser = _(""), const wxString& strPassword = _(""));
  
  // close database  
  virtual bool Close();
  
  // Is the connection to the database open?
  virtual bool IsOpen();

  // transaction support
  virtual void BeginTransaction();
  virtual void Commit();
  virtual void RollBack();
  
  // query database
  virtual int RunQuery(const wxString& strQuery, bool bParseQuery);
  virtual DatabaseResultSet* RunQueryWithResults(const wxString& strQuery);
  
  // PreparedStatement support
  virtual PreparedStatement* PrepareStatement(const wxString& strQuery);
  
  static int TranslateErrorCode(int nCode);
  //static wxString TranslateErrorCodeToString(FirebirdInterface* pInterface, int nCode, ISC_STATUS_ARRAY status);
  static wxString TranslateErrorCodeToString(FirebirdInterface* pInterface, int nCode, void* status);
  static bool IsAvailable();

  void SetServer(const wxString& strServer) { m_strServer = strServer; }
  void SetDatabase(const wxString& strDatabase) { m_strDatabase = strDatabase; }
  void SetUser(const wxString& strUser) { m_strUser = strUser; }
  void SetPassword(const wxString& strPassword) { m_strPassword = strPassword; }
  void SetRole(const wxString& strRole) { m_strRole = strRole; }

  // Database schema API contributed by M. Szeftel (author of wxActiveRecordGenerator)
  virtual bool TableExists(const wxString& table);
  virtual bool ViewExists(const wxString& view);
  virtual wxArrayString GetTables();
  virtual wxArrayString GetViews();
  virtual wxArrayString GetColumns(const wxString& table);

private:
  void Connect();
  void InterpretErrorCodes(); 
  
#ifndef DONT_USE_DYNAMIC_DATABASE_LAYER_LINKING
  FirebirdInterface* m_pInterface;
#endif
  wxString m_strServer;
  wxString m_strDatabase;
  wxString m_strUser;
  wxString m_strPassword;
  wxString m_strRole;

  //isc_db_handle m_pDatabase;
  void* m_pDatabase;
  //isc_tr_handle m_pTransaction;
  void* m_pTransaction;
  //ISC_STATUS_ARRAY m_Status;
  void* m_pStatus;
};

#endif // __FIREBIRD_DATABASE_LAYER_H__

