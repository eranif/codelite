#include "LanguageServerConfig.h"
#include <algorithm>

LanguageServerConfig::LanguageServerConfig()
    : clConfigItem("LSPConfig")
{
}

LanguageServerConfig::~LanguageServerConfig() {}

LanguageServerConfig& LanguageServerConfig::Load()
{
    clConfig conf("LanguageServer.conf");
    conf.ReadItem(this);
    return *this;
}

LanguageServerConfig& LanguageServerConfig::Save()
{
    clConfig conf("LanguageServer.conf");
    conf.WriteItem(this);
    return *this;
}

LanguageServerConfig& LanguageServerConfig::Get()
{
    static LanguageServerConfig config;
    return config;
}

void LanguageServerConfig::FromJSON(const JSONItem& json)
{
    m_servers.clear();
    m_flags = json.namedObject("flags").toSize_t(m_flags);
    if(json.hasNamedObject("servers")) {
        JSONItem servers = json.namedObject("servers");
        size_t count = servers.arraySize();
        for(size_t i = 0; i < count; ++i) {
            JSONItem server = servers.arrayItem(i);
            LanguageServerEntry entry;
            entry.FromJSON(server);
            m_servers.insert({ entry.GetName(), entry });
        }
    }
}

JSONItem LanguageServerConfig::ToJSON() const
{
    JSONItem json = JSONItem::createObject(GetName());
    json.addProperty("flags", m_flags);
    JSONItem servers = JSONItem::createArray("servers");
    std::for_each(m_servers.begin(), m_servers.end(),
                  [&](const LanguageServerEntry::Map_t::value_type& vt) { servers.append(vt.second.ToJSON()); });
    json.append(servers);
    return json;
}
