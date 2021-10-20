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

wxString WebToolsConfig::GetTernProjectFile() const
{
    JSON root(cJSON_Object);
    JSONItem libs = JSONItem::createArray("libs");
    root.toElement().append(libs);

    JSONItem plugins = JSONItem::createObject("plugins");
    root.toElement().append(plugins);

    std::vector<wxString> pluginsToLoad;

    // basic plugins that should always get loaded
    pluginsToLoad.push_back("commonjs");
    pluginsToLoad.push_back("modules");

    std::set<wxString> uniquePlugins;
    std::for_each(pluginsToLoad.begin(), pluginsToLoad.end(), [&](const wxString& name) {
        if(uniquePlugins.count(name) == 0) {
            uniquePlugins.insert(name);
            JSONItem node = JSONItem::createObject(name);
            plugins.append(node);
        }
    });
    return root.toElement().format();
}

WebToolsConfig& WebToolsConfig::Get()
{
    static WebToolsConfig webtoolsConfig;
    return webtoolsConfig;
}

bool WebToolsConfig::IsNodeInstalled() const
{
    wxFileName fn(GetNodejs());
    return fn.IsOk() && fn.FileExists();
}

bool WebToolsConfig::IsNpmInstalled() const
{
    wxFileName fn(GetNpm());
    return fn.IsOk() && fn.FileExists();
}

wxFileName WebToolsConfig::GetTernScript() const
{
    wxFileName fn(GetTempFolder(false), "tern");
    fn.AppendDir("node_modules");
    fn.AppendDir("tern");
    fn.AppendDir("bin");
    return fn;
}

bool WebToolsConfig::IsTernInstalled() const { return GetTernScript().FileExists(); }

wxString WebToolsConfig::GetTempFolder(bool create) const
{
    wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "");
    fn.AppendDir("webtools");
    if(create) {
        fn.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }
    return fn.GetPath();
}
