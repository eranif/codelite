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
#include "manager.h"
#include "cl_editor.h"
#include "frame.h"
#include "cl_command_event.h"
#include "event_notifier.h"
#include "codelite_events.h"

//------------------------------------
// Handle copy events
//------------------------------------
void EditHandler::ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event)
{
    wxUnusedVar(event);
    clEditor* editor = (clEditor*)owner;

    OptionsConfigPtr options = editor->GetOptions();
    switch(event.GetId()) {
    case wxID_ZOOM_IN:
        editor->ZoomIn();
        return;
    case wxID_ZOOM_OUT:
        editor->ZoomOut();
        return;
    case wxID_ZOOM_FIT:
        editor->SetZoom(0);
        return;
    case wxID_COPY:
        if(options->GetCopyLineEmptySelection()) {
            editor->CopyAllowLine();
        } else {
            editor->Copy();
        }
        return;
    case wxID_CUT:
        editor->Cut();
        return;
    case wxID_PASTE:
        editor->Paste();
        return;
    case wxID_SELECTALL:
        editor->SelectAll();
        return;
    case wxID_DUPLICATE:
        editor->SelectionDuplicate();
        return;
    case wxID_DELETE:
        editor->DeleteBack();
        return;
    default:
        break;
    }

    if(event.GetId() == wxID_UNDO) {
        if(editor->GetCommandsProcessor().CanUndo()) {
            editor->Undo();
            editor->GetCommandsProcessor().CloseSciUndoAction();
            editor->GetCommandsProcessor().DecrementCurrentCommand();
        }

    } else if(event.GetId() == wxID_REDO) {
        if(editor->GetCommandsProcessor().CanRedo()) {
            editor->Redo();
            editor->GetCommandsProcessor().CloseSciUndoAction(); // Is this necessary? At least it does no harm
            editor->GetCommandsProcessor().IncrementCurrentCommand();
        }

    } else if(event.GetId() == XRCID("label_current_state")) {
        wxString label =
            wxGetTextFromUser("What would you like to call the current state?", "Label current state", "", editor);
        if(!label.empty()) { editor->GetCommandsProcessor().SetUserLabel(label); }

    } else if(event.GetId() == XRCID("delete_line_end")) {
        editor->DelLineRight();

    } else if(event.GetId() == XRCID("delete_line_start")) {
        editor->DelLineLeft();

    } else if(event.GetId() == XRCID("delete_line")) {
        editor->LineDelete();

    } else if(event.GetId() == XRCID("copy_line")) {
        editor->LineCopy();

    } else if(event.GetId() == XRCID("cut_line")) {
        editor->LineCut();

    } else if(event.GetId() == XRCID("trim_trailing")) {
        editor->TrimText(true, false);

    } else if(event.GetId() == XRCID("to_lower")) {
        editor->ChangeCase(true);

    } else if(event.GetId() == XRCID("to_upper")) {
        editor->ChangeCase(false);

    } else if(event.GetId() == XRCID("transpose_lines")) {
        editor->LineTranspose();

    } else if(event.GetId() == XRCID("move_line_down")) {

        int curline = editor->GetCurrentLine();
        int lastline = editor->LineFromPosition(editor->GetLength() - 1);

        if(editor->GetSelection().empty() ||
           (editor->LineFromPos(editor->GetSelectionStart() == editor->LineFromPos(editor->GetSelectionEnd())))) {
            // No selection (or only a trivial 1-line one)
            if(curline != lastline) {
                editor->LineDown();
                editor->LineTranspose();
            }

        } else {
            editor->MoveSelectedLinesDown(); // There is a selection, so we can move it direct
        }

    } else if(event.GetId() == XRCID("move_line_up")) {

        if(editor->GetSelection().empty() ||
           (editor->LineFromPos(editor->GetSelectionStart() == editor->LineFromPos(editor->GetSelectionEnd())))) {
            // No selection (or only a trivial 1-line one)
            editor->LineTranspose();
            editor->LineUp();

        } else {
            editor->MoveSelectedLinesUp(); // There is a selection, so we can move it direct
        }

    } else if(event.GetId() == XRCID("center_line_roll")) {
        int here = editor->GetCurrentLine();
        int top = editor->GetFirstVisibleLine();
        int count = editor->LinesOnScreen();
        int center = top + (count / 2);
        if(here < center) {
            for(int lnIterator = 0; lnIterator < center - here; lnIterator++)
                editor->LineScrollUp(); // roll up until we get to center
        } else if(here > center) {
            for(int lnIterator = 0; lnIterator < here - center; lnIterator++)
                editor->LineScrollDown(); // roll down until we get to center
        }

    } else if(event.GetId() == XRCID("convert_indent_to_spaces")) {
        editor->ConvertIndentToSpaces();
    } else if(event.GetId() == XRCID("convert_indent_to_tabs")) {
        editor->ConvertIndentToTabs();
    }
}

void EditHandler::ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event)
{
    clEditor* editor = dynamic_cast<clEditor*>(owner);

    if(event.GetId() == wxID_COPY || event.GetId() == XRCID("trim_trailing") || event.GetId() == XRCID("to_lower") ||
       event.GetId() == XRCID("to_upper")) {
        event.Enable(editor);

    } else if(event.GetId() == wxID_CUT) {
        event.Enable(editor);

    } else if(event.GetId() == wxID_PASTE) {
#ifdef __WXGTK__
        event.Enable(editor);
#else
        event.Enable(editor && editor->CanPaste());
#endif
    } else if(event.GetId() == wxID_UNDO) {
        event.Enable(editor && editor->CanUndo());
    } else if(event.GetId() == wxID_REDO) {
        event.Enable(editor && editor->CanRedo());
    } else if(event.GetId() == wxID_SELECTALL) {
        event.Enable(editor && editor->GetLength() > 0);
    } else if(event.GetId() == wxID_DUPLICATE || event.GetId() == wxID_DELETE) {
        event.Enable(true);
    } else {
        event.Enable(false);
    }
    event.Skip(false);
}

//------------------------------------
// brace matching
//------------------------------------
void BraceMatchHandler::ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event)
{
    clEditor* editor = dynamic_cast<clEditor*>(owner);
    if(!editor) { return; }

    if(event.GetId() == XRCID("select_to_brace")) {
        editor->MatchBraceAndSelect(true);
    } else {
        editor->MatchBraceAndSelect(false);
    }
}

void BraceMatchHandler::ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event)
{
    clEditor* editor = dynamic_cast<clEditor*>(owner);
    event.Enable(editor && editor->GetLength() > 0);
}

//------------------------------------
// Find / Replace
//------------------------------------
void FindReplaceHandler::ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event)
{
    clEditor* editor = dynamic_cast<clEditor*>(owner);
    if(editor) {
        if(event.GetId() == wxID_FIND) {
            clMainFrame::Get()->GetMainBook()->ShowQuickBar(editor->GetFirstSelection(), false);
        } else if(event.GetId() == wxID_REPLACE) {
            clMainFrame::Get()->GetMainBook()->ShowQuickBar(editor->GetFirstSelection(), true);

        } else if(event.GetId() == XRCID("ID_QUICK_ADD_NEXT")) {
            editor->QuickAddNext();

        } else if(event.GetId() == XRCID("ID_QUICK_FIND_ALL")) {
            editor->QuickFindAll();
        }
    }
}

void FindReplaceHandler::ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event)
{
    wxUnusedVar(owner);
    wxUnusedVar(event);
}

//----------------------------------
// goto linenumber
//----------------------------------

void GotoHandler::ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event)
{
    wxUnusedVar(event);
    clEditor* editor = dynamic_cast<clEditor*>(owner);
    if(!editor) { return; }

    wxString msg;
    msg.Printf(_("Go to line number (1 - %i):"), editor->GetLineCount());

    while(1) {
        wxTextEntryDialog dlg(editor, msg, _("Go To Line"));
        dlg.SetTextValidator(wxFILTER_NUMERIC);

        if(dlg.ShowModal() == wxID_OK) {
            wxString val = dlg.GetValue();
            long line;
            if(!val.ToLong(&line)) {
                wxString err;
                err.Printf(_("'%s' is not a valid line number"), val.GetData());
                wxMessageBox(err, _("Go To Line"), wxOK | wxICON_INFORMATION);
                continue;
            }

            if(line > editor->GetLineCount()) {
                wxString err;
                err.Printf(_("Please insert a line number in the range of (1 - %i)"), editor->GetLineCount());
                wxMessageBox(err, _("Go To Line"), wxOK | wxICON_INFORMATION);
                continue;
            }

            if(line > 0) {
                editor->CenterLine(line - 1);
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

void GotoHandler::ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event)
{
    wxUnusedVar(event);
    wxUnusedVar(owner);
}

//------------------------------------
// Bookmarks
//------------------------------------
void BookmarkHandler::ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event)
{
    clEditor* editor = dynamic_cast<clEditor*>(owner);
    if(!editor) { return; }

    if(event.GetId() == XRCID("toggle_bookmark")) {
        editor->ToggleMarker();
    } else if(event.GetId() == XRCID("next_bookmark")) {
        editor->FindNextMarker();
    } else if(event.GetId() == XRCID("previous_bookmark")) {
        editor->FindPrevMarker();
    } else if(event.GetId() == XRCID("removeall_current_bookmarks")) {
        editor->DelAllMarkers(0); //  0 == only the currently-active type
    } else if(event.GetId() == XRCID("removeall_bookmarks")) {
        editor->DelAllMarkers(-1); // -1 == all types
    }
}

void BookmarkHandler::ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event)
{
    wxUnusedVar(owner);
    wxUnusedVar(event);
}

//------------------------------------
// Go to definition
//------------------------------------
void GotoDefinitionHandler::ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event)
{
    clEditor* editor = dynamic_cast<clEditor*>(owner);
    if(!editor) { return; }
    if(event.GetId() == XRCID("goto_definition")) { editor->GotoDefinition(); }
}

void GotoDefinitionHandler::ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event)
{
    clEditor* editor = dynamic_cast<clEditor*>(owner);
    if(event.GetId() == XRCID("goto_previous_definition")) {
        event.Enable(editor && editor->CanGotoPreviousDefintion());
    } else {
        event.Enable(editor != NULL);
    }
}

//----------------------------------------------------
// Word wrap handler
//----------------------------------------------------

void WordWrapHandler::ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event)
{
    clEditor* editor = dynamic_cast<clEditor*>(owner);
    if(!editor) {
        event.Enable(false);
        return;
    }

    event.Enable(true);
    event.Check(editor->GetWrapMode() != wxSTC_WRAP_NONE);
}

void WordWrapHandler::ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event)
{
    clEditor* editor = dynamic_cast<clEditor*>(owner);
    if(!editor) { return; }

    editor->SetWrapMode(event.IsChecked() ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE);
}
//----------------------------------------------------
// Fold handler
//----------------------------------------------------

void FoldHandler::ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event)
{
    wxUnusedVar(event);
    clEditor* editor = dynamic_cast<clEditor*>(owner);
    if(!editor) { return; }

    if(event.GetId() == XRCID("toggle_fold"))
        editor->ToggleCurrentFold();
    else if(event.GetId() == XRCID("fold_all_in_selection"))
        editor->ToggleAllFoldsInSelection();
    else if(event.GetId() == XRCID("fold_topmost_in_selection"))
        editor->ToggleTopmostFoldsInSelection();
    else
        editor->FoldAll();
}

void FoldHandler::ProcessUpdateUIEvent(wxWindow* owner,
                                       wxUpdateUIEvent& event) // Used for ToggleAllFoldsInSelection() etc
{
    clEditor* editor = dynamic_cast<clEditor*>(owner);
    if(!editor) {
        event.Enable(false);
        return;
    }

    bool hasSelection = !editor->GetSelection().empty();
    if(hasSelection) {
        // Even if there is a selection, check it's not a trivial amount i.e. spans at least 2 lines
        hasSelection =
            editor->LineFromPos(editor->GetSelectionStart()) != editor->LineFromPos(editor->GetSelectionEnd());
    }
    event.Enable(hasSelection);
}

void DebuggerMenuHandler::ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event)
{
    clEditor* editor = dynamic_cast<clEditor*>(owner);
    if(!editor) { return; }

    if(event.GetId() == XRCID("add_breakpoint")) { editor->AddBreakpoint(); }
    if(event.GetId() == XRCID("insert_breakpoint")) { editor->ToggleBreakpoint(); }
    if((event.GetId() == XRCID("insert_temp_breakpoint")) || (event.GetId() == XRCID("insert_disabled_breakpoint")) ||
       (event.GetId() == XRCID("insert_cond_breakpoint"))) {
        editor->AddOtherBreakpointType(event);
    }
    if(event.GetId() == XRCID("delete_breakpoint")) { editor->DelBreakpoint(); }

    if(event.GetId() == XRCID("toggle_breakpoint_enabled_status")) { editor->ToggleBreakpointEnablement(); }

    if(event.GetId() == XRCID("ignore_breakpoint")) { editor->OnIgnoreBreakpoint(); }

    if(event.GetId() == XRCID("edit_breakpoint")) { editor->OnEditBreakpoint(); }

    if(event.GetId() == XRCID("disable_all_breakpoints")) {
        clDebugEvent event(wxEVT_DBG_UI_DISABLE_ALL_BREAKPOINTS);
        EventNotifier::Get()->ProcessEvent(event);
        ManagerST::Get()->GetBreakpointsMgr()->SetAllBreakpointsEnabledState(false);
    }

    if(event.GetId() == XRCID("enable_all_breakpoints")) {
        clDebugEvent event(wxEVT_DBG_UI_ENABLE_ALL_BREAKPOINTS);
        EventNotifier::Get()->ProcessEvent(event);
        ManagerST::Get()->GetBreakpointsMgr()->SetAllBreakpointsEnabledState(true);
    }

    if(event.GetId() == XRCID("delete_all_breakpoints")) {

        // First let the plugins do this thing
        clDebugEvent event(wxEVT_DBG_UI_DELETE_ALL_BREAKPOINTS);
        EventNotifier::Get()->AddPendingEvent(event);

        // Now clear the manager
        ManagerST::Get()->GetBreakpointsMgr()->DelAllBreakpoints();
    }
}

void DebuggerMenuHandler::ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event)
{
    wxUnusedVar(owner);
    wxUnusedVar(event);
}
