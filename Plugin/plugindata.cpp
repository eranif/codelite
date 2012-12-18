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

//-------------------------------------------
// PluginConfig
//-------------------------------------------
PluginInfoArray::PluginInfoArray()
    : clConfigItem("Plugins")
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
}

JSONElement PluginInfoArray::ToJSON() const
{
    JSONElement el = JSONElement::createObject();
    el.addProperty("disabledPlugins", m_disabledPlugins);
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

