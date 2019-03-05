//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : HelpPluginSettings.h
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

#ifndef HELPPLUGINSETTINGS_H
#define HELPPLUGINSETTINGS_H

#include "JSON.h"
#include "cl_config.h"

class HelpPluginSettings : public clConfigItem
{
    wxString m_cxxDocset;
    wxString m_phpDocset;
    wxString m_htmlDocset;
    wxString m_cmakeDocset;
    wxString m_cssDocset;
    wxString m_jsDocset;
    wxString m_javaDocset;

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;

    HelpPluginSettings();
    virtual ~HelpPluginSettings();

    HelpPluginSettings& Load();
    HelpPluginSettings& Save();

    void SetCmakeDocset(const wxString& cmakeDocset) { this->m_cmakeDocset = cmakeDocset; }
    void SetCssDocset(const wxString& cssDocset) { this->m_cssDocset = cssDocset; }
    void SetCxxDocset(const wxString& cxxDocset) { this->m_cxxDocset = cxxDocset; }
    void SetHtmlDocset(const wxString& htmlDocset) { this->m_htmlDocset = htmlDocset; }
    void SetJavaDocset(const wxString& javaDocset) { this->m_javaDocset = javaDocset; }
    void SetJsDocset(const wxString& jsDocset) { this->m_jsDocset = jsDocset; }
    void SetPhpDocset(const wxString& phpDocset) { this->m_phpDocset = phpDocset; }
    const wxString& GetCmakeDocset() const { return m_cmakeDocset; }
    const wxString& GetCssDocset() const { return m_cssDocset; }
    const wxString& GetCxxDocset() const { return m_cxxDocset; }
    const wxString& GetHtmlDocset() const { return m_htmlDocset; }
    const wxString& GetJavaDocset() const { return m_javaDocset; }
    const wxString& GetJsDocset() const { return m_jsDocset; }
    const wxString& GetPhpDocset() const { return m_phpDocset; }
};

#endif // HELPPLUGINSETTINGS_H
