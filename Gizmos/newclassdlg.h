//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : newclassdlg.h
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
#ifndef __newclassdlg__
#define __newclassdlg__

#include "new_class_dlg_data.h"
#include "newclassbasedlg.h"

class IManager;

struct ClassParentInfo {
    wxString name;
    wxString access;
    wxString fileName;
};

struct NewClassInfo {
    wxString name;
    wxArrayString namespacesList;
    wxString blockGuard;
    wxString path;
    wxString fileName;
    wxString virtualDirectory;
    bool isSingleton = false;
    bool isAssignable = false;
    bool isMovable = false;
    bool isInheritable = false;
    bool isVirtualDtor = false;
    bool isInline = false;
    bool hppHeader = false;
    bool usePragmaOnce = false;
    ClassParentInfo parents;
};

/** Implementing NewClassBaseDlg */
class NewClassDlg : public NewClassBaseDlg
{
    IManager* m_mgr;
    wxString m_basePath;
    NewClassDlgData m_options;
    wxString m_parentClass;

protected:
    void OnCheckSingleton(wxCommandEvent& event) override;
    void OnUseLowerCaseFileName(wxCommandEvent& event) override;
    void OnBlockGuardUI(wxUpdateUIEvent& event) override;
    // Handlers for NewClassBaseDlg events.
    void OnButtonOK(wxCommandEvent& e) override;
    void OnTextEnter(wxCommandEvent& e) override;
    void OnBrowseFolder(wxCommandEvent& e) override;
    void OnBrowseVD(wxCommandEvent& e) override;

    void OnBrowseParentClass(wxCommandEvent& event);
    bool ValidateInput();
    void OnBrowseNamespace(wxCommandEvent& e);
    void OnOkUpdateUI(wxUpdateUIEvent& event);

    void DoUpdateGeneratedPath();
    void DoUpdateCheckBoxes();
    void DoSaveOptions();
    wxString CreateFileName() const;

public:
    /** Constructor */
    NewClassDlg(wxWindow* parent, IManager* mgr);
    ~NewClassDlg() override;

    void GetNewClassInfo(NewClassInfo& info) const;

    void GetInheritance(ClassParentInfo& inheritVec) const;
    bool IsSingleton() const { return m_checkBoxSingleton->GetValue(); }
    wxString GetClassName() const { return m_textClassName->GetValue(); }
    wxString GetClassNamespace() const { return m_textCtrlNamespace->GetValue(); }
    wxString GetClassPath() const;
    wxString GetClassFile() const;
    bool IsCopyableClass() const { return !m_checkBoxNonCopyable->IsChecked(); }
    bool IsMovableClass() const { return !m_checkBoxNonMovable->IsChecked(); }
    bool IsInheritable() const { return !m_checkBoxNonInheritable->IsChecked(); }
    wxString GetVirtualDirectoryPath() const { return m_textCtrlVD->GetValue(); }
    void GetNamespacesList(wxArrayString& namespacesArray) const;
    bool IsInline() const { return m_checkBoxInline->GetValue(); }
    bool HppHeader() const { return m_checkBoxHpp->GetValue(); }
};

#endif // __newclassdlg__
