//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : svncommandhandler.h
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

#ifndef SVNCOMMANDHANDLER_H
#define SVNCOMMANDHANDLER_H

#include <wx/string.h>
#include <wx/event.h>
#include <wx/log.h>
#include "file_logger.h"

class Subversion2;
class IManager;
class IProcess;

#define LOGIN_REQUIRES 1253
#define INTERACTIVE_MODE 1254
#define LOGIN_REQUIRES_URL 1255

class SvnCommandHandler
{
protected:
    Subversion2* m_plugin;
    int m_commandId;
    wxEvtHandler* m_owner;

public:
    SvnCommandHandler(Subversion2* plugin, int commandId, wxEvtHandler* owner)
        : m_plugin(plugin)
        , m_commandId(commandId)
        , m_owner(owner)
    {
    }

    virtual ~SvnCommandHandler() {}

    Subversion2* GetPlugin() { return m_plugin; }

    void SetCommandId(int commandId) { this->m_commandId = commandId; }

    void SetOwner(wxEvtHandler* owner) { this->m_owner = owner; }

    int GetCommandId() const { return m_commandId; }

    wxEvtHandler* GetOwner() { return m_owner; }

    bool TestLoginRequired(const wxString& output)
    {

        wxString svnOutput(output);
        svnOutput.MakeLower();
        if(svnOutput.Contains(wxT("could not authenticate to server")) ||
           svnOutput.Contains(wxT(": authorization failed"))) {
            // failed to login...
            return true;
        }
        return false;
    }

    bool TestVerificationFailed(const wxString& output)
    {
        wxString svnOutput(output);
        svnOutput.MakeLower();
        if(svnOutput.Contains(wxT("certificate verification failed"))) {
            // failed to login...
            return true;
        }
        return false;
    }

    // "(R)eject, accept (t)emporarily or accept (p)ermanently"
    // password for 'login' gnome keyring
    bool TestInteractiveVerification(const wxString& output)
    {
        wxString svnOutput(output);
        svnOutput.MakeLower();
        if(svnOutput.Contains(wxT("(r)eject, accept (t)emporarily or accept (p)ermanently"))) {
            // failed to login...
            return true;
        }
        return false;
    }

    /**
     * @brief handle here process output (e.g. interactive commands that needs response)
     * @param process the process
     * @param output  the process output
     */
    virtual void OnProcessOutput(IProcess* process, const wxString& output);

    /**
     * @brief the svn operation failed due to login error. Retry the last command but this
     * time, pop the login dialog
     */
    virtual void ProcessLoginRequired(const wxString& workingDirectory)
    {
        if((m_commandId != wxNOT_FOUND) && m_owner) {
            int eventId(m_commandId);
            wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, eventId);

            // set the command
            event.SetInt(LOGIN_REQUIRES);

            // set the working directory where the svn was invoked from
            event.SetString(workingDirectory);
            m_owner->AddPendingEvent(event);
        }
    }

    /**
     * @brief the svn operation failed due to login error. Retry the last command but this
     * time, pop the login dialog
     */
    virtual void ProcessLoginRequiredForURL(const wxString& url)
    {
        if(m_commandId != wxNOT_FOUND && m_owner) {
            int eventId(m_commandId);
            wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, eventId);

            // set the command
            event.SetInt(LOGIN_REQUIRES_URL);

            // set the working directory where the svn was invoked from
            event.SetString(url);
            m_owner->AddPendingEvent(event);

        } else {
            if(m_commandId == wxNOT_FOUND) {
                clLogMessage(wxT("svn: ProcessLoginRequired: passed m_commandId = wxNOT_FOUND"));

            } else if(m_owner == NULL) {
                clLogMessage(wxT("svn: ProcessLoginRequired: passed NULL m_owner"));
            }
        }
    }

    /**
     * @brief the svn operation failed due to server certificate errorlogin error. Retry the last command but this
     * time, pop the login dialog
     */
    virtual void ProcessVerificationRequired()
    {
        if(m_commandId != wxNOT_FOUND && m_owner) {
            int eventId(m_commandId);
            wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, eventId);
            event.SetInt(INTERACTIVE_MODE);
            m_owner->AddPendingEvent(event);
        }
    }

    virtual void Process(const wxString& output) = 0;
};

#endif // SVNCOMMANDHANDLER_H
