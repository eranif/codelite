//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clAuiCaptionEnabler.h
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

#ifndef CLAUICAPTIONENABLER_H
#define CLAUICAPTIONENABLER_H

#include "codelite_exports.h"

#include <wx/aui/framemanager.h>
#include <wx/event.h>
#include <wx/timer.h>

class WXDLLIMPEXP_SDK clAuiCaptionEnabler : public wxEvtHandler
{
    wxAuiManager* m_aui;
    wxWindow* m_window;
    wxTimer* m_timer;
    bool m_captionAdded;
    wxString m_caption;
    bool m_ensureCaptionsVisible;

protected:
    void OnTimer(wxTimerEvent& event);
    void OnEditorSettingsChanged(wxCommandEvent& event);

public:
    void Initialize(wxWindow* window, const wxString& windowCaption, wxAuiManager* aui);
    clAuiCaptionEnabler();
    virtual ~clAuiCaptionEnabler();
};

#endif // CLAUICAPTIONENABLER_H
