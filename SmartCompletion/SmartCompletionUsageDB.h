#ifndef SMARTCOMPLETIONUSAGEDB_H
#define SMARTCOMPLETIONUSAGEDB_H

#include "wxStringHash.h"
#include <unordered_map>
#include <wx/string.h>
#include <wx/wxsqlite3.h>

class SmartCompletionUsageDB
{
    wxSQLite3Database m_db;

protected:
    void CreateScheme();

public:
    SmartCompletionUsageDB();
    virtual ~SmartCompletionUsageDB();

    /**
     * @brief open the usage DB
     */
    void Open();

    /**
     * @brief close the usage DB
     */
    void Close();

    /**
     * @brief load the CC weight table from the database
     */
    void LoadCCUsageTable(std::unordered_map<wxString, int>& weightTable);
    /**
     * @brief load the GTA weight table from the database
     */
    void LoadGTAUsageTable(std::unordered_map<wxString, int>& weightTable);

    /**
     * @brief write the CC usage to the database
     */
    void StoreCCUsage(const wxString& key, int weight);
    
    /**
     * @brief write the GTA usage to the database
     */
    void StoreGTAUsage(const wxString& key, int weight);
    
    /**
     * @brief clear the content of the database
     */
    void Clear();
};

#endif // SMARTCOMPLETIONUSAGEDB_H
