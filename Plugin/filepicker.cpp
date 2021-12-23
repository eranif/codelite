//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : filepicker.cpp
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
#include "filepicker.h"

#include "macros.h"
#include "wx/sizer.h"

FilePicker::FilePicker(wxWindow* parent, wxWindowID id, const wxString& defaultFile, const wxString& message,
                       const wxString& wildCard, const wxString& buttonCaption, const wxPoint& pos, const wxSize& size,
                       long style)
    : wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL | wxNO_BORDER)
    , m_buttonCaption(buttonCaption)
    , m_dlgCaption(message)
    , m_dlgStyle(style)
    , m_defaultFile(defaultFile)
    , m_wildCard(wildCard)
{

#if defined(__WXGTK__) || defined(__WXMAC__)
    if(m_wildCard == wxT("*.*")) {
        m_wildCard = wxT("*");
    }
#endif

    CreateControls();
    ConnectEvents();
}

FilePicker::~FilePicker() {}

void FilePicker::CreateControls()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    SetSizer(mainSizer);

    size_t flags = wxRIGHT | wxTOP | wxBOTTOM | wxALIGN_CENTER | wxEXPAND;

    m_path = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
    mainSizer->Add(m_path, 1, flags, 5);

    m_button = new wxButton(this, wxID_ANY, m_buttonCaption);
    mainSizer->Add(m_button, 0, wxALL, 5);

    Layout();
}

void FilePicker::ConnectEvents() { ConnectButton(m_button, FilePicker::OnButtonClicked); }

void FilePicker::OnButtonClicked(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxFileDialog* dlg = new wxFileDialog(this, m_dlgCaption, wxEmptyString, m_defaultFile, m_wildCard, m_dlgStyle);
    if(dlg->ShowModal() == wxID_OK) {
        // Get the dirname
        wxString path = dlg->GetPath();
        m_path->SetValue(path);
    }
    dlg->Destroy();
}
