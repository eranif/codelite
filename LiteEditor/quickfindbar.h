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
    wxFlatButton* m_showReplaceControls;
    wxFlatButton* m_buttonReplace;
    wxMenu* m_regexOrWildMenu;
    eRegexType m_regexType;
    
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
    wxString DoGetSelectedText();
    void DoMarkAll(bool useIndicators = true);
    wchar_t* DoGetSearchStringPtr();

    // General events
    void OnUndo(wxCommandEvent& e);
    void OnRedo(wxCommandEvent& e);
    void OnCopy(wxCommandEvent& e);
    void OnPaste(wxCommandEvent& e);
    void OnSelectAll(wxCommandEvent& e);
    void OnEditUI(wxUpdateUIEvent& e);

    // Control events
    void OnHide(wxCommandEvent& e);
    void OnNext(wxCommandEvent& e);
    void OnPrev(wxCommandEvent& e);
    void OnButtonNext(wxFlatButtonEvent& e);
    void OnButtonPrev(wxFlatButtonEvent& e);
    void OnFindAll(wxFlatButtonEvent& e);
    void OnButtonNextUI(wxUpdateUIEvent& e);
    void OnButtonPrevUI(wxUpdateUIEvent& e);
    void OnText(wxCommandEvent& e);
    void OnKeyDown(wxKeyEvent& e);
    void OnShowReplaceControls(wxFlatButtonEvent& e);
    void OnButtonReplace(wxFlatButtonEvent& e);
    void OnButtonReplaceUI(wxUpdateUIEvent& e);
    void OnEnter(wxCommandEvent& e);
    void OnReplace(wxCommandEvent& e);
    void OnUpdateUI(wxUpdateUIEvent& e);
    void OnReplaceUI(wxUpdateUIEvent& e);
    void OnReplaceEnter(wxCommandEvent& e);
    void OnHighlightMatches(wxFlatButtonEvent& e);
    void OnRegularExpMenu(wxFlatButtonEvent& e);
    void OnUseRegex(wxCommandEvent& e);
    void OnNoRegex(wxCommandEvent& e);
    void OnUseWildcards(wxCommandEvent& e);
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
    wxStyledTextCtrl* DoCheckPlugins();

public:
    QuickFindBar(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~QuickFindBar();
    int GetCloseButtonId();
    bool ShowForPlugins();
    bool Show(bool s = true);
    bool Show(const wxString& findWhat);
    wxStyledTextCtrl* GetEditor() { return m_sci; }
    void SetEditor(wxStyledTextCtrl* sci);
};

#endif // __quickfindbar__
