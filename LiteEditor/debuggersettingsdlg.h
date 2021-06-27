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

#include "debuggersettings.h"
#include "debuggersettingsbasedlg.h"
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
    virtual void OnSuperuserUI(wxUpdateUIEvent& event);
    void OnBrowse(wxCommandEvent& e);
    void OnDebugAssert(wxCommandEvent& e);

    virtual void OnWindowsUI(wxUpdateUIEvent& event);

public:
    DebuggerPage(wxWindow* parent, wxString title);
    virtual ~DebuggerPage();
};

///////////////////////////////////////////////////
// Misc Page
///////////////////////////////////////////////////
class DebuggerPageMisc : public DbgPageMiscBase
{
    friend class DebuggerSettingsDlg;
    wxString m_title;

public:
    virtual void OnDebugAssert(wxCommandEvent& event);
    virtual void OnWindowsUI(wxUpdateUIEvent& event);

    DebuggerPageMisc(wxWindow* parent, const wxString& title);
    virtual ~DebuggerPageMisc();
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
    virtual ~DebuggerPageStartupCmds();
};

///////////////////////////////////////////////////
// PreDefined types Page
///////////////////////////////////////////////////
class DbgPagePreDefTypes : public DbgPagePreDefTypesBase
{
    friend class DebuggerSettingsDlg;

public:
    DbgPagePreDefTypes(wxWindow* parent);
    virtual ~DbgPagePreDefTypes();

    virtual void OnDeleteSet(wxCommandEvent& event);
    virtual void OnDeleteSetUI(wxUpdateUIEvent& event);
    virtual void OnNewSet(wxCommandEvent& event);

    void Save();
};

/** Implementing DebuggerSettingsBaseDlg */
class DebuggerSettingsDlg : public DebuggerSettingsBaseDlg
{
    std::vector<wxWindow*> m_pages;

protected:
    void Initialize();
    void OnOk(wxCommandEvent& e);
    void OnButtonCancel(wxCommandEvent& e);

public:
    /** Constructor */
    DebuggerSettingsDlg(wxWindow* parent);
    virtual ~DebuggerSettingsDlg();
};

class NewPreDefinedSetDlg : public NewPreDefinedSetBaseDlg
{
protected:
public:
    NewPreDefinedSetDlg(wxWindow* parent)
        : NewPreDefinedSetBaseDlg(parent)
    {
    }
    virtual ~NewPreDefinedSetDlg() {}

    wxTextCtrl* GetNameTextctl() { return m_textCtrlName; }
    wxChoice* GetChoiceCopyFrom() { return m_choiceCopyFrom; }
    wxCheckBox* GetCheckBoxMakeActive() { return m_checkBoxMakeActive; }
};

#endif // __debuggersettingsdlg__
