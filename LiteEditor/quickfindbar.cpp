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
#include <wx/xrc/xmlres.h>
#include <wx/regex.h>
#include "editor_config.h"
#include <wx/statline.h>
#include "manager.h"
#include "frame.h"
#include <wx/textctrl.h>
#include <wx/stc/stc.h>
#include "stringsearcher.h"
#include "quickfindbar.h"
#include "event_notifier.h"
#include "plugin.h"

DEFINE_EVENT_TYPE(QUICKFIND_COMMAND_EVENT)

#define CHECK_FOCUS_WIN() {\
        wxWindow *focus = wxWindow::FindFocus();\
        if(focus != m_sci && focus != m_findWhat) {\
            e.Skip();\
            return;\
        }\
        \
        if (!m_sci || m_sci->GetLength() == 0) {\
            e.Skip();\
            return;\
        }\
    }


void PostCommandEvent(wxWindow* destination, wxWindow* FocusedControl)
{
#if wxVERSION_NUMBER >= 2900
    //Posts an event that signals for SelectAll() to be done after a delay
    // This is often needed in >2.9, as scintilla seems to steal the selection
    wxCommandEvent event(QUICKFIND_COMMAND_EVENT);
    event.SetEventObject(FocusedControl);
    wxPostEvent(destination, event);
#endif
}

QuickFindBar::QuickFindBar(wxWindow* parent, wxWindowID id)
    : QuickFindBarBase(parent, id)
    , m_sci(NULL)
    , m_flags(0)
    , m_lastTextPtr(NULL)
    , m_themeHelper(this)
{
    Hide();
    DoShowControls();

    GetSizer()->Fit(this);
    wxTheApp->Connect(wxID_COPY,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnCopy),      NULL, this);
    wxTheApp->Connect(wxID_PASTE,     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnPaste),     NULL, this);
    wxTheApp->Connect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnSelectAll), NULL, this);
    wxTheApp->Connect(wxID_COPY,      wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuickFindBar::OnEditUI), NULL, this);
    wxTheApp->Connect(wxID_PASTE,     wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuickFindBar::OnEditUI), NULL, this);
    wxTheApp->Connect(wxID_SELECTALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuickFindBar::OnEditUI), NULL, this);
    wxTheApp->Connect(XRCID("find_next"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnFindNext),          NULL, this);
    wxTheApp->Connect(XRCID("find_previous"),          wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnFindPrevious),      NULL, this);
    wxTheApp->Connect(XRCID("find_next_at_caret"),     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnFindNextCaret),     NULL, this);
    wxTheApp->Connect(XRCID("find_previous_at_caret"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnFindPreviousCaret), NULL, this);

    EventNotifier::Get()->Connect(wxEVT_FINDBAR_RELEASE_EDITOR, wxCommandEventHandler(QuickFindBar::OnReleaseEditor), NULL, this);
    Connect(QUICKFIND_COMMAND_EVENT, wxCommandEventHandler(QuickFindBar::OnQuickFindCommandEvent), NULL, this);
}

bool QuickFindBar::Show(bool show)
{
    if (!m_sci && show) {
        return false;
    }
    return DoShow(show, wxEmptyString);
}

wchar_t* QuickFindBar::DoGetSearchStringPtr()
{
    wxString text = m_sci->GetText();
    wchar_t *pinput (NULL);
    if(m_lastText == text && m_lastTextPtr) {
        pinput = m_lastTextPtr;

    } else {
        m_lastText    = text;
#if wxVERSION_NUMBER >= 2900
        m_lastTextPtr = const_cast<wchar_t*>(m_lastText.c_str().AsWChar());
#else
        m_lastTextPtr = const_cast<wchar_t*>(m_lastText.c_str());
#endif
        pinput        = m_lastTextPtr;
    }
    return pinput;
}

void QuickFindBar::DoSearch(bool fwd, bool incr)
{
    if (!m_sci || m_sci->GetLength() == 0 || m_findWhat->GetValue().IsEmpty())
        return;
    m_flags = 0;
    if ( m_checkBoxCase->IsChecked()    ) m_flags |= wxSD_MATCHCASE;
    if ( m_checkBoxRegex->IsChecked()   ) m_flags |= wxSD_REGULAREXPRESSION;
    if ( m_checkBoxWildcard->IsChecked()) m_flags |= wxSD_WILDCARD;
    if ( m_checkBoxWord->IsChecked()    ) m_flags |= wxSD_MATCHWHOLEWORD;

    wxString find = m_findWhat->GetValue();
    wchar_t* pinput = DoGetSearchStringPtr();
    if(!pinput)
        return;
    int start = -1, stop = -1;
    m_sci->GetSelection(&start, &stop);

    int offset = !fwd || incr ? start : stop;
    int flags = m_flags | (fwd ? 0 : wxSD_SEARCH_BACKWARD);
    int pos = 0, len = 0;

    if (!StringFindReplacer::Search(pinput, offset, find.wc_str(), flags, pos, len)) {
        offset = fwd ? 0 : wxStrlen(pinput) - 1;
        if (!StringFindReplacer::Search(pinput, offset, find.wc_str(), flags, pos, len)) {
            m_findWhat->SetBackgroundColour(wxT("PINK"));
            m_findWhat->Refresh();
            return;
        }
    }

    m_findWhat->SetBackgroundColour( EditorConfigST::Get()->GetCurrentOutputviewBgColour() );
    m_findWhat->Refresh();
    m_sci->SetSelection(pos, pos+len);

    // Ensure that the found string is visible (i.e. its line isn't folded away)
    // and that the user can see it without having to scroll
    int line = m_sci->LineFromPosition(pos);
    if ( line >= 0 ) {
        m_sci->EnsureVisible(line);
        m_sci->EnsureCaretVisible();
    }
}

void QuickFindBar::OnHide(wxCommandEvent &e)
{

    // Kill any "...continued from start" statusbar message
    clMainFrame::Get()->SetStatusMessage(wxEmptyString, 0);

    Show(false);
    e.Skip();
}

void QuickFindBar::OnNext(wxCommandEvent &e)
{
    wxUnusedVar(e);
    DoSearch(true, false);
}

void QuickFindBar::OnPrev(wxCommandEvent &e)
{
    wxUnusedVar(e);
    DoSearch(false, false);
}

void QuickFindBar::OnText(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DoSearch(true, true);
}

void QuickFindBar::OnKeyDown(wxKeyEvent& e)
{
    switch (e.GetKeyCode()) {
    case WXK_ESCAPE: {
        wxCommandEvent cmd(wxEVT_COMMAND_TOOL_CLICKED, ID_TOOL_CLOSE);
        cmd.SetEventObject(m_auibarClose);
        m_auibarClose->GetEventHandler()->AddPendingEvent(cmd);
        break;
    }
    default:
        e.Skip();
    }
}

void QuickFindBar::OnUpdateUI(wxUpdateUIEvent &e)
{
    e.Enable(ManagerST::Get()->IsShutdownInProgress() == false && m_sci && m_sci->GetLength() > 0 && !m_findWhat->GetValue().IsEmpty());
}

void QuickFindBar::OnEnter(wxCommandEvent& e)
{
    wxUnusedVar(e);

    bool shift = wxGetKeyState(WXK_SHIFT);
    wxCommandEvent evt(wxEVT_COMMAND_TOOL_CLICKED, shift ? ID_TOOL_PREV : ID_TOOL_NEXT);
    evt.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(evt);
}

void QuickFindBar::OnCopy(wxCommandEvent& e)
{
    wxTextCtrl *ctrl = GetFocusedControl();
    if ( !ctrl ) {
        e.Skip();
        return;
    }

    if (ctrl->CanCopy())
        ctrl->Copy();
}

void QuickFindBar::OnPaste(wxCommandEvent& e)
{
    wxTextCtrl *ctrl = GetFocusedControl();
    if ( !ctrl ) {
        e.Skip();
        return;
    }

    if (ctrl->CanPaste())
        ctrl->Paste();
}

void QuickFindBar::OnSelectAll(wxCommandEvent& e)
{
    wxTextCtrl *ctrl = GetFocusedControl();
    if ( !ctrl ) {
        e.Skip();
    } else {
        ctrl->SelectAll();
    }
}

void QuickFindBar::OnEditUI(wxUpdateUIEvent& e)
{
    wxTextCtrl *ctrl = GetFocusedControl();
    if ( !ctrl ) {
        e.Skip();
        return;
    }

    switch (e.GetId()) {
    case wxID_SELECTALL:
        e.Enable(ctrl->GetValue().IsEmpty() == false);
        break;
    case wxID_COPY:
        e.Enable(ctrl->CanCopy());
        break;
    case wxID_PASTE:
        e.Enable(ctrl->CanPaste());
        break;
    default:
        e.Enable(false);
        break;
    }
}

void QuickFindBar::OnReplace(wxCommandEvent& e)
{
    if (!m_sci)
        return;

    // if there is no selection, invoke search
    wxString selectionText = m_sci->GetSelectedText();
    wxString find          = m_findWhat->GetValue();
    wxString replaceWith   = m_replaceWith->GetValue();

#ifndef __WXMAC__
    int re_flags = wxRE_ADVANCED;
#else
    int re_flags = wxRE_DEFAULT;
#endif

    bool caseSearch        = m_flags & wxSD_MATCHCASE;
    if ( !caseSearch ) {
        selectionText.MakeLower();
        find.MakeLower();
    }

    if (find.IsEmpty())
        return;

    // do we got a match?
    if ((selectionText != find) && !(m_flags & wxSD_REGULAREXPRESSION)) {
        DoSearch(true, true);

    } else if(m_flags & wxSD_REGULAREXPRESSION) {
        // regular expression search
        wxString selectedText = m_sci->GetSelectedText();

        // handle back references (\1 \2 etc)
        if( m_sci && selectedText.IsEmpty() == false) {

            // search was regular expression search
            // handle any back references
            caseSearch == false ? re_flags |= wxRE_ICASE : re_flags;
            wxRegEx re(find, re_flags);
            if(re.IsValid() && re.Matches(selectedText)) {
                re.Replace(&selectedText, replaceWith);
            }

            m_sci->ReplaceSelection(selectedText);
        }

        // and search again
        DoSearch(true, true);
    } else {
        // Normal replacement
        m_sci->ReplaceSelection(replaceWith);

        // and search again
        DoSearch(true, true);
    }
}

void QuickFindBar::OnReplaceUI(wxUpdateUIEvent& e)
{
    e.Enable(   ManagerST::Get()->IsShutdownInProgress() == false &&
                m_sci                                             &&
                !m_sci->GetReadOnly()                             &&
                m_sci->GetLength() > 0                            &&
                !m_findWhat->GetValue().IsEmpty());
}

wxTextCtrl* QuickFindBar::GetFocusedControl()
{
    wxWindow *win = wxWindow::FindFocus();

    if (win == m_findWhat)
        return m_findWhat;

    else if (win == m_replaceWith)
        return m_replaceWith;

    return NULL;
}

void QuickFindBar::OnReplaceEnter(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxCommandEvent evt(wxEVT_COMMAND_TOOL_CLICKED, ID_TOOL_REPLACE);
    GetEventHandler()->AddPendingEvent(evt);
}

void QuickFindBar::ShowReplaceControls(bool show)
{
    if ( show && !m_replaceWith->IsShown()) {
        m_replaceWith->Show();
        m_toolBarReplace->Show();
        m_replaceStaticText->Show();
        GetSizer()->Layout();

    } else if ( !show && m_replaceWith->IsShown()) {
        m_replaceWith->Show(false);
        m_toolBarReplace->Show(false);
        m_replaceStaticText->Show(false);
        GetSizer()->Layout();

    }
}

void QuickFindBar::SetEditor(wxStyledTextCtrl* sci)
{
    m_sci = sci;
    if ( !m_sci ) {
        DoShow(false, "");
        return;
    }
    DoShowControls();
}

int QuickFindBar::GetCloseButtonId()
{
    return ID_TOOL_CLOSE;
}

void QuickFindBar::OnToggleReplaceControls(wxCommandEvent& event)
{
    wxUnusedVar(event);
    long v(m_replaceWith->IsShown() ? 0 : 1);
    EditorConfigST::Get()->SaveLongValue(wxT("QuickFindBarShowReplace"), v);
    DoShowControls();
}

void QuickFindBar::DoShowControls()
{
    long v(1);
    EditorConfigST::Get()->GetLongValue(wxT("QuickFindBarShowReplace"), v);
    ShowReplaceControls(/*showReplaceControls*/);
    Refresh();
    GetParent()->GetSizer()->Layout();
}

bool QuickFindBar::Show(const wxString& findWhat)
{
    // Same as Show() but set the 'findWhat' field with findWhat
    if ( !m_sci )
        return false;

    return DoShow(true, findWhat);
}

bool QuickFindBar::DoShow(bool s, const wxString& findWhat)
{
    bool res = wxPanel::Show(s);
    if (res) {
        GetParent()->GetSizer()->Layout();
    }

    DoShowControls();

    if (!m_sci) {
        // nothing to do

    } else if (!s) {
        m_sci->SetFocus();

    } else if (findWhat.IsEmpty() == false) {
        m_findWhat->SetValue(findWhat);
        m_findWhat->SelectAll();
        m_findWhat->SetFocus();
        PostCommandEvent(this, m_findWhat);

    } else {
        wxString findWhat = m_sci->GetSelectedText().BeforeFirst(wxT('\n'));
        if (!findWhat.IsEmpty()) {
            m_findWhat->SetValue(m_sci->GetSelectedText().BeforeFirst(wxT('\n')));
        }
        m_findWhat->SelectAll();
        m_findWhat->SetFocus();
        PostCommandEvent(this, m_findWhat);

    }
    return res;
}

void QuickFindBar::OnFindNext(wxCommandEvent& e)
{
    CHECK_FOCUS_WIN();

    // Highlighted text takes precedence over the current search string
    wxString selectedText = m_sci->GetSelectedText();
    if (selectedText.IsEmpty() == false) {
        m_findWhat->SetValue(selectedText);
        m_findWhat->SelectAll();
    }

    DoSearch(true, false);
}

void QuickFindBar::OnFindPrevious(wxCommandEvent& e)
{
    CHECK_FOCUS_WIN();

    // Highlighted text takes precedence over the current search string
    wxString selectedText = m_sci->GetSelectedText();
    if (selectedText.IsEmpty() == false) {
        m_findWhat->SetValue(selectedText);
        m_findWhat->SelectAll();
    }

    DoSearch(false, false);
}

void QuickFindBar::OnFindNextCaret(wxCommandEvent& e)
{
    CHECK_FOCUS_WIN();

    wxString selection( m_sci->GetSelectedText() );
    if (selection.IsEmpty()) {
        // select the word
        long pos   = m_sci->GetCurrentPos();
        long start = m_sci->WordStartPosition(pos, true);
        long end   = m_sci->WordEndPosition(pos, true);

        selection = m_sci->GetTextRange(start, end);
        if (selection.IsEmpty() == false)
            m_sci->SetCurrentPos(start);
    }

    if (selection.IsEmpty())
        return;

    m_findWhat->SetValue(selection);
    DoSearch(true, false);
}

void QuickFindBar::OnFindPreviousCaret(wxCommandEvent& e)
{
    CHECK_FOCUS_WIN();

    wxString selection( m_sci->GetSelectedText() );
    if (selection.IsEmpty()) {
        // select the word
        long pos   = m_sci->GetCurrentPos();
        long start = m_sci->WordStartPosition(pos, true);
        long end   = m_sci->WordEndPosition(pos, true);

        selection = m_sci->GetTextRange(start, end);
        if (selection.IsEmpty() == false)
            m_sci->SetCurrentPos(start);
    }

    if (selection.IsEmpty())
        return;

    m_findWhat->SetValue(selection);
    DoSearch(false, false);
}


void QuickFindBar::OnToggleReplaceControlsUI(wxUpdateUIEvent& event)
{
    if (ManagerST::Get()->IsShutdownInProgress()) {
        event.Enable(false);

    } else  if (!m_sci || m_sci->GetReadOnly()) {
        event.Enable(false);

    } else {
        event.Enable(true);
        event.Check(m_replaceWith->IsShown());
    }
}

void QuickFindBar::DoMarkAll()
{
    if (!m_sci)
        return;

    LEditor *editor = dynamic_cast<LEditor*>(m_sci);
    if (!editor)
        return;

    wxString findWhat = m_findWhat->GetValue();

    if (findWhat.IsEmpty()) {
        return;
    }

    // Save the caret position
    long savedPos = m_sci->GetCurrentPos();
    size_t flags  = m_flags;

    int pos(0);
    int match_len(0);

    // remove reverse search
    flags &= ~ wxSD_SEARCH_BACKWARD;
    int offset(0);

    wchar_t* pinput = DoGetSearchStringPtr();
    if(!pinput)
        return;

    int fixed_offset(0);

    editor->DelAllMarkers(smt_find_bookmark);

    // set the active indicator to be 1
    editor->SetIndicatorCurrent(1);

    while ( StringFindReplacer::Search(pinput, offset, findWhat.wc_str(), flags, pos, match_len) ) {
        editor->MarkerAdd(editor->LineFromPosition(fixed_offset + pos), smt_find_bookmark);

        // add indicator as well
        editor->IndicatorFillRange(fixed_offset + pos, match_len);
        offset = pos + match_len;
    }

    // Restore the caret
    editor->SetCurrentPos(savedPos);
    editor->EnsureCaretVisible();
}

void QuickFindBar::OnHighlightMatches(wxCommandEvent& event)
{
    bool checked;
    checked = m_auibarFind->GetToolToggled(ID_TOOL_HIGHLIGHT_MATCHES);
    LEditor* editor = dynamic_cast<LEditor*>(m_sci);
    if (checked && editor) {
        editor->SetFindBookmarksActive(true);
        DoMarkAll();

    } else {
        if (editor) {
            editor->DelAllMarkers(smt_find_bookmark);
            editor->SetFindBookmarksActive(false);
        }
    }

    clMainFrame::Get()->SelectBestEnvSet(); // Updates the statusbar display
}

void QuickFindBar::OnHighlightMatchesUI(wxUpdateUIEvent& event)
{
    if (ManagerST::Get()->IsShutdownInProgress()) {
        event.Enable(false);

    } else  if (!m_sci) {
        event.Enable(false);

    } else if ( m_findWhat->GetValue().IsEmpty() ) {
        event.Enable(false);

    } else {
        LEditor *editor = dynamic_cast<LEditor*>( m_sci );
        if( !editor ) {
            event.Enable(false);

        } else {
            // Check to see if there are any markers
            int nLine = editor->LineFromPosition(0);
            int nFoundLine = editor->MarkerNext(nLine, mmt_find_bookmark);

            event.Enable(true);
            event.Check(nFoundLine != wxNOT_FOUND);
        }
    }
}

void QuickFindBar::OnReceivingFocus(wxFocusEvent& event)
{
    event.Skip();
    if ((event.GetEventObject() == m_findWhat) || (event.GetEventObject() == m_replaceWith)) {
        PostCommandEvent(this, wxStaticCast(event.GetEventObject(), wxWindow));
    }
}

void QuickFindBar::OnQuickFindCommandEvent(wxCommandEvent& event)
{
    if (event.GetEventObject() == m_findWhat) {
        m_findWhat->SelectAll();

    } else if (event.GetEventObject() == m_replaceWith) {
        m_replaceWith->SelectAll();
    }
}

QuickFindBar::~QuickFindBar()
{
    wxTheApp->Disconnect(wxID_COPY,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnCopy),      NULL, this);
    wxTheApp->Disconnect(wxID_PASTE,     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnPaste),     NULL, this);
    wxTheApp->Disconnect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnSelectAll), NULL, this);
    wxTheApp->Disconnect(wxID_COPY,      wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuickFindBar::OnEditUI), NULL, this);
    wxTheApp->Disconnect(wxID_PASTE,     wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuickFindBar::OnEditUI), NULL, this);
    wxTheApp->Disconnect(wxID_SELECTALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuickFindBar::OnEditUI), NULL, this);
    wxTheApp->Disconnect(XRCID("find_next"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnFindNext),          NULL, this);
    wxTheApp->Disconnect(XRCID("find_previous"),          wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnFindPrevious),      NULL, this);
    wxTheApp->Disconnect(XRCID("find_next_at_caret"),     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnFindNextCaret),     NULL, this);
    wxTheApp->Disconnect(XRCID("find_previous_at_caret"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnFindPreviousCaret), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_FINDBAR_RELEASE_EDITOR, wxCommandEventHandler(QuickFindBar::OnReleaseEditor), NULL, this);
}

void QuickFindBar::OnReleaseEditor(wxCommandEvent& e)
{
    wxStyledTextCtrl *win = reinterpret_cast<wxStyledTextCtrl*>(e.GetClientData());
    if ( win && win == m_sci ) {
        m_sci = NULL;
        Show(false);
    }
}

wxStyledTextCtrl* QuickFindBar::DoCheckPlugins()
{
    // Let the plugins a chance to provide their own window
    wxCommandEvent evt(wxEVT_FINDBAR_ABOUT_TO_SHOW);
    evt.SetClientData(NULL);
    EventNotifier::Get()->ProcessEvent( evt );

    wxStyledTextCtrl* win = reinterpret_cast<wxStyledTextCtrl*>( evt.GetClientData() );
    return win;
}

bool QuickFindBar::ShowForPlugins()
{
    m_sci = DoCheckPlugins();
    if ( !m_sci ) {
        return DoShow(false, "");
    } else {
        return DoShow(true, "");
    }
}

void QuickFindBar::OnCheckBoxRegex(wxCommandEvent& event)
{
    // regex and wildcard can not co-exist
    if ( event.IsChecked() ) {
        m_checkBoxWildcard->SetValue( false );
    }
}

void QuickFindBar::OnCheckWild(wxCommandEvent& event)
{
    // regex and wildcard can not co-exist
    if ( event.IsChecked() ) {
        m_checkBoxRegex->SetValue( false );
    }
}
