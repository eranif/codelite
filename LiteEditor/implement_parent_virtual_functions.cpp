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

#include "implement_parent_virtual_functions.h"
#include "editor_config.h"
#include "implparentvirtualfunctionsdata.h"
#include <wx/tokenzr.h>
#include "ctags_manager.h"
#include "cpp_comment_creator.h"
#include "windowattrmanager.h"
#include "context_cpp.h"
#include <wx/settings.h>
#include "commentconfigdata.h"

class ImplFuncModel : public FunctionsModel
{
public:
    ImplFuncModel() {}
    virtual ~ImplFuncModel() {}

    virtual bool GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const
    {
        if(col == 1) {
            clFunctionImplDetails* cd = dynamic_cast<clFunctionImplDetails*>(GetClientObject(item));
            if(!cd->IsSelected()) {
                attr.SetColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
                attr.SetBold(false);

            } else {
                attr.SetBold(true);
            }
            return true;
        } else {
            return false;
        }
    }
};

ImplementParentVirtualFunctionsDialog::ImplementParentVirtualFunctionsDialog(wxWindow* parent,
                                                                             const wxString& scopeName,
                                                                             const std::vector<TagEntryPtr>& tags,
                                                                             wxChar doxyPrefix,
                                                                             ContextCpp* contextCpp)
    : ImplementParentVirtualFunctionsBase(parent)
    , m_tags(tags)
    , m_doxyPrefix(doxyPrefix)
    , m_contextCpp(contextCpp)
    , m_scope(scopeName)
{
    unsigned int colCount = m_dataviewModel->GetColumnCount();
    m_dataviewModel = new ImplFuncModel();
    m_dataviewModel->SetColCount(colCount);
    m_dataview->AssociateModel(m_dataviewModel.get());

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

    if(m_checkBoxFormat->IsChecked()) flags |= ImplParentVirtualFunctionsData::FormatText;

    data.SetFlags(flags);
    EditorConfigST::Get()->WriteObject(wxT("ImplParentVirtualFunctionsData"), &data);
}

void ImplementParentVirtualFunctionsDialog::DoInitialize(bool updateDoxyOnly)
{
    m_dataviewModel->Clear();

    wxVector<wxVariant> cols;

    // Add declration
    //////////////////////////////////////////////////////
    for(size_t i = 0; i < m_tags.size(); ++i) {
        cols.clear();
        cols.push_back(true);                           // generate it
        cols.push_back(m_tags.at(i)->GetDisplayName()); // function name
        cols.push_back("public");                       // visibility
        cols.push_back(true);                           // virtual
        cols.push_back(false);                          // doxy

        clFunctionImplDetails* cd = new clFunctionImplDetails();
        cd->SetTag(m_tags.at(i));
        m_dataviewModel->AppendItem(wxDataViewItem(0), cols, cd);
    }
}

wxString ImplementParentVirtualFunctionsDialog::GetDecl(const wxString& visibility)
{
    wxString decl;
    wxDataViewItemArray children;
    m_dataviewModel->GetChildren(wxDataViewItem(0), children);

    for(size_t i = 0; i < children.GetCount(); ++i) {
        clFunctionImplDetails* cd =
            dynamic_cast<clFunctionImplDetails*>(m_dataviewModel->GetClientObject(children.Item(i)));
        if(cd->GetVisibility() == visibility) {
            decl << cd->GetDecl(this);
        }
    }
    return decl;
}

wxString ImplementParentVirtualFunctionsDialog::GetImpl()
{
    wxString impl;
    wxDataViewItemArray children;
    m_dataviewModel->GetChildren(wxDataViewItem(0), children);

    for(size_t i = 0; i < children.GetCount(); ++i) {
        clFunctionImplDetails* cd =
            dynamic_cast<clFunctionImplDetails*>(m_dataviewModel->GetClientObject(children.Item(i)));
        impl << cd->GetImpl(this);
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
    m_contextCpp->DoMakeDoxyCommentString(dc, data.GetCommentBlockPrefix());

    // Format the comment
    wxString textComment = dc.comment;
    textComment.Replace("\r", "\n");
    wxArrayString lines = wxStringTokenize(textComment, "\n", wxTOKEN_STRTOK);
    textComment.Clear();

    for(size_t i = 0; i < lines.GetCount(); ++i) textComment << lines.Item(i) << wxT("\n");
    return textComment;
}

void ImplementParentVirtualFunctionsDialog::SetTargetFile(const wxString& file)
{
    m_textCtrlImplFile->ChangeValue(file);
}

void ImplementParentVirtualFunctionsDialog::OnValueChanged(wxDataViewEvent& event)
{
    event.Skip();
    clFunctionImplDetails* cd = dynamic_cast<clFunctionImplDetails*>(m_dataviewModel->GetClientObject(event.GetItem()));
    wxVector<wxVariant> cols = m_dataviewModel->GetItemColumnsData(event.GetItem());
    cd->SetVisibility(cols.at(2).GetString());
    cd->SetPrependVirtualKeyword(cols.at(3).GetBool());
    cd->SetSelected(cols.at(0).GetBool());
    cd->SetDoxygen(cols.at(4).GetBool());
}

void ImplementParentVirtualFunctionsDialog::OnCheckAll(wxCommandEvent& event)
{
    wxDataViewItemArray items;
    m_dataviewModel->GetChildren(wxDataViewItem(0), items);

    for(size_t i = 0; i < items.GetCount(); ++i) {
        wxVector<wxVariant> cols = m_dataviewModel->GetItemColumnsData(items.Item(i));
        cols.at(0) = true;
        m_dataviewModel->UpdateItem(items.Item(i), cols);
    }
}

void ImplementParentVirtualFunctionsDialog::OnUnCheckAll(wxCommandEvent& event)
{
    wxDataViewItemArray items;
    m_dataviewModel->GetChildren(wxDataViewItem(0), items);

    for(size_t i = 0; i < items.GetCount(); ++i) {
        wxVector<wxVariant> cols = m_dataviewModel->GetItemColumnsData(items.Item(i));
        cols.at(0) = false;
        m_dataviewModel->UpdateItem(items.Item(i), cols);
    }
}

// -----------------------------------------------------------------------------------------
wxString clFunctionImplDetails::GetImpl(ImplementParentVirtualFunctionsDialog* dlg) const
{
    if(!IsSelected()) return "";

    wxString impl;
    m_tag->SetScope(dlg->m_scope);
    impl << TagsManagerST::Get()->FormatFunction(m_tag, FunctionFormat_Impl) << wxT("\n");
    impl.Trim().Trim(false);
    impl << "\n\n";

    return impl;
}

wxString clFunctionImplDetails::GetDecl(ImplementParentVirtualFunctionsDialog* dlg) const
{
    if(!IsSelected()) return "";

    wxString decl;
    if(IsDoxygen()) {
        decl << dlg->DoMakeCommentForTag(m_tag);
    }

    m_tag->SetScope(dlg->m_scope);
    decl << TagsManagerST::Get()->FormatFunction(m_tag, IsPrependVirtualKeyword() ? FunctionFormat_WithVirtual : 0);
    decl.Trim().Trim(false);
    decl << "\n";
    return decl;
}
