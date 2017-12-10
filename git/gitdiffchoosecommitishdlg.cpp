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
#include "processreaderthread.h"
#include "windowattrmanager.h"
#include <wx/tokenzr.h>

/**
 * @class GitDiffCmdHandler
 * Handle e.g. git branch command output
 * Sets the 3 pairs of wxChoices with the output
 */
class GitDiffCmdHandler : public IProcessCallback
{
    wxChoice* m_choice1;
    wxChoice* m_choice2;
    wxString m_output;

public:
    GitDiffCmdHandler(wxChoice* choice1, wxChoice* choice2)
        : m_choice1(choice1)
        , m_choice2(choice2)
    {
    }
    ~GitDiffCmdHandler() {}

    virtual void OnProcessOutput(const wxString& str) { m_output << str; }

    virtual void OnProcessTerminated()
    {
        wxArrayString items = wxStringTokenize(m_output, "\n", wxTOKEN_STRTOK);
        if(m_choice1) { m_choice1->Set(items); }
        if(m_choice2) { m_choice2->Set(items); }
        delete this;
    }
};

GitDiffChooseCommitishDlg::GitDiffChooseCommitishDlg(wxWindow* parent, GitPlugin* plugin)
    : GitDiffChooseCommitishDlgBase(parent)
    , m_plugin(plugin)
    , m_activeChoice1(m_choiceCommit1)
    , m_activeChoice2(m_choiceCommit2)
    , m_selectedRadio1(3)
    , m_selectedRadio2(3)
{
    WindowAttrManager::Load(this);

    // Set a max width for these, else selecting a commit with a long message will break the layout
    m_choiceCommit1->SetMaxSize(wxSize(60, -1));
    m_choiceCommit2->SetMaxSize(wxSize(60, -1));

    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

    m_gitPath = data.GetGITExecutablePath();
    m_gitPath.Trim().Trim(false);

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

    wxString command = m_gitPath + " --no-pager branch -a --no-color";
    m_process = CreateAsyncProcessCB(this, new GitDiffCmdHandler(m_choiceBranch1, m_choiceBranch2), command,
                                     IProcessCreateDefault, m_plugin->GetRepositoryDirectory());

    command = m_gitPath + " --no-pager tag";
    m_process = CreateAsyncProcessCB(this, new GitDiffCmdHandler(m_choiceTag1, m_choiceTag2), command,
                                     IProcessCreateDefault, m_plugin->GetRepositoryDirectory());

    // Restrict the commits to 1000: filling a wxChoice with many more froze CodeLite for several minutes
    // and in any case, selecting one particular commit out of hundreds is not easy!
    command = m_gitPath + " --no-pager log -1000 --format=\"%h %<(60,trunc)%s\"";
    m_process = CreateAsyncProcessCB(this, new GitDiffCmdHandler(m_choiceCommit1, m_choiceCommit2), command,
                                     IProcessCreateDefault, m_plugin->GetRepositoryDirectory());
}

GitDiffChooseCommitishDlg::~GitDiffChooseCommitishDlg()
{
    wxArrayString comboCommitish1Strings = m_comboCommitish1->GetStrings();
    if(m_selectedRadio1 == 3) {
        wxString sel = m_comboCommitish1->GetValue();
        if(!sel.empty()) {
            if(comboCommitish1Strings.Index(sel) != wxNOT_FOUND) { comboCommitish1Strings.Remove(sel); }
            comboCommitish1Strings.Insert(sel, 0);
        }
    }
    wxArrayString comboCommitish2Strings = m_comboCommitish2->GetStrings();
    if(m_selectedRadio1 == 3) {
        wxString sel = m_comboCommitish2->GetValue();
        if(!sel.empty()) {
            if(comboCommitish2Strings.Index(sel) != wxNOT_FOUND) { comboCommitish2Strings.Remove(sel); }
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
        if(value > 0) { setting = wxString::Format("~%i", value); }
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
    wxString command = m_gitPath + " --no-pager log -1000 --format=\"%h %<(60,trunc)%s\" " + newBranch;
    m_process = CreateAsyncProcessCB(this, new GitDiffCmdHandler(m_choiceCommit1, NULL), command, IProcessCreateDefault,
                                     m_plugin->GetRepositoryDirectory());
}
void GitDiffChooseCommitishDlg::OnBranch2Changed(wxCommandEvent& event)
{
    wxString newBranch = m_choiceBranch2->GetString(event.GetInt());
    if(newBranch.StartsWith("* ")) {
        newBranch = newBranch.Mid(2); // Remove the 'active branch' marker
    }
    wxString command = m_gitPath + " --no-pager log -1000 --format=\"%h %<(60,trunc)%s\" " + newBranch;
    m_process = CreateAsyncProcessCB(this, new GitDiffCmdHandler(NULL, m_choiceCommit2), command, IProcessCreateDefault,
                                     m_plugin->GetRepositoryDirectory());
}
