#ifndef SMARTCOMPLETIONSCONFIG_H
#define SMARTCOMPLETIONSCONFIG_H

#include "cl_config.h"
#include <unordered_map>
#include "wxStringHash.h"
#include "SmartCompletionUsageDB.h"

class SmartCompletionsConfig : public clConfigItem
{
public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;

    enum eOptions {
        kEnabled = (1 << 0),
    };

protected:
    size_t m_flags;
    std::unordered_map<wxString, int> m_weight;
    SmartCompletionUsageDB m_db;
    
public:
    SmartCompletionsConfig();
    virtual ~SmartCompletionsConfig();

    SmartCompletionsConfig& Load();
    SmartCompletionsConfig& Save();

    bool IsEnabled() const { return m_flags & kEnabled; }
    void SetEnabled(bool b) { b ? m_flags |= kEnabled : m_flags &= ~kEnabled; }
    std::unordered_map<wxString, int>& GetWeightTable() { return m_weight; }
    
    SmartCompletionUsageDB& GetUsageDb() { return m_db; }
    
};

#endif // SMARTCOMPLETIONSCONFIG_H
