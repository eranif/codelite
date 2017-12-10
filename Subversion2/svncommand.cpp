//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : svncommand.cpp
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

#include "svncommand.h"
#include "environmentconfig.h"
#include "subversion_strings.h"
#include "svn_console.h"
#include "globals.h"
#include "subversion2.h"
#include <wx/aui/framemanager.h>
#include "imanager.h"
#include "file_logger.h"

SvnCommand::SvnCommand(Subversion2* plugin)
    : m_process(NULL)
    , m_handler(NULL)
    , m_plugin(plugin)
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &SvnCommand::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &SvnCommand::OnProcessTerminated, this);
}

SvnCommand::~SvnCommand() { ClearAll(); }

bool SvnCommand::Execute(const wxString& command,
                         const wxString& workingDirectory,
                         SvnCommandHandler* handler,
                         Subversion2* plugin)
{
    // Dont run 2 commands at the same time
    if(m_process) {
        if(handler) {
            // handler->GetPlugin()->GetShell()->AppendText(svnANOTHER_PROCESS_RUNNING);
            delete handler;
        }
        return false;
    }

    ClearAll();

    // Wrap the command in the OS Shell
    wxString cmdShell(command);
    WrapInShell(cmdShell);

    // Apply the environment variables before executing the command
    wxStringMap_t om;
    om.insert(std::make_pair("LC_ALL", "C"));

    bool useOverrideMap = m_plugin->GetSettings().GetFlags() & SvnUsePosixLocale;
    EnvSetter env(m_plugin->GetManager()->GetEnv(), useOverrideMap ? &om : NULL);

    m_process = CreateAsyncProcess(this, command, IProcessCreateDefault, workingDirectory);
    if(!m_process) {
        return false;
    }
    m_workingDirectory = workingDirectory.c_str();
    m_command = command.c_str();
    m_handler = handler;
    return true;
}

void SvnCommand::OnProcessOutput(clProcessEvent& event)
{
    m_output.Append(event.GetOutput());
    clDEBUG1() << "Subversion:" << m_output << clEndl;
}

void SvnCommand::OnProcessTerminated(clProcessEvent& event)
{
    if(m_handler) {
        clDEBUG1() << "Subversion:" << m_output << clEndl;;
        if(m_handler->TestLoginRequired(m_output)) {
            // re-issue the last command but this time with login dialog
            m_handler->GetPlugin()->GetConsole()->AppendText(_("Authentication failed. Retrying...\n"));
            m_handler->ProcessLoginRequired(m_workingDirectory);

        } else if(m_handler->TestVerificationFailed(m_output)) {
            m_handler->GetPlugin()->GetConsole()->AppendText(
                _("Server certificate verification failed. Retrying...\n"));
            m_handler->ProcessVerificationRequired();

        } else {
            // command ended successfully, invoke the "success" callback
            m_handler->Process(m_output);
        }

        delete m_handler;
        m_handler = NULL;
    }

    if(m_process) {
        delete m_process;
        m_process = NULL;
    }
}

void SvnCommand::ClearAll()
{
    m_workingDirectory.Clear();
    m_command.Clear();
    m_output.Clear();
}
