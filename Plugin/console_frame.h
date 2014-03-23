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

#include <wx/intl.h>

#include <wx/sizer.h>
#include "wxterminal.h"
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/frame.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include "codelite_exports.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ConsoleFrame
///////////////////////////////////////////////////////////////////////////////
class IManager;
class WXDLLIMPEXP_SDK ConsoleFrame : public wxPanel
{
private:
    IManager* m_manager;
    wxTerminal *m_terminal;

protected:

    // Virtual event handlers, overide them in your derived class
    virtual void OnDebuggerEnded(wxCommandEvent &e);
    
public:
    ConsoleFrame(wxWindow* parent, IManager* manager, wxWindowID id = wxNOT_FOUND);
    ~ConsoleFrame();
    wxString StartTTY();
};

#endif //__console_frame__
