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

private:
    size_t m_completeTypes;

public:
    WordCompletionSettings();
    virtual ~WordCompletionSettings();

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;

    void SetCompleteTypes(size_t flags) { this->m_completeTypes = flags; }
    size_t GetCompleteTypes() const { return m_completeTypes; }
    WordCompletionSettings& Load();
    WordCompletionSettings& Save();
};

#endif // WORDCOMPLETIONSETTINGS_H
