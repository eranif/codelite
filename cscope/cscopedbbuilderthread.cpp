//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cscopedbbuilderthread.cpp
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
#include "cscope.h"
#include "cscopedbbuilderthread.h"
#include "cscopestatusmessage.h"
#include "dirsaver.h"
#include "file_logger.h"
#include "procutils.h"
#include "wx/filefn.h"

int wxEVT_CSCOPE_THREAD_DONE = wxNewId();
int wxEVT_CSCOPE_THREAD_UPDATE_STATUS = wxNewId();

CscopeDbBuilderThread::CscopeDbBuilderThread() {}

CscopeDbBuilderThread::~CscopeDbBuilderThread() {}

void CscopeDbBuilderThread::ProcessRequest(ThreadRequest* request)
{
    CscopeRequest* req = (CscopeRequest*)request;

    // change dir to the workspace directory
    DirSaver ds;

    wxSetWorkingDirectory(req->GetWorkingDir());
    SendStatusEvent(_("Executing cscope..."), 10, req->GetFindWhat(), req->GetOwner());

    // notify the database creation process as completed
    wxArrayString output;

    // set environment variables required by cscope
    wxSetEnv(wxT("TMPDIR"), wxFileName::GetTempDir());
    clDEBUG() << "CScope:" << req->GetCmd() << clEndl;
    ProcUtils::SafeExecuteCommand(req->GetCmd(), output);
    SendStatusEvent(_("Parsing results..."), 50, wxEmptyString, req->GetOwner());
    clDEBUG1() << "CScope:\n" << output << clEndl;
    CScopeResultTable_t* result = ParseResults(output);
    SendStatusEvent(_("Done"), 100, wxEmptyString, req->GetOwner());

    // send status message
    SendStatusEvent(req->GetEndMsg(), 100, wxEmptyString, req->GetOwner());

    // send the results
    wxCommandEvent e(wxEVT_CSCOPE_THREAD_DONE);
    e.SetClientData(result);
    req->GetOwner()->AddPendingEvent(e);
}

CScopeResultTable_t* CscopeDbBuilderThread::ParseResults(const wxArrayString& output)
{
    CScopeResultTable_t* results = new CScopeResultTable_t();
    for(size_t i = 0; i < output.GetCount(); i++) {
        // parse each line
        wxString line = output.Item(i);
        CscopeEntryData data;

        // first is the file name
        line = line.Trim().Trim(false);
        // skip errors
        if(line.StartsWith(wxT("cscope:"))) { continue; }

        wxString file = line.BeforeFirst(wxT(' '));
        data.SetFile(file);
        line = line.AfterFirst(wxT(' '));

        // next is the scope
        line = line.Trim().Trim(false);
        wxString scope = line.BeforeFirst(wxT(' '));
        line = line.AfterFirst(wxT(' '));
        data.SetScope(scope);

        // next is the line number
        line = line.Trim().Trim(false);
        long nn;
        wxString line_number = line.BeforeFirst(wxT(' '));
        line_number.ToLong(&nn);
        data.SetLine(nn);
        line = line.AfterFirst(wxT(' '));

        // the rest is the pattern
        wxString pattern = line;
        data.SetPattern(pattern);

        // insert the result
        CScopeResultTable_t::const_iterator iter = results->find(data.GetFile());
        std::vector<CscopeEntryData>* vec(NULL);
        if(iter != results->end()) {
            // this file already exist, append the result
            vec = iter->second;
        } else {
            vec = new std::vector<CscopeEntryData>();
            // add it to the map
            (*results)[data.GetFile()] = vec;
        }
        vec->push_back(data);
    }
    return results;
}

void CscopeDbBuilderThread::SendStatusEvent(const wxString& msg, int percent, const wxString& findWhat,
                                            wxEvtHandler* owner)
{
    wxCommandEvent e(wxEVT_CSCOPE_THREAD_UPDATE_STATUS);
    CScopeStatusMessage* statusMsg = new CScopeStatusMessage();
    statusMsg->SetMessage(msg);
    statusMsg->SetPercentage(percent);
    statusMsg->SetFindWhat(findWhat);
    e.SetClientData(statusMsg);

    owner->AddPendingEvent(e);
}
