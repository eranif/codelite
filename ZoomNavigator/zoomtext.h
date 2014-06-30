//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : zoomtext.h
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

//	Copyright: 2013 Brandon Captain
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.

#ifndef ZOOM_NAV_TEXT
#define ZOOM_NAV_TEXT

#include <wx/stc/stc.h>
#include "ieditor.h"

class ZoomText : public wxStyledTextCtrl
{
    int      m_zoomFactor;
    wxColour m_colour;
    wxString m_filename;
    
protected:
    void OnThemeChanged(wxCommandEvent &e);
    
public:
    ZoomText(wxWindow *parent, wxWindowID id=wxID_ANY,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize, long style = 0,
             const wxString& name = wxSTCNameStr);
    virtual ~ZoomText();
    void UpdateLexer(const wxString &filename);
    void OnSettingsChanged(wxCommandEvent &e);
    void UpdateText(IEditor* editort);
    void HighlightLines(int start, int end);
};

#endif // ZOOM_NAV_TEXT
