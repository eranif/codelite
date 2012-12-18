//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : plugindata.h
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
#ifndef __plugindata__
#define __plugindata__

#include "codelite_exports.h"
#include "plugin_version.h"
#include "json_node.h"
#include "cl_config.h"
#include <map>
#include <set>

class WXDLLIMPEXP_SDK PluginInfo
{
    wxString name;
    wxString author;
    wxString description;
    wxString version;

public:
    typedef std::map<wxString, PluginInfo> PluginMap_t;

public:
    PluginInfo();
    virtual ~PluginInfo();

public:
    //Setters
    void SetAuthor(const wxString& author) {
        this->author = author;
    }
    void SetDescription(const wxString& description) {
        this->description = description;
    }
    void SetName(const wxString& name) {
        this->name = name;
    }
    void SetVersion(const wxString& version) {
        this->version = version;
    }

    //Getters
    const wxString& GetAuthor() const {
        return author;
    }
    const wxString& GetDescription() const {
        return description;
    }
    const wxString& GetName() const {
        return name;
    }
    const wxString& GetVersion() const {
        return version;
    }
};

class WXDLLIMPEXP_SDK PluginInfoArray : public clConfigItem
{
    PluginInfo::PluginMap_t  m_plugins;
    wxArrayString m_disabledPlugins;

public:
    PluginInfoArray();
    virtual ~PluginInfoArray();

public:
    void SetPlugins(const PluginInfo::PluginMap_t& plugins) {
        this->m_plugins = plugins;
    }
    const PluginInfo::PluginMap_t& GetPlugins() const {
        return m_plugins;
    }
    void AddPlugin(const PluginInfo& plugin);
    bool CanLoad(const wxString &plugin) const;
    void DisablePugins(const wxArrayString& plugins);
    void DisablePlugin(const wxString& plugin);
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;
};

#endif // __plugindata__
