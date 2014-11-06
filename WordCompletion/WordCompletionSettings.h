#ifndef WORDCOMPLETIONSETTINGS_H
#define WORDCOMPLETIONSETTINGS_H

#include "cl_config.h" // Base class: clConfigItem

class WordCompletionSettings : public clConfigItem
{
public:
    enum {
        kCompleteWords = (1 << 0),
        kCompleteStrings = (1 << 1),
        kCompleteNumbers = (1 << 2),
    };

    enum {
        kComparisonStartsWith = 0,
        kComparisonContains = 1,
    };

private:
    size_t m_completeTypes;
    int m_comparisonMethod;

public:
    WordCompletionSettings();
    virtual ~WordCompletionSettings();

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;

    void SetCompleteTypes(size_t flags) { this->m_completeTypes = flags; }
    size_t GetCompleteTypes() const { return m_completeTypes; }

    void SetComparisonMethod(int comparisonMethod) { this->m_comparisonMethod = comparisonMethod; }
    int GetComparisonMethod() const { return m_comparisonMethod; }
    
    WordCompletionSettings& Load();
    WordCompletionSettings& Save();
};

#endif // WORDCOMPLETIONSETTINGS_H
