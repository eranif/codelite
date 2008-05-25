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

RenameSymbol::RenameSymbol( wxWindow* parent, const std::list<CppToken>& candidates, const std::list<CppToken> &possCandidates  )
		:
		RenameSymbolBase( parent )
{
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
	m_textCtrlNewName->SetFocus();
}

void RenameSymbol::OnItemSelected( wxCommandEvent& event )
{
	int index = event.GetSelection();
	if ( index != wxNOT_FOUND ) {
		DoSelectFile(m_tokens.at((size_t)index));
	}
}

void RenameSymbol::OnItemDClicked( wxCommandEvent& event )
{
	wxUnusedVar(event);
}

void RenameSymbol::OnItemChecked( wxCommandEvent& event )
{
	event.Skip();
}

void RenameSymbol::AddMatch(const CppToken& token, bool check)
{
	wxString msg;
	wxFileName fn(token.getFilename());
	msg << token.getLine() << wxT(": At ") << fn.GetFullName() << wxT(" pos ") << token.getOffset();

	msg.Replace(wxT("\n"), wxT(" "));
	msg.Replace(wxT("\r\n"), wxT(" "));
	msg = msg.Trim().Trim(false);

	int index = m_checkListCandidates->Append(msg);//, new CppToken(token));
	m_checkListCandidates->Check((unsigned int)index, check);
	m_checkListCandidates->Select(0);
}

void RenameSymbol::OnButtonOK(wxCommandEvent& e)
{
	wxUnusedVar(e);
	// TODO:: validate new name
	EndModal(wxID_OK);
}

void RenameSymbol::GetMatches(std::list<CppToken>& matches)
{
	for (unsigned int i=0; i<m_checkListCandidates->GetCount(); i++) {
		if (m_checkListCandidates->IsChecked(i)) {
			matches.push_back(m_tokens.at(i));
		}
	}
}
void RenameSymbol::DoSelectFile(const CppToken& token)
{
	m_preview->SetReadOnly(false);
	m_preview->Create(wxEmptyString, token.getFilename());
	m_preview->SetCaretAt(token.getOffset());
	m_preview->SetSelection(token.getOffset(), token.getOffset()+token.getName().Len());
	m_preview->SetReadOnly(true);
}
