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

#include "newclassbasedlg.h"
#include "vector"
#include "new_class_dlg_data.h"

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
    bool isAssingable;
    bool isVirtualDtor;
    bool implAllPureVirtual;
    bool implAllVirtual;
    bool isInline;
    bool hppHeader;
    bool usePragmaOnce;
    std::vector<ClassParentInfo> parents;

    NewClassInfo()
        : isSingleton(false)
        , isAssingable(false)
        , isVirtualDtor(false)
        , implAllPureVirtual(false)
        , implAllVirtual(false)
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

protected:
    virtual void OnUseLowerCaseFileName(wxCommandEvent& event);
    virtual void OnBlockGuardUI(wxUpdateUIEvent& event);
    // Handlers for NewClassBaseDlg events.
    void OnListItemActivated(wxListEvent& event);
    void OnListItemSelected(wxListEvent& event);
    void OnButtonAdd(wxCommandEvent& event);
    void OnButtonDelete(wxCommandEvent& event);
    void OnButtonDeleteUI(wxUpdateUIEvent& event);
    void OnListItemDeSelected(wxListEvent& e);
    void OnButtonOK(wxCommandEvent& e);
    bool ValidateInput();
    void OnTextEnter(wxCommandEvent& e);
    void OnCheckImpleAllVirtualFunctions(wxCommandEvent& e);
    void OnBrowseFolder(wxCommandEvent& e);
    void OnBrowseVD(wxCommandEvent& e);
    void OnBrowseNamespace(wxCommandEvent& e);
    void OnCheckInline(wxCommandEvent& e);
    void OnOkUpdateUI(wxUpdateUIEvent& event);

    wxString doSpliteByCaptilization(const wxString& str);
    void DoUpdateGeneratedPath();
    void DoSaveOptions();
    wxString CreateFileName() const;

public:
    /** Constructor */
    NewClassDlg(wxWindow* parent, IManager* mgr);
    virtual ~NewClassDlg();

    void GetNewClassInfo(NewClassInfo& info);

    void GetInheritance(std::vector<ClassParentInfo>& inheritVec);
    bool IsSingleton() { return m_checkBoxSingleton->GetValue(); }
    wxString GetClassName() { return m_textClassName->GetValue(); }
    wxString GetClassNamespace() const { return m_textCtrlNamespace->GetValue(); }
    wxString GetClassPath();
    wxString GetClassFile();
    bool IsCopyableClass() { return !m_checkBoxCopyable->IsChecked(); }
    wxString GetVirtualDirectoryPath() { return m_textCtrlVD->GetValue(); }
    void GetNamespacesList(wxArrayString& namespacesArray);
    bool IsInline() const { return m_checkBoxInline->GetValue(); }
    bool HppHeader() const { return m_checkBoxHpp->GetValue(); }
};

#endif // __newclassdlg__
