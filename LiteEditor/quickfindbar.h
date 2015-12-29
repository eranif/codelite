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

class QuickFindBarOptionsMenu;
class wxStyledTextCtrl;
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
    size_t m_flags;
    wxString m_lastText;
    wchar_t* m_lastTextPtr;
    bool m_eventsConnected;
    QuickFindBarOptionsMenu* m_optionsWindow;
    wxComboBox* m_findWhat;
    wxComboBox* m_replaceWith;
    wxFlatButtonBar* m_bar;

    wxFlatButton* m_caseSensitive;
    wxFlatButton* m_wholeWord;
    wxFlatButton* m_regexOrWildButton;
    wxButton* m_buttonReplace;
    wxFlatButton* m_closeButton;
    eRegexType m_regexType;
    bool m_disableTextUpdateEvent;
    friend class QuickFindBarOptionsMenu;

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
        kSearchForward = 0x00000001,
        kSearchIncremental = 0x00000002,
        kSearchMultiSelect = 0x00000004,
    };

private:
    void BindEditEvents(bool bind);
    void DoUpdateSearchHistory();
    void DoUpdateReplaceHistory();
    size_t DoGetSearchFlags();

protected:
    virtual void OnReplaceKeyDown(wxKeyEvent& event);
    void DoSearch(size_t searchFlags, int posToSearchFrom = wxNOT_FOUND);
    void DoFixRegexParen(wxString& findwhat);
    wxString DoGetSelectedText();
    void DoMarkAll();

    // General events
    void OnUndo(wxCommandEvent& e);
    void OnRedo(wxCommandEvent& e);
    void OnCopy(wxCommandEvent& e);
    void OnPaste(wxCommandEvent& e);
    void OnSelectAll(wxCommandEvent& e);
    void OnEditUI(wxUpdateUIEvent& e);
    void DoEnsureSelectionVisible();
    
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
    void OnButtonReplaceUI(wxUpdateUIEvent& e);
    void OnEnter(wxCommandEvent& e);
    void OnReplace(wxCommandEvent& e);
    void OnUpdateUI(wxUpdateUIEvent& e);
    void OnReplaceUI(wxUpdateUIEvent& e);
    void OnReplaceEnter(wxCommandEvent& e);
    void OnHighlightMatches(wxFlatButtonEvent& e);
    void OnHideBar(wxFlatButtonEvent& e);
    void OnRegex(wxFlatButtonEvent& event);
    void OnRegexUI(wxUpdateUIEvent& event);
    void OnHighlightMatchesUI(wxUpdateUIEvent& event);
    void OnQuickFindCommandEvent(wxCommandEvent& event);
    void OnReceivingFocus(wxFocusEvent& event);
    void OnReleaseEditor(wxCommandEvent& e);

    void OnFindNext(wxCommandEvent& e);
    void OnFindPrevious(wxCommandEvent& e);
    void OnFindNextCaret(wxCommandEvent& e);
    void OnFindPreviousCaret(wxCommandEvent& e);

protected:
    bool DoShow(bool s, const wxString& findWhat);
    void DoToggleReplacebar();
    wxStyledTextCtrl* DoCheckPlugins();

public:
    QuickFindBar(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~QuickFindBar();
    int GetCloseButtonId();
    bool ShowForPlugins();
    bool Show(bool s = true);
    bool Show(const wxString& findWhat);
    void ShowReplacebar(bool show);
    void ToggleReplacebar();
    wxStyledTextCtrl* GetEditor() { return m_sci; }
    void SetEditor(wxStyledTextCtrl* sci);
};

#endif // __quickfindbar__
