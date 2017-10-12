//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cppchecksettingsdlg.cpp
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

#include "cppchecksettingsdlg.h"
#include "windowattrmanager.h"
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/textdlg.h>
#include <wx/dirdlg.h>

CppCheckSettingsDialog::CppCheckSettingsDialog(wxWindow* parent, CppCheckSettings* settings, IConfigTool* conf,
                                               const wxString& defaultpath, bool showDefsTab)
    : CppCheckSettingsDialogBase(parent)
    , m_settings(settings)
    , m_conf(conf)
    , m_defaultpath(defaultpath)
{
    m_checkListExtraWarnings->Check(0, settings->GetStyle());
    m_checkListExtraWarnings->Check(1, settings->GetPerformance());
    m_checkListExtraWarnings->Check(2, settings->GetPortability());
    m_checkListExtraWarnings->Check(3, settings->GetUnusedFunctions());
    m_checkListExtraWarnings->Check(4, settings->GetMissingIncludes());
    m_checkListExtraWarnings->Check(5, settings->GetInformation());
    m_checkListExtraWarnings->Check(6, settings->GetPosixStandards());
    m_checkListExtraWarnings->Check(7, settings->GetC99Standards());
    m_checkListExtraWarnings->Check(8, settings->GetCpp11Standards());
    m_cbOptionForce->SetValue(settings->GetForce());
    m_cbJobs->SetValue(settings->GetJobs() > 1);
    m_spinCtrlJobs->SetValue(settings->GetJobs());

    m_listBoxExcludelist->Append(settings->GetExcludeFiles());

    // The Suppressed Warnings strings are stored in a string/string map:
    // e.g. <"cstyleCast", "C-style pointer casting">
    // We display the second, so store the keys in a wxArrayInt so that we can identify each again
    m_SuppressionsKeys.Clear();
    wxStringMap_t::const_iterator iter = settings->GetSuppressedWarningsStrings1()->begin();
    for(; iter != settings->GetSuppressedWarningsStrings1()->end(); ++iter) {
        // First the checked ones
        int index = m_checkListSuppress->Append((*iter).second);
        m_checkListSuppress->Check(index, true);
        m_SuppressionsKeys.Add((*iter).first);
    }

    for(iter = settings->GetSuppressedWarningsStrings0()->begin();
        iter != settings->GetSuppressedWarningsStrings0()->end(); ++iter) {
        // Then the unchecked ones
        int index = m_checkListSuppress->Append((*iter).second);
        m_checkListSuppress->Check(index, false);
        m_SuppressionsKeys.Add((*iter).first);
    }

    m_listBoxIncludeDirs->Append(settings->GetIncludeDirs());
    m_checkBoxSuppressSystemIncludes->SetValue(settings->GetSuppressSystemIncludes());
    m_checkBoxSerialiseIncludeDirs->SetValue(settings->GetSaveIncludeDirs());

    if(showDefsTab) {
        m_listBoxDefinelist->Append(settings->GetDefinitions());
        m_listBoxUndefineList->Append(settings->GetUndefines());
    } else {
        m_DefinesPanel->Hide(); // Don't show this panel unless its contents are valid i.e. we got here via a rt-click
                                // over a project
    }

    SetName("CppCheckSettingsDialog");
    WindowAttrManager::Load(this);
}

void CppCheckSettingsDialog::OnBtnOK(wxCommandEvent& e)
{
    m_settings->SetStyle(m_checkListExtraWarnings->IsChecked(0));
    m_settings->SetPerformance(m_checkListExtraWarnings->IsChecked(1));
    m_settings->SetPortability(m_checkListExtraWarnings->IsChecked(2));
    m_settings->SetUnusedFunctions(m_checkListExtraWarnings->IsChecked(3));
    m_settings->SetMissingIncludes(m_checkListExtraWarnings->IsChecked(4));
    m_settings->SetInformation(m_checkListExtraWarnings->IsChecked(5));
    m_settings->SetPosixStandards(m_checkListExtraWarnings->IsChecked(6));
    m_settings->SetC99Standards(m_checkListExtraWarnings->IsChecked(7));
    m_settings->SetCpp11Standards(m_checkListExtraWarnings->IsChecked(8));
    m_settings->SetForce(m_cbOptionForce->IsChecked());
    if(m_cbJobs->IsChecked()) {
        m_settings->SetJobs(m_spinCtrlJobs->GetValue());
    } else {
        m_settings->SetJobs(1);
    }
    m_settings->SetCheckConfig(m_cbCheckConfig->IsChecked());

    m_settings->SetExcludeFiles(m_listBoxExcludelist->GetStrings());

    m_settings->SetSuppressedWarnings(m_checkListSuppress, m_SuppressionsKeys);
    m_settings->SetSaveSuppressedWarnings(m_checkBoxSerialise->IsChecked());

    m_settings->SetIncludeDirs(m_listBoxIncludeDirs->GetStrings());
    m_settings->SetSuppressSystemIncludes(m_checkBoxSuppressSystemIncludes->IsChecked());
    m_settings->SetSaveIncludeDirs(m_checkBoxSerialiseIncludeDirs->IsChecked());

    m_settings->SetDefinitions(m_listBoxDefinelist->GetStrings());
    m_settings->SetUndefines(m_listBoxUndefineList->GetStrings());

    e.Skip();
}

CppCheckSettingsDialog::~CppCheckSettingsDialog() {}

void CppCheckSettingsDialog::OnChecksTickAll(wxCommandEvent& WXUNUSED(e))
{
    for(size_t n = 0; n < m_checkListExtraWarnings->GetCount(); ++n) {
        m_checkListExtraWarnings->Check(n);
    }
}

void CppCheckSettingsDialog::OnChecksUntickAll(wxCommandEvent& WXUNUSED(e))
{
    for(size_t n = 0; n < m_checkListExtraWarnings->GetCount(); ++n) {
        m_checkListExtraWarnings->Check(n, false);
    }
}

void CppCheckSettingsDialog::OnAddFile(wxCommandEvent& WXUNUSED(e))
{
    wxString filter(wxString(_("C/C++ Files ")));
    filter << wxT("(*.c;*.cpp)|*.c;*.cpp|") << wxString(_("All Files")) << wxT(" (") << wxFileSelectorDefaultWildcardStr
           << wxT(")|") << wxFileSelectorDefaultWildcardStr;

    wxFileDialog dlg(this, _("Add File(s):"), m_defaultpath, wxEmptyString, filter,
                     wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE, wxDefaultPosition);

    if(dlg.ShowModal() == wxID_OK) {
        wxArrayString paths;
        dlg.GetPaths(paths);

        m_listBoxExcludelist->Append(paths);
    }
}

void CppCheckSettingsDialog::OnRemoveFile(wxCommandEvent& WXUNUSED(e))
{
    int sel = m_listBoxExcludelist->GetSelection();
    if(sel != wxNOT_FOUND) {
        m_listBoxExcludelist->Delete((unsigned int)sel);
    }
}

void CppCheckSettingsDialog::OnClearList(wxCommandEvent& e)
{
    wxUnusedVar(e);
    m_listBoxExcludelist->Clear();
}

void CppCheckSettingsDialog::OnAddSuppression(wxCommandEvent& WXUNUSED(e))
{
    CppCheckAddSuppressionDialog dlg(this);
    while(true) {
        int ret = dlg.ShowModal();
        if(ret != wxID_OK) {
            return;
        }
        wxString key = dlg.GetKey()->GetValue();
        if(m_settings->GetSuppressedWarningsStrings0()->count(key) ||
           m_settings->GetSuppressedWarningsStrings1()->count(key)) {
            int ans = wxMessageBox(_("There is already an entry with ID string. Try again?"), _("CppCheck"),
                                   wxYES_NO | wxICON_QUESTION, this);
            if(ans != wxID_YES && ans != wxYES) {
                return;
            } else {
                dlg.GetKey()->Clear();
                continue;
            }
        }

        int index = m_checkListSuppress->Append(dlg.GetDescription()->GetValue()); // If we're here, it worked
        m_checkListSuppress->Check(index, false);
        m_settings->AddSuppressedWarning(dlg.GetKey()->GetValue(), dlg.GetDescription()->GetValue(), false);
        m_SuppressionsKeys.Add(dlg.GetKey()->GetValue());
        return;
    }
}

void CppCheckSettingsDialog::OnRemoveSuppression(wxCommandEvent& WXUNUSED(e))
{
    int ans = wxMessageBox(_("Really remove this warning suppression, rather than just unticking it?"), _("CppCheck"),
                           wxYES_NO | wxICON_QUESTION, this);
    if(ans != wxID_YES && ans != wxYES) {
        return;
    }

    int sel = m_checkListSuppress->GetSelection();
    if(sel != wxNOT_FOUND) {
        m_settings->RemoveSuppressedWarning(m_SuppressionsKeys.Item(sel));
        m_checkListSuppress->Delete((unsigned int)sel);
        m_SuppressionsKeys.RemoveAt(sel);
    }
}

void CppCheckSettingsDialog::OnSuppressTickAll(wxCommandEvent& WXUNUSED(e))
{
    for(size_t n = 0; n < m_checkListSuppress->GetCount(); ++n) {
        m_checkListSuppress->Check(n);
    }
}

void CppCheckSettingsDialog::OnSuppressUntickAll(wxCommandEvent& WXUNUSED(e))
{
    for(size_t n = 0; n < m_checkListSuppress->GetCount(); ++n) {
        m_checkListSuppress->Check(n, false);
    }
}

void CppCheckSettingsDialog::OnAddIncludeDir(wxCommandEvent& WXUNUSED(e))
{
    wxDirDialog dlg(this, _("Select the path containing include files"));
    if(dlg.ShowModal() == wxID_OK) {
        m_listBoxIncludeDirs->Append(dlg.GetPath());
    }
}

void CppCheckSettingsDialog::OnRemoveIncludeDir(wxCommandEvent& WXUNUSED(e))
{
    int sel = m_listBoxIncludeDirs->GetSelection();
    if(sel != wxNOT_FOUND) {
        m_listBoxIncludeDirs->Delete((unsigned int)sel);
    }
}

void CppCheckSettingsDialog::OnAddDefinition(wxCommandEvent& WXUNUSED(e))
{
    wxString newitem =
        wxGetTextFromUser("Enter a definition e.g. 'FOO' or 'BAR=1' (not '-DFoo')", "CodeLite", "", this);
    if(!newitem.empty()) {
        m_listBoxDefinelist->Append(newitem);
    }
}

void CppCheckSettingsDialog::OnRemoveDefinition(wxCommandEvent& WXUNUSED(e))
{
    int sel = m_listBoxDefinelist->GetSelection();
    if(sel != wxNOT_FOUND) {
        m_listBoxDefinelist->Delete((unsigned int)sel);
    }
}

void CppCheckSettingsDialog::OnClearDefinitions(wxCommandEvent& e)
{
    wxUnusedVar(e);
    m_listBoxDefinelist->Clear();
}

void CppCheckSettingsDialog::OnAddUndefine(wxCommandEvent& WXUNUSED(e))
{
    wxString newitem =
        wxGetTextFromUser("Enter a definition NOT to check e.g. 'FOO' or 'BAR=1' (not '-UFoo')", "CodeLite", "", this);
    if(!newitem.empty()) {
        m_listBoxUndefineList->Append(newitem);
    }
}

void CppCheckSettingsDialog::OnRemoveUndefine(wxCommandEvent& WXUNUSED(e))
{
    int sel = m_listBoxUndefineList->GetSelection();
    if(sel != wxNOT_FOUND) {
        m_listBoxUndefineList->Delete((unsigned int)sel);
    }
}

void CppCheckSettingsDialog::OnClearUndefines(wxCommandEvent& e)
{
    wxUnusedVar(e);
    m_listBoxUndefineList->Clear();
}

void CppCheckSettingsDialog::OnChecksTickAllUI(wxUpdateUIEvent& e)
{
    for(size_t n = 0; n < m_checkListExtraWarnings->GetCount(); ++n) {
        if(!m_checkListExtraWarnings->IsChecked(n)) {
            e.Enable(true);
            return;
        }
    }
    e.Enable(false);
}

void CppCheckSettingsDialog::OnChecksUntickAllUI(wxUpdateUIEvent& e)
{
    for(size_t n = 0; n < m_checkListExtraWarnings->GetCount(); ++n) {
        if(m_checkListExtraWarnings->IsChecked(n)) {
            e.Enable(true);
            return;
        }
    }
    e.Enable(false);
}

void CppCheckSettingsDialog::OnJobsUpdateUI(wxUpdateUIEvent& e) { e.Enable(m_cbJobs->IsChecked()); }

void CppCheckSettingsDialog::OnRemoveFileUI(wxUpdateUIEvent& e)
{
    e.Enable(m_listBoxExcludelist->GetSelection() != wxNOT_FOUND);
}

void CppCheckSettingsDialog::OnClearListUI(wxUpdateUIEvent& e) { e.Enable(!m_listBoxExcludelist->IsEmpty()); }

void CppCheckSettingsDialog::OnRemoveSuppressionUI(wxUpdateUIEvent& e)
{
    e.Enable(m_checkListSuppress->GetSelection() != wxNOT_FOUND);
}

void CppCheckSettingsDialog::OnSuppressTickAllUI(wxUpdateUIEvent& e)
{
    for(size_t n = 0; n < m_checkListSuppress->GetCount(); ++n) {
        if(!m_checkListSuppress->IsChecked(n)) {
            e.Enable(true);
            return;
        }
    }
    e.Enable(false);
}

void CppCheckSettingsDialog::OnSuppressUntickAllUI(wxUpdateUIEvent& e)
{
    for(size_t n = 0; n < m_checkListSuppress->GetCount(); ++n) {
        if(m_checkListSuppress->IsChecked(n)) {
            e.Enable(true);
            return;
        }
    }
    e.Enable(false);
}

void CppCheckSettingsDialog::OnIncludeDirsUpdateUI(wxUpdateUIEvent& event)
{
    bool enable(false);
    int index = m_checkListExtraWarnings->FindString(_("Missing includes"));
    if(index != wxNOT_FOUND) {
        enable = m_checkListExtraWarnings->IsChecked(index);
    }
    // Only enable the IncludeDirs panel if the "Missing includes" warning is enabled
    event.Enable(enable);
}

void CppCheckSettingsDialog::OnRemoveIncludeDirUI(wxUpdateUIEvent& event)
{
    event.Enable(m_listBoxIncludeDirs->GetSelection() != wxNOT_FOUND);
}

void CppCheckSettingsDialog::OnRemoveDefinitionUI(wxUpdateUIEvent& e)
{
    e.Enable(m_listBoxDefinelist->GetSelection() != wxNOT_FOUND);
}

void CppCheckSettingsDialog::OnClearDefinitionsUI(wxUpdateUIEvent& e) { e.Enable(!m_listBoxDefinelist->IsEmpty()); }

void CppCheckSettingsDialog::OnRemoveUndefineUI(wxUpdateUIEvent& e)
{
    e.Enable(m_listBoxUndefineList->GetSelection() != wxNOT_FOUND);
}

void CppCheckSettingsDialog::OnClearUndefinesUI(wxUpdateUIEvent& e) { e.Enable(!m_listBoxUndefineList->IsEmpty()); }

void CppCheckAddSuppressionDialog::OnOKButtonUpdateUI(wxUpdateUIEvent& e)
{
    e.Enable(!GetKey()->IsEmpty() && !GetDescription()->IsEmpty());
}
