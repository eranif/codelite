#include "NodeJSWorkspaceConfiguration.h"

NodeJSWorkspaceConfiguration::NodeJSWorkspaceConfiguration()
    : clConfigItem("NodeJS")
{
}

NodeJSWorkspaceConfiguration::~NodeJSWorkspaceConfiguration() {}

void NodeJSWorkspaceConfiguration::FromJSON(const JSONElement& json) 
{
    m_folders = json.namedObject("folders").toArrayString();
}

JSONElement NodeJSWorkspaceConfiguration::ToJSON() const 
{
    // add the metadata
    JSONElement json = JSONElement::createObject(GetName());
    JSONElement metadata = JSONElement::createObject("metadata");
    json.append(metadata);
    metadata.addProperty("version", "1.0");
    metadata.addProperty("ide", "CodeLite");
    metadata.addProperty("type", "NodeJS");
    
    // now add some real properties
    json.addProperty("folders", m_folders);
    return json;
}

NodeJSWorkspaceConfiguration& NodeJSWorkspaceConfiguration::Load(const wxFileName& filename)
{
    clConfig conf(filename.GetFullPath());
    conf.ReadItem(this);
    return *this;
}

NodeJSWorkspaceConfiguration& NodeJSWorkspaceConfiguration::Save(const wxFileName& filename)
{
    clConfig conf(filename.GetFullPath());
    conf.WriteItem(this);
    return *this;
}
