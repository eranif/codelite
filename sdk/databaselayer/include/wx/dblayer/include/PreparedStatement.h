#ifndef __PREPARED_STATEMENT_H__
#define __PREPARED_STATEMENT_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/hashset.h"

#include "DatabaseLayerDef.h"
#include "DatabaseErrorReporter.h"
#include "DatabaseStringConverter.h"
#include "DatabaseResultSet.h"
#include "DatabaseQueryParser.h"

WX_DECLARE_HASH_SET( DatabaseResultSet*, wxPointerHash, wxPointerEqual, StatementResultSetHashSet );

class WXDLLIMPEXP_DATABASELAYER PreparedStatement : public DatabaseErrorReporter, public DatabaseStringConverter
{
public:
  /// Constructor
  PreparedStatement();

  /// Destructor
  virtual ~PreparedStatement();

  /// Close the result set (call DatabaseLayer::ClosePreparedStatement() instead on the statement)
  virtual void Close() = 0;
  
  // set parameters
  /// Set the parameter at the 1-based position to an int value
  virtual void SetParamInt(int nPosition, int nValue) = 0;
  /// Set the parameter at the 1-based position to a double value
  virtual void SetParamDouble(int nPosition, double dblValue) = 0;
  /// Set the parameter at the 1-based position to a wxString value
  virtual void SetParamString(int nPosition, const wxString& strValue) = 0;
  /// Set the parameter at the 1-based position to a NULL  value
  virtual void SetParamNull(int nPosition) = 0;
  /// Set the parameter at the 1-based position to a Blob value
  virtual void SetParamBlob(int nPosition, const wxMemoryBuffer& buffer);
  /// Set the parameter at the 1-based position to a Blob value
  virtual void SetParamBlob(int nPosition, const void* pData, long nDataLength) = 0;
  /// Set the parameter at the 1-based position to a wxDateTime value
  virtual void SetParamDate(int nPosition, const wxDateTime& dateValue) = 0;
  /// Set the parameter at the 1-based position to a boolean value
  virtual void SetParamBool(int nPosition, bool bValue) = 0;
  virtual int GetParameterCount() = 0;
  
  /// Run an insert, update, or delete query on the database
  virtual int RunQuery() = 0;
  /// Run an insert, update, or delete query on the database
  virtual DatabaseResultSet* RunQueryWithResults() = 0;

  // function names more consistent with JDBC and wxSQLite3
  // these just provide wrappers for existing functions
  /// See RunQuery
  int ExecuteUpdate() { return RunQuery(); } 
  /// See RunQueryWithResults
  DatabaseResultSet* ExecuteQuery() { return RunQueryWithResults(); }

  /// Close a result set returned by the database or a prepared statement previously
  virtual bool CloseResultSet(DatabaseResultSet* pResultSet);

protected:
  /// Close all result set objects that have been generated but not yet closed
  void CloseResultSets();
  /// Add result set object pointer to the list for "garbage collection"
  void LogResultSetForCleanup(DatabaseResultSet* pResultSet) { m_ResultSets.insert(pResultSet); }
  
private:
  StatementResultSetHashSet m_ResultSets;
};

#endif // __PREPARED_STATEMENT_H__

