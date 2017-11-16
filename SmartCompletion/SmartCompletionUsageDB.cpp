#include "SmartCompletionUsageDB.h"
#include "cl_standard_paths.h"
#include "file_logger.h"
#include <wx/filename.h>

SmartCompletionUsageDB::SmartCompletionUsageDB() {}

SmartCompletionUsageDB::~SmartCompletionUsageDB() { Close(); }

void SmartCompletionUsageDB::Open()
{
    try {
        if(m_db.IsOpen()) return;

        wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "SmartCompletions.db");
        fn.AppendDir("config");
        m_db.Open(fn.GetFullPath());
        CreateScheme();
    } catch(wxSQLite3Exception& e) {
        clWARNING() << "Failed to open SmartCompletions DB:" << e.GetMessage() << clEndl;
    }
}

void SmartCompletionUsageDB::CreateScheme()
{
    try {
        wxString sql;
        sql = wxT("PRAGMA journal_mode = ON;");
        m_db.ExecuteUpdate(sql);

        sql = wxT("PRAGMA synchronous = OFF;");
        m_db.ExecuteUpdate(sql);

        sql = wxT("PRAGMA temp_store = MEMORY;");
        m_db.ExecuteUpdate(sql);

        sql.Clear();
        sql << "CREATE TABLE IF NOT EXISTS CC_USAGE(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
            << "NAME TEXT, " // The scope type: 0 for namespace, 1 for class
            << "WEIGHT INTEGER)";
        m_db.ExecuteUpdate(sql);

        sql.Clear();
        sql << "CREATE UNIQUE INDEX IF NOT EXISTS CC_USAGE_IDX1 ON CC_USAGE(NAME)";
        m_db.ExecuteUpdate(sql);

        sql.Clear();
        sql << "CREATE TABLE IF NOT EXISTS GOTO_ANYTHING_USAGE(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
            << "NAME TEXT, " // The scope type: 0 for namespace, 1 for class
            << "WEIGHT INTEGER)";
        m_db.ExecuteUpdate(sql);

        sql.Clear();
        sql << "CREATE UNIQUE INDEX IF NOT EXISTS GOTO_ANYTHING_USAGE_IDX1 ON GOTO_ANYTHING_USAGE(NAME)";
        m_db.ExecuteUpdate(sql);

    } catch(wxSQLite3Exception& e) {
        clWARNING() << "SmartCompletionUsageDB::CreateScheme():" << e.GetMessage() << clEndl;
    }
}

void SmartCompletionUsageDB::LoadCCUsageTable(std::unordered_map<wxString, int>& weightTable)
{
    try {
        weightTable.clear();
        wxSQLite3ResultSet res = m_db.ExecuteQuery("select NAME,WEIGHT from CC_USAGE");
        while(res.NextRow()) {
            wxString k = res.GetString(0);
            int v = res.GetInt(1);
            weightTable[k] = v;
        }
    } catch(wxSQLite3Exception& e) {
        clWARNING() << "SQLite 3 error:" << e.GetMessage() << clEndl;
    }
}

void SmartCompletionUsageDB::LoadGTAUsageTable(std::unordered_map<wxString, int>& weightTable)
{
    try {
        weightTable.clear();
        wxSQLite3ResultSet res = m_db.ExecuteQuery("select NAME,WEIGHT from GOTO_ANYTHING_USAGE");
        while(res.NextRow()) {
            wxString k = res.GetString(0);
            int v = res.GetInt(1);
            weightTable[k] = v;
        }
    } catch(wxSQLite3Exception& e) {
        clWARNING() << "SQLite 3 error:" << e.GetMessage() << clEndl;
    }
}

void SmartCompletionUsageDB::StoreCCUsage(const wxString& key, int weight)
{
    try {
        m_db.Begin();
        wxSQLite3Statement st = m_db.PrepareStatement("replace into CC_USAGE (ID, NAME, WEIGHT) values (NULL, ?, ?)");
        st.Bind(1, key);
        st.Bind(2, weight);
        st.ExecuteUpdate();
        m_db.Commit();
    } catch(wxSQLite3Exception& e) {
        clWARNING() << "SQLite 3 error:" << e.GetMessage() << clEndl;
    }
}

void SmartCompletionUsageDB::Close()
{
    if(m_db.IsOpen()) {
        try {
            m_db.Close();
        } catch(wxSQLite3Exception& e) {
            clWARNING() << "SQLite 3 error:" << e.GetMessage() << clEndl;
        }
    }
}

void SmartCompletionUsageDB::Clear()
{
    try {
        m_db.Begin();
        wxString sql = "delete from CC_USAGE";
        m_db.ExecuteUpdate(sql);

        sql = "delete from GOTO_ANYTHING_USAGE";
        m_db.ExecuteUpdate(sql);
        m_db.Commit();
    } catch(wxSQLite3Exception& e) {
        clWARNING() << "SQLite 3 error:" << e.GetMessage() << clEndl;
    }
}

void SmartCompletionUsageDB::StoreGTAUsage(const wxString& key, int weight)
{
    try {
        m_db.Begin();
        wxSQLite3Statement st =
            m_db.PrepareStatement("replace into GOTO_ANYTHING_USAGE (ID, NAME, WEIGHT) values (NULL, ?, ?)");
        st.Bind(1, key);
        st.Bind(2, weight);
        st.ExecuteUpdate();
        m_db.Commit();
    } catch(wxSQLite3Exception& e) {
        clWARNING() << "SQLite 3 error:" << e.GetMessage() << clEndl;
    }
}
