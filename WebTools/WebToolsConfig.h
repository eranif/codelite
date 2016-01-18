//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : WebToolsConfig.h
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

#ifndef WEBTOOLSCONFIG_H
#define WEBTOOLSCONFIG_H

#include "cl_config.h"

class WebToolsConfig : public clConfigItem
{
    size_t m_jsFlags;
    size_t m_xmlFlags;
    size_t m_htmlFlags;
    wxString m_nodejs;
    wxString m_npm;

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;

    enum eJSFlags {
        kJSEnableCC = (1 << 0),
        kJSEnableVerboseLogging = (1 << 1),
        kJSLibraryJQuery = (1 << 2),
        kJSLibraryBrowser = (1 << 3),
        kJSLibraryEcma5 = (1 << 4),
        kJSLibraryEcma6 = (1 << 5),
        kJSLibraryChai = (1 << 6),
        kJSLibraryUnderscore = (1 << 7),
        kJSPluginNode = (1 << 8),
        kJSPluginStrings = (1 << 9),
        kJSPluginAngular = (1 << 10),
        kJSPluginQML = (1 << 11),
        kJSPluginRequireJS = (1 << 12),
        kJSWebPack = (1 << 13),
        kJSNodeExpress = (1 << 14),
    };

    enum eHtmlFlags {
        kHtmlEnableCC = (1 << 0),
    };

    enum eXmlFlags {
        kXmlEnableCC = (1 << 0),
    };

protected:
    void EnableFlag(size_t& flags, int flag, bool b)
    {
        if(b) {
            flags |= flag;
        } else {
            flags &= ~flag;
        }
    }
    bool HasFlag(const size_t& flags, int flag) const { return flags & flag; }

public:
    WebToolsConfig();
    virtual ~WebToolsConfig();
    bool HasJavaScriptFlag(eJSFlags flag) const { return HasFlag(m_jsFlags, flag); }
    void EnableJavaScriptFlag(eJSFlags flag, bool b) { EnableFlag(m_jsFlags, flag, b); }

    bool HasXmlFlag(eXmlFlags flag) const { return HasFlag(m_xmlFlags, flag); }
    void EnableXmlFlag(eXmlFlags flag, bool b) { EnableFlag(m_xmlFlags, flag, b); }

    bool HasHtmlFlag(eHtmlFlags flag) const { return HasFlag(m_htmlFlags, flag); }
    void EnableHtmlFlag(eHtmlFlags flag, bool b) { EnableFlag(m_htmlFlags, flag, b); }

    void SetNodejs(const wxString& nodejs) { this->m_nodejs = nodejs; }
    void SetNpm(const wxString& npm) { this->m_npm = npm; }
    const wxString& GetNodejs() const { return m_nodejs; }
    const wxString& GetNpm() const { return m_npm; }
    
    /**
     * @brief create tern project file content based on the settings
     */
    wxString GetTernProjectFile() const;
    WebToolsConfig& Load();
    WebToolsConfig& Save();
};

#endif // WEBTOOLSCONFIG_H
