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
#include "vector"

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
    bool isSingleton;
    bool isAssignable;
    bool isMovable;
    bool isInheritable;
    bool isVirtualDtor;
    bool isInline;
    bool hppHeader;
    bool usePragmaOnce;
    ClassParentInfo parents;

    NewClassInfo()
        : isSingleton(false)
        , isAssignable(false)
        , isMovable(false)
        , isInheritable(false)
        , isVirtualDtor(false)
        , isInline(false)
        , hppHeader(false)
        , usePragmaOnce(false)
    {
    }

    ~NewClassInfo() {}
};

/** Implementing NewClassBaseDlg */
class NewClassDlg : public NewClassBaseDlg
{
    long m_selectedItem;
    IManager* m_mgr;
    wxString m_basePath;
    NewClassDlgData m_options;
    wxString m_parentClass;

protected:
    virtual void OnCheckSingleton(wxCommandEvent& event);
    virtual void OnCheckImpleAllVirtualFunctions(wxCommandEvent& event);
    virtual void OnUseLowerCaseFileName(wxCommandEvent& event);
    virtual void OnBlockGuardUI(wxUpdateUIEvent& event);
    void OnBrowseParentClass(wxCommandEvent& event);
    // Handlers for NewClassBaseDlg events.
    void OnButtonOK(wxCommandEvent& e);
    bool ValidateInput();
    void OnTextEnter(wxCommandEvent& e);
    void OnBrowseFolder(wxCommandEvent& e);
    void OnBrowseVD(wxCommandEvent& e);
    void OnBrowseNamespace(wxCommandEvent& e);
    void OnOkUpdateUI(wxUpdateUIEvent& event);

    wxString doSpliteByCaptilization(const wxString& str) const;
    void DoUpdateGeneratedPath();
    void DoUpdateCheckBoxes();
    void DoSaveOptions();
    wxString CreateFileName() const;

public:
    /** Constructor */
    NewClassDlg(wxWindow* parent, IManager* mgr);
    virtual ~NewClassDlg();

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
