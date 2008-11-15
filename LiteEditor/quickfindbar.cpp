//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : quickfindbar.cpp              
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
#include "stringsearcher.h"
#include "cl_editor.h"
#include "manager.h"
#include "quickfindbar.h"

QuickFindBar::QuickFindBar( wxWindow* parent )
    : QuickFindBarBase( parent )
{
	Hide();
}

void QuickFindBar::OnButtonNext( wxCommandEvent& event )
{
	DoSearch(true, false);
}

void QuickFindBar::OnButtonPrevious( wxCommandEvent& event )
{
	DoSearch(false, false);
}

void QuickFindBar::OnButtonClose(wxCommandEvent& e)
{
	wxUnusedVar( e );
    DoShow(false);
}

void QuickFindBar::DoSearch(bool next, bool incremental)
{
	wxString findWhat = m_textCtrlFindWhat->GetValue();
	if(findWhat.IsEmpty()) {
		return;
	}
	
	LEditor *editor = ManagerST::Get()->GetActiveEditor();
	if(!editor){
		return;
	}
	
	int pos(0);
	int match_len(0);

	// remove reverse search
	int offset( editor->GetCurrentPos() );
	if(editor->GetSelectedText().IsEmpty() == false && incremental) {
		offset = editor->GetSelectionStart();
	} else if(editor->GetSelectedText().IsEmpty() == false && !next) {
		offset = editor->GetSelectionStart();
	}
	
	size_t flags = 0;
	
	if(!next) {
		flags |= wxSD_SEARCH_BACKWARD;
	}
	
	if(m_checkBoxMatchCase->IsChecked()) {
		flags |= wxSD_MATCHCASE;
	}
	
	if ( StringFindReplacer::Search(editor->GetText(), offset, findWhat, flags, pos, match_len) ) {
		// we have a match
		m_textCtrlFindWhat->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );
		m_textCtrlFindWhat->Refresh();
		editor->SetSelection(pos, pos + match_len);
		editor->SetCurrentPos(pos + match_len);
	} else {
		
		// try again
		int curpos = offset;
		editor->SetCaretAt(next ? 0 : editor->GetLength());
		offset = editor->GetCurrentPos();
		
		if ( StringFindReplacer::Search(editor->GetText(), offset, findWhat, flags, pos, match_len) ) {
			// we have a match
			m_textCtrlFindWhat->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );
			m_textCtrlFindWhat->Refresh();
			editor->SetSelection(pos, pos + match_len);
			editor->SetCurrentPos(pos + match_len);
		} else {
			// restore the caret 
			editor->SetCaretAt(curpos);
			m_textCtrlFindWhat->SetBackgroundColour( wxT("PINK") );
			m_textCtrlFindWhat->Refresh();
			editor->SetSelection(curpos, curpos);
		}
	}
}

void QuickFindBar::OnText(wxCommandEvent& e)
{
	DoSearch(true, true);
}

void QuickFindBar::OnKeyDown(wxKeyEvent& e)
{
    switch (e.GetKeyCode()) {
        case WXK_ESCAPE:
            DoShow(false);
            break;
        case '\r':
            if (e.AltDown()) {
                DoSearch(false, false);
            } else {
                e.Skip();
            }
            break;
        default:
            e.Skip();
            break;
    }
}

void QuickFindBar::DoShow(bool s)
{
    if (Show(s)) {
        wxSizer *sz = GetParent()->GetSizer();
        sz->Show(this, s, true);
        sz->Layout();
    }
    
    LEditor *editor = ManagerST::Get()->GetActiveEditor();
    if (!editor) {
        // nothing to do
    } else if (!s) {
        // return focus to active editor
		editor->SetActive();
    } else {
        // copy selection from editor, and take focus
		wxString selText = editor->GetSelectedText();
		if (!selText.IsEmpty()) {
            // FIXME: should only take one line at most
            m_textCtrlFindWhat->SetValue(selText);
		}
		m_textCtrlFindWhat->SelectAll();
		m_textCtrlFindWhat->SetFocus();
   }
}
