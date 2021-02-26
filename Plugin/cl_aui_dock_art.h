//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : cl_aui_dock_art.h
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

#ifndef CLAUIDOCKART_H
#define CLAUIDOCKART_H

#include "codelite_exports.h"
#include "imanager.h"
#include <wx/aui/auibar.h>
#include <wx/aui/dockart.h>
#include <wx/aui/framemanager.h>
#include <wx/dc.h>
#include <wx/gdicmn.h>
#include <wx/window.h>

class WXDLLIMPEXP_SDK clAuiDockArt : public wxEvtHandler, public wxAuiDefaultDockArt
{
    IManager* m_manager;
    wxColour m_captionColour;
    wxColour m_captionTextColour;
    bool m_useCustomCaptionColour;
    bool m_useCustomBgColour = false;
    wxColour m_bgColour;

public:
    void OnSettingsChanged(clCommandEvent& event);

public:
    clAuiDockArt(IManager* manager);
    virtual ~clAuiDockArt();

    virtual void DrawBorder(wxDC& dc, wxWindow* window, const wxRect& rect, wxAuiPaneInfo& pane);
    virtual void DrawCaption(wxDC& dc, wxWindow* window, const wxString& text, const wxRect& rect, wxAuiPaneInfo& pane);
    virtual void DrawPaneButton(wxDC& dc, wxWindow* window, int button, int button_state, const wxRect& _rect,
                                wxAuiPaneInfo& pane);
    virtual void DrawBackground(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect);
    virtual void DrawSash(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect);
};

#endif // CLAUIDOCKART_H
