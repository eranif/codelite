//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : async_executable_cmd.cpp
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
#include "async_executable_cmd.h"

#include "ZombieReaperPOSIX.h"
#include "event_notifier.h"
#include "file_logger.h"

#include <wx/tokenzr.h>
#include <wx/xrc/xmlres.h>

const wxEventType wxEVT_ASYNC_PROC_ADDLINE = XRCID("async_proc_addline");
const wxEventType wxEVT_ASYNC_PROC_ADDERRLINE = XRCID("async_proc_adderrline");
const wxEventType wxEVT_ASYNC_PROC_STARTED = XRCID("async_proc_started");
const wxEventType wxEVT_ASYNC_PROC_ENDED = XRCID("async_proc_ended");

static int AsyncExeTimerID = wxNewId();

BEGIN_EVENT_TABLE(AsyncExeCmd, wxEvtHandler)
EVT_TIMER(AsyncExeTimerID, AsyncExeCmd::OnTimer)
END_EVENT_TABLE()

AsyncExeCmd::AsyncExeCmd(wxEvtHandler* owner)
    : m_proc(NULL)
    , m_owner(owner)
    , m_busy(false)
    , m_stop(false)
{
#ifndef __WXMSW__
    EventNotifier::Get()->Bind(wxEVT_CL_PROCESS_TERMINATED, &AsyncExeCmd::OnZombieReaperProcessTerminated, this);
#endif
    m_timer = new wxTimer(this, AsyncExeTimerID);
}

AsyncExeCmd::~AsyncExeCmd()
{
#ifndef __WXMSW__
    EventNotifier::Get()->Unbind(wxEVT_CL_PROCESS_TERMINATED, &AsyncExeCmd::OnZombieReaperProcessTerminated, this);
#endif
    wxDELETE(m_timer);
    wxDELETE(m_proc);
};

void AsyncExeCmd::AppendLine(const wxString& line, bool isErr)
{
    if(!m_owner)
        return;

    wxCommandEvent event(isErr ? wxEVT_ASYNC_PROC_ADDERRLINE : wxEVT_ASYNC_PROC_ADDLINE);
    event.SetEventObject(this);
    event.SetString(line);
    m_owner->ProcessEvent(event);
}

void AsyncExeCmd::Stop()
{
    m_stop = true;
    // kill the build process
    if(m_proc) {
        m_proc->Terminate();
    }
}

void AsyncExeCmd::SendStartMsg()
{
    if(!m_owner)
        return;

    wxCommandEvent event(wxEVT_ASYNC_PROC_STARTED);
    event.SetEventObject(this);

    wxString msg;
    msg << _("Current working directory: ") << wxGetCwd() << wxT("\n");
    msg << _("Running program: ") << m_cmdLine << wxT("\n");
    event.SetString(msg);
    m_owner->ProcessEvent(event);
}

void AsyncExeCmd::SendEndMsg(int exitCode)
{
    if(!m_owner)
        return;

    wxCommandEvent event(wxEVT_ASYNC_PROC_ENDED);
    event.SetEventObject(this);
    wxString message(_("Program exited with return code: "));
    message << exitCode << wxT("\n");
    event.SetString(message);
    m_owner->ProcessEvent(event);
}

void AsyncExeCmd::OnTimer(wxTimerEvent& event)
{
    wxUnusedVar(event);
    if(m_stop) {
        m_proc->Terminate();
        return;
    }
    PrintOutput();
}

void AsyncExeCmd::PrintOutput()
{
    if(m_proc->GetRedirect()) {
        wxString data, errors;
        m_proc->HasInput(data, errors);
        DoPrintOutput(data, errors);
    }
}

void AsyncExeCmd::DoPrintOutput(const wxString& out, const wxString& err)
{
    if(!out.IsEmpty()) {
        wxStringTokenizer tt(out, wxT("\n"));
        while(tt.HasMoreTokens()) {
            AppendLine(tt.NextToken() + wxT("\n"));
        }
    }

    if(!err.IsEmpty()) {
        wxStringTokenizer tt(err, wxT("\n"));
        while(tt.HasMoreTokens()) {
            AppendLine(tt.NextToken() + wxT("\n"), true);
        }
    }
}

void AsyncExeCmd::ProcessEnd(wxProcessEvent& event)
{
    // read all input before stopping the timer
    if(!m_stop) {
        if(m_proc->GetRedirect()) {
            wxString err;
            wxString out;
            m_proc->ReadAll(out, err);
            DoPrintOutput(out, err);
            out.Empty();
            err.Empty();
        }
    }

    // stop the timer if needed
    if(m_proc->GetRedirect()) {
        m_timer->Stop();
    }

    m_busy = false;
    m_stop = false;

    SendEndMsg(event.GetExitCode());
}

void AsyncExeCmd::Execute(const wxString& cmdLine, bool hide, bool redirect)
{
    m_cmdLine = cmdLine;
    SetBusy(true);
    SendStartMsg();

    m_proc = new clProcess(wxNewId(), m_cmdLine, redirect);
    if(m_proc) {
        if(m_proc->Start(hide) == 0) {
            wxDELETE(m_proc);
            SetBusy(false);

        } else {
            // process started successfully, start timer if needed
            if(m_proc->GetRedirect()) {
                m_timer->Start(10);
            }
        }
    }
}

void AsyncExeCmd::Terminate() { m_proc->Terminate(); }

#ifndef __WXMSW__
void AsyncExeCmd::OnZombieReaperProcessTerminated(wxProcessEvent& event)
{
    CL_DEBUG("AsyncExeCmd: process %d terminated. (reported by OnZombieReaperProcessTerminated)", event.GetPid());
    event.Skip();
    if(m_proc && (event.GetPid() == m_proc->GetPid())) {
        CL_DEBUG("AsyncExeCmd: this is our process! performing cleanup");
        ProcessEnd(event);
        event.Skip(false);
    }
}
#endif
