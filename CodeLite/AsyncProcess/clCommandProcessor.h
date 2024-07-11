//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clCommandProcessor.h
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

#ifndef CLCOMMANDPROCESSOR_H
#define CLCOMMANDPROCESSOR_H

#include "asyncprocess.h"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <wx/event.h>
#include <wx/string.h>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_COMMAND_PROCESSOR_ENDED, clCommandEvent);

// A process printed output to stdout/err (the process output is accessible via event.GetString())
// The caller may handle this event, incase the process is waiting for input the caller
// may send its input via event.SetString()
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_COMMAND_PROCESSOR_OUTPUT, clCommandEvent);

class clCommandProcessor;
typedef bool (wxEvtHandler::*clCommandProcessorFunc)(const clCommandProcessor* processor);
class WXDLLIMPEXP_CL clCommandProcessor : public wxEvtHandler
{
    clCommandProcessor* m_next;
    clCommandProcessor* m_prev;
    IProcess* m_process;
    wxString m_command;
    wxString m_workingDirectory;
    size_t m_processFlags;
    wxString m_output;

    clCommandProcessorFunc m_postExecCallback;
    wxEvtHandler* m_obj;

protected:
    void DeleteChain();
    clCommandProcessor* GetFirst();
    clCommandProcessor* GetActiveProcess();

public:
    clCommandProcessor(const wxString& command, const wxString& wd, size_t processFlags = IProcessCreateDefault);
    virtual ~clCommandProcessor();

    void ExecuteCommand();

    const wxString& GetCommand() const { return m_command; }
    void SetCommand(const wxString& command) { this->m_command = command; }

    const wxString& GetOutput() const { return m_output; }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }

    void SetWorkingDirectory(const wxString& workingDirectory) { this->m_workingDirectory = workingDirectory; }
    void SetPrev(clCommandProcessor* prev) { this->m_prev = prev; }
    clCommandProcessor* GetPrev() { return m_prev; }
    clCommandProcessor* GetNext() { return m_next; }
    void SetNext(clCommandProcessor* next) { this->m_next = next; }

    IProcess* GetProcess() { return m_process; }
    /**
     * @brief set a callback to be called by the processor when the current command execution
     * is completed. The callback signature is:
     * bool Class::CallbackName(const clCommandProcessor* processor)
     * if the callback returns false, the execution of the rest of the chain is stopped
     */
    void SetPostExecCallback(wxEvtHandler* handler, clCommandProcessorFunc func)
    {
        m_obj = handler;
        m_postExecCallback = func;
    }

    /**
     * @brief link two command so they will be executed one after the other
     * @return a pointer to the next command ("next")
     */
    clCommandProcessor* Link(clCommandProcessor* next);

    /**
     * @brief terminate the process. If we are part of the chain, search the active process
     * within the chain and terminate it (this will cause the next process to start)
     */
    void Terminate();

    void OnProcessOutput(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);
};

#endif // CLCOMMANDPROCESSOR_H
