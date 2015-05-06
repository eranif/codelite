#ifndef WORDCOMPLETIONSETTINGS_H
#define WORDCOMPLETIONSETTINGS_H

#include "cl_config.h" // Base class: clConfigItem

class WordCompletionSettings : public clConfigItem
{
public:
    enum {
        kComparisonStartsWith = 0,
        kComparisonContains = 1,
    };

private:
    int m_comparisonMethod;

public:
    WordCompletionSettings();
    virtual ~WordCompletionSettings();

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;

    void SetComparisonMethod(int comparisonMethod) { this->m_comparisonMethod = comparisonMethod; }
    int GetComparisonMethod() const { return m_comparisonMethod; }
    
    WordCompletionSettings& Load();
    WordCompletionSettings& Save();
};

#endif // WORDCOMPLETIONSETTINGS_H
