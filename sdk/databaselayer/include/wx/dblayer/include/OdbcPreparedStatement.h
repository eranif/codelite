#ifndef __ODBC_PREPARED_STATEMENT_H__
#define __ODBC_PREPARED_STATEMENT_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/dynarray.h"

#include "PreparedStatement.h"
#include "OdbcParameter.h"
#include "OdbcInterface.h"

#include <sql.h>

WX_DEFINE_ARRAY_PTR(SQLHSTMT, StatementVector);
WX_DEFINE_ARRAY_PTR(OdbcParameter*, ArrayOfOdbcParameters);

class DatabaseResultSet;

class OdbcPreparedStatement : public PreparedStatement
{
public:
    // ctor
    OdbcPreparedStatement(OdbcInterface* pInterface, SQLHENV sqlEnvHandle, SQLHDBC sqlHDBC);
    OdbcPreparedStatement(OdbcInterface* pInterface, SQLHENV sqlEnvHandle, SQLHDBC sqlHDBC, SQLHSTMT sqlStatementHandle);
    OdbcPreparedStatement(OdbcInterface* pInterface, SQLHENV sqlEnvHandle, SQLHDBC sqlHDBC, StatementVector statements);

    // dtor
    virtual ~OdbcPreparedStatement();

    virtual void Close();
      
    void AddPreparedStatement(SQLHSTMT pStatement);

    // get field
    virtual void SetParamInt(int nPosition, int nValue);
    virtual void SetParamDouble(int nPosition, double dblValue);
    virtual void SetParamString(int nPosition, const wxString& strValue);
    virtual void SetParamNull(int nPosition);
    virtual void SetParamBlob(int nPosition, const void* pData, long nDataLength);
    virtual void SetParamDate(int nPosition, const wxDateTime& dateValue);
    virtual void SetParamBool(int nPosition, bool bValue);
    virtual int GetParameterCount();

    virtual int RunQuery();
    virtual DatabaseResultSet* RunQueryWithResults();
    virtual DatabaseResultSet* RunQueryWithResults(bool bLogForCleanup);

    SQLHSTMT GetLastStatement() { return (m_Statements.size() > 0) ? m_Statements[m_Statements.size()-1] : NULL; }
    void SetOneTimer(bool bOneTimer = true) { m_bOneTimeStatement = bOneTimer; }

private:
    void InterpretErrorCodes( long nCode, SQLHSTMT stmth_ptr=NULL );
    void FreeParameters();
    void BindParameters();
    void SetParam(int nPosition, OdbcParameter* pParameter);


    int FindStatementAndAdjustPositionIndex(int* pPosition);
 
    bool m_bOneTimeStatement; // Flag to indicate that statement ownership should be handed off to any generated OdbcResultSets
    SQLHENV m_sqlEnvHandle;
    SQLHDBC m_sqlHDBC;
    StatementVector m_Statements;

    ArrayOfOdbcParameters m_Parameters;
    OdbcInterface* m_pInterface;
};

#endif // __ODBC_PREPARED_STATEMENT_H__

