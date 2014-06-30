//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : wxcl_log_text_ctrl.cpp
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

#include "wxcl_log_text_ctrl.h"
#include <wx/settings.h>
#include "drawingutils.h"

wxclTextCtrl::wxclTextCtrl(wxTextCtrl* text)
    : m_pTextCtrl(text)
{
    DoInit();
}

wxclTextCtrl::~wxclTextCtrl()
{
}

void wxclTextCtrl::Reset()
{
    DoInit();
}

void wxclTextCtrl::DoInit()
{
    m_pTextCtrl->Clear();
    wxFont f = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    f.SetFamily(wxFONTFAMILY_TELETYPE);
    m_pTextCtrl->SetFont(f);
    m_pTextCtrl->SetBackgroundColour(DrawingUtils::GetOutputPaneBgColour());
    wxTextAttr defaultStyle;
    defaultStyle.SetBackgroundColour(DrawingUtils::GetOutputPaneBgColour());
    defaultStyle.SetTextColour(DrawingUtils::GetOutputPaneFgColour());
    defaultStyle.SetFont(f);
    m_pTextCtrl->SetDefaultStyle(defaultStyle);
}
