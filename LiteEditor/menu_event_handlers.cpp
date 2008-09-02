//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : menu_event_handlers.cpp
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
#include "menu_event_handlers.h"
#include "cl_editor.h"
#include "frame.h"

//------------------------------------
// Handle copy events
//------------------------------------
void EditHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor *editor = (LEditor*)owner;

	// hide completion box
	editor->HideCompletionBox();

	if (event.GetId() == wxID_COPY) {
		editor->Copy();
	} else if (event.GetId() == wxID_CUT) {
		editor->Cut();
	} else if (event.GetId() == wxID_PASTE) {
		editor->Paste();
	} else if (event.GetId() == wxID_UNDO) {
		editor->Undo();
	} else if (event.GetId() == wxID_REDO) {
		editor->Redo();
	} else if (event.GetId() == wxID_SELECTALL) {
		editor->SelectAll();
	} else if (event.GetId() == wxID_DUPLICATE) {
		editor->SelectionDuplicate();
	} else if (event.GetId() == XRCID("delete_line_end")) {
		editor->DelLineRight();
	} else if (event.GetId() == XRCID("delete_line_start")) {
		editor->DelLineLeft();
	} else if (event.GetId() == XRCID("delete_line")) {
		editor->LineDelete();
	} else if (event.GetId() == XRCID("to_lower")) {
		editor->LowerCase();
	} else if (event.GetId() == XRCID("to_upper")) {
		editor->UpperCase();
	} else if (event.GetId() == XRCID("transpose_lines")) {
		editor->LineTranspose();
	}
}

void EditHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	LEditor *editor = dynamic_cast<LEditor*>(owner);

	if (event.GetId() == wxID_COPY || event.GetId() == XRCID("to_lower") || event.GetId() == XRCID("to_upper")) {
		event.Enable(editor && ( editor->GetSelectionStart() - editor->GetSelectionEnd() != 0 ));
	} else if (event.GetId() == wxID_CUT) {
		event.Enable(editor && ( editor->GetSelectionStart() - editor->GetSelectionEnd() != 0 ));
	} else if (event.GetId() == wxID_PASTE) {
#ifdef __WXGTK__
		event.Enable(editor);
#else
		event.Enable(editor && editor->CanPaste());
#endif
	} else if (event.GetId() == wxID_UNDO) {
		event.Enable(editor && editor->CanUndo());
	} else if (event.GetId() == wxID_REDO) {
		event.Enable(editor && editor->CanRedo());
	} else if (event.GetId() == wxID_SELECTALL) {
		event.Enable(editor && editor->GetLength() > 0);
	} else if (event.GetId() == wxID_DUPLICATE) {
		event.Enable(true);
	} else {
		event.Enable(false);
	}
	event.Skip(false);
}

//------------------------------------
// brace matching
//------------------------------------
void BraceMatchHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	LEditor *editor = dynamic_cast<LEditor*>(owner);
	if ( !editor ) {
		return;
	}

	if (event.GetId() == XRCID("select_to_brace")) {
		editor->MatchBraceAndSelect(true);
	} else {
		editor->MatchBraceAndSelect(false);
	}
}

void BraceMatchHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	LEditor *editor = dynamic_cast<LEditor*>(owner);
	event.Enable(editor &&editor->GetLength() > 0);
}


//------------------------------------
// Find / Replace
//------------------------------------
void FindReplaceHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	LEditor *editor = dynamic_cast<LEditor*>(owner);
	if ( !editor ) {
		return;
	}

	if ( event.GetId() == wxID_FIND ) {
		editor->DoFindAndReplace(false);
	} else if ( event.GetId() == wxID_REPLACE ) {
		editor->DoFindAndReplace(true);
	} else {
		wxString selWord;
		if (event.GetId() == XRCID("find_next_at_caret") || event.GetId() == XRCID("find_previous_at_caret")) {
			wxString selection( editor->GetSelectedText() );
			if (selection.IsEmpty()) {
				// select the word
				selWord = editor->GetWordAtCaret();
			}
		}

		FindReplaceDialog *dlg = LEditor::GetFindReplaceDialog();
		FindReplaceData *data;
		if (!dlg) {
			data = &LEditor::GetFindReplaceData();
		} else {
			data = &dlg->GetData();
		}

		// if we have a selected text, use that text instead of one
		// from the dialog
		if (editor->GetSelectedText().IsEmpty() == false || selWord.IsEmpty() == false) {
			
			if(selWord.IsEmpty()) {
				data->SetFindString(editor->GetSelectedText());
			}else{
				// set the word to search from the word under the caret
				// also set the current search position accordingly 
				if(event.GetId() == XRCID("find_next_at_caret")){
					// search is going forward, so place the current position at the start of the word
					editor->SetCurrentPos(editor->WordStartPosition(editor->GetCurrentPos(), true));
				} else {
					// search is going backward
					editor->SetCurrentPos(editor->WordEndPosition(editor->GetCurrentPos(), true));
				}
				data->SetFindString(selWord);
			}
			if (dlg) {
				dlg->GetData().SetFindString(editor->GetSelectedText());
			}
		}

		if (data->GetFindString().IsEmpty()) {
			return;
		}

		if (event.GetId() == XRCID("find_next") || event.GetId() == XRCID("find_next_at_caret")) {
			// set search direction down
			data->SetFlags(data->GetFlags() & ~(wxFRD_SEARCHUP));
			editor->FindNext( *data );

		} else if ( event.GetId() == XRCID("find_previous") || event.GetId() == XRCID("find_previous_at_caret")) {
			// set search direction up
			data->SetFlags(data->GetFlags() | wxFRD_SEARCHUP);
			editor->FindNext( *data );
		}
	}
}

void FindReplaceHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	wxUnusedVar(owner);
	wxUnusedVar(event);
}

//----------------------------------
// goto linenumber
//----------------------------------

void GotoHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor *editor = dynamic_cast<LEditor*>(owner);
	if ( !editor ) {
		return;
	}

	wxString msg;
	msg.Printf(wxT("Go to line number (1 - %ld):"), editor->GetLineCount());

	while ( 1 ) {
		wxTextEntryDialog dlg(editor, msg, wxT("Go To Line"));
		dlg.SetTextValidator(wxFILTER_NUMERIC);

		if (dlg.ShowModal() == wxID_OK) {
			wxString val = dlg.GetValue();
			long line;
			if (!val.ToLong(&line)) {
				wxString err;
				err.Printf(wxT("'%s' is not a valid line number"), val.GetData());
				wxMessageBox (err, wxT("Go To Line"), wxOK | wxICON_INFORMATION);
				continue;
			}

			if (line > editor->GetLineCount()) {
				wxString err;
				err.Printf(wxT("Please insert a line number in the range of (1 - %ld)"), editor->GetLineCount());
				wxMessageBox (err, wxT("Go To Line"), wxOK | wxICON_INFORMATION);
				continue;
			}

			if (line > 0) {
				editor->GotoLine(line - 1);
				editor->EnsureVisible(line - 1);
				break;
			} else {
				editor->GotoLine(0);
				break;
			}
		} else {
			// wxID_CANCEL
			return;
		}
	}
	editor->SetActive();
}

void GotoHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	wxUnusedVar(event);
	wxUnusedVar(owner);
}

//------------------------------------
// Bookmarks
//------------------------------------
void BookmarkHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	LEditor *editor = dynamic_cast<LEditor*>(owner);
	if ( !editor ) {
		return;
	}

	if		 (event.GetId() == XRCID("toggle_bookmark")) {
		editor->ToggleMarker();
	} else if (event.GetId() == XRCID("next_bookmark")) {
		editor->FindNextMarker();
	} else if (event.GetId() == XRCID("previous_bookmark")) {
		editor->FindPrevMarker();
	} else if (event.GetId() == XRCID("removeall_bookmarks")) {
		editor->DelAllMarkers();
	}
}

void BookmarkHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	wxUnusedVar(owner);
	wxUnusedVar(event);
}


//------------------------------------
// Go to definition
//------------------------------------
void GotoDefinitionHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	LEditor *editor = dynamic_cast<LEditor*>(owner);
	if ( !editor ) {
		return;
	}

	if (event.GetId() == XRCID("goto_definition")) {
		editor->GotoDefinition();
	} else if (event.GetId() == XRCID("goto_previous_definition")) {
		editor->GotoPreviousDefintion();
	}

}

void GotoDefinitionHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	LEditor *editor = dynamic_cast<LEditor*>(owner);
	if (event.GetId() == XRCID("goto_previous_definition")) {
		event.Enable(editor && editor->CanGotoPreviousDefintion());
	} else {
		event.Enable(editor != NULL);
	}
}

//-------------------------------------------------
// View As
//-------------------------------------------------

void ViewAsHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	LEditor *editor = dynamic_cast<LEditor*>(owner);
	if ( !editor ) {
		return;
	}

	wxString lexName = Frame::Get()->GetViewAsLanguageById(event.GetInt());
	if (lexName.IsEmpty() == false) {
		editor->SetSyntaxHighlight(lexName);
	}
}

void ViewAsHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	LEditor *editor = dynamic_cast<LEditor*>(owner);
	if ( !editor ) {
		return;
	}

	event.Enable(true);
	wxString lexName = Frame::Get()->GetViewAsLanguageById(event.GetInt());
	event.Check(editor->GetContext()->GetName() == lexName);
}

//----------------------------------------------------
// Word wrap handler
//----------------------------------------------------

void WordWrapHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	LEditor *editor = dynamic_cast<LEditor*>(owner);
	if ( !editor ) {
		event.Enable(false);
		return;
	}

	event.Enable(true);
	event.Check(editor->GetWrapMode() != wxSCI_WRAP_NONE);
}

void WordWrapHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	LEditor *editor = dynamic_cast<LEditor*>(owner);
	if ( !editor ) {
		return;
	}

	editor->SetWrapMode(event.IsChecked() ? wxSCI_WRAP_WORD : wxSCI_WRAP_NONE);
}
//----------------------------------------------------
// Fold handler
//----------------------------------------------------

void FoldHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor *editor = dynamic_cast<LEditor*>(owner);
	if ( !editor ) {
		return;
	}

	if (event.GetId() == XRCID("toggle_fold")) editor->ToggleCurrentFold();
	  else editor->FoldAll();
}

void FoldHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	wxUnusedVar(owner);
	wxUnusedVar(event);
}

void DebuggerMenuHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	LEditor *editor = dynamic_cast<LEditor*>(owner);
	if ( !editor ) {
		return;
	}

	if (event.GetId() == XRCID("insert_breakpoint")) {
		editor->ToggleBreakpoint();
	}
}

void DebuggerMenuHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	wxUnusedVar(owner);
	wxUnusedVar(event);
}
