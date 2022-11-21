//////////////////////////////////////////////////////////////////////////////
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
#include "quickfindbar.h"

#include "bitmap_loader.h"
#include "bookmark_manager.h"
#include "clBitmapOverlayCtrl.h"
#include "clSystemSettings.h"
#include "cl_command_event.h"
#include "cl_config.h"
#include "cl_editor.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "frame.h"
#include "globals.h"
#include "imanager.h"
#include "manager.h"
#include "plugin.h"
#include "stringsearcher.h"

#include <wx/dcbuffer.h>
#include <wx/gdicmn.h>
#include <wx/regex.h>
#include <wx/statline.h>
#include <wx/stc/stc.h>
#include <wx/stopwatch.h>
#include <wx/textctrl.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

DEFINE_EVENT_TYPE(QUICKFIND_COMMAND_EVENT)

#define CHECK_FOCUS_WIN(evt)                            \
    {                                                   \
        wxWindow* focus = wxWindow::FindFocus();        \
        if(focus != m_sci && focus != m_textCtrlFind) { \
            evt.Skip();                                 \
            return;                                     \
        }                                               \
                                                        \
        if(!m_sci || m_sci->GetLength() == 0) {         \
            evt.Skip();                                 \
            return;                                     \
        }                                               \
    }

namespace
{
void PostCommandEvent(wxWindow* destination, wxWindow* FocusedControl)
{
    // Posts an event that signals for SelectAll() to be done after a delay
    // This is often needed in >2.9, as scintilla seems to steal the selection
    const static int DELAY_COUNT = 10;

    wxCommandEvent event(QUICKFIND_COMMAND_EVENT);
    event.SetInt(DELAY_COUNT);
    event.SetEventObject(FocusedControl);
    wxPostEvent(destination, event);
}

void CenterLine(wxStyledTextCtrl* ctrl, int start_pos, int end_pos)
{
    // calculate the column
    ctrl->ClearSelections();
    ctrl->SetSelection(start_pos, end_pos);

    int line = ctrl->LineFromPosition(start_pos);
    clEditor::CenterLinePreserveSelection(ctrl, line);
}

} // namespace
QuickFindBar::QuickFindBar(wxWindow* parent, wxWindowID id)
    : QuickFindBarBase(parent, id)
    , m_sci(NULL)
    , m_lastTextPtr(NULL)
    , m_eventsConnected(false)
    , m_regexType(kRegexNone)
    , m_disableTextUpdateEvent(false)
    , m_searchFlags(0)
    , m_highlightMatches(false)
    , m_inSelection(false)
{
    m_toolbar->SetMiniToolBar(true);
    auto images = m_toolbar->GetBitmapsCreateIfNeeded();
    m_toolbar->AddTool(wxID_CLOSE, _("Close"), images->Add("file_close"), _("Close"), wxITEM_NORMAL);
    m_toolbar->AddSeparator();
    m_toolbar->AddStretchableSpace();
    m_toolbar->AddTool(XRCID("case-sensitive"), _("Case Sensitive"), images->Add("case-sensitive"), "", wxITEM_CHECK);
    m_toolbar->AddTool(XRCID("whole-word"), _("Whole word"), images->Add("whole-word"), "", wxITEM_CHECK);
    m_toolbar->AddTool(XRCID("use-regex"), _("Regex"), images->Add("regular-expression"), "", wxITEM_CHECK);
    m_toolbar->AddTool(XRCID("highlight-matches"), _("Highlight matches"), images->Add("marker"), "", wxITEM_CHECK);
    m_toolbar->AddTool(XRCID("replace-in-selection"), _("In Selection"), images->Add("text_selection"), "",
                       wxITEM_CHECK);
    m_toolbar->Realize();
    m_toolbar->Bind(wxEVT_TOOL, &QuickFindBar::OnHide, this, wxID_CLOSE);
    m_toolbar->Bind(
        wxEVT_TOOL,
        [&](wxCommandEvent& e) {
            if(e.IsChecked()) {
                m_searchFlags |= wxSTC_FIND_MATCHCASE;
            } else {
                m_searchFlags &= ~wxSTC_FIND_MATCHCASE;
            }
        },
        XRCID("case-sensitive"));
    m_toolbar->Bind(
        wxEVT_TOOL,
        [&](wxCommandEvent& e) {
            if(e.IsChecked()) {
                m_searchFlags |= wxSTC_FIND_WHOLEWORD;
            } else {
                m_searchFlags &= ~wxSTC_FIND_WHOLEWORD;
            }
        },
        XRCID("whole-word"));
    m_toolbar->Bind(
        wxEVT_TOOL,
        [&](wxCommandEvent& e) {
            if(e.IsChecked()) {
                m_searchFlags |= wxSTC_FIND_REGEXP;
            } else {
                m_searchFlags &= ~wxSTC_FIND_REGEXP;
            }
        },
        XRCID("use-regex"));
    m_toolbar->Bind(
        wxEVT_TOOL,
        [&](wxCommandEvent& e) {
            m_highlightMatches = e.IsChecked();
            DoHighlightMatches(m_highlightMatches);
        },
        XRCID("highlight-matches"));
    m_toolbar->Bind(
        wxEVT_TOOL, [&](wxCommandEvent& e) { m_inSelection = e.IsChecked(); }, XRCID("replace-in-selection"));
    m_toolbar->Bind(
        wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& e) { e.Check(m_searchFlags & wxSTC_FIND_MATCHCASE); },
        XRCID("case-sensitive"));
    m_toolbar->Bind(
        wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& e) { e.Check(m_searchFlags & wxSTC_FIND_REGEXP); }, XRCID("use-regex"));
    m_toolbar->Bind(
        wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& e) { e.Check(m_searchFlags & wxSTC_FIND_WHOLEWORD); },
        XRCID("whole-word"));
    m_toolbar->Bind(
        wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& e) { e.Check(m_highlightMatches); }, XRCID("highlight-matches"));
    m_toolbar->Bind(
        wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& e) { e.Check(m_inSelection); }, XRCID("replace-in-selection"));

    wxTheApp->Bind(wxEVT_MENU, &QuickFindBar::OnFindNextCaret, this, XRCID("find_next_at_caret"));
    wxTheApp->Bind(wxEVT_MENU, &QuickFindBar::OnFindPreviousCaret, this, XRCID("find_previous_at_caret"));

    EventNotifier::Get()->Bind(wxEVT_FINDBAR_RELEASE_EDITOR, &QuickFindBar::OnReleaseEditor, this);
    Connect(QUICKFIND_COMMAND_EVENT, wxCommandEventHandler(QuickFindBar::OnQuickFindCommandEvent), NULL, this);

    // Initialize the list with the history
    m_searchHistory.SetItems(clConfig::Get().GetQuickFindSearchItems());
    m_replaceHistory.SetItems(clConfig::Get().GetQuickFindReplaceItems());

    // Update the search flags
    m_searchFlags = clConfig::Get().Read("FindBar/SearchFlags", 0);
    m_highlightMatches = clConfig::Get().Read("FindBar/HighlightOccurences", false);

    SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, [this](clCommandEvent& e) {
        e.Skip();
        SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
        Refresh();
    });

    // Make sure that the 'Replace' field is selected when we hit TAB while in the 'Find' field
    m_textCtrlReplace->MoveAfterInTabOrder(m_textCtrlFind);
    // Bind(wxEVT_PAINT, &QuickFindBar::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent& e) { wxUnusedVar(e); });
    GetSizer()->Fit(this);
    Layout();
}

QuickFindBar::~QuickFindBar()
{
    // Remember the buttons clicked
    clConfig::Get().Write("FindBar/SearchFlags", (int)DoGetSearchFlags());
    clConfig::Get().Write("FindBar/HighlightOccurences", m_highlightMatches);

    wxTheApp->Unbind(wxEVT_MENU, &QuickFindBar::OnFindNextCaret, this, XRCID("find_next_at_caret"));
    wxTheApp->Unbind(wxEVT_MENU, &QuickFindBar::OnFindPreviousCaret, this, XRCID("find_previous_at_caret"));
    EventNotifier::Get()->Unbind(wxEVT_FINDBAR_RELEASE_EDITOR, &QuickFindBar::OnReleaseEditor, this);
}

bool QuickFindBar::Show(bool show)
{
    if(!m_sci && show) {
        return false;
    }
    return DoShow(show, wxEmptyString);
}

void QuickFindBar::OnHide(wxCommandEvent& e)
{
    // Kill any "...continued from start" statusbar message
    clMainFrame::Get()->GetStatusBar()->SetMessage(wxEmptyString);

    // Clear all
    Show(false);
    e.Skip();
}

#define UPDATE_FIND_HISTORY()                                                \
    if(!m_textCtrlFind->GetValue().IsEmpty()) {                              \
        m_searchHistory.Add(m_textCtrlFind->GetValue());                     \
        clConfig::Get().SetQuickFindSearchItems(m_searchHistory.GetItems()); \
    }

void QuickFindBar::OnText(wxCommandEvent& e)
{
    e.Skip();
    if(!m_inSelection && !m_disableTextUpdateEvent) {
        // searching down
        DoFindWithWrap(FIND_INCREMENT | FIND_GOTOLINE);
    }
}

void QuickFindBar::OnKeyDown(wxKeyEvent& e)
{
    switch(e.GetKeyCode()) {
    case WXK_DOWN: {
        // DoArrowDown(m_searchHistory, m_textCtrlFind);
        break;
    }
    case WXK_UP: {
        // DoArrowUp(m_searchHistory, m_textCtrlFind);
        break;
    }
    case WXK_ESCAPE: {
        wxCommandEvent dummy;
        OnHide(dummy);
        DoHighlightMatches(false);
        break;
    }
    default: {
        e.Skip();
        break;
    }
    }
}
void QuickFindBar::OnReplaceKeyDown(wxKeyEvent& e)
{
    switch(e.GetKeyCode()) {
    case WXK_DOWN: {
        // DoArrowDown(m_replaceHistory, m_textCtrlReplace);
        break;
    }
    case WXK_UP: {
        // DoArrowUp(m_replaceHistory, m_textCtrlReplace);
        break;
    }
    case WXK_ESCAPE: {
        wxCommandEvent dummy;
        OnHide(dummy);
        DoHighlightMatches(false);
        break;
    }
    default: {
        e.Skip();
        break;
    }
    }
}

void QuickFindBar::OnUpdateUI(wxUpdateUIEvent& e)
{
    e.Enable(ManagerST::Get()->IsShutdownInProgress() == false && m_sci && m_sci->GetLength() > 0 &&
             !m_textCtrlFind->GetValue().IsEmpty());
}

void QuickFindBar::OnEnter(wxCommandEvent& e)
{
    wxUnusedVar(e);

    if(!m_textCtrlFind->GetValue().IsEmpty()) {
        m_searchHistory.Add(m_textCtrlFind->GetValue());
        clConfig::Get().SetQuickFindSearchItems(m_searchHistory.GetItems());
    }

    bool shift = wxGetKeyState(WXK_SHIFT);
    size_t find_flags = shift ? FIND_PREV : FIND_DEFAULT;
    find_flags |= FIND_GOTOLINE;
    DoFindWithWrap(find_flags);

    // Without this call, the caret is placed at the start of the searched
    // text, this at least places the caret at the end
    CallAfter(&QuickFindBar::DoSetCaretAtEndOfText);
}

void QuickFindBar::OnReplace(wxCommandEvent& event)
{
    CHECK_PTR_RET(m_sci);

    TargetRange target;
    int start_pos = 0;
    if(m_sci->HasSelection()) {
        // we are starting with a selected text
        // check this range to see if it matches the 'find-what'
        // string
        TargetRange result = DoFind(FIND_DEFAULT, { m_sci->GetSelectionStart(), m_sci->GetSelectionEnd() });
        if(result.IsOk()) {
            // the selected text is a match, replace it
            int len = DoReplace(result);
            if(len != wxNOT_FOUND) {
                CenterLine(m_sci, result.start_pos, result.start_pos + len);
                m_sci->SetFocus();
                start_pos = result.start_pos + len;
            } else {
                // should not happen...
                return;
            }
        } else {
            start_pos = m_sci->GetSelectionStart();
        }
    } else {
        start_pos = m_sci->GetCurrentPos();
    }

    // perform a search
    DoFindWithMessage(FIND_DEFAULT | FIND_GOTOLINE, { start_pos, static_cast<int>(m_sci->GetLastPosition()) });
}

int QuickFindBar::DoReplace(const TargetRange& range)
{
    // we replace the selection
    TargetRange target_range;
    if(range.IsOk()) {
        target_range = range;
    } else if(!m_sci->HasSelection()) {
        target_range = { m_sci->GetSelectionStart(), m_sci->GetSelectionEnd() };
    }

    if(!target_range.IsOk()) {
        return wxNOT_FOUND;
    }

    m_sci->SetTargetStart(target_range.start_pos);
    m_sci->SetTargetEnd(target_range.end_pos);

    if(m_searchFlags & wxSTC_FIND_REGEXP) {
        return m_sci->ReplaceTargetRE(m_textCtrlReplace->GetValue());
    } else {
        return m_sci->ReplaceTarget(m_textCtrlReplace->GetValue());
    }
}

void QuickFindBar::OnReplaceEnter(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxCommandEvent evt(wxEVT_COMMAND_TOOL_CLICKED, ID_TOOL_REPLACE);
    GetEventHandler()->AddPendingEvent(evt);
}

void QuickFindBar::SetEditor(wxStyledTextCtrl* sci)
{
    m_sci = sci;
    if(!m_sci) {
        DoShow(false, "");
        return;
    }
}

int QuickFindBar::GetCloseButtonId() { return ID_TOOL_CLOSE; }

bool QuickFindBar::Show(const wxString& findWhat, bool showReplace)
{
    // Same as Show() but set the 'findWhat' field with findWhat
    // and show/hide the 'Replace' section depending on the bool
    if(!m_sci)
        return false;
    return DoShow(true, findWhat, showReplace);
}

bool QuickFindBar::DoShow(bool s, const wxString& findWhat, bool showReplace)
{
#ifdef __WXMSW__
    wxWindowUpdateLocker locker(this);
#endif
    bool res = wxPanel::Show(s);
    if(s && m_sci) {
        // Delete the indicators
        m_sci->SetIndicatorCurrent(1);
        m_sci->IndicatorClearRange(0, m_sci->GetLength());

        if(EditorConfigST::Get()->GetOptions()->GetClearHighlitWordsOnFind()) {
            m_sci->SetIndicatorCurrent(MARKER_FIND_BAR_WORD_HIGHLIGHT);
            m_sci->IndicatorClearRange(0, m_sci->GetLength());
        }
    }

    if(s) {
        // Show or Hide the 'Replace' section as requested
        wxSizer* flexgridsizer = m_textCtrlFind->GetContainingSizer();
        if(flexgridsizer) {
            if(!GetSizer()->IsShown(flexgridsizer)) {
                GetSizer()->Show(flexgridsizer);
            }
            if(showReplace) {
                flexgridsizer->ShowItems(true);
            } else {
                for(size_t n = 4; n < 7; ++n) {
                    flexgridsizer->Hide(n);
                }
            }
        }
    }
    if(res) {
        GetParent()->GetSizer()->Layout();
    }

    m_inSelection = !findWhat.IsEmpty() && findWhat.Contains("\n");
    if(!m_sci) {
        // nothing to do

    } else if(!s) {
        // hiding
        DoHighlightMatches(false);
        m_sci->SetFocus();

    } else if(!findWhat.IsEmpty()) {

        if(findWhat.Contains("\n")) {
            // Multiline selection
            // enable the 'Replace in Selection'
            m_textCtrlFind->ChangeValue("");
            m_textCtrlFind->SetFocus();
        } else {
            m_textCtrlFind->ChangeValue(findWhat);
            m_textCtrlFind->SelectAll();
            m_textCtrlFind->SetFocus();
            if(m_highlightMatches) {
                if(!(m_searchFlags & wxSTC_FIND_REGEXP) || m_textCtrlFind->GetValue().Length() > 2) {
                    DoHighlightMatches(true);
                }
            }
            PostCommandEvent(this, m_textCtrlFind);
        }

    } else {
        if(m_sci->GetSelections() > 1) {}
        wxString findWhat = DoGetSelectedText().BeforeFirst(wxT('\n'));
        if(!findWhat.IsEmpty()) {
            m_textCtrlFind->ChangeValue(findWhat);
        }

        m_textCtrlFind->SelectAll();
        m_textCtrlFind->SetFocus();
        if(m_highlightMatches) {
            if(!(m_searchFlags & wxSTC_FIND_REGEXP) || m_textCtrlFind->GetValue().Length() > 2) {
                DoHighlightMatches(true);
            }
        }
        PostCommandEvent(this, m_textCtrlFind);
    }
    return res;
}

void QuickFindBar::OnFindNextCaret(wxCommandEvent& e)
{
    CHECK_FOCUS_WIN(e);

    wxString selection(DoGetSelectedText());
    if(selection.IsEmpty()) {
        // select the word
        long pos = m_sci->GetCurrentPos();
        long start = m_sci->WordStartPosition(pos, true);
        long end = m_sci->WordEndPosition(pos, true);

        selection = m_sci->GetTextRange(start, end);
        if(selection.IsEmpty() == false)
            m_sci->SetCurrentPos(start);
    }

    if(selection.IsEmpty())
        return;

    m_textCtrlFind->ChangeValue(selection);
    DoFind(FIND_DEFAULT | FIND_GOTOLINE);
}

void QuickFindBar::OnFindPreviousCaret(wxCommandEvent& e)
{
    CHECK_FOCUS_WIN(e);

    wxString selection(DoGetSelectedText());
    if(selection.IsEmpty()) {
        // select the word
        long pos = m_sci->GetCurrentPos();
        long start = m_sci->WordStartPosition(pos, true);
        long end = m_sci->WordEndPosition(pos, true);

        selection = m_sci->GetTextRange(start, end);
        if(selection.IsEmpty() == false)
            m_sci->SetCurrentPos(start);
    }

    if(selection.IsEmpty())
        return;

    m_textCtrlFind->ChangeValue(selection);
    DoFind(FIND_PREV | FIND_GOTOLINE);
}

void QuickFindBar::DoSelectAll()
{
    clGetManager()->SetStatusMessage(wxEmptyString);
    if(!m_sci) {
        return;
    }

    wxBusyCursor bc;
    TargetRange target = GetBestTargetRange();

    auto matches = DoFindAll(target);
    if(matches.empty()) {
        clGetManager()->SetStatusMessage(_("No matches found"));
        return;
    }

    // add selections
    m_sci->ClearSelections();
    for(size_t i = 0; i < matches.size(); ++i) {
        if(i == 0) {
            m_sci->SetSelection(matches[i].start_pos, matches[i].end_pos);
            m_sci->SetMainSelection(0);
            CenterLine(m_sci, matches[i].start_pos, matches[i].end_pos);
        } else {
            m_sci->AddSelection(matches[i].start_pos, matches[i].end_pos);
        }
    }
    Show(false);
    clGetManager()->SetStatusMessage(wxString::Format(_("Selected %lu matches"), matches.size()));
    m_sci->SetMainSelection(0);
}

TargetRange::Vec_t QuickFindBar::DoFindAll(const TargetRange& target)
{
    if(!m_sci) {
        return {};
    }

    TargetRange range{ 0, static_cast<int>(m_sci->GetLastPosition()) };
    if(target.IsOk()) {
        range = target;
    }
    // perform a search

    TargetRange::Vec_t matches;
    matches.reserve(1000); // enough room for 1000 martches

    auto target_result = DoFind(FIND_DEFAULT, target);
    while(target_result.IsOk()) {

        matches.push_back(target_result);
        range.start_pos = target_result.end_pos;
        if(range.start_pos >= range.end_pos) {
            break;
        }
        target_result = DoFind(FIND_DEFAULT, range);
    }
    return matches;
}

void QuickFindBar::DoHighlightMatches(bool checked)
{
    clEditor* editor = dynamic_cast<clEditor*>(m_sci);
    if(!editor) {
        clMainFrame::Get()->SelectBestEnvSet();
        return;
    }

    if(checked && !m_textCtrlFind->GetValue().IsEmpty()) {
        auto matches = DoFindAll(GetBestTargetRange());
        if(matches.empty()) {
            return;
        }

        // clear selections and old markers
        m_sci->ClearSelections();
        editor->SetFindBookmarksActive(true);
        editor->DelAllMarkers(smt_find_bookmark);
        m_sci->SetIndicatorCurrent(MARKER_FIND_BAR_WORD_HIGHLIGHT);
        m_sci->IndicatorClearRange(0, m_sci->GetLength());

        for(const auto& match : matches) {
            m_sci->SetIndicatorCurrent(MARKER_FIND_BAR_WORD_HIGHLIGHT);
            m_sci->IndicatorFillRange(match.start_pos, match.end_pos - match.start_pos);
            m_sci->MarkerAdd(m_sci->LineFromPosition(match.start_pos), smt_find_bookmark);
        }

        wxString message;
        message << _("Found ") << matches.size() << wxPLURAL(" result", " results", matches.size());
        clGetManager()->GetStatusBar()->SetMessage(message);

    } else {
        editor->SetFindBookmarksActive(false);
        editor->DelAllMarkers(smt_find_bookmark);

        IEditor::List_t editors;
        clGetManager()->GetAllEditors(editors);
        std::for_each(editors.begin(), editors.end(), [&](IEditor* pEditor) {
            pEditor->GetCtrl()->MarkerDeleteAll(smt_find_bookmark);
            pEditor->GetCtrl()->SetIndicatorCurrent(MARKER_FIND_BAR_WORD_HIGHLIGHT);
            pEditor->GetCtrl()->IndicatorClearRange(0, pEditor->GetCtrl()->GetLength());
        });
        clGetManager()->GetStatusBar()->SetMessage(wxEmptyString);
    }
    clMainFrame::Get()->SelectBestEnvSet(); // Updates the statusbar display
}

void QuickFindBar::OnReceivingFocus(wxFocusEvent& event)
{
    event.Skip();
    if((event.GetEventObject() == m_textCtrlFind) || (event.GetEventObject() == m_textCtrlReplace)) {
        PostCommandEvent(this, wxStaticCast(event.GetEventObject(), wxWindow));
    }
}

void QuickFindBar::OnQuickFindCommandEvent(wxCommandEvent& event)
{
    if(event.GetInt() > 0) {
        // We need to delay further, or focus might be set too soon
        event.SetInt(event.GetInt() - 1);
        wxPostEvent(this, event);
    }

    if(event.GetEventObject() == m_textCtrlFind) {
        m_textCtrlFind->SetFocus();
        m_textCtrlFind->SelectAll();

    } else if(event.GetEventObject() == m_textCtrlReplace) {
        m_textCtrlReplace->SetFocus();
        m_textCtrlReplace->SelectAll();
    }
}

void QuickFindBar::OnReleaseEditor(clFindEvent& e)
{
    wxStyledTextCtrl* win = e.GetCtrl();
    if(win && win == m_sci) {
        m_sci = NULL;
        Show(false);
    }
}

wxStyledTextCtrl* QuickFindBar::DoCheckPlugins()
{
    // Let the plugins a chance to provide their own window
    clFindEvent evt(wxEVT_FINDBAR_ABOUT_TO_SHOW);
    EventNotifier::Get()->ProcessEvent(evt);
    return evt.GetCtrl();
}

bool QuickFindBar::ShowForPlugins()
{
    m_sci = DoCheckPlugins();
    if(!m_sci) {
        return DoShow(false, "");
    } else {
        return DoShow(true, "");
    }
}

wxString QuickFindBar::DoGetSelectedText()
{
    if(!m_sci) {
        return wxEmptyString;
    }

    if(m_sci->GetSelections() > 1) {
        for(int i = 0; i < m_sci->GetSelections(); ++i) {
            int selStart = m_sci->GetSelectionNStart(i);
            int selEnd = m_sci->GetSelectionNEnd(i);
            if(selEnd > selStart) {
                return m_sci->GetTextRange(selStart, selEnd);
            }
        }
        return wxEmptyString;

    } else {
        return m_sci->GetSelectedText();
    }
}

size_t QuickFindBar::DoGetSearchFlags() const { return m_searchFlags; }
void QuickFindBar::OnFindAll(wxCommandEvent& e) { DoSelectAll(); }

void QuickFindBar::DoFixRegexParen(wxString& findwhat)
{
    // Scintilla's REGEX group markers are \( and \)
    // while wxRegEx is usig bare ( and ) and the escaped version for
    // non regex manner
    findwhat.Replace("\\(", "/<!@#$");
    findwhat.Replace("\\)", "/>!@#$");
    findwhat.Replace("(", "<!@#$");
    findwhat.Replace(")", ">!@#$");

    findwhat.Replace("/<!@#$", "(");
    findwhat.Replace("/>!@#$", ")");
    findwhat.Replace("<!@#$", "\\(");
    findwhat.Replace(">!@#$", "\\)");
}

void QuickFindBar::DoSetCaretAtEndOfText() { m_textCtrlFind->SetInsertionPointEnd(); }

void QuickFindBar::OnReplaceAll(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DoReplaceAll(m_inSelection);
}

void QuickFindBar::DoReplaceAll(bool selectionOnly)
{
    CHECK_PTR_RET(m_sci);

    wxBusyCursor bc;
    wxStopWatch sw;

    TargetRange target;
    if(selectionOnly) {
        target = { m_sci->GetSelectionStart(), m_sci->GetSelectionEnd() };
    } else {
        target = { 0, static_cast<int>(m_sci->GetLastPosition()) };
    }

    // keep the current line, we will restore it after the replacment is done
    int starting_line = m_sci->GetCurrentLine();

    int replacements_done = 0;
    sw.Start();

    // Problem: wxStyledTextCtrl reports matches in **bytes**
    // wxString offsets are in chars. This means that if a double byte
    // char is found in the text (i.e. no Latin ASCII chars), the replacement
    // offset is wrong by 1 position.
    // To workaround this, we only apply replacement optimizations for non UTF8
    // strings

    bool isUTF8 = false;

    wxString input_buffer = m_sci->GetText();
    unsigned int utfLen = ::clUTF8Length(input_buffer.ToStdWstring().c_str(), input_buffer.length());
    isUTF8 = (utfLen != input_buffer.length());

    if(!(m_searchFlags & wxSTC_FIND_REGEXP) && !IsReplacementRegex() && !isUTF8) {
        // simple search, we can optimize it by applying the replacement on
        // a buffer instead of the editor
        replacements_done = DoReplaceInBuffer(target);

    } else {
        // perform a search
        m_sci->BeginUndoAction();
        while(true) {
            auto target_result = DoFind(FIND_DEFAULT, target);
            if(!target_result.IsOk()) {
                break;
            }
            int match_len = DoReplace(target_result);
            if(match_len == wxNOT_FOUND) {
                break;
            }
            replacements_done++;
            target.start_pos += match_len;
            if(target.start_pos >= target_result.end_pos) {
                break;
            }
        }
        m_sci->EndUndoAction();
    }
    double ms = sw.Time();
    clDEBUG() << "Replace all took:" << (double)(ms / 1000.0) << "seconds" << endl;
    if(replacements_done) {
        int pos = m_sci->PositionFromLine(starting_line);
        CenterLine(m_sci, pos, pos);
    }

    clGetManager()->SetStatusMessage(wxString::Format(_("Made %d replacements"), replacements_done));
    Show(false);
    m_sci->SetFocus();
}

TargetRange QuickFindBar::DoFind(size_t find_flags, const TargetRange& target)
{
    // define the target range
    size_t search_options = DoGetSearchFlags();
    int target_start = wxNOT_FOUND, target_end = wxNOT_FOUND;

    // if the caller provided a target, use it
    if(target.IsOk()) {
        target_start = target.start_pos;
        target_end = target.end_pos;
        if(find_flags & FIND_PREV) {
            // searching backward, switch the range positions
            wxSwap(target_start, target_end);
        }
    } else {
        if(m_inSelection) {
            // set the target to fit the selection
            target_start = m_sci->GetSelectionStart();
            target_end = m_sci->GetSelectionEnd();
            // if we are searching backward, switch the end and start positions
            if(find_flags & FIND_PREV) {
                wxSwap(target_start, target_end);
            }

        } else {
            // non selection search
            int sel_start = m_sci->GetSelectionStart();
            int sel_end = m_sci->GetSelectionEnd();
            if(find_flags & FIND_INCREMENT) {
                // incremental search only goes one way: down

                // if we have a text selected in the editor, use the selection start position
                // as our search point, otherwise, use the current position
                if(sel_start != wxNOT_FOUND && sel_end != wxNOT_FOUND) {
                    target_start = sel_start;
                } else {
                    target_start = m_sci->GetCurrentPos();
                }
                target_end = m_sci->GetLastPosition();
            } else {
                // Normal search

                // if we have a text selected in the editor, use the selection position as our search anchor
                // (depending on the search direction)
                if(sel_start != wxNOT_FOUND && sel_end != wxNOT_FOUND) {
                    target_start = find_flags & FIND_PREV ? sel_start : sel_end;
                } else {
                    // start the search from the current position
                    target_start = m_sci->GetCurrentPos();
                }
                // set the range to search:
                // by default, its the end of the document
                // unless searching backward and in this case
                // we set it to the start of the document (0)
                target_end = m_sci->GetLastPosition();
                if(find_flags & FIND_PREV) {
                    target_end = 0;
                }
            }
        }
    }
    // do we have a target?
    if(target_end == target_start) {
        TargetRange res = { wxNOT_FOUND, wxNOT_FOUND };
        res.why = TargetRange::EMPTY_RANGE;
        return res;
    }

    m_sci->SetTargetStart(target_start);
    m_sci->SetTargetEnd(target_end);

    size_t stc_search_options = 0;

    if(search_options & wxSTC_FIND_REGEXP)
        stc_search_options |= wxSTC_FIND_REGEXP | wxSTC_FIND_POSIX;
    if(search_options & wxSTC_FIND_MATCHCASE)
        stc_search_options |= wxSTC_FIND_MATCHCASE;
    if(search_options & wxSTC_FIND_WHOLEWORD)
        stc_search_options |= wxSTC_FIND_WHOLEWORD;

    m_sci->SetSearchFlags(stc_search_options);
    int where = m_sci->SearchInTarget(m_textCtrlFind->GetValue());
    if(where != wxNOT_FOUND) {
        // the target contains the matched string
        target_start = m_sci->GetTargetStart();
        target_end = m_sci->GetTargetEnd();
        if(find_flags & FIND_GOTOLINE) {
            CenterLine(m_sci, target_start, target_end);
        }
        return { target_start, target_end };
    } else {
        TargetRange res = { wxNOT_FOUND, wxNOT_FOUND };
        res.why = find_flags & FIND_PREV ? TargetRange::REACHED_SOF : TargetRange::REACHED_EOF;
        return res;
    }
}

void QuickFindBar::OnFind(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoFindWithWrap(FIND_DEFAULT | FIND_GOTOLINE);
}

void QuickFindBar::OnFindPrev(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoFindWithWrap(FIND_PREV | FIND_GOTOLINE);
}

void QuickFindBar::OnFindAllUI(wxUpdateUIEvent& event) { event.Enable(!m_textCtrlFind->IsEmpty()); }
void QuickFindBar::OnFindPrevUI(wxUpdateUIEvent& event) { event.Enable(!m_textCtrlFind->IsEmpty()); }
void QuickFindBar::OnFindUI(wxUpdateUIEvent& event) { event.Enable(!m_textCtrlFind->IsEmpty()); }
void QuickFindBar::OnReplaceAllUI(wxUpdateUIEvent& event) { event.Enable(!m_textCtrlFind->IsEmpty()); }
void QuickFindBar::OnReplaceUI(wxUpdateUIEvent& event) { event.Enable(!m_textCtrlFind->IsEmpty() && !m_inSelection); }

void QuickFindBar::DoArrowDown(clTerminalHistory& history, wxTextCtrl* ctrl)
{
    const wxString& str = history.ArrowDown();
    if(!str.IsEmpty()) {
        ctrl->SelectAll();
        long from, to;
        ctrl->GetSelection(&from, &to);
        ctrl->Replace(from, to, str);
        ctrl->SelectNone();
        ctrl->SetInsertionPoint(ctrl->GetLastPosition());
    }
}

void QuickFindBar::DoArrowUp(clTerminalHistory& history, wxTextCtrl* ctrl)
{
    const wxString& str = history.ArrowUp();
    if(!str.IsEmpty()) {
        ctrl->SelectAll();
        long from, to;
        ctrl->GetSelection(&from, &to);
        ctrl->Replace(from, to, str);
        ctrl->SelectNone();
        ctrl->SetInsertionPoint(ctrl->GetLastPosition());
    }
}

void QuickFindBar::OnButtonKeyDown(wxKeyEvent& event)
{
    switch(event.GetKeyCode()) {
    case WXK_ESCAPE: {
        wxCommandEvent dummy;
        OnHide(dummy);
        DoHighlightMatches(false);
        break;
    }
    default: {
        event.Skip();
        break;
    }
    }
}

void QuickFindBar::OnPaint(wxPaintEvent& e)
{
    wxAutoBufferedPaintDC dc(this);
    dc.SetBrush(clSystemSettings::GetDefaultPanelColour());
    dc.SetPen(clSystemSettings::GetDefaultPanelColour());
    dc.DrawRectangle(GetClientRect());
}

void QuickFindBar::ShowToolBarOnly()
{
    wxPanel::Show();
    wxSizer* sz = m_textCtrlFind->GetContainingSizer();
    if(sz && sz != GetSizer()) {
        GetSizer()->Hide(sz);
    }
    GetParent()->GetSizer()->Layout();
}

void QuickFindBar::FindPrevious() { DoFindWithWrap(FIND_PREV | FIND_GOTOLINE); }

void QuickFindBar::FindNext() { DoFindWithWrap(FIND_DEFAULT | FIND_GOTOLINE); }

size_t QuickFindBar::DoReplaceInBuffer(const TargetRange& range)
{
    if(!range.IsOk()) {
        return 0;
    }

    wxString in_buffer = m_sci->GetTextRange(range.start_pos, range.end_pos);
    auto matches = DoFindAll(range);

    // adjust the replacement position to match the range
    int offset = -range.start_pos;
    wxString replace_with = m_textCtrlReplace->GetValue();
    size_t replace_with_len = replace_with.length();
    for(const auto& range : matches) {
        size_t match_len = range.end_pos - range.start_pos;
        in_buffer.replace(range.start_pos + offset, match_len, replace_with);
        offset += (replace_with_len - match_len);
    }
    m_sci->Replace(range.start_pos, range.end_pos, in_buffer);
    return matches.size();
}

bool QuickFindBar::IsReplacementRegex() const
{
    wxString replace_with = m_textCtrlReplace->GetValue();
    for(size_t i = 0; i < 10; ++i) {
        wxString re_group;
        re_group << "\\" << i;
        if(replace_with.Contains(re_group)) {
            return true;
        }
    }
    return false;
}

TargetRange QuickFindBar::DoFindWithMessage(size_t find_flags, const TargetRange& target)
{
    clGetManager()->GetStatusBar()->SetMessage(wxEmptyString);
    auto res = DoFind(find_flags, target);
    if(!res.IsOk()) {
        if(find_flags & FIND_PREV) {
            clGetManager()->GetStatusBar()->SetMessage(_("Reached the start of the document"));
        } else {
            clGetManager()->GetStatusBar()->SetMessage(_("Reached the end of the document"));
        }
    }
    return res;
}

TargetRange QuickFindBar::DoFindWithWrap(size_t find_flags, const TargetRange& target)
{
    auto res = DoFindWithMessage(find_flags, target);
    if(!res.IsOk()) {
        // reached end or start of the document
        // start from the beginning (the range will switch if FIND_PREV is set)
        clGetManager()->SetStatusMessage(_("Wrapped past end of file"), 2);
        res = DoFindWithMessage(find_flags, { 0, static_cast<int>(m_sci->GetLastPosition()) });
    }
    return res;
}

TargetRange QuickFindBar::GetBestTargetRange() const
{
    if(!m_sci) {
        return {};
    }

    if(m_inSelection) {
        return { m_sci->GetSelectionStart(), m_sci->GetSelectionEnd() };
    } else {
        return { 0, static_cast<int>(m_sci->GetLastPosition()) };
    }
}
void QuickFindBar::OnReplaceTextEnter(wxCommandEvent& event) {}
void QuickFindBar::OnReplaceTextUpdated(wxCommandEvent& event) {}
