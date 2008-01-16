///////////////////////////////////////////////////////////////////////////////
// Name:        minimal.cpp
// Purpose:     Test program for the wxSQLite3 class
//              This example is based on the CppSQLite example.
// Author:      Ulrich Telle
// Modified by:
// Created:     2005-07-14
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/wxsqlite3.h"
#include <ctime>
#include <iostream>

using namespace std;

// User defined aggregate function
class MyAggregateFunction : public wxSQLite3AggregateFunction
{
public:
  // Concatenate all values
  virtual void Aggregate(wxSQLite3FunctionContext& ctx)
  {
    // Get the temporary memory for storing the intermediate result
    wxString** acc = (wxString**) ctx.GetAggregateStruct(sizeof (wxString**));

    // Allocate a wxString instance in the first aggregate step
    if (*acc == NULL) {
      *acc = new wxString(_T(""));
    }

    // Concatenate all arguments
    for (int i = 0; i < ctx.GetArgCount(); i++) {
       (*acc)->Append(ctx.GetString(i));
       (*acc)->Append(_T(" "));
    }
  }

  // Set the result of the aggregate function
  virtual void Finalize(wxSQLite3FunctionContext& ctx)
  {
    // Get the temporary memory conatining the result
    wxString** acc = (wxString**) ctx.GetAggregateStruct(sizeof (wxString**));

    // Set the result
    ctx.SetResult(*(*acc));

    // Important: Free the allocated wxString
    delete *acc;
    *acc = 0;
  }
};

// Dummy authorizer logging only when the function is called
class MyAuthorizer : public wxSQLite3Authorizer
{
public:
  virtual wxAuthorizationResult Authorize(wxAuthorizationCode type, 
                                          const wxString& arg1, const wxString& arg2, 
                                          const wxString& arg3, const wxString& arg4)
  {
    cout << "AUTH: " 
         << (const char*) AuthorizationCodeToString(type).mb_str(wxConvUTF8) << ","
         << (const char*) arg1.mb_str(wxConvUTF8) << ","
         << (const char*) arg2.mb_str(wxConvUTF8) << ","
         << (const char*) arg3.mb_str(wxConvUTF8) << ","
         << (const char*) arg4.mb_str(wxConvUTF8) << endl;
    return wxSQLite3Authorizer::SQLITE_OK;
  }
};

class MyCallback : public wxSQLite3Hook
{
public:
  virtual bool CommitCallback()
  {
    cout << "Here is the COMMIT callback" << endl;
    return false;
  }

  virtual void RollbackCallback()
  {
    cout << "Here is the ROLLBACK callback" << endl;
  }

  virtual void UpdateCallback(wxUpdateType type,
                              const wxString& database, const wxString& table,
                              wxLongLong rowid)
  {
    char* strType;
    cout << "Here is the UPDATE callback" << endl;
    switch (type)
    {
      case SQLITE_DELETE:
        strType = "DELETE row ";
        break;
      case SQLITE_INSERT:
        strType = "INSERT row ";
        break;
      case SQLITE_UPDATE:
        strType = "UPDATE row ";
        break;
    }
    cout << strType << (const char*) rowid.ToString().mb_str()
         << " in table " << (const char*) table.mb_str()
         << " of database " << (const char*) database.mb_str() << endl;
  }
  virtual void Dummy() {}
};

class MyCollation : public wxSQLite3Collation
{
public:
  virtual int Compare(const wxString& text1, const wxString& text2)
  {
    wxString loc1;
    wxString loc2;
    size_t j1 = text1.Length();
    size_t j2 = text2.Length();
    size_t j;
    loc1.Alloc(j1);
    loc2.Alloc(j2);
    for (j = 0; j < j1; j++) loc1 += text1[j1-1-j];
    for (j = 0; j < j2; j++) loc2 += text2[j2-1-j];
    return loc1.Cmp(loc2);
  }
};

int main(int argc, char** argv)
{
  const wxString gszFile = wxGetCwd() + _T("/test.db");

  MyAggregateFunction myAggregate;
  MyAuthorizer myAuthorizer;
  MyCallback myCallback;
  MyCollation myCollation;

  try
  {
    int i, fld;
    time_t tmStart, tmEnd;
    wxSQLite3Database db;

    cout << "SQLite3 Version: " << (const char*) db.GetVersion().mb_str(wxConvUTF8) << endl;

    remove(gszFile.mb_str());
    if (wxSQLite3Database::HasEncryptionSupport())
    {
      db.Open(gszFile, wxString(_T("password")));
    }
    else
    {
      db.Open(gszFile);
    }

    cout << endl << "emp table exists=" << (db.TableExists(_T("emp")) ? "TRUE":"FALSE") << endl;
    cout << endl << "Creating emp table" << endl;
    db.ExecuteUpdate(_T("create table emp(empno int, empname char(20));"));
    cout << endl << "emp table exists=" << (db.TableExists(_T("emp")) ? "TRUE":"FALSE") << endl;
    
    // Execute some DML, and print number of rows affected by each one

    db.SetUpdateHook(&myCallback);

    cout << endl << "DML tests" << endl;
    wxString insertCmd(_T("insert into emp values (7, 'Franz Beckenbauer');"));
    int nRows = db.ExecuteUpdate(insertCmd);
    cout << nRows << " rows inserted" << endl;

    wxSQLite3ResultSet q1 = db.ExecuteQuery("select empname from emp order by 1;");

    while (q1.NextRow())
    {
      cout << (const char*)(q1.GetString(0).mb_str()) << endl;
    }

    nRows = db.ExecuteUpdate("update emp set empname = 'Lukas Podolski' where empno = 7;");
    cout << nRows << " rows updated" << endl;

    nRows = db.ExecuteUpdate("delete from emp where empno = 7;");
    cout << nRows << " rows deleted" << endl;

    db.SetUpdateHook(NULL);

    // Transaction Demo
    
    int nRowsToCreate(50000);
    cout << endl << "Transaction test, creating " << nRowsToCreate;
    cout << " rows please wait..." << endl;
    tmStart = time(0);
    db.Begin();

    for (i = 0; i < nRowsToCreate; i++)
    {
      char buf[128];
      sprintf(buf, "insert into emp values (%d, 'Empname%06d');", i, i);
      db.ExecuteUpdate(buf);
    }

    db.Commit();
    tmEnd = time(0);

    // Demonstrate scalar query

    cout << db.ExecuteScalar("select count(*) from emp;") << " rows in emp table in ";
    cout << tmEnd-tmStart << " seconds" << endl;

    // Re-create emp table with auto-increment field

    cout << endl << "Auto increment test" << endl;
    db.ExecuteUpdate("drop table emp;");
    db.ExecuteUpdate("create table emp(empno integer primary key, empname char(20));");
    cout << nRows << " rows deleted" << endl;

    for (i = 0; i < 5; i++)
    {
      char buf[128];
      sprintf(buf, "insert into emp (empname) values ('Empname%06d');", i+1);
      db.ExecuteUpdate(buf);
      cout << " primary key: " << db.GetLastRowId().ToLong() << endl;
    }

    // Query data and also show results of inserts into auto-increment field

    wxSQLite3ResultSet q = db.ExecuteQuery("select * from emp order by 1;");

    if (wxSQLite3Database::HasMetaDataSupport())
    {
      cout << endl << "Meta data test" << endl;

      wxString dataType;
      wxString collation;
      bool notNull;
      bool primaryKey;
      bool autoIncrement;
      db.GetMetaData(wxEmptyString, _T("emp"), _T("empname"), &dataType, &collation, &notNull, &primaryKey, &autoIncrement);
      cout << "Meta data of table 'emp', column 'empname'" << endl 
           << "Data type: " << (const char*) dataType.mb_str(wxConvUTF8)
           << ", Collation: " << (const char*) collation.mb_str(wxConvUTF8)
           << ", Not Null: " << notNull
           << ", Primary Key: " << primaryKey
           << ", Auto Increment: " << autoIncrement << endl;

      for (fld = 0; fld < q.GetColumnCount(); fld++)
      {
        cout << (fld+1) << ". field:" << endl 
             << "database = (" << (const char*) q.GetDatabaseName(fld).mb_str(wxConvUTF8) << ")" << endl
             << "table    = (" << (const char*) q.GetTableName(fld).mb_str(wxConvUTF8) << ")" << endl
             << "origin   = (" << (const char*) q.GetOriginName(fld).mb_str(wxConvUTF8) << ")" << endl;
      }
      cout << endl;
    }

    cout << endl << "Select statement test" << endl;
    for (fld = 0; fld < q.GetColumnCount(); fld++)
    {
      cout << (const char*) q.GetColumnName(fld).mb_str(wxConvUTF8)
           << "(" << (const char*) q.GetDeclaredColumnType(fld).mb_str(wxConvUTF8)
           << ")|";
    }
    cout << endl;

    while (q.NextRow())
    {
      cout << q.GetInt(0) << "|";
      cout << (const char*)(q.GetString(1).mb_str()) << "|" << endl;
    }

    // SQLite3's printf() functionality. Handles embedded quotes and NULLs

    cout << endl << "SQLite sprintf test" << endl;
    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("insert into emp (empname) values (%Q);", "He's bad");
    cout << (const char*)bufSQL << endl;
    db.ExecuteUpdate(bufSQL);

    bufSQL.Format("insert into emp (empname) values (%Q);", NULL);
    cout << (const char*)bufSQL << endl;
    db.ExecuteUpdate(bufSQL);

    // Fetch table at once

    cout << endl << "getTable() test" << endl;
    wxSQLite3Table t = db.GetTable("select * from emp order by 1;");

    for (fld = 0; fld < t.GetColumnCount(); fld++)
    {
      cout << (const char*) t.GetColumnName(fld).mb_str(wxConvUTF8) << "|";
    }
    cout << endl;
    for (int row = 0; row < t.GetRowCount(); row++)
    {
      t.SetRow(row);
      for (int fld = 0; fld < t.GetColumnCount(); fld++)
      {
        if (!t.IsNull(fld))
        {
          cout << (const char*) t.GetAsString(fld).mb_str(wxConvUTF8) << "|";
        }
        else
        {
          cout << "NULL" << "|";
        }
      }
      cout << endl;
    }

    db.CreateFunction(_T("myagg"), 1, myAggregate);

    cout << endl << "Select statement test" << endl;
    wxSQLite3ResultSet q2 = db.ExecuteQuery("select myagg(empname) from emp order by 1;");

    while (q2.NextRow())
    {
      cout << (const char*)(q2.GetString(0).mb_str()) << endl;
    }

    // Test storing/retrieving some binary data, checking
    // it afterwards to make sure it is the same

    cout << endl << "Binary data test" << endl;
    db.ExecuteUpdate("create table bindata(desc char(10), data blob);");
        
    unsigned char bin[256];
    for (i = 0; i < sizeof bin; i++)
    {
      bin[i] = i;
    }
    wxSQLite3Statement stmt = db.PrepareStatement("insert into bindata values ('testing', ?);");
    stmt.Bind(1,bin,sizeof bin);
    stmt.ExecuteUpdate();
    cout << "Stored binary Length: " << sizeof bin << endl;

    q = db.ExecuteQuery("select data from bindata where desc = 'testing';");

    const unsigned char* pbin;
    if (q.NextRow())
    {
      int blobLen;
      const wxString columnName(_T("data"));
      pbin = q.GetBlob(wxString(_T("data")),blobLen);
      cout << "Retrieved binary Length: " << blobLen << endl;
    }

    for (i = 0; i < sizeof bin; i++)
    {
      if (pbin[i] != i)
      {
        cout << "Problem: i: ," << i << " bin[i]: " << pbin[i] << endl;
      }
    }
    q.Finalize();

    db.SetAuthorizer(myAuthorizer);

    // Pre-compiled Statements Demo

    cout << endl << "Transaction test, creating " << nRowsToCreate;
    cout << " rows please wait..." << endl;
    db.ExecuteUpdate("drop table emp;");
    db.ExecuteUpdate("create table emp(empno int, empname char(20));");
    tmStart = time(0);
    db.Begin();

    wxSQLite3Statement stmt2 = db.PrepareStatement("insert into emp values (?, ?);");
    for (i = 0; i < nRowsToCreate; i++)
    {
      char buf[16];
      sprintf(buf, "EmpName%06d", i);
      stmt2.Bind(1, i);
      stmt2.Bind(2, buf);
      stmt2.ExecuteUpdate();
      stmt2.Reset();
    }

    db.Commit();
    tmEnd = time(0);

    cout << db.ExecuteScalar("select count(*) from emp;") << " rows in emp table in ";
    cout << tmEnd-tmStart << " seconds" << endl;

    cout << endl << "User Defined Collation Sequence Test" << endl;
    db.SetCollation(_T("reversed"), &myCollation);
    db.ExecuteUpdate(_T("create table testcol(textcol char(20) collate reversed);"));
    db.ExecuteUpdate(_T("insert into testcol values ('anton');"));
    db.ExecuteUpdate(_T("insert into testcol values ('berta');"));
    db.ExecuteUpdate(_T("insert into testcol values ('cesar');"));

    wxSQLite3ResultSet q3 = db.ExecuteQuery("select textcol from testcol order by 1 desc;");

    while (q3.NextRow())
    {
      cout << (const char*)(q3.GetString(0).mb_str()) << endl;
    }
    if (wxSQLite3Database::HasEncryptionSupport())
    {
      cout << endl << "Rekey the database (that is, decrypt it)" << endl;
      db.ReKey(wxEmptyString);
    }

    cout << endl << "End of tests" << endl;
  }
  catch (wxSQLite3Exception& e)
  {
    cerr << e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }

  // Loop until user enters q or Q

  char c(' ');
  while (c != 'q' && c != 'Q')
  {
    cout << "Press q then enter to quit: ";
    cin >> c;
  }
  return 0;
}

