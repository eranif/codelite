#include "SmartCompletionUsageDB.h"
#include "file_logger.h"
#include <wx/filename.h>
#include "cl_standard_paths.h"

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

        sql << "CREATE TABLE IF NOT EXISTS CC_USAGE(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
            << "NAME TEXT, " // The scope type: 0 for namespace, 1 for class
            << "WEIGHT INTEGER)";
        m_db.ExecuteUpdate(sql);

    } catch(wxSQLite3Exception& e) {
        clWARNING() << "SmartCompletionUsageDB::CreateScheme():" << e.GetMessage() << clEndl;
    }
}

void SmartCompletionUsageDB::LoadUsage(std::unordered_map<wxString, int>& weightTable)
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

void SmartCompletionUsageDB::StoreUsage(const std::unordered_map<wxString, int>& weightTable)
{
    try {
        m_db.Begin();
        m_db.ExecuteQuery("delete from CC_USAGE");

        wxSQLite3Statement st = m_db.PrepareStatement("replace into CC_USAGE (ID, NAME, WEIGHT) values (NULL, ?, ?)");
        std::for_each(weightTable.begin(), weightTable.end(),
                      [&](const std::unordered_map<wxString, int>::value_type& vt) {
                          st.Bind(1, vt.first);
                          st.Bind(2, vt.second);
                          st.ExecuteUpdate();
                      });
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
