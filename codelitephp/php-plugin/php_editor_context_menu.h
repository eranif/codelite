//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : php_editor_context_menu.h
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

#ifndef PHPEDITORCONTEXTMENU_H
#define PHPEDITORCONTEXTMENU_H

#include "cl_command_event.h"
#include "ieditor.h"

#include <wx/event.h>

enum PhpEditorEventIds {
    wxID_OPEN_PHP_FILE = 10105,
    wxID_GOTO_DEFINITION = 10108,
    wxID_FIND_REFERENCES = 10109,
    wxID_ADD_DOXY_COMMENT = 10110,
    wxID_GENERATE_GETTERS_SETTERS = 10111,
};

// Check values macros
#define SET_CARET_POS(caret_pos)                 \
    {                                            \
        sci->SetSelection(caret_pos, caret_pos); \
        sci->ChooseCaretX();                     \
    }

class IManager;
class PhpPlugin;
class PHPEditorContextMenu : public wxEvtHandler
{
    friend class PhpPlugin;
    static PHPEditorContextMenu* ms_instance;
    wxString m_selectedWord;
    IManager* m_manager;
    wxString m_comment_line_1;
    wxString m_comment_line_2;
    wxString m_start_comment;
    wxString m_close_comment;

protected:
    // Helpers
    void DoContextMenu(IEditor* editor, wxCommandEvent& e);
    wxStyledTextCtrl* DoGetActiveScintila();

protected:
    // Event handlers
    void OnContextMenu(clContextMenuEvent& e);
    void OnMarginContextMenu(clContextMenuEvent& e);
    void OnContextOpenDocument(wxCommandEvent& event);
    void OnPopupClicked(wxCommandEvent& event);

    // Helpers
    void DoOpenPHPFile();
    void DoGotoBeginningOfScope();
    void DoGotoEndOfScope();
    void DoGotoDefinition();
    void OnInsertDoxyComment(wxCommandEvent& e);
    void OnGenerateSettersGetters(wxCommandEvent& e);

public:
    static PHPEditorContextMenu* Instance();
    static void Release();
    void ConnectEvents();

    void SetManager(IManager* manager) { this->m_manager = manager; }

    bool IsPHPSection(int styleAtPos) const;
    bool IsPHPCommentOrString(int styleAtPos) const;

private:
    PHPEditorContextMenu();
    virtual ~PHPEditorContextMenu();
    
    void OnCommentSelection(wxCommandEvent &event);
    void OnCommentLine(wxCommandEvent &event);
    
    void DoBuildMenu(wxMenu* menu, IEditor* editor);
    bool IsIncludeOrRequireStatement(wxString& includeWhat);
    bool GetIncludeOrRequireFileName(wxString& fn);
    bool IsLineMarked(); // does a bookmark exist in the current lines
    int
    RemoveComment(wxStyledTextCtrl* sci, int posFrom, const wxString& value); // return the number of characters removed
    void CommentSingleLine(wxStyledTextCtrl* sci, int style, int line_number, int& caret_pos);
    bool RemoveSingleLineComment(wxStyledTextCtrl* sci, int& caret_pos);
    bool RemoveTokenFirstIteration(wxStyledTextCtrl* sci, const wxString& token, bool direction, int& caret_pos);
    int GetTokenPosInScope(wxStyledTextCtrl* sci,
                           const wxString& token,
                           int start_pos,
                           int end_pos,
                           bool direction,
                           const wxArrayString& tokensBlackList);
    bool IsTokenInBlackList(wxStyledTextCtrl* sci,
                            const wxString& token,
                            int token_pos,
                            const wxArrayString& tokensBlackList);
};

#define GET_EDITOR_SCI_BOOL()                       \
    IEditor* editor = m_manager->GetActiveEditor(); \
    if(!editor) return false;                       \
    wxStyledTextCtrl* sci = editor->GetCtrl();       \
    if(!sci) return false;

#define GET_EDITOR_SCI_VOID()                       \
    IEditor* editor = m_manager->GetActiveEditor(); \
    if(!editor) return;                             \
    wxStyledTextCtrl* sci = editor->GetCtrl();       \
    if(!sci) return;

#endif // PHPEDITORCONTEXTMENU_H
