//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2017 Eran Ifrah
// file name            : gitdiffchoosecommitishdlg.cpp
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

#include "asyncprocess.h"
#include "cl_config.h"
#include "git.h"
#include "gitdiffchoosecommitishdlg.h"
#include "gitentry.h"
#include "globals.h"
#include "processreaderthread.h"
#include "windowattrmanager.h"
#include <wx/tokenzr.h>

GitDiffChooseCommitishDlg::GitDiffChooseCommitishDlg(wxWindow* parent, GitPlugin* plugin)
    : GitDiffChooseCommitishDlgBase(parent)
    , m_plugin(plugin)
    , m_selectedRadio1(3)
    , m_selectedRadio2(3)
    , m_activeChoice1(m_choiceCommit1)
    , m_activeChoice2(m_choiceCommit2)
{
    WindowAttrManager::Load(this);

    // Set a max width for these, else selecting a commit with a long message will break the layout
    m_choiceCommit1->SetMaxSize(wxSize(60, -1));
    m_choiceCommit2->SetMaxSize(wxSize(60, -1));

    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

    m_selectedRadio1 = data.GetGitDiffChooseDlgRadioSel1();
    m_selectedRadio2 = data.GetGitDiffChooseDlgRadioSel2();
    wxRadioButton* radiosL[] = { m_radioBranch1, m_radioTag1, m_radioCommit1, m_radioUserEntered1 };
    wxRadioButton* radiosR[] = { m_radioBranch2, m_radioTag2, m_radioCommit2, m_radioUserEntered2 };
    wxItemContainerImmutable* choicesL[] = { m_choiceBranch1, m_choiceTag1, m_choiceCommit1, m_comboCommitish1 };
    wxItemContainerImmutable* choicesR[] = { m_choiceBranch2, m_choiceTag2, m_choiceCommit2, m_comboCommitish2 };
    if(m_selectedRadio1 > -1 && m_selectedRadio1 < 4) {
        radiosL[m_selectedRadio1]->SetValue(1);
        m_activeChoice1 = choicesL[m_selectedRadio1];
    }
    if(m_selectedRadio2 > -1 && m_selectedRadio2 < 4) {
        radiosR[m_selectedRadio2]->SetValue(1);
        m_activeChoice2 = choicesR[m_selectedRadio2];
    }
    m_comboCommitish1->Append(data.GetGitDiffChooseDlgCBoxValues1());
    m_comboCommitish2->Append(data.GetGitDiffChooseDlgCBoxValues2());

    m_plugin->AsyncRunGitWithCallback(
        " --no-pager branch -a --no-color",
        [this](const wxString& output) {
            wxArrayString items = wxStringTokenize(output, "\n", wxTOKEN_STRTOK);
            m_choiceBranch1->Set(items);
            m_choiceBranch1->Set(items);
        },
        IProcessCreateDefault | IProcessWrapInShell, m_plugin->GetRepositoryPath());

    m_plugin->AsyncRunGitWithCallback(
        " --no-pager tag",
        [this](const wxString& output) {
            wxArrayString items = wxStringTokenize(output, "\n", wxTOKEN_STRTOK);
            m_choiceTag1->Set(items);
            m_choiceTag2->Set(items);
        },
        IProcessCreateDefault | IProcessWrapInShell, m_plugin->GetRepositoryPath());

    // Restrict the commits to 1000: filling a wxChoice with many more froze CodeLite for several minutes
    // and in any case, selecting one particular commit out of hundreds is not easy!
    m_plugin->AsyncRunGitWithCallback(
        " --no-pager log -1000 --format=\"%h %<(60,trunc)%s\"",
        [this](const wxString& output) {
            wxArrayString items = wxStringTokenize(output, "\n", wxTOKEN_STRTOK);
            m_choiceCommit1->Set(items);
            m_choiceCommit2->Set(items);
        },
        IProcessCreateDefault | IProcessWrapInShell, m_plugin->GetRepositoryPath());
}

GitDiffChooseCommitishDlg::~GitDiffChooseCommitishDlg()
{
    wxArrayString comboCommitish1Strings = m_comboCommitish1->GetStrings();
    if(m_selectedRadio1 == 3) {
        wxString sel = m_comboCommitish1->GetValue();
        if(!sel.empty()) {
            if(comboCommitish1Strings.Index(sel) != wxNOT_FOUND) {
                comboCommitish1Strings.Remove(sel);
            }
            comboCommitish1Strings.Insert(sel, 0);
        }
    }
    wxArrayString comboCommitish2Strings = m_comboCommitish2->GetStrings();
    if(m_selectedRadio1 == 3) {
        wxString sel = m_comboCommitish2->GetValue();
        if(!sel.empty()) {
            if(comboCommitish2Strings.Index(sel) != wxNOT_FOUND) {
                comboCommitish2Strings.Remove(sel);
            }
            comboCommitish2Strings.Insert(sel, 0);
        }
    }
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

    data.SetGitDiffChooseDlgRadioSel1(m_selectedRadio1);
    data.SetGitDiffChooseDlgRadioSel2(m_selectedRadio2);
    data.SetGitDiffChooseDlgCBoxValues1(comboCommitish1Strings);
    data.SetGitDiffChooseDlgCBoxValues2(comboCommitish2Strings);

    conf.WriteItem(&data);
}

wxString GitDiffChooseCommitishDlg::GetAncestorSetting(wxSpinCtrl* spin) const
{
    wxString setting;
    if(spin) {
        int value = spin->GetValue();
        if(value > 0) {
            setting = wxString::Format("~%i", value);
        }
    }
    return setting;
}

void GitDiffChooseCommitishDlg::OnUpdateUIBranch1(wxUpdateUIEvent& event) { event.Enable(m_radioBranch1->GetValue()); }
void GitDiffChooseCommitishDlg::OnUpdateUIBranch2(wxUpdateUIEvent& event) { event.Enable(m_radioBranch2->GetValue()); }
void GitDiffChooseCommitishDlg::OnUpdateUICommit1(wxUpdateUIEvent& event) { event.Enable(m_radioCommit1->GetValue()); }
void GitDiffChooseCommitishDlg::OnUpdateUICommit2(wxUpdateUIEvent& event) { event.Enable(m_radioCommit2->GetValue()); }
void GitDiffChooseCommitishDlg::OnUpdateUICommitish1(wxUpdateUIEvent& event)
{
    event.Enable(m_radioUserEntered1->GetValue());
}
void GitDiffChooseCommitishDlg::OnUpdateUICommitish2(wxUpdateUIEvent& event)
{
    event.Enable(m_radioUserEntered2->GetValue());
}
void GitDiffChooseCommitishDlg::OnUpdateUITags1(wxUpdateUIEvent& event) { event.Enable(m_radioTag1->GetValue()); }
void GitDiffChooseCommitishDlg::OnUpdateUITags2(wxUpdateUIEvent& event) { event.Enable(m_radioTag2->GetValue()); }
void GitDiffChooseCommitishDlg::OnRadioBranch1Selected(wxCommandEvent& WXUNUSED(event))
{
    m_activeChoice1 = m_choiceBranch1;
    m_selectedRadio1 = 0;
}
void GitDiffChooseCommitishDlg::OnRadioBranch2Selected(wxCommandEvent& WXUNUSED(event))
{
    m_activeChoice2 = m_choiceBranch2;
    m_selectedRadio2 = 0;
}
void GitDiffChooseCommitishDlg::OnRadioTag1Selected(wxCommandEvent& WXUNUSED(event))
{
    m_activeChoice1 = m_choiceTag1;
    m_selectedRadio1 = 1;
}
void GitDiffChooseCommitishDlg::OnRadioTag2Selected(wxCommandEvent& WXUNUSED(event))
{
    m_activeChoice2 = m_choiceTag2;
    m_selectedRadio2 = 1;
}
void GitDiffChooseCommitishDlg::OnRadioCommit1Selected(wxCommandEvent& WXUNUSED(event))
{
    m_activeChoice1 = m_choiceCommit1;
    m_selectedRadio1 = 2;
}
void GitDiffChooseCommitishDlg::OnRadioCommit2Selected(wxCommandEvent& WXUNUSED(event))
{
    m_activeChoice2 = m_choiceCommit2;
    m_selectedRadio2 = 2;
}
void GitDiffChooseCommitishDlg::OnRadioUser1Selected(wxCommandEvent& WXUNUSED(event))
{
    m_activeChoice1 = m_comboCommitish1;
    m_selectedRadio1 = 3;
}
void GitDiffChooseCommitishDlg::OnRadioUser2Selected(wxCommandEvent& WXUNUSED(event))
{
    m_activeChoice2 = m_comboCommitish2;
    m_selectedRadio2 = 3;
}
void GitDiffChooseCommitishDlg::OnTextFirstUI(wxUpdateUIEvent& event)
{
    wxString text =
        (m_activeChoice1 == m_comboCommitish1) ? m_comboCommitish1->GetValue() : m_activeChoice1->GetStringSelection();
    if(text.StartsWith("* ")) {
        text = text.Mid(2); // Remove the 'active branch' marker
    }
    if(m_activeChoice1 == m_choiceCommit1) {
        text = text.BeforeFirst(' '); // We want only the commit hash, not the message
    }

    static_cast<wxTextCtrl*>(event.GetEventObject())->ChangeValue(text.Trim().Trim(false));
}
void GitDiffChooseCommitishDlg::OnTextSecondUI(wxUpdateUIEvent& event)
{
    wxString text =
        (m_activeChoice2 == m_comboCommitish2) ? m_comboCommitish2->GetValue() : m_activeChoice2->GetStringSelection();
    if(text.StartsWith("* ")) {
        text = text.Mid(2); // Remove the 'active branch' marker
    }
    if(m_activeChoice2 == m_choiceCommit2) {
        text = text.BeforeFirst(' '); // We want only the commit hash, not the message
    }

    static_cast<wxTextCtrl*>(event.GetEventObject())->ChangeValue(text.Trim().Trim(false));
}
void GitDiffChooseCommitishDlg::OnBranch1Changed(wxCommandEvent& event)
{
    wxString newBranch = m_choiceBranch1->GetString(event.GetInt());
    if(newBranch.StartsWith("* ")) {
        newBranch = newBranch.Mid(2); // Remove the 'active branch' marker
    }
    m_plugin->AsyncRunGitWithCallback(
        " --no-pager log -1000 --format=\"%h %<(60,trunc)%s\" " + newBranch,
        [this](const wxString& output) {
            wxArrayString items = wxStringTokenize(output, "\n", wxTOKEN_STRTOK);
            m_choiceCommit1->Set(items);
        },
        IProcessCreateDefault | IProcessWrapInShell, m_plugin->GetRepositoryPath());
}
void GitDiffChooseCommitishDlg::OnBranch2Changed(wxCommandEvent& event)
{
    wxString newBranch = m_choiceBranch2->GetString(event.GetInt());
    if(newBranch.StartsWith("* ")) {
        newBranch = newBranch.Mid(2); // Remove the 'active branch' marker
    }

    m_plugin->AsyncRunGitWithCallback(
        " --no-pager log -1000 --format=\"%h %<(60,trunc)%s\" " + newBranch,
        [this](const wxString& output) {
            wxArrayString items = wxStringTokenize(output, "\n", wxTOKEN_STRTOK);
            m_choiceCommit2->Set(items);
        },
        IProcessCreateDefault | IProcessWrapInShell, m_plugin->GetRepositoryPath());
}
