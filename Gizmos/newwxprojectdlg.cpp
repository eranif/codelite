//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : newwxprojectdlg.cpp
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
#include "newwxprojectdlg.h"

#include "imanager.h"
#include "windowattrmanager.h"
#include "workspace.h"

#include <wx/msgdlg.h>
#include <wx/xrc/xmlres.h>

NewWxProjectDlg::NewWxProjectDlg(wxWindow* parent, IManager* mgr)
    : NewWxProjectBaseDlg(parent)
    , m_mgr(mgr)
{
    m_bitmap1->SetBitmap(wxXmlResource::Get()->LoadBitmap(wxT("wx_project_header")));
    m_choiceApplicationType->SetSelection(wxProjectTypeSimpleMain);
    m_dirPicker->SetPath(
        m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR));
    const wxString VersionChoices[] = { wxTRANSLATE("Default"), wxT("2.6"), wxT("2.8"), wxT("2.9"), wxT("3.0") };
    m_stringManager.AddStrings(
        sizeof(VersionChoices) / sizeof(wxString), VersionChoices, wxT("Default"), m_choiceVersion);

    m_textCtrlName->SetFocus();

#if defined(__WXMSW__)
    m_checkBoxMWindows->SetValue(true);
    m_checkBoxWinRes->SetValue(true);
#else
    m_checkBoxMWindows->SetValue(false);
    m_checkBoxMWindows->Enable(false);
    m_checkBoxWinRes->SetValue(false);
    m_checkBoxWinRes->Enable(false);
#endif
    SetName("NewWxProjectDlg");
    WindowAttrManager::Load(this);
}

NewWxProjectDlg::~NewWxProjectDlg() {}

void NewWxProjectDlg::OnButtonCancel(wxCommandEvent& e) { EndModal(wxID_CANCEL); }

void NewWxProjectDlg::OnButtonOK(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if(ValidateInput()) {
        EndModal(wxID_OK);
    }
}

bool NewWxProjectDlg::ValidateInput()
{
    if(m_textCtrlName->GetValue().IsEmpty()) {
        wxString msg;
        msg << _("Invalid project name '") << m_textCtrlName->GetValue() << wxT("'\n");
        msg << _("Valid characters for project name are [0-9A-Za-z_]");
        wxMessageBox(msg, _("CodeLite"), wxICON_WARNING | wxOK);
        return false;
    }

    if(m_textCtrlName->GetValue()
           .find_first_not_of(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_")) !=
       wxString::npos) {
        wxString msg;
        msg << _("Invalid project name '") << m_textCtrlName->GetValue() << wxT("'\n");
        msg << _("Valid characters for project name are [0-9A-Za-z_]");
        wxMessageBox(msg, _("CodeLite"), wxICON_WARNING | wxOK);
        return false;
    }

    wxString path = m_dirPicker->GetPath();

    if(m_checkBoxCreateSeparateDir->IsChecked()) {
        path << wxFileName::GetPathSeparator();
        path << m_textCtrlName->GetValue();
    }

    wxFileName::Mkdir(path, 0777, wxPATH_MKDIR_FULL);
    if(!wxDirExists(path)) {
        wxMessageBox(
            wxString::Format(_("Failed to create the path: %s\nA permissions problem, perhaps?"), path.c_str()),
            _("Error"),
            wxOK | wxICON_HAND);
        return false;
    }

    return true;
}

void NewWxProjectDlg::GetProjectInfo(NewWxProjectInfo& info)
{
    size_t flag = 0;
    wxString path = m_dirPicker->GetPath();

    if(m_checkBoxMWindows->IsChecked()) {
        flag |= wxWidgetsSetMWindows;
    }

    if(m_checkBoxWinRes->IsChecked()) {
        flag |= wxWidgetsWinRes;
    }

    if(m_checkBoxUnicode->IsChecked()) {
        flag |= wxWidgetsUnicode;
    }

    if(m_checkBoxStatic->IsChecked()) {
        flag |= wxWidgetsStatic;
    }

    if(m_checkBoxUniversal->IsChecked()) {
        flag |= wxWidgetsUniversal;
    }

    if(m_checkBoxPCH->IsChecked()) {
        flag |= wxWidgetsPCH;
    }

    if(m_checkBoxCreateSeparateDir->IsChecked()) {
        path << wxFileName::GetPathSeparator();
        path << m_textCtrlName->GetValue();
    }

    info.SetFlags(flag);
    info.SetType(m_choiceApplicationType->GetSelection());
    info.SetName(m_textCtrlName->GetValue());
    info.SetPath(path);
    info.SetPrefix(m_textCtrlPrefix->GetValue());
    info.SetVersion(m_stringManager.GetStringSelection());
}

void NewWxProjectDlg::OnChoiceChanged(wxCommandEvent& e)
{
#ifdef __WXMSW__
    if(e.GetSelection() == wxProjectTypeSimpleMain) {
        m_checkBoxMWindows->SetValue(false);
        m_checkBoxMWindows->Enable(false);
    } else {
        m_checkBoxMWindows->Enable(true);
    }
#else
    wxUnusedVar(e);
#endif
}
