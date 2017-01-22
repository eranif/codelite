//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : quickfindbar.h
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
#ifndef __quickfindbar__
#define __quickfindbar__

#include <wx/panel.h>
#include "quickfindbarbase.h"
#include "wxFlatButton.h"
#include <wx/combobox.h>
#include "wxFlatButtonBar.h"
#include "clEditorEditEventsHandler.h"

class QuickFindBarOptionsMenu;
class wxStyledTextCtrl;

class clNoMatchBitmap : public wxPanel
{
    wxBitmap m_warningBmp;
    bool m_visible;

protected:
    void OnPaint(wxPaintEvent& event);
    void OnEraseBg(wxEraseEvent& event) { wxUnusedVar(event); }

public:
    clNoMatchBitmap(wxWindow* parent);
    virtual ~clNoMatchBitmap();

    void SetVisible(bool visible) { this->m_visible = visible; }
    bool IsVisible() const { return m_visible; }

    void SetWarningBmp(const wxBitmap& warningBmp) { this->m_warningBmp = warningBmp; }
    const wxBitmap& GetWarningBmp() const { return m_warningBmp; }
};

class QuickFindBar : public QuickFindBarBase
{
public:
    enum eRegexType {
        kRegexNone,
        kRegexWildcard,
        kRegexPosix,
    };

protected:
    wxStyledTextCtrl* m_sci;
    wxString m_lastText;
    wchar_t* m_lastTextPtr;
    bool m_eventsConnected;
    bool m_onNextPrev;
    QuickFindBarOptionsMenu* m_optionsWindow;
    wxComboBox* m_findWhat;
    wxComboBox* m_replaceWith;
    wxFlatButtonBar* m_bar;

    wxFlatButton* m_caseSensitive;
    wxFlatButton* m_wholeWord;
    wxFlatButton* m_regexOrWildButton;
    wxFlatButton* m_highlightOccurrences;
    wxStaticText* m_matchesFound;
    wxButton* m_buttonReplace;
    wxButton* m_buttonReplaceAll;
    wxButton* btnPrev;
    wxButton* btnNext;
    wxButton* btnAll;
    wxFlatButton* m_closeButton;
    wxFlatButton* m_replaceInSelectionButton;
    clNoMatchBitmap* m_noMatchBmp;
    eRegexType m_regexType;
    bool m_disableTextUpdateEvent;
    friend class QuickFindBarOptionsMenu;

    clEditEventsHandler::Ptr_t m_findEventsHandler;
    clEditEventsHandler::Ptr_t m_replaceEventsHandler;

public:
    enum {
        ID_TOOL_REPLACE = 1000,
        ID_TOOL_CLOSE,
        ID_TOOL_FIND,
    };

    enum {
        ID_MENU_REGEX = 2000,
        ID_MENU_WILDCARD,
        ID_MENU_NO_REGEX,
    };

    enum {
        // We mix between wxSTC flags and our own, to avoid the mixing the values
        // we start from the 16th bit
        kSearchForward = (1 << 16),
        kDisableDisplayErrorMessages = (1 << 17),
        kBreakWhenWrapSearch = (1 << 18),
    };

private:
    void BindEditEvents(bool bind);
    void DoUpdateSearchHistory();
    void DoUpdateReplaceHistory();
    size_t DoGetSearchFlags();
    void DoReplaceAll(bool selectionOnly);

protected:
    virtual void OnReplaceKeyDown(wxKeyEvent& event);
    virtual void OnFindKeyDown(wxKeyEvent& event);
    bool DoSearch(size_t searchFlags);
    void DoSearchCB(size_t searchFlags) { DoSearch(searchFlags); }
    void DoReplace();
    void DoSetCaretAtEndOfText();
    void DoFixRegexParen(wxString& findwhat);
    wxString DoGetSelectedText();
    void DoSelectAll(bool addMarkers);
    void DoHighlightMatches(bool checked);

    // General events
    void OnUndo(wxCommandEvent& e);
    void OnRedo(wxCommandEvent& e);
    void OnCopy(wxCommandEvent& e);
    void OnPaste(wxCommandEvent& e);
    void OnSelectAll(wxCommandEvent& e);
    void OnEditUI(wxUpdateUIEvent& e);
    void DoEnsureLineIsVisible(int line = wxNOT_FOUND);

    // Control events
    void OnHide(wxCommandEvent& e);
    void OnNext(wxCommandEvent& e);
    void OnPrev(wxCommandEvent& e);
    void OnFindAll(wxCommandEvent& e);
    void OnButtonNext(wxCommandEvent& e);
    void OnButtonPrev(wxCommandEvent& e);
    void OnButtonNextUI(wxUpdateUIEvent& e);
    void OnButtonPrevUI(wxUpdateUIEvent& e);
    void OnText(wxCommandEvent& e);
    void OnKeyDown(wxKeyEvent& e);
    void OnFindMouseWheel(wxMouseEvent& e);
    void OnButtonReplace(wxCommandEvent& e);
    void OnReplaceAll(wxCommandEvent& e);
    void OnButtonReplaceUI(wxUpdateUIEvent& e);
    void OnButtonReplaceAllUI(wxUpdateUIEvent& e);
    void OnEnter(wxCommandEvent& e);
    void OnReplace(wxCommandEvent& e);
    void OnUpdateUI(wxUpdateUIEvent& e);
    void OnReplaceEnter(wxCommandEvent& e);
    void OnHighlightMatches(wxFlatButtonEvent& e);
    void OnReplaceInSelection(wxFlatButtonEvent& e);
    void OnHideBar(wxFlatButtonEvent& e);
    void OnRegex(wxFlatButtonEvent& event);
    void OnRegexUI(wxUpdateUIEvent& event);
    void OnHighlightMatchesUI(wxUpdateUIEvent& event);
    void OnReplaceInSelectionUI(wxUpdateUIEvent& event);
    void OnQuickFindCommandEvent(wxCommandEvent& event);
    void OnReceivingFocus(wxFocusEvent& event);
    void OnReleaseEditor(clFindEvent& e);

    void OnFindNext(wxCommandEvent& e);
    void OnFindPrevious(wxCommandEvent& e);
    void OnFindNextCaret(wxCommandEvent& e);
    void OnFindPreviousCaret(wxCommandEvent& e);

protected:
    bool DoShow(bool s, const wxString& findWhat, bool replaceBar);
    wxStyledTextCtrl* DoCheckPlugins();

public:
    QuickFindBar(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~QuickFindBar();
    int GetCloseButtonId();
    bool ShowForPlugins();
    bool Show(bool s = true, bool replaceBar = false);
    bool Show(const wxString& findWhat, bool replaceBar = false);
    void ShowReplacebar(bool show);
    wxStyledTextCtrl* GetEditor() { return m_sci; }
    void SetEditor(wxStyledTextCtrl* sci);
};

#endif // __quickfindbar__
