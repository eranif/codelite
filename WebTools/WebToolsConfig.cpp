#include "WebToolsConfig.h"
#include "json_node.h"
#include "NodeJSLocator.h"

WebToolsConfig::WebToolsConfig()
    : clConfigItem("WebTools")
    , m_jsFlags(kJSEnableCC | kJSLibraryBrowser | kJSLibraryEcma5 | kJSLibraryEcma6 | kJSPluginStrings | kJSPluginNode)
    , m_xmlFlags(kXmlEnableCC)
    , m_htmlFlags(kHtmlEnableCC)
{
    NodeJSLocator locator;
    locator.Locate();
    m_nodejs = locator.GetNodejs();
    m_npm = locator.GetNpm();
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
    m_xmlFlags = json.namedObject("m_xmlFlags").toSize_t(m_xmlFlags);
    m_htmlFlags = json.namedObject("m_htmlFlags").toSize_t(m_htmlFlags);
    m_nodejs = json.namedObject("m_nodejs").toString(m_nodejs);
    m_npm = json.namedObject("m_npm").toString(m_npm);
}

JSONElement WebToolsConfig::ToJSON() const
{
    JSONElement element = JSONElement::createObject(GetName());
    element.addProperty("m_jsFlags", m_jsFlags);
    element.addProperty("m_xmlFlags", m_xmlFlags);
    element.addProperty("m_htmlFlags", m_htmlFlags);
    element.addProperty("m_nodejs", m_nodejs);
    element.addProperty("m_npm", m_npm);
    return element;
}

wxString WebToolsConfig::GetTernProjectFile() const
{
    JSONRoot root(cJSON_Object);
    JSONElement libs = JSONElement::createArray("libs");
    root.toElement().append(libs);

    if(m_jsFlags & kJSLibraryBrowser) libs.arrayAppend("browser");
    if(m_jsFlags & kJSLibraryChai) libs.arrayAppend("chai");
    if(m_jsFlags & kJSLibraryEcma5) libs.arrayAppend("ecma5");
    if(m_jsFlags & kJSLibraryEcma6) libs.arrayAppend("ecma6");
    if(m_jsFlags & kJSLibraryJQuery) libs.arrayAppend("jquery");
    if(m_jsFlags & kJSLibraryUnderscore) libs.arrayAppend("underscore");
    if(m_jsFlags & kJSPluginQML) libs.arrayAppend("qml");

    JSONElement plugins = JSONElement::createObject("plugins");
    root.toElement().append(plugins);

    if(m_jsFlags & kJSPluginNode) {
        JSONElement node = JSONElement::createObject("node");
        plugins.append(node);
    }

    if(m_jsFlags & kJSPluginRequireJS) {
        JSONElement node = JSONElement::createObject("requirejs");
        plugins.append(node);
    }

    if(m_jsFlags & kJSPluginStrings) {
        JSONElement complete_strings = JSONElement::createObject("complete_strings");
        plugins.append(complete_strings);
    }

    if(m_jsFlags & kJSPluginAngular) {
        JSONElement angular = JSONElement::createObject("angular");
        plugins.append(angular);
    }

    return root.toElement().format();
}
