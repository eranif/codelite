#ifndef __TDS_DATABASE_LAYER_H__
#define __TDS_DATABASE_LAYER_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/arrstr.h"
#include "wx/hashmap.h"

#include "DatabaseLayer.h"

#include "tds.h"

class PreparedStatement;
class TdsDatabaseLayer;

WX_DECLARE_HASH_MAP(TDSCONTEXT*, TdsDatabaseLayer*, wxPointerHash, wxPointerEqual, TdsContextToDatabaseLayerMap);

class TdsDatabaseLayer : public DatabaseLayer
{
public:
  // ctor()
  TdsDatabaseLayer();
  TdsDatabaseLayer(const wxString& strServer, const wxString& strDatabase, const wxString& strUser, const wxString& strPassword, int nTdsVersion = TDS_80);
  
  // dtor()
  virtual ~TdsDatabaseLayer();
  
  // open database
  virtual bool Open(const wxString& strDatabase);
  
  bool Connect();

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
  PreparedStatement* PrepareStatement(const wxString& strQuery, bool bLogForCleanup);
  
  void SetServer(const wxString& strServer) { m_strServer = strServer; }
  void SetDatabase(const wxString& strDatabase) { m_strDatabase = strDatabase; }
  void SetLogin(const wxString& strLogin) { m_strLogin = strLogin; }
  void SetPassword(const wxString& strPassword) { m_strPassword = strPassword; }
  void SetTdsVersion(int nTdsVersion) { m_nTdsVersion = nTdsVersion; }

  enum
  {
    TDS_42 = 0,
    TDS_46,
    TDS_50,
    TDS_70,
    TDS_80
  };

  // Database schema API contributed by M. Szeftel (author of wxActiveRecordGenerator)
  virtual bool TableExists(const wxString& table);
  virtual bool ViewExists(const wxString& view);
  virtual wxArrayString GetTables();
  virtual wxArrayString GetViews();
  virtual wxArrayString GetColumns(const wxString& table);

  static TdsDatabaseLayer* LookupTdsLayer(const TDSCONTEXT* pContext);
  void SetError(int nCode, const wxString& strMessage);

private:
  static int TranslateErrorCode(int nCode);
  void FreeAllocatedResultSets();
  static void AddTdsLayer(TDSCONTEXT* pContext, TdsDatabaseLayer* pLayer);
  static void RemoveTdsLayer(TDSCONTEXT* pContext);

  // Map of TDSCONTEXT* to TdsDatabaseLayer* to be used by the error handler
  static TdsContextToDatabaseLayerMap m_ContextLookupMap;

  wxString m_strServer;
  wxString m_strDatabase;
  wxString m_strLogin;
  wxString m_strPassword;
  int m_nTdsVersion;

  TDSSOCKET* m_pDatabase;
  TDSLOGIN* m_pLogin;
  TDSCONTEXT* m_pContext;
};

#endif // __TDS_DATABASE_LAYER_H__

