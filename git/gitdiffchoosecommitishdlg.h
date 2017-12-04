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
#include "gitui.h"

class GitPlugin;
class IProcess;

class GitDiffChooseCommitishDlg : public GitDiffChooseCommitishDlgBase
{
public:
    GitDiffChooseCommitishDlg(wxWindow* parent, GitPlugin* plugin);
    virtual ~GitDiffChooseCommitishDlg();
    
    wxString GetFirstCommit() const {
        return m_textFirst->GetValue() + GetAncestorSetting(m_spinCtrl1);
    }
    wxString GetSecondCommit() const {
        return m_textSecond->GetValue() + GetAncestorSetting(m_spinCtrl2);
    }
    wxString GetJoiner() const {
        return m_checkTripleDot->GetValue() ? "...":" ";
    }
protected:
    wxString m_gitPath;
    GitPlugin* m_plugin;
    IProcess* m_process;
    
    int m_selectedRadio1;
    int m_selectedRadio2;
    wxItemContainerImmutable* m_activeChoice1; // May be a wxChoice or a wxComboBox
    wxItemContainerImmutable* m_activeChoice2;
    
    wxString GetAncestorSetting(wxSpinCtrl* spin) const; // Returns any '~5' setting

    virtual void OnBranch1Changed(wxCommandEvent& event);
    virtual void OnBranch2Changed(wxCommandEvent& event);
    virtual void OnRadioBranch1Selected(wxCommandEvent& event);
    virtual void OnRadioBranch2Selected(wxCommandEvent& event);
    virtual void OnRadioCommit1Selected(wxCommandEvent& event);
    virtual void OnRadioCommit2Selected(wxCommandEvent& event);
    virtual void OnRadioTag1Selected(wxCommandEvent& event);
    virtual void OnRadioTag2Selected(wxCommandEvent& event);
    virtual void OnRadioUser1Selected(wxCommandEvent& event);
    virtual void OnRadioUser2Selected(wxCommandEvent& event);
    virtual void OnUpdateUIBranch1(wxUpdateUIEvent& event);
    virtual void OnUpdateUIBranch2(wxUpdateUIEvent& event);
    virtual void OnUpdateUICommit1(wxUpdateUIEvent& event);
    virtual void OnUpdateUICommit2(wxUpdateUIEvent& event);
    virtual void OnUpdateUICommitish1(wxUpdateUIEvent& event);
    virtual void OnUpdateUICommitish2(wxUpdateUIEvent& event);
    virtual void OnUpdateUITags1(wxUpdateUIEvent& event);
    virtual void OnUpdateUITags2(wxUpdateUIEvent& event);
    virtual void OnTextFirstUI(wxUpdateUIEvent& event);
    virtual void OnTextSecondUI(wxUpdateUIEvent& event);
};
#endif // GITDIFFCHOOSECOMMITISHDLG_H
