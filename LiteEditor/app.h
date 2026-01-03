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
#pragma once

#include "clPersistenceManager.h"
#include "frame.h"

#include <memory>
#include <wx/cmdline.h>
#include <wx/snglinst.h>

class CodeLiteApp : public wxApp
{
public:
    enum PluginPolicy { PP_None = 0, PP_All, PP_FromList };

protected:
    clMainFrame* m_pMainFrame;
    wxSingleInstanceChecker* m_singleInstance;
    wxArrayString m_parserPaths;
    wxLocale m_locale;
    wxArrayString m_allowedPlugins;
    PluginPolicy m_pluginLoadPolicy;
    std::unique_ptr<clPersistenceManager> m_persistencManager;
    bool m_startedInDebuggerMode;

    // When starting in debugger mode
    wxString m_exeToDebug;
    wxString m_debuggerArgs;
    wxString m_debuggerWorkingDirectory;
    static bool m_restartCodeLite;
    static wxString m_restartCommand;
    static wxString m_restartWD;
    wxCmdLineParser m_parser;

private: // Methods
    bool CopySettings(const wxString& destDir, wxString& installPath);
    bool IsSingleInstance(const wxCmdLineParser& parser);
    void DoCopyGdbPrinters();
    void MSWReadRegistry();
    wxString DoFindMenuFile(const wxString& installDirectory);
    void AdjustPathForCygwinIfNeeded();
    void AdjustPathForMSYSIfNeeded();
    void PrintUsage(const wxCmdLineParser& parser);

public:
    CodeLiteApp();
    virtual ~CodeLiteApp();

    static void FinalizeShutdown();
    void SetAllowedPlugins(const wxArrayString& allowedPlugins) { this->m_allowedPlugins = allowedPlugins; }
    void SetPluginLoadPolicy(const PluginPolicy& pluginLoadPolicy) { this->m_pluginLoadPolicy = pluginLoadPolicy; }
    const wxArrayString& GetAllowedPlugins() const { return m_allowedPlugins; }
    const PluginPolicy& GetPluginLoadPolicy() const { return m_pluginLoadPolicy; }
    void SetParserPaths(const wxArrayString& parserPaths) { this->m_parserPaths = parserPaths; }
    const wxArrayString& GetParserPaths() const { return m_parserPaths; }

    void MacOpenFile(const wxString& fileName);

    void SetStartedInDebuggerMode(bool startedInDebuggerMode) { this->m_startedInDebuggerMode = startedInDebuggerMode; }
    bool IsStartedInDebuggerMode() const { return m_startedInDebuggerMode; }

    void SetDebuggerArgs(const wxString& debuggerArgs) { this->m_debuggerArgs = debuggerArgs; }
    void SetDebuggerWorkingDirectory(const wxString& debuggerWorkingDirectory)
    {
        this->m_debuggerWorkingDirectory = debuggerWorkingDirectory;
    }
    void SetExeToDebug(const wxString& exeToDebug) { this->m_exeToDebug = exeToDebug; }
    const wxString& GetDebuggerArgs() const { return m_debuggerArgs; }
    const wxString& GetDebuggerWorkingDirectory() const { return m_debuggerWorkingDirectory; }
    const wxString& GetExeToDebug() const { return m_exeToDebug; }

    // Restart related variables
    static void SetRestartCodeLite(bool restartCodeLite) { m_restartCodeLite = restartCodeLite; }
    static void SetRestartCommand(const wxString& restartCommand, const wxString& workingDirectory)
    {
        m_restartCommand = restartCommand;
        m_restartWD = workingDirectory;
    }
    static bool IsRestartCodeLite() { return m_restartCodeLite; }
    static const wxString& GetRestartCommand() { return m_restartCommand; }

    void ProcessCommandLineParams();

protected:
    virtual bool OnInit();
    virtual int OnExit();
    virtual void OnFatalException();
    void OpenFolder(const wxString& path);
    void OpenFile(const wxString& path, long lineNumber);
    void OpenItem(const wxString& path, long lineNumber);
};
