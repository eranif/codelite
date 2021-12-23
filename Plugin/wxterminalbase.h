//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : wxterminalbase.h
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

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __wxterminalbase__
#define __wxterminalbase__

#include <wx/colour.h>
#include <wx/font.h>
#include <wx/gdicmn.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stc/stc.h>
#include <wx/string.h>
#include <wx/textctrl.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class wxTerminalBase
///////////////////////////////////////////////////////////////////////////////
class wxTerminalBase : public wxPanel
{
protected:
    wxStyledTextCtrl* m_textCtrl;

protected:
    virtual void OnDown(wxKeyEvent& event) { event.Skip(); }
    virtual void OnUp(wxKeyEvent& event) { event.Skip(); }
    virtual void OnLeft(wxKeyEvent& event) { event.Skip(); }
    virtual void OnRight(wxKeyEvent& event) { event.Skip(); }
    virtual void OnEnter() {}
    virtual void OnCtrlC(wxKeyEvent& event) { event.Skip(); }
    virtual void OnKey(wxKeyEvent& event);
    void OnCharAdded(wxStyledTextEvent& event);
    void AddMarker();

public:
    void CaretToEnd();
    wxTerminalBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxSize(-1, -1), long style = wxTAB_TRAVERSAL);
    virtual ~wxTerminalBase();
};

#endif //__wxterminalbase__
