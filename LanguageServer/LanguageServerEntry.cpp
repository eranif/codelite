#include "LanguageServerEntry.h"

LanguageServerEntry::LanguageServerEntry() {}

LanguageServerEntry::~LanguageServerEntry() {}

void LanguageServerEntry::FromJSON(const JSONItem& json)
{
    m_name = json.namedObject("name").toString();
    m_exepath = json.namedObject("exepath").toString();
    m_args = json.namedObject("args").toString();
    m_workingDirectory = json.namedObject("workingDirectory").toString();
    m_languages = json.namedObject("languages").toArrayString();
    m_enabled = json.namedObject("enabled").toBool(m_enabled);
}

JSONItem LanguageServerEntry::ToJSON() const
{
    JSONItem json = JSONItem::createObject();
    json.addProperty("name", m_name);
    json.addProperty("exepath", m_exepath);
    json.addProperty("args", m_args);
    json.addProperty("languages", m_languages);
    json.addProperty("enabled", m_enabled);
    json.addProperty("workingDirectory", m_workingDirectory);
    return json;
}
