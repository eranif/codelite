//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : plugindata.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#include "plugindata.h"

PluginInfo::PluginInfo()
{
}

PluginInfo::~PluginInfo()
{
}

void PluginInfo::FromJSON(const JSONElement& json)
{
    name = json.namedObject("name").toString();
    author = json.namedObject("author").toString();
    description = json.namedObject("description").toString();
    version = json.namedObject("version").toString();
}

JSONElement PluginInfo::ToJSON() const
{
    JSONElement e = JSONElement::createObject();
    e.addProperty("name", name);
    e.addProperty("author", author);
    e.addProperty("description", description);
    e.addProperty("version", version);
    return e;
}


//-------------------------------------------
// PluginConfig
//-------------------------------------------
PluginInfoArray::PluginInfoArray()
    : clConfigItem("codelite-plugins")
{
}

PluginInfoArray::~PluginInfoArray()
{
}

void PluginInfoArray::DisablePugins(const wxArrayString& plugins)
{
    m_disabledPlugins = plugins;
}

bool PluginInfoArray::CanLoad(const wxString &plugin) const
{
    return m_disabledPlugins.Index(plugin) == wxNOT_FOUND;
}

void PluginInfoArray::FromJSON(const JSONElement& json)
{
    m_disabledPlugins = json.namedObject("disabledPlugins").toArrayString();
    m_plugins.clear();
    JSONElement arr = json.namedObject("installed-plugins");
    for(int i=0; i<arr.arraySize(); ++i) {
        PluginInfo pi;
        pi.FromJSON( arr.arrayItem(i) );
        m_plugins.insert(std::make_pair(pi.GetName(), pi));
    }
}

JSONElement PluginInfoArray::ToJSON() const
{
    JSONElement el = JSONElement::createObject(GetName());
    el.addProperty("disabledPlugins", m_disabledPlugins);
    
    JSONElement arr = JSONElement::createArray("installed-plugins");
    PluginInfo::PluginMap_t::const_iterator iter = m_plugins.begin();
    for( ; iter != m_plugins.end(); ++iter ) {
        arr.arrayAppend( iter->second.ToJSON() );
    }
    el.append(arr);
    return el;
}

void PluginInfoArray::AddPlugin(const PluginInfo& plugin)
{
    if ( m_plugins.count(plugin.GetName()) )
        m_plugins.erase(plugin.GetName());
        
    m_plugins.insert(std::make_pair(plugin.GetName(), plugin));
}

void PluginInfoArray::DisablePlugin(const wxString& plugin)
{
    if ( m_disabledPlugins.Index(plugin) == wxNOT_FOUND )
        m_disabledPlugins.Add( plugin );
}
