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

namespace
{
wxString astyleHelpUrl(wxT("http://astyle.sourceforge.net/astyle.html"));
wxString clangFormatHelpUrl(wxT("http://clang.llvm.org/docs/ClangFormatStyleOptions.html"));
wxString phpFormatHelpUrl(wxT("https://github.com/FriendsOfPHP/PHP-CS-Fixer"));
wxString rustfmtHelpUrl(wxT("https://rust-lang.github.io/rustfmt"));

constexpr int ID_ASTYLE_HELP = 1309;
constexpr int ID_CLANG_FORMAST_HELP = 1310;
constexpr int ID_PHP_FORMAST_HELP = 1311;
constexpr int ID_RUSTFMT_HELP = 1312;

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

        auto argv = StringUtils::BuildArgv(line);
        for(const auto& arg : argv) {
            command_array.push_back(arg);
        }
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

    // enabled?
    m_checkBoxEnabled->SetValue(formatter->IsEnabled());
    m_isDirty = false;

    m_stcCommand->CallAfter(&wxStyledTextCtrl::SetFocus);
}

bool CodeFormatterDlg::IsDirty() const
{
    return m_isDirty || m_stcCommand->CanUndo() || m_textCtrlWD->CanUndo() || m_stcRemoteCommand->CanUndo();
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
