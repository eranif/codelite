//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : dirpicker.cpp
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
#include "dirpicker.h"

#include "file_logger.h"
#include "macros.h"
#include "wx/dir.h"
#include "wx/filename.h"
#include "wx/log.h"
#include "wx/sizer.h"

#include <wx/dirdlg.h>

BEGIN_EVENT_TABLE(DirPicker, wxPanel)
EVT_TEXT(wxID_ANY, DirPicker::OnText)
END_EVENT_TABLE()

DirPicker::DirPicker(wxWindow* parent, wxWindowID id, const wxString& buttonCaption, const wxString& defaultPos,
                     const wxString& message, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL | wxNO_BORDER)
    , m_path(NULL)
    , m_combo(NULL)
    , m_buttonCaption(buttonCaption)
    , m_dlgCaption(message)
    , m_style(style)
    , m_defaultPos(defaultPos)
{
    CreateControls();
    ConnectEvents();
}

DirPicker::~DirPicker() {}

void DirPicker::OnText(wxCommandEvent& event) { GetParent()->GetEventHandler()->ProcessEvent(event); }

void DirPicker::CreateControls()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    SetSizer(mainSizer);

    size_t flags = wxRIGHT | wxTOP | wxBOTTOM | wxEXPAND;

    if(m_style & wxDP_USE_TEXTCTRL) {
        m_path = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
        mainSizer->Add(m_path, 1, flags, 5);
    } else {
        m_combo = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
        mainSizer->Add(m_combo, 1, flags, 5);
    }

    m_button = new wxButton(this, wxID_ANY, m_buttonCaption, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    m_button->SetToolTip(_("Browse for folder..."));
    mainSizer->Add(m_button, 0, wxEXPAND | wxALL, 5);

    Layout();
}

void DirPicker::ConnectEvents() { ConnectButton(m_button, DirPicker::OnButtonClicked); }

void DirPicker::OnButtonClicked(wxCommandEvent& event)
{
    wxUnusedVar(event);

    // no default position was provided by the user
    // try to take it from the text control
    wxString work_dir = GetPath();
    m_defaultPos.Empty();
    if(work_dir.IsEmpty() == false) {
        if(wxDir::Exists(work_dir)) {
            wxFileName fn(work_dir, wxEmptyString);
            clLogMessage(work_dir + wxT(" exists"));
            fn.MakeAbsolute();
            m_defaultPos = fn.GetFullPath();
        }
    }

    if(m_defaultPos.IsEmpty()) {
        m_defaultPos = wxGetCwd();
    }

    clLogMessage(wxT("setting working dir to : ") + m_defaultPos);

    wxDirDialog* dlg = new wxDirDialog(this, m_dlgCaption, m_defaultPos);
    if(dlg->ShowModal() == wxID_OK) {
        // Get the dirname
        wxString path = dlg->GetPath();
        SetPath(path);
    }
    dlg->Destroy();
}

wxString DirPicker::GetPath()
{
    if(m_style & wxDP_USE_TEXTCTRL)
        return m_path->GetValue();
    else {
        wxString path(m_combo->GetValue());
        // if the path is not whitin the combobox list, add it
        if(m_combo->FindString(path) == wxNOT_FOUND) {
            SetPath(path);
        }
        return path;
    }
}

void DirPicker::SetPath(const wxString& path)
{
    if(m_style & wxDP_USE_TEXTCTRL)
        m_path->SetValue(path);
    else {
        int where = m_combo->FindString(path);
        if(where != wxNOT_FOUND) {
            m_combo->SetSelection(where);
        } else {
            where = m_combo->Append(path);
            m_combo->SetSelection(where);
        }
    }
}

void DirPicker::SetValues(const wxArrayString& values, int sel)
{
    wxASSERT_MSG(m_style & wxDP_USE_COMBOBOX, wxT("SetValues is available only for wxDP_USE_COMBOBOX style"));
    m_combo->Append(values);
    m_combo->SetSelection(sel);
}

wxArrayString DirPicker::GetValues() const
{
    wxArrayString dummy;
    if(m_style & wxDP_USE_COMBOBOX) {
        return m_combo->GetStrings();
    } else {
        return dummy;
    }
}

int DirPicker::GetCurrentSelection() const
{
    wxCHECK_MSG(m_style & wxDP_USE_COMBOBOX, wxNOT_FOUND,
                wxT("GetCurrentSelection is available only for wxDP_USE_COMBOBOX style"));

    int index = wxNOT_FOUND;
    wxString stringsel = m_combo->GetValue();
    if(!stringsel.empty()) {
        index = m_combo->FindString(stringsel);
    }

    return index;
}
