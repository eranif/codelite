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

namespace
{
wxString to_wx_string(const wxArrayString& command_arr, const wxString& desc)
{
    wxString command;
    command << "# " << desc << "\n";
    command << "# Command to execute:\n";
    command << "\n";

    for(size_t i = 0; i < command_arr.size(); ++i) {
        if(i > 0) {
            command << "  ";
        }
        command << command_arr[i] << "\n";
    }
    return command;
}

wxArrayString to_wx_array(const wxString& command)
{
    wxArrayString lines = ::wxStringTokenize(command, "\n", wxTOKEN_STRTOK);

    wxArrayString command_array;
    command_array.reserve(lines.size());

    for(auto& line : lines) {
        line.Trim().Trim(false);
        if(line.StartsWith("#") || line.IsEmpty()) {
            continue;
        }

        int count = 0;
        auto argv = StringUtils::BuildArgv(line, count);
        for(int i = 0; i < count; ++i) {
            command_array.push_back(argv[i]);
        }
        StringUtils::FreeArgv(argv, count);
    }
    return command_array;
}
} // namespace

CodeFormatterDlg::CodeFormatterDlg(wxWindow* parent, CodeFormatterManager& mgr)
    : CodeFormatterBaseDlg(parent)
    , m_formatter_manager(mgr)
{
    m_dvListCtrl->Bind(wxEVT_DATAVIEW_SELECTION_CHANGING, &CodeFormatterDlg::OnSelectionChanging, this);
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
    Save();
    EndModal(wxID_OK);
}

void CodeFormatterDlg::Save()
{
    auto f = m_formatter_manager.GetFormatterByName(m_dvListCtrl->GetItemText(m_dvListCtrl->GetSelection()));
    if(!f) {
        return;
    }

    f->SetEnabled(m_checkBoxEnabled->IsChecked());
    f->SetWorkingDirectory(m_textCtrlWD->GetValue());
    f->SetCommand(to_wx_array(m_stcCommand->GetText()));
    f->SetRemoteCommand(to_wx_array(m_stcRemoteCommand->GetText()));
    f->SetLanguages(wxStringTokenize(m_textCtrlFileTypes->GetValue(), ";", wxTOKEN_STRTOK));
    f->SetInlineFormatter(m_checkBoxInline->IsChecked());
    m_isDirty = false;
}

void CodeFormatterDlg::OnExportFile(wxCommandEvent& event) { wxUnusedVar(event); }

void CodeFormatterDlg::OnSelectionChanged(wxDataViewEvent& event)
{
    auto item = event.GetItem();
    auto formatter = m_formatter_manager.GetFormatterByName(m_dvListCtrl->GetItemText(item));
    if(!formatter) {
        return;
    }

    m_stcCommand->Clear();
    m_stcCommand->SetText(to_wx_string(formatter->GetCommand(), formatter->GetDescription()));
    m_stcCommand->EmptyUndoBuffer();

    m_stcRemoteCommand->Clear();
    m_stcRemoteCommand->SetText(to_wx_string(formatter->GetRemoteCommand(), formatter->GetDescription()));
    m_stcRemoteCommand->EmptyUndoBuffer();

    // set the working directory
    m_textCtrlWD->SetValue(formatter->GetWorkingDirectory());
    m_textCtrlWD->EmptyUndoBuffer();

    m_textCtrlFileTypes->SetEditable(true);
    m_textCtrlFileTypes->SetValue(wxJoin(formatter->GetLanguages(), ';'));
    m_textCtrlFileTypes->EmptyUndoBuffer();
    m_textCtrlFileTypes->SetEditable(false);

    // enabled?
    m_checkBoxEnabled->SetValue(formatter->IsEnabled());
    m_checkBoxInline->SetValue(formatter->IsInlineFormatter());
    m_isDirty = false;

    m_stcCommand->CallAfter(&wxStyledTextCtrl::SetFocus);
}

bool CodeFormatterDlg::IsDirty() const
{
    return m_isDirty || m_stcCommand->CanUndo() || m_textCtrlWD->CanUndo() || m_stcRemoteCommand->CanUndo() ||
           m_textCtrlFileTypes->CanUndo();
}

void CodeFormatterDlg::OnEnabled(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_isDirty = true;
}

void CodeFormatterDlg::OnSelectionChanging(wxDataViewEvent& event)
{
    event.Skip();
    Save();
}

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
    wxArrayString selected;

    wxString cur = m_textCtrlFileTypes->GetValue();
    wxArrayString initial = wxStringTokenize(cur, ";", wxTOKEN_STRTOK);
    if(!::clShowFileTypeSelectionDialog(this, initial, &selected)) {
        return;
    }

    wxString value = wxJoin(selected, ';');
    m_textCtrlFileTypes->ChangeValue(value);
}

void CodeFormatterDlg::OnInplaceEdit(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_isDirty = true;
}
