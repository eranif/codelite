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

#include "clEditorEditEventsHandler.h"
#include "clTerminalHistory.h"
#include "quickfindbarbase.h"
#include <wx/combobox.h>
#include <wx/panel.h>
#include <codelite_exports.h>

class clToolBar;
class wxStyledTextCtrl;

class WXDLLIMPEXP_SDK clPluginsFindBar : public QuickFindBarBase
{
public:
    enum eRegexType {
        kRegexNone,
        kRegexWildcard,
        kRegexPosix,
    };

    wxStyledTextCtrl* m_sci;
    wxString m_lastText;
    wchar_t* m_lastTextPtr;
    bool m_eventsConnected;
    bool m_onNextPrev;
    eRegexType m_regexType;
    bool m_disableTextUpdateEvent;
    clEditEventsHandler::Ptr_t m_findEventsHandler;
    clEditEventsHandler::Ptr_t m_replaceEventsHandler;
    size_t m_searchFlags;
    bool m_highlightMatches;
    bool m_replaceInSelection;
    clTerminalHistory m_searchHistory;
    clTerminalHistory m_replaceHistory;
    wxStaticText* m_matchesFound = nullptr;

protected:
    virtual void OnButtonKeyDown(wxKeyEvent& event);
    virtual void OnReplaceAllUI(wxUpdateUIEvent& event);
    virtual void OnReplaceUI(wxUpdateUIEvent& event);
    virtual void OnFind(wxCommandEvent& event);
    virtual void OnFindAllUI(wxUpdateUIEvent& event);
    virtual void OnFindPrev(wxCommandEvent& event);
    virtual void OnFindPrevUI(wxUpdateUIEvent& event);
    virtual void OnFindUI(wxUpdateUIEvent& event);
    virtual void OnReplaceTextEnter(wxCommandEvent& event);
    virtual void OnReplaceTextUpdated(wxCommandEvent& event);

    void DoArrowDown(clTerminalHistory& history, wxTextCtrl* ctrl);
    void DoArrowUp(clTerminalHistory& history, wxTextCtrl* ctrl);

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
    size_t DoGetSearchFlags() const;
    void DoReplaceAll(bool selectionOnly);

protected:
    virtual void OnReplaceKeyDown(wxKeyEvent& event);
    bool DoSearch(size_t searchFlags);
    void DoSearchCB(size_t searchFlags) { DoSearch(searchFlags); }
    void DoReplace();
    void DoSetCaretAtEndOfText();
    void DoFixRegexParen(wxString& findwhat);
    wxString DoGetSelectedText();
    void DoSelectAll(bool addMarkers);
    void DoHighlightMatches(bool checked);

    // General events
    static void DoEnsureLineIsVisible(wxStyledTextCtrl* sci, int line = wxNOT_FOUND);

    // Control events
    void OnHide(wxCommandEvent& e);
    void OnNext(wxCommandEvent& e);
    void OnPrev(wxCommandEvent& e);
    void OnFindAll(wxCommandEvent& e);
    void OnButtonNext(wxCommandEvent& e);
    void OnButtonPrev(wxCommandEvent& e);
    void OnText(wxCommandEvent& e);
    void OnKeyDown(wxKeyEvent& e);
    void OnFindMouseWheel(wxMouseEvent& e);
    void OnButtonReplace(wxCommandEvent& e);
    void OnReplaceAll(wxCommandEvent& e);
    void OnEnter(wxCommandEvent& e);
    void OnReplace(wxCommandEvent& e);
    void OnReplaceEnter(wxCommandEvent& e);
    void OnQuickFindCommandEvent(wxCommandEvent& event);
    void OnReceivingFocus(wxFocusEvent& event);
    void OnReleaseEditor(clFindEvent& e);
    void OnPaint(wxPaintEvent& e);
    void OnFindNextCaret(wxCommandEvent& e);
    void OnFindPreviousCaret(wxCommandEvent& e);

protected:
    bool DoShow(bool s, const wxString& findWhat, bool showReplace = false);
    wxStyledTextCtrl* DoCheckPlugins();

public:
    clPluginsFindBar(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~clPluginsFindBar();
    int GetCloseButtonId();
    bool ShowForPlugins();
    bool Show(bool s = true);
    bool Show(const wxString& findWhat, bool showReplace);
    wxStyledTextCtrl* GetEditor() { return m_sci; }
    void SetEditor(wxStyledTextCtrl* sci);

    /**
     * @brief search a stc control for 'findwhat'. Use kSearchForward to indicate searching forward, pass 0
     * for backward.
     * 'This' is used internally, so pass it NULL
     */
    static bool Search(wxStyledTextCtrl* ctrl, const wxString& findwhat, size_t search_flags,
                       clPluginsFindBar* This = NULL);

    wxString GetFindWhat() const { return m_textCtrlFind->GetValue(); }
    void SetFindWhat(const wxString& findwhat) { m_textCtrlFind->ChangeValue(findwhat); }
};

#endif // __quickfindbar__
