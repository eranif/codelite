//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : newworkspacedlg.cpp
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
#include "cl_config.h"
#include "cl_standard_paths.h"
#include "globals.h"
#include "newworkspacedlg.h"
#include "windowattrmanager.h"
#include "wx/dirdlg.h"
#include "wx/filename.h"
#include "wx/msgdlg.h"

NewWorkspaceDlg::NewWorkspaceDlg(wxWindow* parent)
    : NewWorkspaceBase(parent)
{
    wxArrayString history;
    history = clConfig::Get().Read("C++NewWorkspace/Paths", history);

    wxString defaultPath =
        clConfig::Get().Read("C++NewWorkspace/DefaultLocation", clStandardPaths::Get().GetDocumentsDir());
    m_comboBoxPath->SetValue(defaultPath);
    m_comboBoxPath->Append(history);

    m_textCtrlWorkspaceName->SetFocus();
    ::clSetSmallDialogBestSizeAndPosition(this);
}

NewWorkspaceDlg::~NewWorkspaceDlg()
{
    // store the recent locations, we keep up to 20 locations
    wxArrayString history = m_comboBoxPath->GetStrings();
    history.Insert(m_comboBoxPath->GetValue(),
                   0); // Place the current value at the top so we make sure it gets stored in the history
    wxArrayString uniqueArr;
    for(size_t i = 0; i < history.size(); ++i) {
        if(uniqueArr.Index(history.Item(i)) == wxNOT_FOUND && (uniqueArr.size() < 20)) {
            uniqueArr.Add(history.Item(i));
        }
    }
    clConfig::Get().Write("C++NewWorkspace/Paths", uniqueArr);
    clConfig::Get().Write("C++NewWorkspace/DefaultLocation", m_comboBoxPath->GetValue());
}

void NewWorkspaceDlg::OnWorkspacePathUpdated(wxCommandEvent& event)
{
    wxUnusedVar(event);
    // set the workspace name to the last directory part
    // but only if the user did not set the name manually
    if(!m_manualNameTyped) {
        wxFileName fnPath(m_comboBoxPath->GetValue(), "");
        if(fnPath.GetDirCount()) { m_textCtrlWorkspaceName->ChangeValue(fnPath.GetDirs().Last()); }
    }
    DoUpdateFilePath();
}

void NewWorkspaceDlg::OnWorkspaceDirPicker(wxCommandEvent& event)
{
    const wxString& dir = ::wxDirSelector(_("Choose a folder:"), m_comboBoxPath->GetValue());
    if(!dir.empty()) {

        static wxString INVALID_CHARS = " ,'()";
        if(dir.find_first_of(INVALID_CHARS) != wxString::npos) {
            int answer = ::wxMessageBox(wxString() << _("The selected project path '") << dir
                                                   << _("'\nContains some invalid characters\nContinue anyways?"),
                                        "CodeLite", wxYES_NO | wxCANCEL | wxICON_WARNING, this);
            if(answer != wxYES) { return; }
        }

        // Use SetValue to ensure that an TEXT_UPDATE event is fired
        m_comboBoxPath->SetValue(dir);
    }
}

void NewWorkspaceDlg::OnButtonCreate(wxCommandEvent& event)
{
    // validate that the path part is valid
    m_workspacePath = m_staticTextWorkspaceFileName->GetLabel();
    wxFileName fn(m_workspacePath);

    if(m_checkBoxCreateSeparateDir->IsChecked()) {
        // dont test the result
        fn.Mkdir(fn.GetPath(), 0777, wxPATH_MKDIR_FULL);
    }

    if(!wxDirExists(fn.GetPath())) {
        wxMessageBox(_("Invalid path: ") + fn.GetPath(), _("Error"), wxOK | wxICON_HAND);
        return;
    }
    EndModal(wxID_OK);
}
void NewWorkspaceDlg::OnOKUI(wxUpdateUIEvent& event)
{
    wxFileName fnWspFile(m_staticTextWorkspaceFileName->GetLabel());
    event.Enable(fnWspFile.IsOk() && !fnWspFile.FileExists());
}

void NewWorkspaceDlg::OnWorkspaceNameUpdated(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoUpdateFilePath();
    m_manualNameTyped = true;
}

void NewWorkspaceDlg::DoUpdateFilePath()
{
    // update the static text control with the actual path
    wxString workspacePath;
    workspacePath << m_comboBoxPath->GetValue();

    workspacePath = workspacePath.Trim().Trim(false);

    wxString tmpSep(wxFileName::GetPathSeparator());
    if(!workspacePath.EndsWith(tmpSep) && workspacePath.IsEmpty() == false) {
        workspacePath << wxFileName::GetPathSeparator();
    }

    if(m_textCtrlWorkspaceName->GetValue().Trim().Trim(false).IsEmpty()) {
        m_staticTextWorkspaceFileName->SetLabel(wxEmptyString);
        return;
    }

    if(m_checkBoxCreateSeparateDir->IsChecked()) {
        workspacePath << m_textCtrlWorkspaceName->GetValue();
        workspacePath << wxFileName::GetPathSeparator();
    }

    workspacePath << m_textCtrlWorkspaceName->GetValue();
    workspacePath << wxT(".workspace");
    m_staticTextWorkspaceFileName->SetLabel(workspacePath);

    if(wxFileName::FileExists(workspacePath)) {
        m_infobar->ShowMessage(_("A workspace file with this name already exists"), wxICON_WARNING);
    } else {
        m_infobar->Hide();
    }
    SendSizeEvent();
}
