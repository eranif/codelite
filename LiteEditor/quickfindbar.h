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
#include "theme_handler_helper.h"

class QuickFindBarOptionsMenu;
class wxStyledTextCtrl;
class QuickFindBar : public QuickFindBarBase
{
    wxStyledTextCtrl  *m_sci;
    size_t             m_flags;
    wxString           m_lastText;
    wchar_t*           m_lastTextPtr;
    ThemeHandlerHelper m_themeHelper;
    bool               m_eventsConnected;
    QuickFindBarOptionsMenu* m_optionsWindow;

    friend class QuickFindBarOptionsMenu;
public:
    enum {
        ID_TOOL_REPLACE = 1000,
        ID_TOOL_CLOSE,
        ID_TOOL_FIND,
    };

    enum {
        kSearchForward      = 0x00000001,
        kSearchIncremental  = 0x00000002,
        kSearchMultiSelect  = 0x00000004,
    };
private:
    void BindEditEvents(bool bind);
    void DoUpdateSearchHistory();
    void DoUpdateReplaceHistory();

    QuickFindBarOptionsMenu* GetOptionsMenu();
    
protected:
    virtual void OnOptions(wxCommandEvent& event);
    virtual void OnReplaceKeyDown(wxKeyEvent& event);
    virtual void OnCheckBoxRegex(const wxCommandEvent& event);
    virtual void OnCheckWild(const wxCommandEvent& event);
    void     DoSearch( size_t searchFlags );
    wxString DoGetSelectedText();
    void     DoMarkAll();
    wchar_t* DoGetSearchStringPtr();

    // General events
    void OnUndo         (wxCommandEvent  &e);
    void OnRedo         (wxCommandEvent  &e);
    void OnCopy         (wxCommandEvent  &e);
    void OnPaste        (wxCommandEvent  &e);
    void OnSelectAll    (wxCommandEvent  &e);
    void OnEditUI       (wxUpdateUIEvent &e);


    // Control events
    void OnHide         (wxCommandEvent  &e);
    void OnNext         (wxCommandEvent  &e);
    void OnPrev         (wxCommandEvent  &e);
    void OnText         (wxCommandEvent  &e);
    void OnKeyDown      (wxKeyEvent      &e);
    void OnEnter        (wxCommandEvent  &e);
    void OnReplace      (wxCommandEvent  &e);
    void OnUpdateUI     (wxUpdateUIEvent &e);
    void OnReplaceUI    (wxUpdateUIEvent &e);
    void OnReplaceEnter (wxCommandEvent &e);
    void OnHighlightMatches(const wxCommandEvent& event);
    void OnHighlightMatchesUI(wxUpdateUIEvent& event);
    void OnQuickFindCommandEvent(wxCommandEvent& event);
    void OnReceivingFocus(wxFocusEvent& event);
    void OnReleaseEditor(wxCommandEvent &e);

    void OnFindNext         (wxCommandEvent &e);
    void OnFindPrevious     (wxCommandEvent &e);
    void OnFindNextCaret    (wxCommandEvent &e);
    void OnFindPreviousCaret(wxCommandEvent &e);

protected:
    bool DoShow(bool s, const wxString &findWhat);
    wxStyledTextCtrl *DoCheckPlugins();

public:
    QuickFindBar(wxWindow *parent, wxWindowID id = wxID_ANY);
    virtual ~QuickFindBar();
    int GetCloseButtonId();
    bool ShowForPlugins();
    bool Show(bool s = true);
    bool Show(const wxString &findWhat);
    wxStyledTextCtrl *GetEditor() {
        return m_sci;
    }
    void SetEditor(wxStyledTextCtrl *sci) ;
};

#endif // __quickfindbar__
