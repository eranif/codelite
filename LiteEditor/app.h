//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : app.h
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
#ifndef LITEEDITOR_APP_H
#define LITEEDITOR_APP_H

#include "frame.h"
#include <set>

class wxSplashScreen;
class wxSingleInstanceChecker;

class CodeLiteApp : public wxApp
{
public:
    enum PluginPolicy { PP_None = 0, PP_All, PP_FromList };

protected:
    wxSplashScreen* m_splash;
    clMainFrame* m_pMainFrame;
    wxSingleInstanceChecker* m_singleInstance;
    wxArrayString m_parserPaths;
    wxLocale m_locale;
    wxArrayString m_allowedPlugins;
    PluginPolicy m_pluginLoadPolicy;

private: // Methods
    bool CopySettings(const wxString& destDir, wxString& installPath);
    bool IsSingleInstance(const wxCmdLineParser& parser, const wxString& curdir);
    void DoCopyGdbPrinters();
    void MSWReadRegistry();
    wxString DoFindMenuFile(const wxString& installDirectory, const wxString& requiredVersion);
    void AdjustPathForCygwinIfNeeded();
    void AdjustPathForMSYSIfNeeded();

#ifdef __WXMSW__
    HINSTANCE m_handler;
#endif

public:
    CodeLiteApp(void);
    virtual ~CodeLiteApp(void);

    void SetAllowedPlugins(const wxArrayString& allowedPlugins) { this->m_allowedPlugins = allowedPlugins; }
    void SetPluginLoadPolicy(const PluginPolicy& pluginLoadPolicy) { this->m_pluginLoadPolicy = pluginLoadPolicy; }
    const wxArrayString& GetAllowedPlugins() const { return m_allowedPlugins; }
    const PluginPolicy& GetPluginLoadPolicy() const { return m_pluginLoadPolicy; }
    void SetParserPaths(const wxArrayString& parserPaths) { this->m_parserPaths = parserPaths; }
    const wxArrayString& GetParserPaths() const { return m_parserPaths; }

    void MacOpenFile(const wxString& fileName);

protected:
    virtual bool OnInit();
    virtual int OnExit();
    virtual void OnFatalException();

    //    DECLARE_EVENT_TABLE()
    //    void OnAppAcitvated(wxActivateEvent &e);
};

#endif // LITEEDITOR_APP_H
