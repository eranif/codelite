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

#include "ColoursAndFontsManager.h"
#include "Keyboard/clKeyboardManager.h"
#include "bitmap_loader.h"
#include "bookmark_manager.h"
#include "clSystemSettings.h"
#include "clThemedTextCtrl.hpp"
#include "cl_command_event.h"
#include "cl_config.h"
#include "cl_editor.h"
#include "codelite_events.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "frame.h"
#include "globals.h"
#include "imanager.h"
#include "manager.h"
#include "plugin.h"

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

#define CHECK_FOCUS_WIN(evt)                             \
    {                                                    \
        wxWindow* focus = wxWindow::FindFocus();         \
        if (focus != m_sci && focus != m_textCtrlFind) { \
            evt.Skip();                                  \
            return;                                      \
        }                                                \
                                                         \
        if (!m_sci || m_sci->GetLength() == 0) {         \
            evt.Skip();                                  \
            return;                                      \
        }                                                \
    }

namespace
{
void TextCtrlShowMenu(wxTextCtrl* button, wxMenu& menu, wxPoint* point = nullptr)
{
    wxPoint menuPos;
    if (point) {
        menuPos = *point;
    } else {
        menuPos = button->GetClientRect().GetBottomLeft();
#ifdef __WXOSX__
        menuPos.y += 5;
#endif
    }
    button->PopupMenu(&menu, menuPos);
}

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
    : clFindReplaceDialogBase(parent, id)
    , m_sci(NULL)
    , m_lastTextPtr(NULL)
    , m_eventsConnected(false)
    , m_regexType(kRegexNone)
    , m_disableTextUpdateEvent(false)
    , m_searchFlags(0)
    , m_highlightMatches(false)
    , m_inSelection(false)
{
    Hide();

    m_toolbar->SetMiniToolBar(true);

    auto bitmaps = clGetManager()->GetStdIcons();
    auto images = m_toolbar->GetBitmapsCreateIfNeeded();

#define TB_ADD_BITMAP(name) images->Add(bitmaps->GetBundle(name).GetBitmap(wxSize(24, 24)), name)

    m_toolbar->AddTool(
        XRCID("case-sensitive"), _("Case Sensitive"), TB_ADD_BITMAP("case-sensitive"), wxEmptyString, wxITEM_CHECK);
    m_toolbar->AddTool(XRCID("whole-word"), _("Whole word"), TB_ADD_BITMAP("whole-word"), wxEmptyString, wxITEM_CHECK);
    m_toolbar->AddTool(
        XRCID("use-regex"), _("Regex"), TB_ADD_BITMAP("regular-expression"), wxEmptyString, wxITEM_CHECK);
    m_toolbar->AddTool(
        XRCID("highlight-matches"), _("Highlight matches"), TB_ADD_BITMAP("marker"), wxEmptyString, wxITEM_CHECK);
    m_toolbar->AddTool(
        XRCID("replace-in-selection"), _("In Selection"), TB_ADD_BITMAP("text_selection"), wxEmptyString, wxITEM_CHECK);
    m_toolbar->AddStretchableSpace();
    m_message = new wxStaticText(m_toolbar, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1));
    m_toolbar->AddControl(m_message);
#undef TB_ADD_BITMAP

    m_toolbar->Realize();
    m_toolbar->Bind(
        wxEVT_TOOL,
        [&](wxCommandEvent& e) {
            if (e.IsChecked()) {
                m_searchFlags |= wxSTC_FIND_MATCHCASE;
            } else {
                m_searchFlags &= ~wxSTC_FIND_MATCHCASE;
            }
        },
        XRCID("case-sensitive"));
    m_toolbar->Bind(
        wxEVT_TOOL,
        [&](wxCommandEvent& e) {
            if (e.IsChecked()) {
                m_searchFlags |= wxSTC_FIND_WHOLEWORD;
            } else {
                m_searchFlags &= ~wxSTC_FIND_WHOLEWORD;
            }
        },
        XRCID("whole-word"));
    m_toolbar->Bind(
        wxEVT_TOOL,
        [&](wxCommandEvent& e) {
            if (e.IsChecked()) {
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
        wxEVT_UPDATE_UI,
        [&](wxUpdateUIEvent& e) { e.Check(m_searchFlags & wxSTC_FIND_MATCHCASE); },
        XRCID("case-sensitive"));
    m_toolbar->Bind(
        wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& e) { e.Check(m_searchFlags & wxSTC_FIND_REGEXP); }, XRCID("use-regex"));
    m_toolbar->Bind(
        wxEVT_UPDATE_UI,
        [&](wxUpdateUIEvent& e) { e.Check(m_searchFlags & wxSTC_FIND_WHOLEWORD); },
        XRCID("whole-word"));
    m_toolbar->Bind(
        wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& e) { e.Check(m_highlightMatches); }, XRCID("highlight-matches"));
    m_toolbar->Bind(
        wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& e) { e.Check(m_inSelection); }, XRCID("replace-in-selection"));

    wxTheApp->Bind(wxEVT_MENU, &QuickFindBar::OnFindNextCaret, this, XRCID("find_next_at_caret"));
    wxTheApp->Bind(wxEVT_MENU, &QuickFindBar::OnFindPreviousCaret, this, XRCID("find_previous_at_caret"));
    wxTheApp->Bind(wxEVT_MENU, &QuickFindBar::OnFindNext, this, XRCID("find_next"));

    EventNotifier::Get()->Bind(wxEVT_FINDBAR_RELEASE_EDITOR, &QuickFindBar::OnReleaseEditor, this);
    Connect(QUICKFIND_COMMAND_EVENT, wxCommandEventHandler(QuickFindBar::OnQuickFindCommandEvent), NULL, this);

    // Initialize the list with the history
    m_findHistory.Set(clConfig::Get().GetQuickFindSearchItems());
    m_replaceHistory.Set(clConfig::Get().GetQuickFindReplaceItems());

    // Update the search flags
    m_searchFlags = clConfig::Get().Read("FindBar/SearchFlags", 0);
    m_highlightMatches = clConfig::Get().Read("FindBar/HighlightOccurences", false);

    SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());

#ifdef __WXMSW__
    auto set_font_cb = [this]() {
        auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
        if (lexer) {
            auto font = lexer->GetFontForStyle(0, this);
            font.SetFractionalPointSize(font.GetFractionalPointSize() * 0.8);
            m_textCtrlFind->SetFont(font);
            m_textCtrlReplace->SetFont(font);
        }
    };
    set_font_cb();

    EventNotifier::Get()->Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, [this, set_font_cb](clCommandEvent& e) {
        e.Skip();
        set_font_cb();
    });
#endif

    EventNotifier::Get()->Bind(wxEVT_STC_GOT_FOCUS, &QuickFindBar::OnFocusGained, this);
    EventNotifier::Get()->Bind(wxEVT_STC_LOST_FOCUS, &QuickFindBar::OnFocusLost, this);

    m_textCtrlReplace->Bind(wxEVT_UPDATE_UI, &QuickFindBar::OnReplaceTextUI, this);
    // Make sure that the 'Replace' field is selected when we hit TAB while in the 'Find' field
    m_textCtrlReplace->MoveAfterInTabOrder(m_textCtrlFind);

    int dlg_width = wxNOT_FOUND;
    int dlg_height = wxNOT_FOUND;

    int display_width, display_height;
    wxDisplaySize(&display_width, &display_height);
    wxSize min_size(display_width / 4, GetSizer()->CalcMin().GetHeight());

    if (clConfig::Get().Read("FindBar/Width", dlg_width) && clConfig::Get().Read("FindBar/Height", dlg_height)) {
        SetSize(dlg_width, dlg_height);
        SetSizeHints(dlg_width, dlg_height);
    } else {
        // first time, place it at the top
        Move(wxNOT_FOUND, parent->GetPosition().y);
    }
    SetMinClientSize(min_size);
    GetSizer()->Fit(this);
    Layout();
}

QuickFindBar::~QuickFindBar()
{
    // Remember the buttons clicked
    clConfig::Get().Write("FindBar/SearchFlags", (int)DoGetSearchFlags());
    clConfig::Get().Write("FindBar/HighlightOccurences", m_highlightMatches);
    clConfig::Get().Write("FindBar/Width", GetSize().GetWidth());
    clConfig::Get().Write("FindBar/Height", GetSize().GetHeight());

    clConfig::Get().SetQuickFindSearchItems(m_findHistory.m_commands);
    clConfig::Get().SetQuickFindReplaceItems(m_replaceHistory.m_commands);

    wxTheApp->Unbind(wxEVT_MENU, &QuickFindBar::OnFindNextCaret, this, XRCID("find_next_at_caret"));
    wxTheApp->Unbind(wxEVT_MENU, &QuickFindBar::OnFindPreviousCaret, this, XRCID("find_previous_at_caret"));
    wxTheApp->Unbind(wxEVT_MENU, &QuickFindBar::OnFindNext, this, XRCID("id_find"));

    EventNotifier::Get()->Unbind(wxEVT_FINDBAR_RELEASE_EDITOR, &QuickFindBar::OnReleaseEditor, this);
    EventNotifier::Get()->Unbind(wxEVT_STC_GOT_FOCUS, &QuickFindBar::OnFocusGained, this);
    EventNotifier::Get()->Unbind(wxEVT_STC_LOST_FOCUS, &QuickFindBar::OnFocusLost, this);
}

bool QuickFindBar::Show(bool show)
{
    if (!m_sci && show) {
        return false;
    }
    return DoShow(show, wxEmptyString);
}

void QuickFindBar::OnHide(wxCommandEvent& e)
{
    e.Skip();
    // Clear all
    Show(false);
}

void QuickFindBar::OnText(wxCommandEvent& e)
{
    e.Skip();
    if (!m_inSelection && !m_disableTextUpdateEvent) {
        // searching down
        DoFindWithWrap(FIND_INCREMENT | FIND_GOTOLINE);
    }
}

void QuickFindBar::OnKeyDown(wxKeyEvent& e)
{
    if (HandleKeyboardShortcuts(e)) {
        return;
    }

    switch (e.GetKeyCode()) {
    case WXK_ESCAPE: {
        wxCommandEvent dummy;
        OnHide(dummy);
        DoHighlightMatches(false);
        break;
    }
    case WXK_UP:
    case WXK_DOWN:
        ShowMenuForFindCtrl();
        break;
    default:
        e.Skip();
        break;
    }
}

void QuickFindBar::OnReplaceKeyDown(wxKeyEvent& e)
{
    if (HandleKeyboardShortcuts(e)) {
        return;
    }

    switch (e.GetKeyCode()) {
    case WXK_UP:
    case WXK_DOWN:
        ShowMenuForReplaceCtrl();
        break;
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
    e.Skip(false);

    wxString find_what = m_textCtrlFind->GetValue();
    m_findHistory.Add(find_what);

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
    if (m_sci->HasSelection()) {
        // we are starting with a selected text
        // check this range to see if it matches the 'find-what'
        // string
        TargetRange result = DoFind(FIND_DEFAULT, { m_sci->GetSelectionStart(), m_sci->GetSelectionEnd() });
        if (result.IsOk()) {
            // the selected text is a match, replace it
            wxString replace_with = m_textCtrlReplace->GetValue();
            int len = DoReplace(result);
            if (len != wxNOT_FOUND) {
                CenterLine(m_sci, result.start_pos, result.start_pos + len);
                m_sci->SetFocus();
                start_pos = result.start_pos + len;
                m_replaceHistory.Add(replace_with);
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
    if (range.IsOk()) {
        target_range = range;
    } else if (!m_sci->HasSelection()) {
        target_range = { m_sci->GetSelectionStart(), m_sci->GetSelectionEnd() };
    }

    if (!target_range.IsOk()) {
        return wxNOT_FOUND;
    }

    m_sci->SetTargetStart(target_range.start_pos);
    m_sci->SetTargetEnd(target_range.end_pos);

    if (m_searchFlags & wxSTC_FIND_REGEXP) {
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
    if (!m_sci) {
        DoShow(false, "");
        return;
    }
}

bool QuickFindBar::Show(const wxString& findWhat, bool showReplace)
{
    // Same as Show() but set the 'findWhat' field with findWhat
    // and show/hide the 'Replace' section depending on the bool
    if (!m_sci)
        return false;
    return DoShow(true, findWhat, showReplace);
}

bool QuickFindBar::DoShow(bool s, const wxString& findWhat, bool showReplace)
{
    wxWindowUpdateLocker locker{ this };
    int dummy = wxNOT_FOUND;
    if (!clConfig::Get().Read("FindBar/Height", dummy)) {
        // first time, place it at the top
        Move(wxNOT_FOUND, GetParent()->GetPosition().y);
    }

    bool res = wxDialog::Show(s);
    if (s && m_sci) {
        // Delete the indicators
        m_sci->SetIndicatorCurrent(1);
        m_sci->IndicatorClearRange(0, m_sci->GetLength());

        if (EditorConfigST::Get()->GetOptions()->GetClearHighlightedWordsOnFind()) {
            m_sci->SetIndicatorCurrent(INDICATOR_FIND_BAR_WORD_HIGHLIGHT);
            m_sci->IndicatorClearRange(0, m_sci->GetLength());
        }
    }

    m_inSelection = !findWhat.IsEmpty() && findWhat.Contains("\n");
    if (!m_sci) {
        // nothing to do

    } else if (!s) {
        // hiding
        DoHighlightMatches(false);
        m_sci->SetFocus();

    } else if (!findWhat.IsEmpty()) {

        if (findWhat.Contains("\n")) {
            // Multiline selection
            // enable the 'Replace in Selection'
            m_textCtrlFind->ChangeValue("");
            m_textCtrlFind->SetFocus();
        } else {
            m_textCtrlFind->ChangeValue(findWhat);
            m_textCtrlFind->SelectAll();
            m_textCtrlFind->SetFocus();
            if (m_highlightMatches) {
                if (!(m_searchFlags & wxSTC_FIND_REGEXP) || m_textCtrlFind->GetValue().Length() > 2) {
                    DoHighlightMatches(true);
                }
            }
            PostCommandEvent(this, m_textCtrlFind);
        }

    } else {
        wxString findWhat = DoGetSelectedText().BeforeFirst(wxT('\n'));
        if (!findWhat.IsEmpty()) {
            m_textCtrlFind->ChangeValue(findWhat);
        }

        m_textCtrlFind->SelectAll();
        m_textCtrlFind->SetFocus();
        if (m_highlightMatches) {
            if (!(m_searchFlags & wxSTC_FIND_REGEXP) || m_textCtrlFind->GetValue().Length() > 2) {
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
    if (selection.IsEmpty()) {
        // select the word
        long pos = m_sci->GetCurrentPos();
        long start = m_sci->WordStartPosition(pos, true);
        long end = m_sci->WordEndPosition(pos, true);

        selection = m_sci->GetTextRange(start, end);
        if (selection.IsEmpty() == false)
            m_sci->SetCurrentPos(start);
    }

    if (selection.IsEmpty())
        return;

    m_textCtrlFind->ChangeValue(selection);
    DoFind(FIND_DEFAULT | FIND_GOTOLINE);
}

void QuickFindBar::OnFindNext(wxCommandEvent& e)
{
    clSYSTEM() << "Find next" << endl;
    CHECK_FOCUS_WIN(e);
    DoFindWithWrap(FIND_DEFAULT | FIND_GOTOLINE);
}

void QuickFindBar::OnFindPreviousCaret(wxCommandEvent& e)
{
    CHECK_FOCUS_WIN(e);

    wxString selection(DoGetSelectedText());
    if (selection.IsEmpty()) {
        // select the word
        long pos = m_sci->GetCurrentPos();
        long start = m_sci->WordStartPosition(pos, true);
        long end = m_sci->WordEndPosition(pos, true);

        selection = m_sci->GetTextRange(start, end);
        if (selection.IsEmpty() == false)
            m_sci->SetCurrentPos(start);
    }

    if (selection.IsEmpty())
        return;

    m_textCtrlFind->ChangeValue(selection);
    DoFind(FIND_PREV | FIND_GOTOLINE);
}

void QuickFindBar::DoSelectAll()
{
    m_message->SetLabel(wxEmptyString);
    if (!m_sci) {
        return;
    }

    wxBusyCursor bc;
    TargetRange target = GetBestTargetRange();

    auto matches = DoFindAll(target);
    if (matches.empty()) {
        m_message->SetLabel(_("No matches found"));
        return;
    }

    // add selections
    m_sci->ClearSelections();
    for (size_t i = 0; i < matches.size(); ++i) {
        if (i == 0) {
            m_sci->SetSelection(matches[i].start_pos, matches[i].end_pos);
            m_sci->SetMainSelection(0);
            CenterLine(m_sci, matches[i].start_pos, matches[i].end_pos);
        } else {
            m_sci->AddSelection(matches[i].start_pos, matches[i].end_pos);
        }
    }
    Show(false);
    m_message->SetLabel(wxString::Format(_("Selected %lu matches"), matches.size()));
    m_sci->SetMainSelection(0);
}

TargetRange::Vec_t QuickFindBar::DoFindAll(const TargetRange& target)
{
    if (!m_sci) {
        return {};
    }

    TargetRange range{ 0, static_cast<int>(m_sci->GetLastPosition()) };
    if (target.IsOk()) {
        range = target;
    }
    // perform a search

    TargetRange::Vec_t matches;
    matches.reserve(1000); // enough room for 1000 matches

    auto target_result = DoFind(FIND_DEFAULT, target);
    while (target_result.IsOk()) {

        matches.push_back(target_result);
        range.start_pos = target_result.end_pos;
        if (range.start_pos >= range.end_pos) {
            break;
        }
        target_result = DoFind(FIND_DEFAULT, range);
    }
    return matches;
}

void QuickFindBar::DoHighlightMatches(bool checked)
{
    clEditor* editor = dynamic_cast<clEditor*>(m_sci);
    if (!editor) {
        clMainFrame::Get()->SelectBestEnvSet();
        return;
    }

    if (checked && !m_textCtrlFind->GetValue().IsEmpty()) {
        auto matches = DoFindAll(GetBestTargetRange());
        if (matches.empty()) {
            return;
        }

        // clear selections and old markers
        m_sci->ClearSelections();
        editor->SetFindBookmarksActive(true);
        editor->DelAllMarkers(smt_find_bookmark);
        m_sci->SetIndicatorCurrent(INDICATOR_FIND_BAR_WORD_HIGHLIGHT);
        m_sci->IndicatorClearRange(0, m_sci->GetLength());

        for (const auto& match : matches) {
            m_sci->SetIndicatorCurrent(INDICATOR_FIND_BAR_WORD_HIGHLIGHT);
            m_sci->IndicatorFillRange(match.start_pos, match.end_pos - match.start_pos);
            m_sci->MarkerAdd(m_sci->LineFromPosition(match.start_pos), smt_find_bookmark);
        }

        wxString message;
        message << _("Found ") << matches.size() << wxPLURAL(" result", " results", matches.size());
        m_message->SetLabel(message);

    } else {
        editor->SetFindBookmarksActive(false);
        editor->DelAllMarkers(smt_find_bookmark);

        IEditor::List_t editors;
        clGetManager()->GetAllEditors(editors);
        for (IEditor* pEditor : editors) {
            pEditor->GetCtrl()->MarkerDeleteAll(smt_find_bookmark);
            pEditor->GetCtrl()->SetIndicatorCurrent(INDICATOR_FIND_BAR_WORD_HIGHLIGHT);
            pEditor->GetCtrl()->IndicatorClearRange(0, pEditor->GetCtrl()->GetLength());
        }
        m_message->SetLabel(wxEmptyString);
    }
    clMainFrame::Get()->SelectBestEnvSet(); // Updates the statusbar display
}

void QuickFindBar::OnReceivingFocus(wxFocusEvent& event)
{
    event.Skip();
    if ((event.GetEventObject() == m_textCtrlFind) || (event.GetEventObject() == m_textCtrlReplace)) {
        PostCommandEvent(this, wxStaticCast(event.GetEventObject(), wxWindow));
    }
}

void QuickFindBar::OnQuickFindCommandEvent(wxCommandEvent& event)
{
    if (event.GetInt() > 0) {
        // We need to delay further, or focus might be set too soon
        event.SetInt(event.GetInt() - 1);
        wxPostEvent(this, event);
    }

    if (event.GetEventObject() == m_textCtrlFind) {
        m_textCtrlFind->SetFocus();
        m_textCtrlFind->SelectAll();

    } else if (event.GetEventObject() == m_textCtrlReplace) {
        m_textCtrlReplace->SetFocus();
        m_textCtrlReplace->SelectAll();
    }
}

void QuickFindBar::OnReleaseEditor(clFindEvent& e)
{
    wxStyledTextCtrl* win = e.GetCtrl();
    if (win && win == m_sci) {
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
    if (!m_sci) {
        return DoShow(false, "");
    } else {
        return DoShow(true, "");
    }
}

wxString QuickFindBar::DoGetSelectedText()
{
    if (!m_sci) {
        return wxEmptyString;
    }

    if (m_sci->GetSelections() > 1) {
        for (int i = 0; i < m_sci->GetSelections(); ++i) {
            int selStart = m_sci->GetSelectionNStart(i);
            int selEnd = m_sci->GetSelectionNEnd(i);
            if (selEnd > selStart) {
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
    if (selectionOnly) {
        target = { m_sci->GetSelectionStart(), m_sci->GetSelectionEnd() };
    } else {
        target = { 0, static_cast<int>(m_sci->GetLastPosition()) };
    }

    // keep the current line, we will restore it after the replacement is done
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
    unsigned int utfLen = StringUtils::UTF8Length(input_buffer.ToStdWstring().c_str(), input_buffer.length());
    isUTF8 = (utfLen != input_buffer.length());

    // perform a search
    m_sci->BeginUndoAction();

    while (true) {
        int before_replace_doc_len = m_sci->GetLength();
        auto find_result = DoFind(FIND_DEFAULT, target);
        if (!find_result.IsOk()) {
            break;
        }
        int replacement_len = DoReplace(find_result);
        if (replacement_len == wxNOT_FOUND) {
            break;
        }

        replacements_done++;

        // Adjust the target range
        int after_replace_doc_len = m_sci->GetLength();
        if (after_replace_doc_len > before_replace_doc_len) {
            // the "replace with" is longer than the "find what"
            // e.g. replacing "A" -> "AB"
            target.end_pos += (after_replace_doc_len - before_replace_doc_len);
        } else if (after_replace_doc_len < before_replace_doc_len) {
            // the "find what" is larger than the "replace with"
            // e.g. replacing "AB" -> "A"
            target.end_pos -= (before_replace_doc_len - after_replace_doc_len);
        }

        // We resume the search from the first position after the replacement
        target.start_pos = find_result.start_pos + replacement_len;

        if (target.start_pos >= target.end_pos) {
            break;
        }
    }
    m_sci->EndUndoAction();

    double ms = sw.Time();
    clDEBUG() << "Replace all took:" << (double)(ms / 1000.0) << "seconds" << endl;
    if (replacements_done) {
        int pos = m_sci->PositionFromLine(starting_line);
        CenterLine(m_sci, pos, pos);
    }

    m_message->SetLabel(wxString::Format(_("Made %d replacements"), replacements_done));
    Show(false);
    m_sci->SetFocus();
}

TargetRange QuickFindBar::DoFind(size_t find_flags, const TargetRange& target)
{
    if (!m_sci) {
        return {};
    }

    // define the target range
    size_t search_options = DoGetSearchFlags();
    int target_start = wxNOT_FOUND, target_end = wxNOT_FOUND;

    // if the caller provided a target, use it
    if (target.IsOk()) {
        target_start = target.start_pos;
        target_end = target.end_pos;
        if (find_flags & FIND_PREV) {
            // searching backward, switch the range positions
            wxSwap(target_start, target_end);
        }
    } else {
        if (m_inSelection) {
            // set the target to fit the selection
            target_start = m_sci->GetSelectionStart();
            target_end = m_sci->GetSelectionEnd();
            // if we are searching backward, switch the end and start positions
            if (find_flags & FIND_PREV) {
                wxSwap(target_start, target_end);
            }

        } else {
            // non selection search
            int sel_start = m_sci->GetSelectionStart();
            int sel_end = m_sci->GetSelectionEnd();
            if (find_flags & FIND_INCREMENT) {
                // incremental search only goes one way: down

                // if we have a text selected in the editor, use the selection start position
                // as our search point, otherwise, use the current position
                if (sel_start != wxNOT_FOUND && sel_end != wxNOT_FOUND) {
                    target_start = sel_start;
                } else {
                    target_start = m_sci->GetCurrentPos();
                }
                target_end = m_sci->GetLastPosition();
            } else {
                // Normal search

                // if we have a text selected in the editor, use the selection position as our search anchor
                // (depending on the search direction)
                if (sel_start != wxNOT_FOUND && sel_end != wxNOT_FOUND) {
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
                if (find_flags & FIND_PREV) {
                    target_end = 0;
                }
            }
        }
    }
    // do we have a target?
    if (target_end == target_start) {
        TargetRange res = { wxNOT_FOUND, wxNOT_FOUND };
        res.why = TargetRange::EMPTY_RANGE;
        return res;
    }

    m_sci->SetTargetStart(target_start);
    m_sci->SetTargetEnd(target_end);

    size_t stc_search_options = 0;

    if (search_options & wxSTC_FIND_REGEXP)
        stc_search_options |= wxSTC_FIND_REGEXP | wxSTC_FIND_POSIX;
    if (search_options & wxSTC_FIND_MATCHCASE)
        stc_search_options |= wxSTC_FIND_MATCHCASE;
    if (search_options & wxSTC_FIND_WHOLEWORD)
        stc_search_options |= wxSTC_FIND_WHOLEWORD;

    m_sci->SetSearchFlags(stc_search_options);
    int where = m_sci->SearchInTarget(m_textCtrlFind->GetValue());
    if (where != wxNOT_FOUND) {
        // the target contains the matched string
        target_start = m_sci->GetTargetStart();
        target_end = m_sci->GetTargetEnd();
        if (find_flags & FIND_GOTOLINE) {
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

void QuickFindBar::OnFindAllUI(wxUpdateUIEvent& event) { event.Enable(!m_textCtrlFind->GetValue().IsEmpty()); }
void QuickFindBar::OnFindPrevUI(wxUpdateUIEvent& event) { event.Enable(!m_textCtrlFind->GetValue().IsEmpty()); }
void QuickFindBar::OnFindUI(wxUpdateUIEvent& event) { event.Enable(!m_textCtrlFind->GetValue().IsEmpty()); }
void QuickFindBar::OnReplaceAllUI(wxUpdateUIEvent& event)
{
    event.Enable(m_sci && m_sci->IsEditable() && !m_textCtrlFind->GetValue().IsEmpty());
}
void QuickFindBar::OnReplaceUI(wxUpdateUIEvent& event)
{
    event.Enable(m_sci && m_sci->IsEditable() && !m_textCtrlFind->GetValue().IsEmpty() && !m_inSelection);
}

void QuickFindBar::OnButtonKeyDown(wxKeyEvent& event)
{
    if (HandleKeyboardShortcuts(event)) {
        return;
    }
    switch (event.GetKeyCode()) {
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
    wxDialog::Show();
    wxSizer* sz = m_textCtrlFind->GetContainingSizer();
    if (sz && sz != GetSizer()) {
        GetSizer()->Hide(sz);
    }
    GetParent()->GetSizer()->Layout();
}

void QuickFindBar::FindPrevious() { DoFindWithWrap(FIND_PREV | FIND_GOTOLINE); }

void QuickFindBar::FindNext() { DoFindWithWrap(FIND_DEFAULT | FIND_GOTOLINE); }

size_t QuickFindBar::DoReplaceInBuffer(const TargetRange& range)
{
    if (!range.IsOk()) {
        return 0;
    }

    wxString in_buffer = m_sci->GetTextRange(range.start_pos, range.end_pos);
    auto matches = DoFindAll(range);

    // adjust the replacement position to match the range
    int offset = -range.start_pos;
    wxString replace_with = m_textCtrlReplace->GetValue();
    size_t replace_with_len = replace_with.length();
    for (const auto& range : matches) {
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
    for (size_t i = 0; i < 10; ++i) {
        wxString re_group;
        re_group << "\\" << i;
        if (replace_with.Contains(re_group)) {
            return true;
        }
    }
    return false;
}

TargetRange QuickFindBar::DoFindWithMessage(size_t find_flags, const TargetRange& target)
{
    if (!m_sci) {
        return {};
    }

    m_message->SetLabel(wxEmptyString);
    auto res = DoFind(find_flags, target);
    if (!res.IsOk()) {
        if (find_flags & FIND_PREV) {
            m_message->SetLabel(_("Reached the start of the document"));
        } else {
            m_message->SetLabel(_("Reached the end of the document"));
        }
    }
    return res;
}

TargetRange QuickFindBar::DoFindWithWrap(size_t find_flags, const TargetRange& target)
{
    auto res = DoFindWithMessage(find_flags, target);
    if (!res.IsOk()) {
        // reached end or start of the document
        // start from the beginning (the range will switch if FIND_PREV is set)
        res = DoFindWithMessage(find_flags, { 0, static_cast<int>(m_sci->GetLastPosition()) });
    }
    return res;
}

TargetRange QuickFindBar::GetBestTargetRange() const
{
    if (!m_sci) {
        return {};
    }

    if (m_inSelection) {
        return { m_sci->GetSelectionStart(), m_sci->GetSelectionEnd() };
    } else {
        return { 0, static_cast<int>(m_sci->GetLastPosition()) };
    }
}
void QuickFindBar::OnReplaceTextEnter(wxCommandEvent& event) {}
void QuickFindBar::OnReplaceTextUpdated(wxCommandEvent& event) {}

void QuickFindBar::OnFocusGained(clCommandEvent& e)
{
    // an wxSTC got the focus
    e.Skip();

    wxStyledTextCtrl* stc = dynamic_cast<wxStyledTextCtrl*>(wxWindow::FindFocus());
    CHECK_PTR_RET(stc);

    // Replace the editor
    SetEditor(stc);
}

void QuickFindBar::OnFocusLost(clCommandEvent& e)
{
    // an wxSTC lost the focus, currently we do nothing
    e.Skip();
}

void QuickFindBar::OnTimer(wxTimerEvent& event) { event.Skip(); }

void QuickFindBar::OnReplaceTextUI(wxUpdateUIEvent& event) { event.Enable(!m_textCtrlFind->GetValue().IsEmpty()); }

bool QuickFindBar::HandleKeyboardShortcuts(wxKeyEvent& event)
{
#ifdef __WXMSW__
    if (event.GetKeyCode() == WXK_BACK && event.GetModifiers() == wxMOD_CONTROL) {
        auto text_ctrl = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
        if (text_ctrl) {
            text_ctrl->Clear();
        }
        return true;
    }
#endif

    auto accl_find = clKeyboardManager::Get()->GetShortcutForCommand("find_next");
    auto accl_find_prev = clKeyboardManager::Get()->GetShortcutForCommand("find_previous");

    auto find_next = accl_find.ToAccelerator("Find Next");
    auto find_prev = accl_find_prev.ToAccelerator("Find Previous");

    // Note that we compare here: GetFlags() (wxACCL_*) against GetModifiers() (wxMOD_*)
    // the primary modifiers: Ctrl, Shift, Alt are the same so we don't care...
    if (find_next->GetKeyCode() == event.GetKeyCode() && find_next->GetFlags() == event.GetModifiers()) {
        DoFindWithWrap(FIND_DEFAULT | FIND_GOTOLINE);
        return true;
    } else if (find_prev->GetKeyCode() == event.GetKeyCode() && find_prev->GetFlags() == event.GetModifiers()) {
        DoFindWithWrap(FIND_PREV | FIND_GOTOLINE);
        return true;
    }
    return false;
}

void QuickFindBar::ShowMenuForFindCtrl()
{
    if (m_findHistory.m_commands.empty()) {
        return;
    }
    wxMenu menu;
    for (const auto& word : m_findHistory.m_commands) {
        int resource_id = wxXmlResource::GetXRCID(word);
        menu.Append(resource_id, word);
        menu.Bind(
            wxEVT_MENU,
            [this, word](wxCommandEvent& event) {
                wxUnusedVar(event);
                m_textCtrlFind->SetValue(word);
                m_textCtrlFind->SetInsertionPointEnd();
            },
            resource_id);
    }

    TextCtrlShowMenu(m_textCtrlFind, menu);
}

void QuickFindBar::ShowMenuForReplaceCtrl()
{
    if (m_replaceHistory.m_commands.empty()) {
        return;
    }

    wxMenu menu;
    for (const auto& word : m_replaceHistory.m_commands) {
        int resource_id = wxXmlResource::GetXRCID(word);
        menu.Append(resource_id, word);
        menu.Bind(
            wxEVT_MENU,
            [this, word](wxCommandEvent& event) {
                wxUnusedVar(event);
                m_textCtrlReplace->SetValue(word);
                m_textCtrlReplace->SetInsertionPointEnd();
            },
            resource_id);
    }

    TextCtrlShowMenu(m_textCtrlReplace, menu);
}
