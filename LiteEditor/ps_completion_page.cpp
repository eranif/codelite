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
#include <wx/filedlg.h>

PSCompletionPage::PSCompletionPage(wxWindow* parent, ProjectSettingsDlg* dlg)
    : PSCompletionBase(parent)
    , m_dlg(dlg)
{
}

void PSCompletionPage::OnStcEvtVModified(wxStyledTextEvent& event)
{
    event.Skip();
    m_dlg->SetIsDirty(true);
}

void PSCompletionPage::Clear()
{
    m_textCtrlSearchPaths->Clear();
    m_textCtrlMacros->Clear();
    m_textCtrlSearchPaths->Clear();
}

void PSCompletionPage::Load(BuildConfigPtr buildConf)
{
    m_textCtrlMacros->SetValue(buildConf->GetClangPPFlags());
    m_textCtrlSearchPaths->SetValue(buildConf->GetCcSearchPaths());
}

void PSCompletionPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    buildConf->SetClangPPFlags(m_textCtrlMacros->GetValue());
    buildConf->SetCcSearchPaths(m_textCtrlSearchPaths->GetValue());
}

void PSCompletionPage::OnBrowsePCH(wxCommandEvent& event) {}

void PSCompletionPage::OnProjectEnabledUI(wxUpdateUIEvent& event) { event.Enable(m_dlg->IsProjectEnabled()); }
