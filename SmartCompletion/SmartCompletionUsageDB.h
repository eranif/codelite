#ifndef SMARTCOMPLETIONUSAGEDB_H
#define SMARTCOMPLETIONUSAGEDB_H

#include <wx/string.h>
#include <unordered_map>
#include "wxStringHash.h"
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
     * @brief load the weight table from the database
     */
    void LoadUsage(std::unordered_map<wxString, int>& weightTable);
    
    /**
     * @brief write the usage to the database
     */
    void StoreUsage(const std::unordered_map<wxString, int>& weightTable);
};

#endif // SMARTCOMPLETIONUSAGEDB_H
