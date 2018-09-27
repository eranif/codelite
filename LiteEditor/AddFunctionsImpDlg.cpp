//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : AddFunctionsImpDlg.cpp
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

#include "AddFunctionsImpDlg.h"
#include "ctags_manager.h"
#include "windowattrmanager.h"

class MyAddFunctionsModel : public AddFunctionsModel
{
public:
    MyAddFunctionsModel() {}
    virtual ~MyAddFunctionsModel() {}

    virtual bool GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const
    {

        if(col == 1) {
            wxVariant v;
            GetValue(v, item, 0);
            if(!v.GetBool()) {
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

AddFunctionsImpDlg::AddFunctionsImpDlg(wxWindow* parent, const TagEntryPtrVector_t& tags, const wxString& targetFile)
    : AddFunctionsImplBaseDlg(parent)
{
    unsigned int colCount = m_dataviewModel->GetColCount();
    m_dataviewModel = new MyAddFunctionsModel();
    m_dataviewModel->SetColCount(colCount);
    m_dataview->AssociateModel(m_dataviewModel.get());

    m_tags.insert(m_tags.end(), tags.begin(), tags.end());
    for(size_t i = 0; i < m_tags.size(); ++i) {

        wxVector<wxVariant> cols;
        cols.push_back(true);
        cols.push_back(m_tags.at(i)->GetDisplayName());

        // keep the implementation as the client data
        wxString body;
        TagEntryPtr tag = m_tags.at(i);
        tag->SetSignature(TagsManagerST::Get()->NormalizeFunctionSig(
            tag->GetSignature(), Normalize_Func_Name | Normalize_Func_Reverse_Macro));
        body << TagsManagerST::Get()->FormatFunction(tag, FunctionFormat_Impl);
        body << wxT("\n");
        m_dataviewModel->AppendItem(wxDataViewItem(0), cols, new wxStringClientData(body));
    }
    m_filePicker->SetPath(targetFile);
    SetName("AddFunctionsImpDlg");
    WindowAttrManager::Load(this);
}

AddFunctionsImpDlg::~AddFunctionsImpDlg() {}

wxString AddFunctionsImpDlg::GetFileName() const { return m_filePicker->GetPath(); }

wxString AddFunctionsImpDlg::GetText() const
{
    wxString text;
    wxDataViewItemArray items;
    m_dataviewModel->GetChildren(wxDataViewItem(0), items);

    for(size_t i = 0; i < items.GetCount(); ++i) {
        wxVector<wxVariant> cols = m_dataviewModel->GetItemColumnsData(items.Item(i));
        if(cols.at(0).GetBool()) {
            if(text.IsEmpty()) text << "\n";
            // checked
            wxStringClientData* cd = dynamic_cast<wxStringClientData*>(m_dataviewModel->GetClientObject(items.Item(i)));
            if(cd) { text << cd->GetData(); }
        }
    }

    if(!text.IsEmpty()) text.RemoveLast();
    return text;
}

void AddFunctionsImpDlg::OnCheckAll(wxCommandEvent& event)
{
    wxDataViewItemArray items;
    m_dataviewModel->GetChildren(wxDataViewItem(0), items);

    for(size_t i = 0; i < items.GetCount(); ++i) {
        wxVector<wxVariant> cols = m_dataviewModel->GetItemColumnsData(items.Item(i));
        cols.at(0) = true;
        m_dataviewModel->UpdateItem(items.Item(i), cols);
    }
}

void AddFunctionsImpDlg::OnUncheckAll(wxCommandEvent& event)
{
    wxDataViewItemArray items;
    m_dataviewModel->GetChildren(wxDataViewItem(0), items);

    for(size_t i = 0; i < items.GetCount(); ++i) {
        wxVector<wxVariant> cols = m_dataviewModel->GetItemColumnsData(items.Item(i));
        cols.at(0) = false;
        m_dataviewModel->UpdateItem(items.Item(i), cols);
    }
}

void AddFunctionsImpDlg::OnOKUI(wxUpdateUIEvent& event)
{
    wxDataViewItemArray items;
    m_dataviewModel->GetChildren(wxDataViewItem(0), items);

    bool atLeastOneChecked = false;
    for(size_t i = 0; i < items.GetCount(); ++i) {
        wxVector<wxVariant> cols = m_dataviewModel->GetItemColumnsData(items.Item(i));
        if(cols.at(0).GetBool()) {
            atLeastOneChecked = true;
            break;
        }
    }

    event.Enable(atLeastOneChecked);
}
