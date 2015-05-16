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

#include "project.h"
#include "queuecommand.h"
#include "wx/event.h"
#include "codelite_exports.h"
#include "cl_command_event.h"

class IManager;
class IProcess;

extern WXDLLIMPEXP_SDK const wxEventType wxEVT_SHELL_COMMAND_ADDLINE;
extern WXDLLIMPEXP_SDK const wxEventType wxEVT_SHELL_COMMAND_STARTED;
extern WXDLLIMPEXP_SDK const wxEventType wxEVT_SHELL_COMMAND_PROCESS_ENDED;
extern WXDLLIMPEXP_SDK const wxEventType wxEVT_SHELL_COMMAND_STARTED_NOCLEAN;

/**
 * @class BuildEventDetails
 * @brief associated data that (ClientData) which is sent with the
 * wxEVT_SHELL_COMMAND_STARTED and wxEVT_SHELL_COMMAND_STARTED_NOCLEAN events
 */
class WXDLLIMPEXP_SDK BuildEventDetails : public wxClientData
{
    wxString m_projectName;
    wxString m_configuration;
    bool m_isClean;
    bool m_isCustomProject;

public:
    BuildEventDetails() {}
    virtual ~BuildEventDetails() {}

    void SetConfiguration(const wxString& configuration) { this->m_configuration = configuration; }
    void SetProjectName(const wxString& projectName) { this->m_projectName = projectName; }
    const wxString& GetConfiguration() const { return m_configuration; }
    const wxString& GetProjectName() const { return m_projectName; }
    void SetIsClean(bool isClean) { this->m_isClean = isClean; }
    void SetIsCustomProject(bool isCustomProject) { this->m_isCustomProject = isCustomProject; }
    bool IsClean() const { return m_isClean; }
    bool IsCustomProject() const { return m_isCustomProject; }
};

class WXDLLIMPEXP_SDK ShellCommand : public wxEvtHandler
{
protected:
    IProcess* m_proc;
    QueueCommand m_info;

protected:
    virtual void DoPrintOutput(const wxString& out);
    virtual void OnProcessOutput(clProcessEvent& e);
    virtual void OnProcessTerminated(clProcessEvent& e);

    void CleanUp();
    void DoSetWorkingDirectory(ProjectPtr proj, bool isCustom, bool isFileOnly);

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
    void SendStartMsg();
    void SendEndMsg();
};
#endif
