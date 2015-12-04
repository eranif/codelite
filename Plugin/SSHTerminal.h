//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : SSHTerminal.h
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

#ifndef SSHTERMINAL_H
#define SSHTERMINAL_H

#include "sftp_ui.h"
#if USE_SFTP
#include "cl_command_event.h"
#include "cl_ssh.h"

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_SSH_TERMINAL_CLOSING, clCommandEvent);
class WXDLLIMPEXP_SDK SSHTerminal : public SSHTerminalBase
{
    clSSH::Ptr_t m_ssh;
    
    DECLARE_EVENT_TABLE()
    
    void OnSshOutput(clCommandEvent &event);
    void OnSshCommandDoneWithError(clCommandEvent &event);
    void OnSshCommandDone(clCommandEvent &event);
    
public:
    SSHTerminal(wxWindow* parent, clSSH::Ptr_t ssh);
    virtual ~SSHTerminal();
    void Clear();
    void AppendText(const wxString &text);
    
protected:
    virtual void OnClear(wxCommandEvent& event);
    virtual void OnClearUI(wxUpdateUIEvent& event);
    virtual void OnSendCommand(wxCommandEvent& event);
};
#endif
#endif // SSHTERMINAL_H
