//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : implement_parent_virtual_functions.cpp
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

#include "commentconfigdata.h"
#include "context_cpp.h"
#include "cpp_comment_creator.h"
#include "ctags_manager.h"
#include "editor_config.h"
#include "globals.h"
#include "implement_parent_virtual_functions.h"
#include "implparentvirtualfunctionsdata.h"
#include "windowattrmanager.h"
#include <wx/settings.h>
#include <wx/tokenzr.h>

ImplementParentVirtualFunctionsDialog::ImplementParentVirtualFunctionsDialog(wxWindow* parent,
                                                                             const wxString& scopeName,
                                                                             const std::vector<TagEntryPtr>& tags,
                                                                             wxChar doxyPrefix, ContextCpp* contextCpp)
    : ImplementParentVirtualFunctionsBase(parent)
    , m_tags(tags)
    , m_doxyPrefix(doxyPrefix)
    , m_contextCpp(contextCpp)
    , m_scope(scopeName)
{
    SetName("ImplementParentVirtualFunctionsDialog");
    WindowAttrManager::Load(this);
    ImplParentVirtualFunctionsData data;
    EditorConfigST::Get()->ReadObject(wxT("ImplParentVirtualFunctionsData"), &data);
    m_checkBoxFormat->SetValue(data.GetFlags() & ImplParentVirtualFunctionsData::FormatText);
    DoInitialize(false);
}

ImplementParentVirtualFunctionsDialog::~ImplementParentVirtualFunctionsDialog()
{
    ImplParentVirtualFunctionsData data;
    size_t flags(0);
    if(m_checkBoxFormat->IsChecked()) { flags |= ImplParentVirtualFunctionsData::FormatText; }
    data.SetFlags(flags);
    EditorConfigST::Get()->WriteObject(wxT("ImplParentVirtualFunctionsData"), &data);
    Clear();
}

void ImplementParentVirtualFunctionsDialog::DoInitialize(bool updateDoxyOnly)
{
    Clear();
    wxVector<wxVariant> cols;

    // Add declration
    //////////////////////////////////////////////////////
    for(size_t i = 0; i < m_tags.size(); ++i) {
        cols.clear();
        cols.push_back(::MakeCheckboxVariant(m_tags.at(i)->GetDisplayName(), true, wxNOT_FOUND)); // generate it, 0
        cols.push_back("public");                                                                 // visibility, 1
        cols.push_back(true);                                                                     // virtual, 2
        cols.push_back(false);                                                                    // document, 3
        m_dvListCtrl->AppendItem(cols, (wxUIntPtr)i);
    }
}

wxString ImplementParentVirtualFunctionsDialog::GetDecl(const wxString& visibility)
{
    wxString decl;

    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        clFunctionImplDetails details;
        UpdateDetailsForRow(details, i); // Update the values according to the user choices
        if(details.GetVisibility() == visibility) { decl << details.GetDecl(this); }
    }
    return decl;
}

wxString ImplementParentVirtualFunctionsDialog::GetImpl()
{
    wxString impl;
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        clFunctionImplDetails details;
        UpdateDetailsForRow(details, i); // Update the values according to the user choices
        impl << details.GetImpl(this);
    }
    return impl;
}

wxString ImplementParentVirtualFunctionsDialog::DoMakeCommentForTag(TagEntryPtr tag) const
{
    // Add doxygen comment
    CommentConfigData data;
    EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

    CppCommentCreator commentCreator(tag, m_doxyPrefix);
    DoxygenComment dc;
    dc.comment = commentCreator.CreateComment();
    dc.name = tag->GetName();
    m_contextCpp->DoMakeDoxyCommentString(dc, data.GetCommentBlockPrefix(), m_doxyPrefix);

    // Format the comment
    wxString textComment = dc.comment;
    textComment.Replace("\r", "\n");
    wxArrayString lines = wxStringTokenize(textComment, "\n", wxTOKEN_STRTOK);
    textComment.Clear();

    for(size_t i = 0; i < lines.GetCount(); ++i)
        textComment << lines.Item(i) << wxT("\n");
    return textComment;
}

void ImplementParentVirtualFunctionsDialog::SetTargetFile(const wxString& file)
{
    m_textCtrlImplFile->ChangeValue(file);
}

void ImplementParentVirtualFunctionsDialog::UpdateDetailsForRow(clFunctionImplDetails& details, size_t row)
{
    wxDataViewItem item = m_dvListCtrl->RowToItem(row);
    details.SetSelected(m_dvListCtrl->IsItemChecked(item, 0));
    details.SetVisibility(m_dvListCtrl->GetItemText(item, 1));
    details.SetPrependVirtualKeyword(m_dvListCtrl->IsItemChecked(item, 2));
    details.SetDoxygen(m_dvListCtrl->IsItemChecked(item, 3));
    details.SetTagIndex(m_dvListCtrl->GetItemData(item));
}

void ImplementParentVirtualFunctionsDialog::OnCheckAll(wxCommandEvent& event) { DoCheckAll(true); }

void ImplementParentVirtualFunctionsDialog::OnUnCheckAll(wxCommandEvent& event) { DoCheckAll(false); }

void ImplementParentVirtualFunctionsDialog::Clear() { m_dvListCtrl->DeleteAllItems(); }

TagEntryPtr ImplementParentVirtualFunctionsDialog::GetAssociatedTag(const wxDataViewItem& item) const
{
    int index = (int)m_dvListCtrl->GetItemData(item);
    return GetTag(index);
}

TagEntryPtr ImplementParentVirtualFunctionsDialog::GetAssociatedTag(size_t row) const
{
    return GetAssociatedTag(m_dvListCtrl->RowToItem(row));
}

// -----------------------------------------------------------------------------------------
wxString clFunctionImplDetails::GetImpl(ImplementParentVirtualFunctionsDialog* dlg) const
{
    if(!IsSelected()) { return ""; };

    TagEntryPtr tag = dlg->GetTag(m_tagIndex);
    if(!tag) { return ""; }

    wxString impl;
    tag->SetScope(dlg->m_scope);
    impl << TagsManagerST::Get()->FormatFunction(tag, FunctionFormat_Impl) << wxT("\n");
    impl.Trim().Trim(false);
    impl << "\n\n";

    return impl;
}

void ImplementParentVirtualFunctionsDialog::DoCheckAll(bool check)
{
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        m_dvListCtrl->SetItemChecked(m_dvListCtrl->RowToItem(i), check, 0);
    }
}

TagEntryPtr ImplementParentVirtualFunctionsDialog::GetTag(size_t index) const
{
    if(index >= m_tags.size()) { return TagEntryPtr(NULL); }
    return m_tags[index];
}

wxString clFunctionImplDetails::GetDecl(ImplementParentVirtualFunctionsDialog* dlg) const
{
    if(!IsSelected()) { return ""; };

    TagEntryPtr tag = dlg->GetTag(m_tagIndex);
    if(!tag) { return ""; }

    wxString decl;
    if(IsDoxygen()) { decl << dlg->DoMakeCommentForTag(tag); }

    tag->SetScope(dlg->m_scope);
    decl << TagsManagerST::Get()->FormatFunction(tag, IsPrependVirtualKeyword() ? FunctionFormat_WithVirtual : 0);
    decl.Trim().Trim(false);
    decl << "\n";
    return decl;
}
