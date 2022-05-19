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

#include "ColoursAndFontsManager.h"
#include "ctags_manager.h"
#include "globals.h"
#include "windowattrmanager.h"

AddFunctionsImpDlg::AddFunctionsImpDlg(wxWindow* parent, const TagEntryPtrVector_t& tags, const wxString& targetFile)
    : AddFunctionsImplBaseDlg(parent)
{
    m_dvListCtrl->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    m_dvListCtrl->SetDefaultFont(lexer->GetFontForStyle(0, m_dvListCtrl));

    int functionIndex = clGetManager()->GetStdIcons()->GetMimeImageId(BitmapLoader::kFunctionPublic);
    m_dvListCtrl->SetSortFunction(nullptr); // Disable sorting

    // Keep the tags
    m_tags = tags;
    // Clear the impl array
    m_implArr.Clear();
    m_implArr.reserve(tags.size());
    for(size_t i = 0; i < m_tags.size(); ++i) {
        wxVector<wxVariant> cols;
        cols.push_back(::MakeCheckboxVariant(m_tags.at(i)->GetDisplayName(), true, functionIndex));

        // keep the implementation as the client data
        TagEntryPtr tag = m_tags[i];
        wxString body = tag->GetFunctionDefinition();
        if(tag->GetTypename() == "std::string" || tag->GetTypename() == "string") {
            body << "\n{ return \"\"; }";
        } else if(tag->GetTypename() == "bool") {
            body << "\n{ return false; }";
        } else {
            body << "\n{}";
        }

        m_implArr.Add(body + "\n");
        m_dvListCtrl->AppendItem(cols, (wxUIntPtr)&m_implArr.Item(i));
    }
    m_filePicker->SetPath(targetFile);
    ::clSetDialogBestSizeAndPosition(this);
}

AddFunctionsImpDlg::~AddFunctionsImpDlg() {}

wxString AddFunctionsImpDlg::GetFileName() const { return m_filePicker->GetPath(); }

wxString AddFunctionsImpDlg::GetText() const
{
    wxString text;
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrl->RowToItem(i);
        if(m_dvListCtrl->IsItemChecked(item, 0)) {
            if(text.IsEmpty())
                text << "\n";
            // checked
            wxString* impl = reinterpret_cast<wxString*>(m_dvListCtrl->GetItemData(item));
            if(impl) {
                text << (*impl);
            }
        }
    }

    if(!text.IsEmpty()) {
        text.RemoveLast();
    }
    return text;
}

void AddFunctionsImpDlg::OnCheckAll(wxCommandEvent& event) { DoCheckAll(true); }

void AddFunctionsImpDlg::OnUncheckAll(wxCommandEvent& event) { DoCheckAll(false); }

void AddFunctionsImpDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(false);
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        if(m_dvListCtrl->IsItemChecked(m_dvListCtrl->RowToItem(i), 0)) {
            event.Enable(true);
            return;
        }
    }
}

void AddFunctionsImpDlg::DoCheckAll(bool checked)
{
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        m_dvListCtrl->SetItemChecked(m_dvListCtrl->RowToItem(i), checked, 0);
    }
}
