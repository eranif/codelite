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

#include <vector>
#include <wx/combobox.h>
#include <wx/panel.h>

class wxStaticText;
class clToolBar;
class wxStyledTextCtrl;

struct TargetRange {
    enum FailReason {
        NONE,
        REACHED_EOF,
        REACHED_SOF,
        EMPTY_RANGE,
    };
    int start_pos = wxNOT_FOUND;
    int end_pos = wxNOT_FOUND;
    FailReason why = NONE;

    TargetRange() {}

    // this constructor is required to silence errors on macOS with latest clang
    TargetRange(int start, int end)
        : start_pos(start)
        , end_pos(end)
        , why(NONE)
    {
    }

    bool Equals(const TargetRange& other) const { return start_pos == other.start_pos && end_pos == other.end_pos; }
    bool PosInside(int pos) const { return start_pos >= pos && pos < end_pos; }
    bool IsOk() const { return start_pos != wxNOT_FOUND && end_pos != wxNOT_FOUND; }
    typedef std::vector<TargetRange> Vec_t;
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
    virtual void OnReplaceTextEnter(wxCommandEvent& event);
    virtual void OnReplaceTextUpdated(wxCommandEvent& event);
    enum eFindFlags {
        FIND_DEFAULT = 0,
        FIND_PREV = (1 << 0),
        FIND_INCREMENT = (1 << 1),
        FIND_GOTOLINE = (1 << 2),
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
    size_t m_searchFlags = 0;
    bool m_highlightMatches;
    bool m_inSelection;
    clTerminalHistory m_searchHistory;
    clTerminalHistory m_replaceHistory;

protected:
    virtual void OnButtonKeyDown(wxKeyEvent& event);
    virtual void OnReplaceAllUI(wxUpdateUIEvent& event);
    virtual void OnReplaceUI(wxUpdateUIEvent& event);
    virtual void OnFind(wxCommandEvent& event);
    virtual void OnFindAllUI(wxUpdateUIEvent& event);
    virtual void OnFindPrev(wxCommandEvent& event);
    virtual void OnFindPrevUI(wxUpdateUIEvent& event);
    virtual void OnFindUI(wxUpdateUIEvent& event);

    void DoArrowDown(clTerminalHistory& history, wxTextCtrl* ctrl);
    void DoArrowUp(clTerminalHistory& history, wxTextCtrl* ctrl);

    /**
     * @brief find text in the editor the select it
     * @param find_prev should the search go backward? (i.e. from the current position to the start of the document)
     * @param target use this range for the search
     * @return a pair of indexes indicating the selection start and end positions
     */
    TargetRange DoFind(size_t find_flags, const TargetRange& target = {});

    /**
     * @brief same as the above, but also print a message when hitting the bottom or the
     * top of the document
     */
    TargetRange DoFindWithMessage(size_t find_flags, const TargetRange& target = {});

    /**
     * @brief same as the above, but attempt to wrap incase of failure
     */
    TargetRange DoFindWithWrap(size_t find_flags, const TargetRange& target = {});

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

    // replace the current target
    // returns the length of the replacement text
    int DoReplace(const TargetRange& range = {});

    void DoSetCaretAtEndOfText();
    void DoFixRegexParen(wxString& findwhat);
    wxString DoGetSelectedText();
    void DoSelectAll();
    TargetRange::Vec_t DoFindAll(const TargetRange& target);
    size_t DoReplaceInBuffer(const TargetRange& range);
    void DoHighlightMatches(bool checked);
    bool IsReplacementRegex() const;

    /**
     * @brief return the best target range taking into account
     * the `In Selection` flag
     */
    TargetRange GetBestTargetRange() const;

    // Control events
    void OnHide(wxCommandEvent& e);
    void OnFindAll(wxCommandEvent& e);
    void OnText(wxCommandEvent& e);
    void OnKeyDown(wxKeyEvent& e);
    void OnReplaceAll(wxCommandEvent& e);
    void OnEnter(wxCommandEvent& e);
    void OnReplace(wxCommandEvent& e);
    void OnUpdateUI(wxUpdateUIEvent& e);
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
    QuickFindBar(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~QuickFindBar();
    int GetCloseButtonId();
    bool ShowForPlugins();
    bool Show(bool s = true);
    bool Show(const wxString& findWhat, bool showReplace);
    wxStyledTextCtrl* GetEditor() { return m_sci; }
    void SetEditor(wxStyledTextCtrl* sci);
    void ShowToolBarOnly();

    /// allow outside controls to trigger the search
    void FindPrevious();
    void FindNext();

    size_t GetSearchFlags() const { return m_searchFlags; }
    wxString GetFindWhat() const { return m_textCtrlFind->GetValue(); }
    void SetFindWhat(const wxString& findwhat) { m_textCtrlFind->ChangeValue(findwhat); }
};

#endif // __quickfindbar__
