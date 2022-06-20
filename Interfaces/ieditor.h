//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : ieditor.h
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

#ifndef IEDITOR_H
#define IEDITOR_H

#include "LSP/basic_types.h"
#include "browse_record.h"
#include "cl_calltip.h"
#include "entry.h"
#include "optionsconfig.h"
#include "wx/string.h"

#include <list>
#include <map>
#include <string>
#include <vector>
#include <wx/colour.h>
#include <wx/filename.h>

class SFTPClientData;
class wxStyledTextCtrl;

class NavMgr;
class wxClientData;

enum {
    Format_Text_Default = 0x00000000,
    Format_Text_Indent_Prev_Line = 0x00000001,
    Format_Text_Save_Empty_Lines = 0x00000002,
};

//------------------------------------------------------------------
// Defines the interface to the editor control
//------------------------------------------------------------------
/**
 * @class IEditor
 * @author Eran
 * @date 05/07/08
 * @file ieditor.h
 * @brief an interface for the plugin to provide clean access to the editor class
 */
class IEditor
{
public:
    typedef std::list<IEditor*> List_t;
    typedef std::map<wxString, wxClientData*> ClientDataMap_t;

protected:
    IEditor::ClientDataMap_t m_data;

public:
    IEditor() {}
    virtual ~IEditor()
    {
        IEditor::ClientDataMap_t::iterator iter = m_data.begin();
        for(; iter != m_data.end(); ++iter) {
            wxDELETE(iter->second);
        }
        m_data.clear();
    }

    /**
     * @brief toggle line comment
     * @param commentSymbol the comment symbol to insert (e.g. "//")
     * @param commentStyle the wxSTC line comment style (e.g. wxSTC_C_COMMENTLINE)
     */
    virtual void ToggleLineComment(const wxString& commentSymbol, int commentStyle) = 0;

    /**
     * @brief block comment the selection
     */
    virtual void CommentBlockSelection(const wxString& commentBlockStart, const wxString& commentBlockEnd) = 0;

    /**
     * @brief return true if the editor is modified
     */
    virtual bool IsEditorModified() const = 0;

    /**
     * @brief Get the editor's modification count
     */
    virtual wxUint64 GetModificationCount() const = 0;

    /**
     * @brief return the current editor content
     */
    virtual wxString GetEditorText() = 0;

    /**
     * @brief get editor text raw, in an efficient way
     */
    virtual size_t GetEditorTextRaw(std::string& content) = 0;

    /**
     * @brief sets the current editor's content with 'text'
     */
    virtual void SetEditorText(const wxString& text) = 0;

    /**
     * @brief append text to the editor
     * @param text text to append
     */
    virtual void AppendText(const wxString& text) = 0;

    /**
     * @brief insert text at a given position
     * @param text test to insert
     * @param pos position to insert it
     */
    virtual void InsertText(int pos, const wxString& text) = 0;

    /**
     * @brief return the document length
     * @return document length
     */
    virtual int GetLength() = 0;

    /**
     * @brief place the carte at a given position
     * @param pos position to place the caret
     */
    virtual void SetCaretAt(long pos) = 0;

    /**
     * @brief reload the current file from disk - this function discards all changes made
     * to the  current file
     */
    virtual void OpenFile() = 0;

    /**
     * @brief reload file content from the disk
     * @param keepUndoHistory
     */
    virtual void ReloadFromDisk(bool keepUndoHistory = false) = 0;
    /**
     * @brief save the editor
     */
    virtual bool Save() = 0;

    /**
     * @brief save the current editor with a different name
     */
    virtual bool SaveAs(const wxString& defaultName = wxEmptyString, const wxString& savePath = wxEmptyString) = 0;

    /**
     * @brief return the current position of the caret
     */
    virtual long GetCurrentPosition() = 0;

    /**
     * @brief return the current file name
     */
    virtual const wxFileName& GetFileName() const = 0;

    /**
     * @brief return the project which owns the current file name. return wxEmptyString
     * if this file has not project owner
     */
    virtual const wxString& GetProjectName() const = 0;

    /**
     * @brief return the position under the mouse pointer
     */
    virtual int GetPosAtMousePointer() = 0;

    /**
     * @brief return the current word under the caret. May return wxEmptyString
     */
    virtual wxString GetWordAtCaret(bool wordCharsOnly = true) = 0;

    /**
     * @brief return the word under the mouse pointer.
     * If a selection exists, return it instead
     */
    virtual void GetWordAtMousePointer(wxString& word, wxRect& wordRect) = 0;

    /**
     * @brief get word at a given position
     * @param pos word's position
     * @param wordCharsOnly when set to false, return the string between the nearest whitespaces
     */
    virtual wxString GetWordAtPosition(int pos, bool wordCharsOnly = true) = 0;

    /**
     * @brief return the EOL mode of the editor.
     * 	wxSCI_EOL_CRLF 	0
     *	wxSCI_EOL_CR 	1
     *	wxSCI_EOL_LF 	2
     */
    virtual int GetEOL() = 0;

    /**
     * @brief return the current line number
     * \return the line number starting from 0
     */
    virtual int GetCurrentLine() = 0;

    /**
     * @brief replace the selection with 'text'. This function does nothing if there is no selected text
     * @param text text to replace the selection
     */
    virtual void ReplaceSelection(const wxString& text) = 0;

    /**
     * @brief return the selected text start position in bytes.
     */
    virtual int GetSelectionStart() = 0;

    /**
     * @brief return the selected text end position in bytes.
     */
    virtual int GetSelectionEnd() = 0;

    /**
     * @brief return the selected text.
     * \return the selected text, or wxEmptyString if no selection exist in the document
     */
    virtual wxString GetSelection() = 0;

    /**
     * @brief select text at given position and length
     * @param startPos selection start position
     * @param len number of characters to select
     */
    virtual void SelectText(int startPos, int len) = 0;

    /**
     * @brief set user indicator style and colour
     * @param style can be any of
     *  #define wxSCI_INDIC_PLAIN 0
     *  #define wxSCI_INDIC_SQUIGGLE 1
     *  #define wxSCI_INDIC_TT 2
     *  #define wxSCI_INDIC_DIAGONAL 3
     *  #define wxSCI_INDIC_STRIKE 4
     *  #define wxSCI_INDIC_HIDDEN 5
     *  #define wxSCI_INDIC_BOX 6
     *  #define wxSCI_INDIC_ROUNDBOX 7
     * @param colour indicator colour
     */
    virtual void SetUserIndicatorStyleAndColour(int style, const wxColour& colour) = 0;
    /**
     * @brief set indicator at startPos with given length
     * @param startPos position to set the indicator
     * @param len indicator's length
     */
    virtual void SetUserIndicator(int startPos, int len) = 0;

    /**
     * @brief clear all user indicators from the document
     */
    virtual void ClearUserIndicators() = 0;

    /**
     * @brief return true if there is a breakpoint marker on the given line
     */
    virtual bool HasBreakpointMarker(int line_number = wxNOT_FOUND) = 0;

    /**
     * @brief delete all breakpoint markers from the given line
     * @param line_number if set to wxNOT_FOUND(-1), delete all breakpoints from the editor
     */
    virtual void DeleteBreakpointMarkers(int line_number = wxNOT_FOUND) = 0;

    /**
     * @brief delete all breakpoint markers from the given line
     */
    virtual void SetBreakpointMarker(int line_number = wxNOT_FOUND, const wxString& tooltip = wxEmptyString) = 0;

    /**
     * @brief return all lines that have breakpoint marker
     */
    virtual size_t GetBreakpointMarkers(std::vector<int>* lines) = 0;

    /**
     * @brief return the first user indicator starting from 'pos'. along with 'GetUserIndicatorEnd' caller can
     * iterate through all user indicator in the document
     * @param pos position to search from
     * \return start position of the indicator
     */
    virtual int GetUserIndicatorStart(int pos) = 0;
    /**
     * @brief return end of indicator range from pos
     * @param pos position to search from
     * \return end of indicator range
     */
    virtual int GetUserIndicatorEnd(int pos) = 0;

    /**
     * @brief return the style at given position. Depends on the current lexer (Different lexers have different styles)
     * @param pos the position inside the editor styles.
     * \return style number.
     */
    virtual int GetStyleAtPos(int pos) = 0;

    /**
     * @brief return the editor's lexer ID as described in wxscintilla.h (wxSCI_LEX_*)
     */
    virtual int GetLexerId() = 0;

    /**
     * @brief display codelite calltip at the current position
     * @param tip tip to display
     */
    virtual void ShowCalltip(clCallTipPtr tip) = 0;

    /**
     * @brief display a tooltip
     * @param tip tip text
     * @param pos position for the tip. If wxNOT_FOUND the tip is positioned at the mouse
     */
    virtual void ShowTooltip(const wxString& tip, const wxString& title = wxEmptyString, int pos = wxNOT_FOUND) = 0;

    /**
     * @brief display a rich tooltip (a tip that supports basic markup, such as <a></a>, <strong></strong> etc)
     * @param tip tip text
     * @param pos position for the tip. If wxNOT_FOUND the tip is positioned at the mouse
     */
    virtual void ShowRichTooltip(const wxString& tip, const wxString& title, int pos = wxNOT_FOUND) = 0;

    /**
     * @brief return true if the completion box is visible
     */
    virtual bool IsCompletionBoxShown() = 0;

    /**
     * @brief Get position of start of word.
     * @param pos from position
     * @param onlyWordCharacters
     */
    virtual int WordStartPos(int pos, bool onlyWordCharacters) = 0;

    /**
     * @brief  Get position of end of word.
     * @param pos from position
     * @param onlyWordCharacters
     */
    virtual int WordEndPos(int pos, bool onlyWordCharacters) = 0;

    /**
     * Prepend the indentation level found at line at 'pos' to each line in the input string
     * @param text text to enter
     * @param pos position to insert the text
     * @param flags set the formatting flags
     */
    virtual wxString FormatTextKeepIndent(const wxString& text, int pos, size_t flags = 0) = 0;

    /**
     * @brief creating a browsing record that can be stored in the navigation manager
     * @return browsing record
     */
    virtual BrowseRecord CreateBrowseRecord() = 0;

    /**
     * @brief search the editor for pattern. If pattern is found, the editor will then search for 'what'
     * inside the pattern and will select it
     * @param pattern pattern to search in the editor
     * @param what    sub string of pattern to select
     * @param navmgr  Navigation manager to place browsing recrods
     * @return return true if a match was found, false otherwise
     */
    virtual bool FindAndSelect(const wxString& pattern, const wxString& what, int from_pos, NavMgr* navmgr) = 0;

    /**
     * @brief select range
     */
    virtual bool SelectRange(const LSP::Range& range) = 0;

    /**
     * @brief select range from a given location
     */
    virtual bool SelectLocation(const LSP::Location& location) = 0;

    /**
     * @brief Similar to the above but returns void, and is implemented asynchronously
     */
    virtual void FindAndSelectV(const wxString& pattern, const wxString& what, int pos = 0, NavMgr* navmgr = NULL) = 0;

    /**
     * @brief set a lexer to the editor
     * @param lexerName
     */
    virtual void SetLexerName(const wxString& lexerName) = 0;

    /**
     * @brief return the line numebr containing 'pos'
     * @param pos the position
     */
    virtual int LineFromPos(int pos) = 0;
    /**
     * @brief return the start pos of line nummber
     * @param line the line number
     * @return line nummber or 0 if the document is empty
     */
    virtual int PosFromLine(int line) = 0;

    /**
     * @brief return the END position of 'line'
     * @param line the line number
     */
    virtual int LineEnd(int line) = 0;

    /**
     * @brief return text from a given pos -> endPos
     * @param startPos
     * @param endPos
     * @return
     */
    virtual wxString GetTextRange(int startPos, int endPos) = 0;

    /**
     * @brief return the previous char in the editor
     * @param pos starting position
     * @param foundPos matched position
     * @param wantWhitespace set to false if whitespace chars should be skipped
     * @return return the matched char or 0 if at the start of document
     */
    virtual wxChar PreviousChar(const int& pos, int& foundPos, bool wantWhitespace) = 0;

    /**
     * @brief The minimum position returned is 0 and the maximum is the last position in the document
     */
    virtual int PositionAfterPos(int pos) = 0;

    /**
     * @brief The minimum position returned is 0 and the maximum is the last position in the document
     */
    virtual int PositionBeforePos(int pos) = 0;

    /**
     * @brief return the char at the 'pos', 0 if an error occurs
     */
    virtual int GetCharAtPos(int pos) = 0;

    /**
     * @brief center the editor around line and optionally a column
     */
    virtual void CenterLine(int line, int col = wxNOT_FOUND) = 0;

    /**
     * @brief center the editor around line, keeping any selection
     */
    virtual void CenterLinePreserveSelection(int line) = 0;

    /**
     * @brief return a pointer to the underlying scintilla control
     */
    virtual wxStyledTextCtrl* GetCtrl() = 0;

    /**
     * @brief set the focus to the current editor
     */
    virtual void SetActive() = 0;

    /**
     * @brief set the focus to the current editor, after a delay
     */
    virtual void DelayedSetActive() = 0;

    //-----------------------------------------------
    // Error/Warnings markers
    //-----------------------------------------------

    /**
     * @brief set a warning marker in the editor with a given text
     */
    virtual void SetWarningMarker(int lineno, const wxString& annotationText) = 0;
    /**
     * @brief set a warning marker in the editor with a given text
     */
    virtual void SetErrorMarker(int lineno, const wxString& annotationText) = 0;

    /**
     * @brief set a code completion annotation at the given line. code completion
     * annotations are automatically cleared on the next char added
     * @param text
     * @param lineno
     */
    virtual void SetCodeCompletionAnnotation(const wxString& text, int lineno) = 0;
    /**
     * @brief delete all compiler markers (warnings/errors)
     */
    virtual void DelAllCompilerMarkers() = 0;

    //-------------------------------------------------
    // Provide a user client data API
    //-------------------------------------------------
    /**
     * @brief set client data to this editor with key. If client data with this key
     * already exists, delete and replace  it
     * @param key
     * @param data
     */
    void SetClientData(const wxString& key, wxClientData* data)
    {
        IEditor::ClientDataMap_t::iterator iter = m_data.find(key);
        if(iter != m_data.end()) {
            wxDELETE(iter->second);
            m_data.erase(iter);
        }
        m_data.insert(std::make_pair(key, data));
    }

    /**
     * @brief force a syntax highlight of 'langname' to the editor
     */
    virtual void SetSyntaxHighlight(const wxString& langname) = 0;

    /**
     * @brief return the client data associated with this editor and identified by key
     * @param key
     * @return client data or NULL
     */
    wxClientData* GetClientData(const wxString& key) const
    {
        IEditor::ClientDataMap_t::const_iterator iter = m_data.find(key);
        if(iter != m_data.end()) {
            return iter->second;
        }
        return NULL;
    }

    /**
     * @brief delete the client data associated with this editor and identified by 'key'
     * this method also delete the memory allocated by the data
     * @param key
     */
    void DeleteClientData(const wxString& key)
    {
        IEditor::ClientDataMap_t::iterator iter = m_data.find(key);
        if(iter != m_data.end()) {
            wxDELETE(iter->second);
            m_data.erase(iter);
        }
    }

    /**
     * @brief return a string representing all the classes coloured by this editor
     */
    virtual const wxString& GetKeywordClasses() const = 0;
    /**
     * @brief return a string representing all the local variables coloured by this editor
     */
    virtual const wxString& GetKeywordLocals() const = 0;
    /**
     * @brief return a string representing all the methods coloured by this editor
     */
    virtual const wxString& GetKeywordMethods() const = 0;
    /**
     * @brief return a string representing all the other tokens
     */
    virtual const wxString& GetKeywordOthers() const = 0;

    /**
     * @brief get the options associated with this editor
     */
    virtual OptionsConfigPtr GetOptions() = 0;

    /**
     * @brief apply editor configuration (TAB vs SPACES, tab size, EOL mode etc)
     */
    virtual void ApplyEditorConfig() = 0;

    /**
     * @brief return list of bookmarks for a given editor
     * @param editor the editor
     * @param bookmarksVector output, contains pairs of: LINE:SNIPPET
     * @return number of bookmarks found
     */
    virtual size_t GetFindMarkers(std::vector<std::pair<int, wxString>>& bookmarksVector) = 0;

    /**
     * @brief incase this editor represents a remote file, return its remote path
     */
    virtual wxString GetRemotePath() const = 0;

    /**
     * @brief if this editor represents a remote file
     * return its remote path, otherwise return the local path
     * this is equal for writing:
     *
     * ```
     * wxString fullpath = editor->IsRemoteFile() ? editor->GetRemotePath() : editor->GetFileName().GetFullPath();
     * return fullpath;
     * ```
     */
    virtual wxString GetRemotePathOrLocal() const = 0;

    /**
     * @brief return true if this file represents a remote file
     */
    virtual bool IsRemoteFile() const = 0;

    /**
     * @brief return a pointer to the remote data
     * @return remote file info, or null if this file is not a remote a file
     */
    virtual SFTPClientData* GetRemoteData() const = 0;

    /**
     * @brief set semantic tokens for this editor
     */
    virtual void SetSemanticTokens(const wxString& classes, const wxString& variables, const wxString& methods,
                                   const wxString& others) = 0;

    /**
     * @brief similar to wxStyledTextCtrl::GetColumn(), but treat TAB as a single char
     * width
     */
    virtual int GetColumnInChars(int pos) = 0;

    /**
     * @brief highlight a given line in the editor
     */
    virtual void HighlightLine(int lineno) = 0;

    /**
     * @brief clear all highlights (added by `HighlightLine` calls)
     */
    virtual void UnHighlightAll() = 0;
};

#endif // IEDITOR_H
