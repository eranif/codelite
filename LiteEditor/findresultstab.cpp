//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : findresultstab.cpp              
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
 #include "findresultstab.h"
#include "wx/string.h"
#include "manager.h"
#include "cl_editor.h"

FindResultsTab::FindResultsTab(wxWindow *parent, wxWindowID id, const wxString &name)
: OutputTabWindow(parent, id, name)
{
	//SCLEX_FIF
	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	
	m_sci->SetLexer(wxSCI_LEX_FIF);
	m_sci->StyleSetForeground(wxSCI_LEX_FIF_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
	m_sci->StyleSetBackground(wxSCI_LEX_FIF_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	
	m_sci->StyleSetForeground(wxSCI_LEX_FIF_FILE, wxT("BLUE"));
	m_sci->StyleSetBackground(wxSCI_LEX_FIF_FILE, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	
	font.SetWeight(wxBOLD);
	m_sci->StyleSetFont(wxSCI_LEX_FIF_FILE, font);
	
	font.SetWeight(wxNORMAL);
	m_sci->StyleSetFont(wxSCI_LEX_FIF_DEFAULT, font);
	
	m_sci->StyleSetHotSpot(wxSCI_LEX_FIF_FILE, true);
}

FindResultsTab::~FindResultsTab()
{
}

void FindResultsTab::OnHotspotClicked(wxScintillaEvent &event)
{
	//same as double click
	OnMouseDClick(event);
}

void FindResultsTab::OnMouseDClick(wxScintillaEvent &event)
{
	long pos = event.GetPosition();
	int line = m_sci->LineFromPosition(pos);
	wxString lineText = m_sci->GetLine(line);

	//remove selection
	m_sci->SetSelectionStart(pos);
	m_sci->SetSelectionEnd(pos);
	
	// each line has the format of 
	// file(line, col, len): text
	wxString fileName = lineText.BeforeFirst(wxT('('));
	wxString strLineNumber = lineText.AfterFirst(wxT('('));
	wxString strLen = strLineNumber.AfterFirst(wxT(',')).AfterFirst(wxT(',')).BeforeFirst(wxT(')'));
	wxString strCol = strLineNumber.AfterFirst(wxT(',')).BeforeFirst(wxT(','));
	
	strLineNumber = strLineNumber.BeforeFirst(wxT(','));
	strLineNumber = strLineNumber.Trim();
	
	long lineNumber = -1;
	long matchLen = wxNOT_FOUND;
	long col = wxNOT_FOUND;
	
	strLineNumber.ToLong(&lineNumber);
	
	strLen = strLen.Trim().Trim(false);
	strLen.ToLong( &matchLen );
	
	strCol = strCol.Trim().Trim(false);
	strCol.ToLong( &col );
	
	// open the file in the editor
	if(ManagerST::Get()->OpenFile(fileName, wxEmptyString, lineNumber - 1 )) {
		//select the matched result
		LEditor *editor = ManagerST::Get()->GetActiveEditor();
		if(editor) {
			if(col >= 0 && matchLen >= 0){
				int offset = editor->PositionFromLine(lineNumber-1);
        editor->EnsureVisible(lineNumber-1);
				editor->SetSelection(offset + col, offset + col + matchLen);
			}
		}
	}
}
