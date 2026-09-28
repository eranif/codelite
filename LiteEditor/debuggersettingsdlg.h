//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : debuggersettingsdlg.h
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
#ifndef __debuggersettingsdlg__
#define __debuggersettingsdlg__

#include "Debugger/debuggersettings.h"
#include "debuggersettingsbasedlg.hpp"
#include "filepicker.h"

#include <vector>

class DebuggerSettingsDlg;
class wxCheckBox;
class DebuggerPageBase;

///////////////////////////////////////////////////
// General Page
///////////////////////////////////////////////////
class DebuggerPage : public DbgPageGeneralBase
{
    friend class DebuggerSettingsDlg;
    wxString m_title;

protected:
    void OnSuperuserUI(wxUpdateUIEvent& event) override;
    void OnBrowse(wxCommandEvent& e) override;
    void OnDebugAssert(wxCommandEvent& e);

    virtual void OnWindowsUI(wxUpdateUIEvent& event);

public:
    DebuggerPage(wxWindow* parent, wxString title);
    ~DebuggerPage() override = default;
};

///////////////////////////////////////////////////
// Misc Page
///////////////////////////////////////////////////
class DebuggerPageMisc : public DbgPageMiscBase
{
    friend class DebuggerSettingsDlg;
    wxString m_title;

public:
    void OnDebugAssert(wxCommandEvent& event) override;
    void OnWindowsUI(wxUpdateUIEvent& event) override;

    DebuggerPageMisc(wxWindow* parent, const wxString& title);
    ~DebuggerPageMisc() override = default;
};

///////////////////////////////////////////////////
// Startup Commands Page
///////////////////////////////////////////////////
class DebuggerPageStartupCmds : public DbgPageStartupCmdsBase
{
    friend class DebuggerSettingsDlg;
    wxString m_title;

public:
    DebuggerPageStartupCmds(wxWindow* parent, const wxString& title);
    ~DebuggerPageStartupCmds() override = default;
};

///////////////////////////////////////////////////
// PreDefined types Page
///////////////////////////////////////////////////
class DbgPagePreDefTypes : public DbgPagePreDefTypesBase
{
    friend class DebuggerSettingsDlg;

public:
    DbgPagePreDefTypes(wxWindow* parent);
    ~DbgPagePreDefTypes() override = default;

    void OnDeleteSet(wxCommandEvent& event) override;
    void OnDeleteSetUI(wxUpdateUIEvent& event) override;
    void OnNewSet(wxCommandEvent& event) override;

    void Save();
};

/** Implementing DebuggerSettingsBaseDlg */
class DebuggerSettingsDlg : public DebuggerSettingsBaseDlg
{
    std::vector<wxWindow*> m_pages;

protected:
    void Initialize();
    void OnOk(wxCommandEvent& e) override;
    void OnButtonCancel(wxCommandEvent& e) override;

public:
    /** Constructor */
    DebuggerSettingsDlg(wxWindow* parent);
    ~DebuggerSettingsDlg() override = default;
};

class NewPreDefinedSetDlg : public NewPreDefinedSetBaseDlg
{
protected:
public:
    NewPreDefinedSetDlg(wxWindow* parent)
        : NewPreDefinedSetBaseDlg(parent)
    {
    }
    ~NewPreDefinedSetDlg() override = default;

    wxTextCtrl* GetNameTextctl() { return m_textCtrlName; }
    wxChoice* GetChoiceCopyFrom() { return m_choiceCopyFrom; }
    wxCheckBox* GetCheckBoxMakeActive() { return m_checkBoxMakeActive; }
};

#endif // __debuggersettingsdlg__
