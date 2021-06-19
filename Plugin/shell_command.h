//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : compiler_action.h
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
#ifndef COMPILER_ACTION_H
#define COMPILER_ACTION_H

#include "cl_command_event.h"
#include "codelite_exports.h"
#include "project.h"
#include "queuecommand.h"
#include "wx/event.h"

class IManager;
class IProcess;

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BUILD_PROCESS_ADDLINE, clBuildEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BUILD_PROCESS_STARTED, clBuildEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BUILD_PROCESS_ENDED, clBuildEvent);

class WXDLLIMPEXP_SDK ShellCommand : public wxEvtHandler
{
private:
    IProcess* m_proc = nullptr;

protected:
    QueueCommand m_info;

protected:
    virtual void DoPrintOutput(const wxString& out);
    virtual void OnProcessOutput(clProcessEvent& e);
    virtual void OnProcessTerminated(clProcessEvent& e);

    void CleanUp();
    void DoSetWorkingDirectory(ProjectPtr proj, bool isCustom, bool isFileOnly);
    bool StartProcess(const wxString& cmd, size_t create_flags = IProcessCreateDefault | IProcessWrapInShell);

public:
    bool IsBusy() const { return m_proc != NULL; }

    void Stop();

    void SetInfo(const QueueCommand& info) { this->m_info = info; }
    const QueueCommand& GetInfo() const { return m_info; }

public:
    // construct a compiler action
    ShellCommand(const QueueCommand& buildInfo);

    virtual ~ShellCommand() {}
    virtual void Process(IManager* manager) = 0;

    void AppendLine(const wxString& line);
    void SendStartMsg(const wxString& toolchain);
    void SendEndMsg();
};
#endif
