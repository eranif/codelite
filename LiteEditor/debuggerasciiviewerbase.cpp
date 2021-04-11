//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : debuggerasciiviewerbase.cpp
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
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "debuggerasciiviewerbase.h"

///////////////////////////////////////////////////////////////////////////

DebuggerAsciiViewerBase::DebuggerAsciiViewerBase(wxWindow* parent, wxWindowID id, const wxPoint& pos,
                                                 const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
{
    wxBoxSizer* mainSizer;
    mainSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* bSizer2;
    bSizer2 = new wxBoxSizer(wxHORIZONTAL);

    m_staticText1 = new wxStaticText(this, wxID_ANY, _("Displaying:"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText1->Wrap(-1);
    m_staticText1->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString));

    bSizer2->Add(m_staticText1, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    m_textCtrlExpression =
        new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_RICH2);
    m_textCtrlExpression->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

    bSizer2->Add(m_textCtrlExpression, 1, wxALL | wxEXPAND, 5);

    m_buttonClear = new wxButton(this, wxID_ANY, _("&Clear"), wxDefaultPosition, wxDefaultSize, 0);
    m_buttonClear->SetDefault();
    bSizer2->Add(m_buttonClear, 0, wxALL, 5);

    mainSizer->Add(bSizer2, 0, wxEXPAND, 5);

    m_textView = new wxStyledTextCtrl(this);
    mainSizer->Add(m_textView, 1, wxALL | wxEXPAND, 5);

    this->SetSizer(mainSizer);
    this->Layout();

    // Connect Events
    m_buttonClear->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DebuggerAsciiViewerBase::OnClearView),
                           NULL, this);
}

DebuggerAsciiViewerBase::~DebuggerAsciiViewerBase()
{
    // Disconnect Events
    m_buttonClear->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DebuggerAsciiViewerBase::OnClearView),
                              NULL, this);
}
