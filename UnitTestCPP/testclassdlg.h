//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : testclassdlg.h
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

#ifndef __testclassdlg__
#define __testclassdlg__

#include "testclassbasedlg.h"
#include "entry.h"
#include <vector>
#include "wxStringHash.h"
#include <unordered_map>

class IManager;
class UnitTestPP;

class TestClassDlg : public TestClassBaseDlg
{
    IManager* m_manager;
    UnitTestPP* m_plugin;
    std::unordered_map<wxString, TagEntryPtrVector_t> m_tags;

protected:
    virtual void OnClassNameUpdated(wxCommandEvent& event);
    // Handlers for TestClassBaseDlg events.
    void OnRefreshFunctions(wxCommandEvent& event);
    void OnUseActiveEditor(wxCommandEvent& event);
    void OnRefreshButtonUI(wxUpdateUIEvent& e);
    void OnCheckAll(wxCommandEvent& e);
    void OnUnCheckAll(wxCommandEvent& e);
    void OnUseFixture(wxCommandEvent& e);
    void OnButtonOk(wxCommandEvent& e);
    void OnShowClassListDialog(wxCommandEvent& e);
    void DoRefreshFunctions(bool reportError = true);
    void EscapeName(wxString& name);

public:
    /** Constructor */
    TestClassDlg(wxWindow* parent, IManager* mgr, UnitTestPP* plugin);
    virtual ~TestClassDlg();
    wxArrayString GetTestsList();

    wxString GetFileName() { return m_textCtrlFileName->GetValue(); }

    wxString GetFixtureName() { return m_textCtrlFixtureName->GetValue(); }

    void SetClassName(const wxString& clsName);

    wxString GetProjectName() { return m_choiceProjects->GetStringSelection(); }
};

#endif // __testclassdlg__
