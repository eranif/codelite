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
 #include "renamesymboldlg.h"
#include "globals.h"

class RenameSymbolData : public wxClientData
{
public:
	CppToken m_token;

public:
	RenameSymbolData(const CppToken& token) : m_token(token) {}
	~RenameSymbolData(){}
};

RenameSymbol::RenameSymbol( wxWindow* parent, const std::list<CppToken>& candidates, const std::list<CppToken> &possCandidates, const wxString& oldname/* = wxT("")*/  )
		:
		RenameSymbolBase( parent )
{
	// Initialize the columns
	m_checkListCandidates->InsertColumn(0, wxT(" "));
	m_checkListCandidates->InsertColumn(1, _("File"));
	m_checkListCandidates->InsertColumn(2, _("Position"));
	m_checkListCandidates->SetColumnWidth(0, 20);
	m_checkListCandidates->SetColumnWidth(1, 200);

	m_preview->SetReadOnly(true);

	m_tokens.clear();
	std::list<CppToken>::const_iterator iter = candidates.begin();
	for (; iter != candidates.end(); iter++) {
		AddMatch(*iter, true);
		m_tokens.push_back(*iter);
	}

	iter = possCandidates.begin();
	for (; iter != possCandidates.end(); iter++) {
		AddMatch(*iter, false);
		m_tokens.push_back(*iter);
	}

	if (m_tokens.empty() == false) {
		DoSelectFile(m_tokens.at((size_t)0));
	}

	m_textCtrlNewName->SetValue(oldname);
	m_textCtrlNewName->SetFocus();

	m_checkListCandidates->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( RenameSymbol::OnItemSelected ), NULL, this );
}

void RenameSymbol::OnItemSelected( wxListEvent& event )
{
	RenameSymbolData* data = (RenameSymbolData*)m_checkListCandidates->GetItemData(event.m_itemIndex);
	if(data) {
		DoSelectFile( data->m_token );
	}
}

void RenameSymbol::AddMatch(const CppToken& token, bool check)
{
	wxString relativeTo = WorkspaceST::Get()->GetWorkspaceFileName().GetPath();
	wxFileName fn( token.getFilename() );
	fn.MakeRelativeTo( relativeTo );

	long index = m_checkListCandidates->AppendRow();
	m_checkListCandidates->SetTextColumn(index, 1, fn.GetFullPath());
	m_checkListCandidates->SetTextColumn(index, 2, wxString::Format(wxT("%u"), (unsigned int)token.getOffset()));
	m_checkListCandidates->Check(index, check);
	m_checkListCandidates->SetItemClientData(index, new RenameSymbolData(token));
}

void RenameSymbol::OnButtonOK(wxCommandEvent& e)
{
	wxUnusedVar(e);

	if(!IsValidCppIndetifier(m_textCtrlNewName->GetValue())){
		wxMessageBox(_("Invalid C/C++ symbol name"), _("CodeLite"), wxICON_WARNING|wxOK);
		return;
	}

	EndModal(wxID_OK);
}

void RenameSymbol::GetMatches(std::list<CppToken>& matches)
{
	for (int i=0; i<m_checkListCandidates->GetItemCount(); i++) {
		if (m_checkListCandidates->IsChecked(i)) {
			matches.push_back( ((RenameSymbolData*) m_checkListCandidates->GetItemData(i))->m_token );
		}
	}
}
void RenameSymbol::DoSelectFile(const CppToken& token)
{
	m_preview->SetReadOnly(false);

	// Recreate the editor only if needed
	if(m_preview->GetFileName().GetFullPath() != token.getFilename())
		m_preview->Create(wxEmptyString, token.getFilename());

	m_preview->SetCaretAt(token.getOffset());
	m_preview->SetSelection(token.getOffset(), token.getOffset()+token.getName().Len());
	m_preview->SetReadOnly(true);
}
