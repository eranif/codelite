#include "LanguageServerConfig.h"
#include <algorithm>
#include <NodeJSLocator.h>

LanguageServerConfig::LanguageServerConfig()
    : clConfigItem("LSPConfig")
{
    NodeJSLocator locator;
    locator.Locate();

    m_nodejs = locator.GetNodejs();
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
    m_nodejs = json.namedObject("nodejs").toString(m_nodejs);
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
    json.addProperty("nodejs", m_nodejs);
    JSONItem servers = JSONItem::createArray("servers");
    std::for_each(m_servers.begin(), m_servers.end(),
                  [&](const LanguageServerEntry::Map_t::value_type& vt) { servers.append(vt.second.ToJSON()); });
    json.append(servers);
    return json;
}

void LanguageServerConfig::AddServer(const LanguageServerEntry& server)
{
    RemoveServer(server.GetName());
    m_servers.insert({ server.GetName(), server });
}

void LanguageServerConfig::RemoveServer(const wxString& name)
{
    if(m_servers.count(name)) { m_servers.erase(name); }
}

const LanguageServerEntry& LanguageServerConfig::GetServer(const wxString& name) const
{
    static LanguageServerEntry NullEntry;
    if(m_servers.count(name) == 0) { return NullEntry; }
    return m_servers.find(name)->second;
}

LanguageServerEntry& LanguageServerConfig::GetServer(const wxString& name)
{
    static LanguageServerEntry NullEntry;
    if(m_servers.count(name) == 0) { return NullEntry; }
    return m_servers[name];
}
