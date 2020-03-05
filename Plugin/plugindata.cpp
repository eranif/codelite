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
    : m_flags(kNone)
{
}

PluginInfo::~PluginInfo() {}

void PluginInfo::FromJSON(const JSONItem& json)
{
    m_name = json.namedObject("name").toString();
    m_author = json.namedObject("author").toString();
    m_description = json.namedObject("description").toString();
    m_version = json.namedObject("version").toString();
    m_flags = json.namedObject("flags").toSize_t();
}

JSONItem PluginInfo::ToJSON() const
{
    JSONItem e = JSONItem::createObject();
    e.addProperty("name", m_name);
    e.addProperty("author", m_author);
    e.addProperty("description", m_description);
    e.addProperty("version", m_version);
    e.addProperty("flags", m_flags);
    return e;
}

//-------------------------------------------
// PluginConfig
//-------------------------------------------

PluginInfoArray::PluginInfoArray()
    : clConfigItem("codelite-plugins")
{
}

PluginInfoArray::~PluginInfoArray() {}

bool PluginInfoArray::CanLoad(const PluginInfo& plugin) const
{
    return m_enabledPlugins.Index(plugin.GetName()) != wxNOT_FOUND;
}

void PluginInfoArray::FromJSON(const JSONItem& json)
{
    m_enabledPlugins.Clear();
    if(json.hasNamedObject("enabledPlugins")) {
        m_enabledPlugins = json.namedObject("enabledPlugins").toArrayString();
    } else if(json.hasNamedObject("disabledPlugins")) {
        // first time, merge old settings
        wxArrayString disabledPlugins = json.namedObject("disabledPlugins").toArrayString();
        std::unordered_set<wxString> all_plugins;
        JSONItem arr = json.namedObject("installed-plugins");
        size_t count = arr.arraySize();
        for(size_t i = 0; i < count; ++i) {
            all_plugins.insert(arr.arrayItem(i).namedObject("name").toString());
        }

        for(const auto& disabledPlugin : disabledPlugins) {
            if(all_plugins.count(disabledPlugin)) {
                all_plugins.erase(disabledPlugin);
            }
        }

        m_enabledPlugins.reserve(all_plugins.size());
        for(const auto& enabledPlugin : all_plugins) {
            m_enabledPlugins.Add(enabledPlugin);
        }
    }
}

JSONItem PluginInfoArray::ToJSON() const
{
    JSONItem el = JSONItem::createObject(GetName());
    el.addProperty("enabledPlugins", m_enabledPlugins);
    return el;
}

void PluginInfoArray::EnablePlugin(const wxString& plugin)
{
    if(m_enabledPlugins.Index(plugin) == wxNOT_FOUND) {
        m_enabledPlugins.Add(plugin);
    }
}

void PluginInfoArray::DisablePlugin(const wxString& plugin)
{
    int where = m_enabledPlugins.Index(plugin);
    if(where != wxNOT_FOUND) {
        m_enabledPlugins.RemoveAt(where);
    }
}
