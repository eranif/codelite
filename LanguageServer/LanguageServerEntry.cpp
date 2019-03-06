#include "LanguageServerEntry.h"

LanguageServerEntry::LanguageServerEntry()
    : clConfigItem("LSP")
{
}

LanguageServerEntry::~LanguageServerEntry() {}

void LanguageServerEntry::FromJSON(const JSONItem& json) 
{
    m_name = json.namedObject("name").toString();
    m_exepath = json.namedObject("exepath").toString();
    m_args = json.namedObject("args").toString();
    m_languageId  = json.namedObject("languageId").toString();
    m_filesPattern = json.namedObject("pattern").toString();
    m_enabled = json.namedObject("enabled").toBool(m_enabled);
}

JSONItem LanguageServerEntry::ToJSON() const 
{
    JSONItem json = JSONItem::createObject();
    json.addProperty("name", m_name);
    json.addProperty("exepath", m_exepath);
    json.addProperty("args", m_args);
    json.addProperty("languageId", m_languageId);
    json.addProperty("pattern", m_filesPattern);
    json.addProperty("enabled", m_enabled);
    return json;
}
