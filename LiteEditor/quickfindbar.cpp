#include "stringsearcher.h"
#include "cl_editor.h"
#include "manager.h"
#include <wx/xrc/xmlres.h>
#include "quickfindbar.h"
#include "frame.h"

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
	LEditor *editor = ManagerST::Get()->GetActiveEditor();
	if(editor) {
		editor->SetActive();
	}
	Frame::Get()->GetMainBook()->HideQuickBar();
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
	if( e.GetKeyCode() == WXK_ESCAPE ) {
		LEditor *editor = ManagerST::Get()->GetActiveEditor();
		if(editor) {
			editor->SetActive();
		}
		
		Frame::Get()->GetMainBook()->HideQuickBar();
		return;
	}
	e.Skip();
	
}
