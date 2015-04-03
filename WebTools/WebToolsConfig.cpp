#include "WebToolsConfig.h"
#include "json_node.h"

WebToolsConfig::WebToolsConfig()
    : clConfigItem("WebTools")
    , m_jsFlags(kJSEnableCC | kJSLibraryBrowser | kJSLibraryEcma5)
{
}

WebToolsConfig::~WebToolsConfig() {}

WebToolsConfig& WebToolsConfig::Load()
{
    clConfig conf("WebTools.conf");
    conf.ReadItem(this);
    return *this;
}

WebToolsConfig& WebToolsConfig::Save()
{
    clConfig conf("WebTools.conf");
    conf.WriteItem(this);
    return *this;
}

void WebToolsConfig::FromJSON(const JSONElement& json) 
{
    m_jsFlags = json.namedObject("m_jsFlags").toSize_t(m_jsFlags);
}

JSONElement WebToolsConfig::ToJSON() const
{
    JSONElement element = JSONElement::createObject(GetName());
    element.addProperty("m_jsFlags", m_jsFlags);
    return element;
}
