//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : buildprocess.h
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

#ifndef BUILDPROCESS_H
#define BUILDPROCESS_H

#include "asyncprocess.h"

class BuildProcess
{
    IProcess* m_process = nullptr;
    wxEvtHandler* m_evtHandler = nullptr;
    wxString m_fileName;

public:
    BuildProcess();
    virtual ~BuildProcess();

    bool Execute(const wxString& cmd, const wxString& fileName, const wxString& workingDirectory,
                 wxEvtHandler* evtHandler);
    void Stop();
    bool IsBusy();

    void SetFileName(const wxString& fileName) { this->m_fileName = fileName; }
    const wxString& GetFileName() const { return m_fileName; }

    int GetPid() const
    {
        if(m_process) {
            return m_process->GetPid();
        }
        return wxNOT_FOUND;
    }
};

#endif // BUILDPROCESS_H
