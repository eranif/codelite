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
#include "globals.h"
#include "windowattrmanager.h"

namespace
{
constexpr unsigned kColCheck = 0; // the checkbox column of the list
} // namespace

AddFunctionsImpDlg::AddFunctionsImpDlg(wxWindow* parent, const TagEntryPtrVector_t& tags, const wxString& targetFile)
    : AddFunctionsImplBaseDlg(parent)
{
    ::AdjustDataViewAlternateColour(m_dvListCtrl);

    // Columns: a checkbox, and the function with an icon (add them only if the base class did not create them).
    // The rows are not sorted: row `i` is the function `i`
    if (m_dvListCtrl->GetColumnCount() == 0) {
        m_dvListCtrl->AppendToggleColumn(
            wxEmptyString, wxDATAVIEW_CELL_ACTIVATABLE, FromDIP(40), wxALIGN_CENTER, wxDATAVIEW_COL_RESIZABLE);
        m_dvListCtrl->AppendIconTextColumn(
            _("Function"), wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
    }

    const wxBitmap functionBmp = clGetManager()->GetStdIcons()->LoadBitmap("function_public");

    // Keep the tags
    m_tags = tags;
    // Clear the impl array
    m_implArr.Clear();
    m_implArr.reserve(tags.size());
    for (size_t i = 0; i < m_tags.size(); ++i) {
        wxVector<wxVariant> cols;
        cols.push_back(wxVariant(true)); // checked
        cols.push_back(::MakeIconText(m_tags.at(i)->GetDisplayName(), functionBmp));

        // keep the implementation: the index is the row
        TagEntryPtr tag = m_tags[i];
        wxString body = tag->GetFunctionDefinition();
        if (tag->GetTypename() == "std::string" || tag->GetTypename() == "string") {
            body << "\n{ return \"\"; }";
        } else if (tag->GetTypename() == "bool") {
            body << "\n{ return false; }";
        } else {
            body << "\n{}";
        }

        m_implArr.Add(body + "\n");
        m_dvListCtrl->AppendItem(cols);
    }
    m_filePicker->SetPath(targetFile);
    ::clSetDialogBestSizeAndPosition(*this);
}

wxString AddFunctionsImpDlg::GetFileName() const { return m_filePicker->GetPath(); }

wxString AddFunctionsImpDlg::GetText() const
{
    wxString text;
    for (unsigned i = 0; i < m_dvListCtrl->GetItemCount() && i < m_implArr.size(); ++i) {
        if (m_dvListCtrl->GetToggleValue(i, kColCheck)) {
            if (text.IsEmpty())
                text << "\n";
            // checked
            text << m_implArr.Item(i);
        }
    }

    if (!text.IsEmpty()) {
        text.RemoveLast();
    }
    return text;
}

void AddFunctionsImpDlg::OnCheckAll(wxCommandEvent& event) { DoCheckAll(true); }

void AddFunctionsImpDlg::OnUncheckAll(wxCommandEvent& event) { DoCheckAll(false); }

void AddFunctionsImpDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(false);
    for (unsigned i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        if (m_dvListCtrl->GetToggleValue(i, kColCheck)) {
            event.Enable(true);
            return;
        }
    }
}

void AddFunctionsImpDlg::DoCheckAll(bool checked)
{
    for (unsigned i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        m_dvListCtrl->SetToggleValue(checked, i, kColCheck);
    }
}
