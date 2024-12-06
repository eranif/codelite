#include "JSON.h"
#include "NodeJSLocator.h"
#include "WebToolsConfig.h"
#include <algorithm>
#include <set>
#include <vector>

WebToolsConfig::WebToolsConfig()
    : clConfigItem("WebTools")
    , m_xmlFlags(kXmlEnableCC)
    , m_htmlFlags(kHtmlEnableCC)
    , m_nodeOptions(0)
{
}

WebToolsConfig::~WebToolsConfig() {}

WebToolsConfig& WebToolsConfig::Load()
{
    clConfig conf("WebTools.conf");
    conf.ReadItem(this);
    return *this;
}

WebToolsConfig& WebToolsConfig::SaveConfig()
{
    clConfig conf("WebTools.conf");
    conf.WriteItem(this);
    return *this;
}

void WebToolsConfig::FromJSON(const JSONItem& json)
{
    m_xmlFlags = json.namedObject("m_xmlFlags").toSize_t(m_xmlFlags);
    m_htmlFlags = json.namedObject("m_htmlFlags").toSize_t(m_htmlFlags);
    m_nodeOptions = json.namedObject("m_nodeOptions").toSize_t(m_nodeOptions);
    m_portNumber = json.namedObject("m_portNumber").toInt(m_portNumber);

    wxString v;
    v = json.namedObject("m_nodejs").toString(v);
    if(!v.IsEmpty() && wxFileName::FileExists(v)) {
        m_nodejs = v;
    }
    v.clear();
    v = json.namedObject("m_npm").toString(v);
    if(!v.IsEmpty() && wxFileName::FileExists(v)) {
        m_npm = v;
    }
}

JSONItem WebToolsConfig::ToJSON() const
{
    JSONItem element = JSONItem::createObject(GetName());
    element.addProperty("m_xmlFlags", m_xmlFlags);
    element.addProperty("m_htmlFlags", m_htmlFlags);
    element.addProperty("m_nodejs", m_nodejs);
    element.addProperty("m_npm", m_npm);
    element.addProperty("m_portNumber", m_portNumber);
    return element;
}

WebToolsConfig& WebToolsConfig::Get()
{
    static WebToolsConfig webtoolsConfig;
    return webtoolsConfig;
}
