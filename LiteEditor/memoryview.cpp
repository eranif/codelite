//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : memoryview.cpp              
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

#include <wx/tokenzr.h>
#include <wx/regex.h>
#include "manager.h"
#include "memoryview.h"
#include "frame.h"

MemoryView::MemoryView( wxWindow* parent )
		: MemoryViewBase( parent )
{
	m_buttonUpdate->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( MemoryView::OnUpdateUI ), NULL, this );
}

void MemoryView::OnEvaluate( wxCommandEvent& event )
{
	wxUnusedVar(event);
	ManagerST::Get()->UpdateDebuggerPane();
}

void MemoryView::OnEvaluateUI( wxUpdateUIEvent& event )
{
	event.Enable(m_textCtrlExpression->GetValue().IsEmpty() == false);
}

void MemoryView::Clear()
{
	m_textCtrlExpression->Clear();
	m_textCtrlMemory->Clear();
}

size_t MemoryView::GetSize() const
{
	long value(0);
	m_textCtrlSize->GetValue().ToLong(&value);

	// set default memory size to 32 bytes
	if (!value) {
		value = 32;
	}
	return value;
}

void MemoryView::SetViewString(const wxString& text)
{
	m_textCtrlMemory->Freeze();
	// get old value
	wxString oldValue = m_textCtrlMemory->GetValue();

	m_textCtrlMemory->Clear();
	// first check that we are trying to view the same addresses
	wxString newAddr = text.BeforeFirst(wxT(':'));
	wxString oldAddr = oldValue.BeforeFirst(wxT(':'));

	// set the new value
	m_textCtrlMemory->SetValue(text);

	if (newAddr == oldAddr) {

		size_t shortLen = text.Length() < oldValue.Length() ? text.Length() : oldValue.Length();

		// same address, loop over the strings
		wxTextAttr style;
		style.SetTextColour(wxT("RED"));

		long start(wxNOT_FOUND);
		bool needColouring(false);
		long possibleStart(0);

		for (size_t i=0; i<shortLen; i++) {

			// reset word
			if ((text.GetChar(i) == wxT(' ') || text.GetChar(i) == wxT('\n')) && needColouring) {

				if (text.GetChar(i) != wxT('\n')) {
					m_textCtrlMemory->SetStyle(start, (long)i, style);
				}

				start = wxNOT_FOUND;
				needColouring = false;
			}

			if (text.GetChar(i) == wxT(' ') || text.GetChar(i) == wxT('\n')) {
				possibleStart = (long)i;
			}

			if (text.GetChar(i) != oldValue.GetChar(i)) {
				needColouring = true;
				if (start == wxNOT_FOUND) {
					start = possibleStart;
				}
			}
		}
	}

	// loop over the text and set the address in grey colour
	wxTextAttr addrAttr;
	addrAttr.SetTextColour(wxT("GREY"));
	wxFont addrFont = m_textCtrlMemory->GetFont();
	addrFont.SetWeight(wxBOLD);
	addrFont.SetStyle(wxFONTSTYLE_ITALIC);

	addrAttr.SetFont(addrFont);

	wxArrayString lines = wxStringTokenize(text, wxT("\n"), wxTOKEN_STRTOK);
	for (size_t i=0; i<lines.GetCount(); i++) {
		long addr_end = lines.Item(i).Find(wxT(':'));
		if (addr_end != wxNOT_FOUND) {
			long pos = m_textCtrlMemory->XYToPosition(0, (long)i);
			m_textCtrlMemory->SetStyle(pos, pos + addr_end, addrAttr);
		}
	}
	
	m_textCtrlMemory->DiscardEdits(); // make this operation undoable
	m_textCtrlMemory->Thaw();
}

void MemoryView::OnUpdate(wxCommandEvent& e)
{
	static wxRegEx reHex(wxT("[0][x][0-9a-fA-F][0-9a-fA-F]"));

	// extract the text memory from the text control and pass it to the debugger
	wxString memory;
	wxArrayString lines = wxStringTokenize(m_textCtrlMemory->GetValue(), wxT("\n"), wxTOKEN_STRTOK);
	for (size_t i=0; i<lines.GetCount(); i++) {
		wxString line = lines.Item(i).AfterFirst(wxT(':')).BeforeLast(wxT(':')).Trim().Trim(false);
		wxArrayString hexValues = wxStringTokenize(line, wxT(" "), wxTOKEN_STRTOK);
		for (size_t y=0; y<hexValues.GetCount(); y++) {
			if (reHex.Matches(hexValues.Item(y)) && hexValues.Item(y).Len() == 4) {
				// OK
				continue;
			} else {
				wxMessageBox(wxString::Format(wxT("Invalid memory value: %s"), hexValues.Item(y).c_str()), wxT("CodeLite"), wxICON_WARNING|wxOK);
				// update the pane to old value
				ManagerST::Get()->UpdateDebuggerPane();
				return;
			}
		}

		if (line.IsEmpty() == false) {
			memory << line << wxT(" ");
		}
	}
	
	// set the new memory
	memory = memory.Trim().Trim(false);
	ManagerST::Get()->SetMemory(m_textCtrlExpression->GetValue(), GetSize(), memory);

	// update the view
	ManagerST::Get()->UpdateDebuggerPane();
}

void MemoryView::OnUpdateUI(wxUpdateUIEvent& event)
{
	event.Enable(m_textCtrlMemory->IsModified());
}
