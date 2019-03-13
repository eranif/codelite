//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : renamesymboldlg.cpp
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

#include "wx/msgdlg.h"
#include "renamesymboldlg.h"
#include "globals.h"
#include "editor_config.h"
#include "windowattrmanager.h"
#include "macros.h"

class RenameSymbolData : public wxClientData
{
public:
    CppToken m_token;

public:
    RenameSymbolData(const CppToken& token)
        : m_token(token)
    {
    }
    ~RenameSymbolData() {}
};

RenameSymbol::RenameSymbol(wxWindow* parent, const CppToken::Vec_t& candidates, const CppToken::Vec_t& possCandidates,
                           const wxString& oldname)
    : RenameSymbolBase(parent)
{
    m_preview->SetReadOnly(true);
    EditorConfigST::Get()->GetLexer("C++")->Apply(m_preview, true);
    m_tokens.clear();

    CppToken::Vec_t::const_iterator iter = candidates.begin();
    for(; iter != candidates.end(); ++iter) {
        AddMatch(*iter, true);
        m_tokens.push_back(*iter);
    }

    iter = possCandidates.begin();
    for(; iter != possCandidates.end(); iter++) {
        AddMatch(*iter, false);
        m_tokens.push_back(*iter);
    }

    if(m_tokens.empty() == false) { DoSelectFile(m_tokens.at((size_t)0)); }

    m_textCtrlNewName->SetValue(oldname);
    m_textCtrlNewName->SetFocus();
    ::clSetTLWindowBestSizeAndPosition(this);
}

void RenameSymbol::AddMatch(const CppToken& token, bool check)
{
    wxString relativeTo = clCxxWorkspaceST::Get()->GetWorkspaceFileName().GetPath();
    wxFileName fn(token.getFilename());
    fn.MakeRelativeTo(relativeTo);

    wxVector<wxVariant> cols;
    cols.push_back(check);
    cols.push_back(fn.GetFullPath());
    cols.push_back(wxString() << token.getOffset());
    m_dvListCtrl->AppendItem(cols, (wxUIntPtr) new RenameSymbolData(token));
}

void RenameSymbol::OnButtonOK(wxCommandEvent& e)
{
    wxUnusedVar(e);

    if(!IsValidCppIndetifier(m_textCtrlNewName->GetValue())) {
        wxMessageBox(_("Invalid C/C++ symbol name"), _("CodeLite"), wxICON_WARNING | wxOK);
        return;
    }

    EndModal(wxID_OK);
}

void RenameSymbol::GetMatches(CppToken::Vec_t& matches)
{
    wxVariant v;
    for(int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        m_dvListCtrl->GetValue(v, i, 0);

        if(v.GetBool()) {
            matches.push_back(((RenameSymbolData*)m_dvListCtrl->GetItemData(m_dvListCtrl->RowToItem(i)))->m_token);
        }
    }
}

void RenameSymbol::DoSelectFile(const CppToken& token)
{
    m_preview->SetReadOnly(false);

    wxString file_name(token.getFilename());
    if(m_filename != file_name) {
        m_preview->LoadFile(file_name);
        m_filename = file_name;
    }

    m_preview->ClearSelections();
    m_preview->SetSelection(token.getOffset(), token.getOffset() + token.getName().length());
    int line_number = m_preview->LineFromPosition(token.getOffset());

    int linesOnScreen = m_preview->LinesOnScreen();
    // To place our line in the middle, the first visible line should be
    // the: line - (linesOnScreen / 2)
    int firstVisibleLine = line_number - (linesOnScreen / 2);
    if(firstVisibleLine < 0) { firstVisibleLine = 0; }
    m_preview->EnsureVisible(firstVisibleLine);
    m_preview->SetFirstVisibleLine(firstVisibleLine);
    m_preview->SetReadOnly(true);
}

void RenameSymbol::OnSelection(wxDataViewEvent& event)
{
    CHECK_ITEM_RET(event.GetItem());

    RenameSymbolData* data = (RenameSymbolData*)m_dvListCtrl->GetItemData(event.GetItem());
    if(data) { DoSelectFile(data->m_token); }
}

RenameSymbol::~RenameSymbol() {}

void RenameSymbol::OnCheckAll(wxCommandEvent& event)
{
    for(int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        m_dvListCtrl->SetValue(true, i, 0);
    }
}

void RenameSymbol::OnUncheckAll(wxCommandEvent& event)
{
    for(int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        m_dvListCtrl->SetValue(false, i, 0);
    }
}
