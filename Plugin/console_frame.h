//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : console_frame.h
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

#ifndef __console_frame__
#define __console_frame__

#include "clSSHChannel.h"
#include "cl_command_event.h"
#include "cl_ssh.h"
#include "codelite_exports.h"
#include "wxterminal.h"
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/frame.h>
#include <wx/gdicmn.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/string.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class ConsoleFrame
///////////////////////////////////////////////////////////////////////////////
class WXDLLIMPEXP_SDK ConsoleFrame : public wxFrame
{
protected:
    wxTerminal* m_terminal;
#if USE_SFTP
    clSSH::Ptr_t m_ssh;
    clSSHChannel::Ptr_t m_channel;
#endif

protected:
    void CreateGUIControls();
#if USE_SFTP
    void OnExecuteRemoteCommand(clCommandEvent& event);
    void OnChannelReadError(clCommandEvent& event);
    void OnChannelRead(clCommandEvent& event);
    void OnChannelPty(clCommandEvent& event);
    void OnChannelClosed(clCommandEvent& event);
#endif
    void OnExitWhenDone(clCommandEvent& event);
    void OnTerminalCtrlC(clCommandEvent& event);

public:
    ConsoleFrame(wxWindow* parent);
#if USE_SFTP
    ConsoleFrame(wxWindow* parent, clSSH::Ptr_t ssh);
#endif
    virtual ~ConsoleFrame();
    void Execute(const wxString& command, const wxString& wd);
};

#endif //__console_frame__
