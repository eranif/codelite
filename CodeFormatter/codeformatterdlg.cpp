//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : codeformatterdlg.cpp
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
#include "codeformatterdlg.h"

#include "ColoursAndFontsManager.h"
#include "StringUtils.h"
#include "clSTCLineKeeper.h"
#include "codeformatter.h"
#include "editor_config.h"
#include "fileextmanager.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "windowattrmanager.h"

#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>

CodeFormatterDlg::CodeFormatterDlg(wxWindow* parent, CodeFormatterManager& mgr)
    : CodeFormatterBaseDlg(parent)
    , m_formatter_manager(mgr)
{
    m_dvListCtrl->Bind(wxEVT_DATAVIEW_SELECTION_CHANGING, &CodeFormatterDlg::OnSelectionChanging, this);
    m_page = new FormatterPage(m_mainPanel);
    boxSizerFormatter->Add(m_page, wxSizerFlags(1).Expand().Border(wxALL, 5));

    InitDialog();
    ::clSetDialogBestSizeAndPosition(this);
}

CodeFormatterDlg::~CodeFormatterDlg()
{
    m_dvListCtrl->Unbind(wxEVT_DATAVIEW_SELECTION_CHANGING, &CodeFormatterDlg::OnSelectionChanging, this);
}

void CodeFormatterDlg::InitDialog()
{
    wxArrayString all_formatters;
    m_formatter_manager.GetAllNames(&all_formatters);
    m_dvListCtrl->DeleteAllItems();
    m_dvListCtrl->Begin();
    for(const auto& name : all_formatters) {
        wxVector<wxVariant> cols;
        cols.push_back(name);
        m_dvListCtrl->AppendItem(cols);
    }

    if(!m_dvListCtrl->IsEmpty()) {
        m_dvListCtrl->SelectRow(0);
    }
    m_dvListCtrl->Commit();
}

void CodeFormatterDlg::OnOK(wxCommandEvent& e)
{
    wxUnusedVar(e);
    EndModal(wxID_OK);
}

void CodeFormatterDlg::OnSelectionChanged(wxDataViewEvent& event)
{
    auto item = event.GetItem();
    auto formatter = m_formatter_manager.GetFormatterByName(m_dvListCtrl->GetItemText(item));
    m_page->Load(formatter);
}

void CodeFormatterDlg::OnSelectionChanging(wxDataViewEvent& event) { event.Skip(); }

void CodeFormatterDlg::OnRevert(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if(::wxMessageBox(_("Lose all your modifications and restore default settings?"), "CodeLite",
                      wxICON_WARNING | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT) != wxYES) {
        return;
    }
    m_formatter_manager.RestoreDefaults();
    InitDialog();
}

void CodeFormatterDlg::OnSelectFileTypes(wxCommandEvent& event)
{
    //    wxArrayString selected;
    //
    //    wxString cur = m_textCtrlFileTypes->GetValue();
    //    wxArrayString initial = wxStringTokenize(cur, ";", wxTOKEN_STRTOK);
    //    if(!::clShowFileTypeSelectionDialog(this, initial, &selected)) {
    //        return;
    //    }
    //
    //    wxString value = wxJoin(selected, ';');
    //    m_textCtrlFileTypes->ChangeValue(value);
}
