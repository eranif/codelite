//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : FindSymbolDatabase.h
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#ifndef _find_symbol_database_h_
#define _find_symbol_database_h_

#include "codelite_exports.h"

#include <memory>
#include <unordered_map>
#include <wx/filename.h>
#include <wx/wxsqlite3.h>

class WXDLLIMPEXP_SDK FindSymbolDatabase : public wxSQLite3Database
{
public:
    static constexpr const char* kSchemaVersion = "1";

public:
    FindSymbolDatabase() = default;

    static std::unique_ptr<FindSymbolDatabase> OpenDatabase(const wxFileName& fileName);
    void CreateSchema();
    wxString GetSchemaVersion() const;
    void Begin();
    void Commit();
    void Rollback();
    wxSQLite3Statement GetPrepareStatement(const wxString& sql) { return wxSQLite3Database::PrepareStatement(sql); }

    void Close()
    {
        if (IsOpen())
            wxSQLite3Database::Close();
        m_statements.clear();
    }

private:
    std::unordered_map<wxString, wxSQLite3Statement> m_statements;
};

#endif //_find_symbol_database_h_
