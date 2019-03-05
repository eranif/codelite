//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clTernServer.h
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

#ifndef CLTERNSERVER_H
#define CLTERNSERVER_H

#include "wx/event.h" // Base class: wxEvtHandler
#include <wx/filename.h>
#include <wx/arrstr.h>
#include "wxCodeCompletionBoxEntry.h"
#include "clTernWorkerThread.h"
#include "cl_calltip.h"
#include "JSON.h"
#include "cl_command_event.h"

class IEditor;
class wxStyledTextCtrl;
class JSCodeCompletion;
class IProcess;
class clTernWorkerThread;

struct clTernDefinition {
    wxString url;
    wxString file;
    int start;
    int end;

    clTernDefinition()
        : start(wxNOT_FOUND)
        , end(wxNOT_FOUND)
    {
    }
    
    /**
     * @brief is this location defined on the web?
     */
    bool IsURL() const {
        return !url.IsEmpty();
    }
    
};

class clTernServer : public wxEvtHandler
{
    friend class clTernWorkerThread;

    JSCodeCompletion* m_jsCCManager;
    IProcess* m_tern;
    wxFileName m_nodePath;
    wxString m_ternBinFolder;
    bool m_goingDown;
    wxCodeCompletionBoxEntry::Vec_t m_entries;
    clTernWorkerThread* m_workerThread;
    bool m_fatalError;
    long m_port;
    size_t m_recycleCount;
    wxString m_workingDirectory;

protected:
    void OnTernTerminated(clProcessEvent& event);
    void OnTernOutput(clProcessEvent& event);
    void PrintMessage(const wxString& message);

    void ProcessOutput(const wxString& output, wxCodeCompletionBoxEntry::Vec_t& entries);
    bool ProcessDefinitionOutput(const wxString& output, clTernDefinition& loc);
    clCallTipPtr ProcessCalltip(const wxString& output);

    wxString PrepareDoc(const wxString& doc, const wxString& url);
    void ProcessType(const wxString& type, wxString& signature, wxString& retValue, int& imgID);

    // Worker thread callbacks
    void OnTernWorkerThreadDone(const clTernWorkerThread::Reply& reply);
    void OnError(const wxString& why);
    JSONItem CreateLocation(wxStyledTextCtrl* ctrl, int pos = wxNOT_FOUND);
    JSONItem CreateFilesArray(IEditor *editor, bool forDelete = false);

public:
    void RecycleIfNeeded(bool force = false);
    clTernServer(JSCodeCompletion* cc);
    virtual ~clTernServer();

    long GetPort() const { return m_port; }
    bool Start(const wxString& workingDirectory);
    void Terminate();
    void ClearFatalErrorFlag();
    
    /**
     * @brief tell tern to reset the server
     */
    bool PostResetCommand(bool forgetFiles);
    /**
     * @brief reparse the current file
     */
    bool PostReparseCommand(IEditor *editor);
    
    /**
     * @brief post a CC request at the current editor position
     */
    bool PostCCRequest(IEditor* editor);
    /**
     * @brief post a function calltip at a given position. pos is the first position
     * before the open brace
     */
    bool PostFunctionTipRequest(IEditor* editor, int pos);

    /**
     * @brief locate a definition of expression under the caret
     */
    bool PostFindDefinitionRequest(IEditor* editor);
    
    const wxCodeCompletionBoxEntry::Vec_t& GetEntries() const { return m_entries; }

    /**
     * @brief locate nodejs executable on this machine
     */
    static bool LocateNodeJS(wxFileName& nodeJS);
};

#endif // CLTERNSERVER_H
