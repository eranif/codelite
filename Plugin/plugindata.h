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
#include "JSON.h"
#include "cl_config.h"
#include <map>
#include <set>

class WXDLLIMPEXP_SDK PluginInfo
{
public:
    enum eFlags {
        kNone = 0,
        kDisabledByDefault = (1 << 0),
    };

protected:
    wxString m_name;
    wxString m_author;
    wxString m_description;
    wxString m_version;
    size_t m_flags;

public:
    typedef std::map<wxString, PluginInfo> PluginMap_t;

public:
    PluginInfo();
    ~PluginInfo();

public:
    // Setters
    void SetAuthor(const wxString& author) { this->m_author = author; }
    void SetDescription(const wxString& description) { this->m_description = description; }
    void SetName(const wxString& name) { this->m_name = name; }
    void SetVersion(const wxString& version) { this->m_version = version; }
    void EnableFlag(PluginInfo::eFlags flag, bool b)
    {
        if(b) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }
    bool HasFlag(PluginInfo::eFlags flag) const { return m_flags & flag; }

    // Getters
    const wxString& GetAuthor() const { return m_author; }
    const wxString& GetDescription() const { return m_description; }
    const wxString& GetName() const { return m_name; }
    const wxString& GetVersion() const { return m_version; }

    JSONItem ToJSON() const;
    void FromJSON(const JSONItem& json);
};

class WXDLLIMPEXP_SDK PluginInfoArray : public clConfigItem
{
    PluginInfo::PluginMap_t m_plugins;
    wxArrayString m_disabledPlugins;

public:
    PluginInfoArray();
    virtual ~PluginInfoArray();

public:
    void SetPlugins(const PluginInfo::PluginMap_t& plugins) { this->m_plugins = plugins; }
    const PluginInfo::PluginMap_t& GetPlugins() const { return m_plugins; }
    void AddPlugin(const PluginInfo& plugin);
    bool CanLoad(const PluginInfo& plugin) const;
    void DisablePugins(const wxArrayString& plugins);
    void DisablePlugin(const wxString& plugin);
    const wxArrayString& GetDisabledPlugins() const { return m_disabledPlugins; }
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;
};

#endif // __plugindata__
