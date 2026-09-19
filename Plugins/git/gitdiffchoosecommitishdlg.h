//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2017 Eran Ifrah
// file name            : gitdiffchoosecommitishdlg.h
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
#ifndef GITDIFFCHOOSECOMMITISHDLG_H
#define GITDIFFCHOOSECOMMITISHDLG_H
#include "gitui.hpp"

class GitPlugin;
class IProcess;

class GitDiffChooseCommitishDlg : public GitDiffChooseCommitishDlgBase
{
public:
    GitDiffChooseCommitishDlg(wxWindow* parent, GitPlugin* plugin);
    ~GitDiffChooseCommitishDlg() override;

    wxString GetFirstCommit() const { return m_textFirst->GetValue() + GetAncestorSetting(m_spinCtrl1); }
    wxString GetSecondCommit() const { return m_textSecond->GetValue() + GetAncestorSetting(m_spinCtrl2); }
    wxString GetJoiner() const { return m_checkTripleDot->GetValue() ? "..." : " "; }

protected:
    GitPlugin* m_plugin;
    IProcess* m_process;

    int m_selectedRadio1;
    int m_selectedRadio2;
    wxItemContainerImmutable* m_activeChoice1; // May be a wxChoice or a wxComboBox
    wxItemContainerImmutable* m_activeChoice2;

    wxString GetAncestorSetting(wxSpinCtrl* spin) const; // Returns any '~5' setting

    void OnBranch1Changed(wxCommandEvent& event) override;
    virtual void OnBranch2Changed(wxCommandEvent& event);
    void OnRadioBranch1Selected(wxCommandEvent& event) override;
    void OnRadioBranch2Selected(wxCommandEvent& event) override;
    void OnRadioCommit1Selected(wxCommandEvent& event) override;
    void OnRadioCommit2Selected(wxCommandEvent& event) override;
    void OnRadioTag1Selected(wxCommandEvent& event) override;
    void OnRadioTag2Selected(wxCommandEvent& event) override;
    void OnRadioUser1Selected(wxCommandEvent& event) override;
    void OnRadioUser2Selected(wxCommandEvent& event) override;
    void OnUpdateUIBranch1(wxUpdateUIEvent& event) override;
    void OnUpdateUIBranch2(wxUpdateUIEvent& event) override;
    void OnUpdateUICommit1(wxUpdateUIEvent& event) override;
    void OnUpdateUICommit2(wxUpdateUIEvent& event) override;
    void OnUpdateUICommitish1(wxUpdateUIEvent& event) override;
    void OnUpdateUICommitish2(wxUpdateUIEvent& event) override;
    void OnUpdateUITags1(wxUpdateUIEvent& event) override;
    void OnUpdateUITags2(wxUpdateUIEvent& event) override;
    void OnTextFirstUI(wxUpdateUIEvent& event) override;
    void OnTextSecondUI(wxUpdateUIEvent& event) override;
};
#endif // GITDIFFCHOOSECOMMITISHDLG_H
