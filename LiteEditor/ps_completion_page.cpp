//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ps_completion_page.cpp
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

#include "ps_completion_page.h"
#include "clang_code_completion.h"
#include <wx/filedlg.h>

PSCompletionPage::PSCompletionPage( wxWindow* parent, ProjectSettingsDlg* dlg )
    : PSCompletionBase( parent )
    , m_dlg(dlg)
    , m_ccSettingsModified(false)
{
}

void PSCompletionPage::OnCmdEvtVModified( wxCommandEvent& event )
{
    wxUnusedVar(event);
    m_dlg->SetIsDirty(true);
    m_ccSettingsModified = true;
}

void PSCompletionPage::Clear()
{
    m_textCtrlSearchPaths->Clear();
    m_textCtrlMacros->Clear();
    m_textCtrlSearchPaths->Clear();
    m_ccSettingsModified = false;
}

void PSCompletionPage::Load(BuildConfigPtr buildConf)
{
    m_textCtrlMacros->SetValue(buildConf->GetClangPPFlags());
    m_textCtrlSearchPaths->SetValue(buildConf->GetCcSearchPaths());
    m_checkBoxC11->SetValue(buildConf->IsClangC11());
    m_checkBoxC14->SetValue(buildConf->IsClangC14());
	m_checkBoxC17->SetValue(buildConf->IsClangC17());
}

void PSCompletionPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    buildConf->SetClangPPFlags(m_textCtrlMacros->GetValue());
    buildConf->SetCcSearchPaths(m_textCtrlSearchPaths->GetValue());
    buildConf->SetClangC11(m_checkBoxC11->IsChecked());
    buildConf->SetClangC14(m_checkBoxC14->IsChecked());
	buildConf->SetClangC17(m_checkBoxC17->IsChecked());
    // Save was requested
    if(m_ccSettingsModified) {
        m_ccSettingsModified = false;

#if HAS_LIBCLANG
        ClangCodeCompletion::Instance()->ClearCache();
#endif

    }
}

void PSCompletionPage::OnBrowsePCH(wxCommandEvent& event)
{
//    wxString filename = wxFileSelector(wxT("Select a Header File:"));
//    if ( !filename.empty() ) {
//        // work with the file
//        m_filePickerClangPCH->SetValue(filename);
//        OnCmdEvtVModified(event);
//    }
}
void PSCompletionPage::OnProjectEnabledUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dlg->IsProjectEnabled());
}
