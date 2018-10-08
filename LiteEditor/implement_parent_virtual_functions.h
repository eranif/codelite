//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : implement_parent_virtual_functions.h
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

#ifndef __implement_parent_virtual_functions__
#define __implement_parent_virtual_functions__

#include "entry.h"
#include "implementparentvirtualfunctionsbase.h"
#include <vector>
#include <wx/clntdata.h>

class ContextCpp;
class ImplementParentVirtualFunctionsDialog;

class clFunctionImplDetails : public wxClientData
{
protected:
    wxString m_visibility = "public";
    bool m_prependVirtualKeyword = true;
    int m_tagIndex = wxNOT_FOUND;
    bool m_selected = true;
    bool m_doxygen = true;

    friend class ImplementParentVirtualFunctionsDialog;

protected:
    clFunctionImplDetails() {}
    virtual ~clFunctionImplDetails() {}

public:
    void SetVisibility(const wxString& visibility) { this->m_visibility = visibility; }
    const wxString& GetVisibility() const { return m_visibility; }
    void SetPrependVirtualKeyword(bool prependVirtualKeyword) { this->m_prependVirtualKeyword = prependVirtualKeyword; }
    bool IsPrependVirtualKeyword() const { return m_prependVirtualKeyword; }
    void SetSelected(bool selected) { this->m_selected = selected; }
    bool IsSelected() const { return m_selected; }
    void SetDoxygen(bool doxygen) { this->m_doxygen = doxygen; }
    bool IsDoxygen() const { return m_doxygen; }

    void SetTagIndex(int tagIndex) { this->m_tagIndex = tagIndex; }
    wxString GetImpl(ImplementParentVirtualFunctionsDialog* dlg) const;
    wxString GetDecl(ImplementParentVirtualFunctionsDialog* dlg) const;
};

// ------------------------------------------------------------

class ImplementParentVirtualFunctionsDialog : public ImplementParentVirtualFunctionsBase
{
    std::vector<TagEntryPtr> m_tags;
    wxChar m_doxyPrefix;
    ContextCpp* m_contextCpp;
    wxString m_scope;
    friend class clFunctionImplDetails;

protected:
    void OnCheckAll(wxCommandEvent& event);
    void OnUnCheckAll(wxCommandEvent& event);
    void DoCheckAll(bool check);
    void UpdateDetailsForRow(clFunctionImplDetails& details, size_t row);
    void DoInitialize(bool updateDoxyOnly);
    void Clear();

    TagEntryPtr GetAssociatedTag(const wxDataViewItem& item) const;
    TagEntryPtr GetAssociatedTag(size_t row) const;

    wxString DoMakeCommentForTag(TagEntryPtr tag) const;

public:
    /** Constructor */
    ImplementParentVirtualFunctionsDialog(wxWindow* parent, const wxString& scopeName,
                                          const std::vector<TagEntryPtr>& tags, wxChar doxyPrefix /* @ or \\ */,
                                          ContextCpp* contextCpp);
    virtual ~ImplementParentVirtualFunctionsDialog();

    wxString GetDecl(const wxString& visibility);
    wxString GetImpl();

    void SetTargetFile(const wxString& file);
    wxString GetTargetFile() const { return m_textCtrlImplFile->GetValue(); }

    bool IsFormatAfterInsert() const { return m_checkBoxFormat->IsChecked(); }
    TagEntryPtr GetTag(size_t index) const;
};

#endif // __implement_parent_virtual_functions__
