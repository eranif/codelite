//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : buildprocess.cpp
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

#include "buildprocess.h"

BuildProcess::BuildProcess()
    : m_process(NULL)
    , m_evtHandler(NULL)
{
}

BuildProcess::~BuildProcess() { Stop(); }

bool BuildProcess::Execute(const wxString& cmd, const wxString& fileName, const wxString& workingDirectory,
                           wxEvtHandler* evtHandler)
{
    if(m_process) {
        return false;
    }
    m_process = ::CreateAsyncProcess(evtHandler, cmd, IProcessCreateDefault | IProcessWrapInShell, workingDirectory);
    if(!m_process)
        return false;

    SetFileName(fileName);
    return true;
}

void BuildProcess::Stop()
{
    if(m_process) {
        delete m_process;
        m_process = NULL;
    }
    m_fileName.Clear();
}

bool BuildProcess::IsBusy() { return m_process != NULL; }
