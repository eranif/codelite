//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : progress_dialog.cpp
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

#include "progress_dialog.h"
#if wxUSE_GUI
#ifdef __WXMSW__

clProgressDlg::clProgressDlg(wxWindow* parent, const wxString& title, const wxString& msg, int maxValue)
    : clProgressDlgBase(parent)
{
    SetSizeHints(600, -1);
    SetTitle(title);
    m_staticLabel->SetLabel(msg);
    m_gauge->SetRange(maxValue);
    GetSizer()->Fit(this);
    CenterOnParent();
    Show();
    wxYieldIfNeeded();
}

clProgressDlg::~clProgressDlg() {}

bool clProgressDlg::Update(int value, const wxString& msg)
{
    m_staticLabel->SetLabel(msg);
    m_gauge->SetValue(value);

    if(value % 20 == 0) { wxSafeYield(this, true); }
    return true;
}

bool clProgressDlg::Pulse(const wxString& msg)
{
    int curval = m_gauge->GetValue();
    m_gauge->SetValue(curval + 1);
    if(!msg.IsEmpty()) m_staticLabel->SetLabel(msg);
    return true;
}

#else  // UNIX

clProgressDlg::clProgressDlg(wxWindow* parent, const wxString& title, const wxString& msg, int maxValue)
    : wxProgressDialog(title, msg, maxValue, NULL, wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_AUTO_HIDE)
{
    SetSizeHints(400, -1);
    GetSizer()->Fit(this);
    CenterOnScreen();
}

clProgressDlg::~clProgressDlg() {}

bool clProgressDlg::Update(int value, const wxString& msg) { return wxProgressDialog::Update(value, msg, NULL); }

bool clProgressDlg::Pulse(const wxString& msg) { return wxProgressDialog::Pulse(msg, NULL); }
#endif // #if wxUSE_GUI
#endif
