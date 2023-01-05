//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cl_editor.h
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
#ifndef LITEEDITOR_EDITOR_H
#define LITEEDITOR_EDITOR_H

#include "LSP/CompletionItem.h"
#include "SFTPClientData.hpp"
#include "bookmark_manager.h"
#include "browse_record.h"
#include "clEditorStateLocker.h"
#include "cl_calltip.h"
#include "cl_defs.h"
#include "cl_unredo.h"
#include "context_base.h"
#include "debuggermanager.h"
#include "entry.h"
#include "findreplacedlg.h"
#include "globals.h"
#include "navigationmanager.h"
#include "plugin.h"
#include "stringhighlighterjob.h"
#include "wx/filename.h"
#include "wx/menu.h"

#include <map>
#include <stack>
#include <vector>
#include <wx/bitmap.h>
#include <wx/cmndata.h>
#include <wx/stc/stc.h>

#define DEBUGGER_INDICATOR 11
#define MATCH_INDICATOR 10
#define MARKER_WORD_HIGHLIGHT 2
#define USER_INDICATOR 3
#define HYPERLINK_INDICATOR 4
#define MARKER_FIND_BAR_WORD_HIGHLIGHT 5
#define MARKER_CONTEXT_WORD_HIGHLIGHT 6

#if(wxVERSION_NUMBER < 3101)
// Some wxSTC keycodes names were altered in 311, & the old versions deprecated
// So, to avoid deprecation-warning spam, #define for older versions
#define wxSTC_KEYMOD_NORM wxSTC_SCMOD_NORM
#define wxSTC_KEYMOD_SHIFT wxSTC_SCMOD_SHIFT
#define wxSTC_KEYMOD_CTRL wxSTC_SCMOD_CTRL
#define wxSTC_KEYMOD_ALT wxSTC_SCMOD_ALT
#define wxSTC_KEYMOD_SUPER wxSTC_SCMOD_SUPER
#define wxSTC_KEYMOD_META wxSTC_SCMOD_META
#endif

class wxRichToolTip;
class CCBoxTipWindow;
class IManager;
class wxFindReplaceDialog;
class CCBox;
class clEditorTipWindow;
class DisplayVariableDlg;
class EditorDeltasHolder;

enum sci_annotation_styles { eAnnotationStyleError = 128, eAnnotationStyleWarning };

/**************** NB: enum sci_marker_types has now moved to bookmark_manager.h ****************/

/**
 * @class BPtoMarker
 * Holds which marker and mask are associated with each breakpoint type
 */
typedef struct _BPtoMarker {
    enum BreakpointType bp_type; // An enum of possible break/watchpoint types. In debugger.h
    sci_marker_types marker;
    marker_mask_type mask;
    sci_marker_types marker_disabled;
    marker_mask_type mask_disabled;
} BPtoMarker;

wxDECLARE_EVENT(wxCMD_EVENT_REMOVE_MATCH_INDICATOR, wxCommandEvent);
wxDECLARE_EVENT(wxCMD_EVENT_ENABLE_WORD_HIGHLIGHT, wxCommandEvent);

/**
 * \ingroup LiteEditor
 * clEditor CodeLite editing component based on Scintilla
 * clEditor provides most of the C++/C editing capablities including:
 * -# Auto Completion
 * -# Find and replace
 * -# Bookmarks
 * -# Folding
 * -# Find definition of a symbol
 * and many many more
 *
 * \version 1.0
 * first version
 *
 * \date 04-21-2007
 *
 * \author Eran
 *
 */
class clEditor : public wxStyledTextCtrl, public IEditor
{
private:
    struct SelectionInfo {
        std::vector<std::pair<int, int>> selections;

        SelectionInfo() {}

        /**
         * @brief add selection range
         * @param from selection start position
         * @param to selectio end position
         */
        void AddSelection(int from, int to) { this->selections.push_back(std::make_pair(from, to)); }

        /**
         * @brief return the number of selections
         */
        size_t GetCount() const { return this->selections.size(); }

        /**
         * @brief return the selection at position i
         * @param from
         * @param to
         */
        void At(size_t i, int& from, int& to) const
        {
            const std::pair<int, int>& pair = this->selections.at(i);
            from = pair.first;
            to = pair.second;
        }

        /**
         * @brief clear the selection info
         */
        void Clear() { this->selections.clear(); }

        /**
         * @brief is this object OK?
         */
        bool IsOk() const { return !this->selections.empty(); }

        /**
         * @brief sort the selections from top to bottom
         */
        void Sort();
    };

    struct MarkWordInfo {
    private:
        bool m_hasMarkers;
        int m_firstOffset;
        wxString m_word;

    public:
        MarkWordInfo()
            : m_hasMarkers(false)
            , m_firstOffset(wxNOT_FOUND)
        {
        }

        void Clear()
        {
            m_hasMarkers = false;
            m_firstOffset = wxNOT_FOUND;
            m_word.Clear();
        }

        bool IsValid(wxStyledTextCtrl* ctrl) const
        {
            return ctrl->PositionFromLine(ctrl->GetFirstVisibleLine()) == m_firstOffset && m_hasMarkers;
        }

        // setters/getters
        void SetFirstOffset(int firstOffset) { this->m_firstOffset = firstOffset; }
        void SetHasMarkers(bool hasMarkers) { this->m_hasMarkers = hasMarkers; }
        void SetWord(const wxString& word) { this->m_word = word; }
        int GetFirstOffset() const { return m_firstOffset; }
        bool IsHasMarkers() const { return m_hasMarkers; }
        const wxString& GetWord() const { return m_word; }
    };

    enum eStatusBarFields {
        kShowLine = (1 << 0),
        kShowColumn = (1 << 1),
        kShowPosition = (1 << 2),
        kShowLen = (1 << 3),
        kShowSelectedChars = (1 << 4),
    };

    enum eLineStatus : short {
        LINE_NONE,
        LINE_MODIFIED,
        LINE_SAVED,
    };

protected:
    wxFileName m_fileName;
    wxString m_project;
    wxStopWatch m_watch;
    ContextBasePtr m_context;
    EditorDeltasHolder* m_deltas; // Holds any text position changes, in case they affect FindinFiles results
    std::vector<wxMenuItem*> m_dynItems;
    std::vector<BPtoMarker> m_BPstoMarkers;
    static FindReplaceDialog* m_findReplaceDlg;
    static FindReplaceData m_findReplaceData;
    int m_lastMatchPos;
    static std::map<wxString, int> ms_bookmarkShapes;
    bool m_popupIsOn;
    bool m_isDragging;
    time_t m_modifyTime;
    wxUint64 m_modificationCount;
    std::map<int, wxString> m_customCmds;
    bool m_isVisible;
    int m_hyperLinkIndicatroStart;
    int m_hyperLinkIndicatroEnd;
    bool m_hightlightMatchedBraces;
    bool m_autoAddMatchedCurlyBrace;
    bool m_autoAddNormalBraces;
    bool m_smartParen = true;
    std::map<int, std::vector<clDebuggerBreakpoint>> m_breakpointsInfo;
    bool m_autoAdjustHScrollbarWidth;
    bool m_reloadingFile;
    bool m_disableSmartIndent;
    bool m_disableSemicolonShift;
    clEditorTipWindow* m_functionTip;
    CCBoxTipWindow* m_calltip;
    wxChar m_lastCharEntered;
    int m_lastCharEnteredPos;
    bool m_isFocused;
    bool m_pluginInitializedRMenu;
    BOM m_fileBom;
    int m_positionToEnsureVisible;
    bool m_preserveSelection;
    std::vector<std::pair<int, int>> m_savedMarkers;
    bool m_findBookmarksActive;
    std::map<int, wxString> m_compilerMessagesMap;
    CLCommandProcessor m_commandsProcessor;
    wxString m_preProcessorsWords;
    SelectionInfo m_prevSelectionInfo;
    MarkWordInfo m_highlightedWordInfo;
    wxTimer* m_timerHighlightMarkers;
    IManager* m_mgr;
    OptionsConfigPtr m_options;
    bool m_hasCCAnnotation;
    wxRichToolTip* m_richTooltip;
    wxString m_keywordClasses;
    wxString m_keywordMethods;
    wxString m_keywordOthers;
    wxString m_keywordLocals;
    int m_editorBitmap = wxNOT_FOUND;
    size_t m_statusBarFields;
    int m_lastBeginLine = wxNOT_FOUND;
    int m_lastLine = wxNOT_FOUND;
    int m_lastEndLine;
    int m_lastLineCount;
    wxColour m_selTextColour;
    wxColour m_selTextBgColour;
    bool m_zoomProgrammatically = false;
    std::vector<eLineStatus> m_modifiedLines;
    bool m_trackChanges = false;
    std::unordered_map<int, wxString> m_breakpoints_tooltips;
    size_t m_default_text_width = wxNOT_FOUND;
    bool m_scrollbar_recalc_is_required = false;

public:
    static bool m_ccShowPrivateMembers;
    static bool m_ccShowItemsComments;
    static bool m_ccInitialized;

    typedef std::vector<clEditor*> Vec_t;

    IManager* GetManager() { return m_mgr; }

    /**
     * @brief CodeLite preferences updated
     */
    void PreferencesChanged();
    /**
     * @brief are the CC annotations visible?
     */
    bool IsHasCCAnnotation() const { return m_hasCCAnnotation; }
    void ClearCCAnnotations();

    void SetEditorBitmap(int editorBitmap) { this->m_editorBitmap = editorBitmap; }
    int GetEditorBitmap() const { return m_editorBitmap; }

    void NotifyMarkerChanged(int lineNumber = wxNOT_FOUND);

    /**
     * @brief static version of the below `CenterLine`
     * so we can use it outside of this class, e.g. from `QuickFindBar`
     */
    static void CenterLine(wxStyledTextCtrl* ctrl, int line, int col);

    /**
     * @brief static version
     */
    static void CenterLinePreserveSelection(wxStyledTextCtrl* ctrl, int line);

    /**
     * @brief static version of the below
     */
    static void SetCaretAt(wxStyledTextCtrl* ctrl, long pos);

    /**
     * @brief wrapper calling CenterLinePreserveSelection using CallAfter()
     */
    void CenterLinePreserveSelectionAfter(int line);

public:
    static FindReplaceData& GetFindReplaceData() { return m_findReplaceData; }

    void SetPreProcessorsWords(const wxString& preProcessorsWords) { this->m_preProcessorsWords = preProcessorsWords; }
    const wxString& GetPreProcessorsWords() const { return m_preProcessorsWords; }
    void SetLineVisible(int lineno);

    void SetReloadingFile(const bool& reloadingFile) { this->m_reloadingFile = reloadingFile; }
    const bool& GetReloadingFile() const { return m_reloadingFile; }

    clEditorTipWindow* GetFunctionTip() { return m_functionTip; }

    bool IsFocused() const;
    CLCommandProcessor& GetCommandsProcessor() { return m_commandsProcessor; }

public:
    /// Construct a clEditor object
    clEditor(wxWindow* parent);

    /// Default destructor
    virtual ~clEditor();

    // Save the editor data into file
    virtual bool SaveFile();

    // Save content of the editor to a given file (Save As...)
    // this function prompts the user for selecting file name
    bool SaveFileAs(const wxString& newname = wxEmptyString, const wxString& savePath = wxEmptyString);

    /**
     * @brief print the editor content using the printing framework
     */
    void Print();

    /**
     * @brief setup the print page
     */
    void PageSetup();

    const wxString& GetKeywordClasses() const override { return m_keywordClasses; }
    const wxString& GetKeywordLocals() const override { return m_keywordLocals; }
    const wxString& GetKeywordMethods() const override { return m_keywordMethods; }
    const wxString& GetKeywordOthers() const override { return m_keywordOthers; }

    // used by other plugins
    void SetKeywordClasses(const wxString& keywords) { this->m_keywordClasses = keywords; }
    void SetKeywordLocals(const wxString& keywords) { this->m_keywordLocals = keywords; }
    void SetKeywordMethods(const wxString& keywords) { this->m_keywordMethods = keywords; }
    void SetKeywordOthers(const wxString& keywords) { this->m_keywordOthers = keywords; }

    /**
     * @brief set semantic tokens for this editor
     */
    void SetSemanticTokens(const wxString& classes, const wxString& variables, const wxString& methods,
                           const wxString& others) override;

    /**
     * @brief split the current selection into multiple carets.
     * i.e. place a caret at the end of each line in the selection
     */
    void SplitSelection();

    void SetDisableSmartIndent(bool disableSmartIndent) { this->m_disableSmartIndent = disableSmartIndent; }

    bool GetDisableSmartIndent() const { return m_disableSmartIndent; }
    /**
     * @brief set the EOL mode of the file by applying this logic:
     * - if the file has content, use the current cotext EOL
     * - if the file is empty and the EOL mode is set to Default, make it EOL of the hosting OS
     * - Use the setting provided by the user
     */
    void SetEOL();

    void CompleteWord(LSP::CompletionItem::eTriggerKind triggerKind, bool onlyRefresh = false);

    /**
     * @brief chage the case of the current selection. If selection is empty,
     * change the selection of the character to the right of the cart. In case of changing
     * the char to the right, move the caret to the right as well.
     * @param toLower change to lower case.
     */
    void ChangeCase(bool toLower);

    // set this editor file name
    void SetFileName(const wxFileName& name) { m_fileName = name; }

    // Return the project name
    const wxString& GetProject() const { return m_project; }
    // Set the project name
    void SetProject(const wxString& proj) { m_project = proj; }

    /**
     * @brief attempt to code complete the expression up until the caret position
     * @param refreshingList when set to true, it means that the 'CodeComplete' was invoked
     * by the code completion box itself in attempt to request new items for the list
     * This feature is only supported internally for C++ and is not exposed to plugins
     * i.e. the event wxEVT_CC_CODE_COMPLETE is fired only when refreshingList == false
     */
    void CodeComplete(bool refreshingList = false);

    /**
     * @brief toggle line comment
     * @param commentSymbol the comment symbol to insert (e.g. "//")
     * @param commentStyle the wxSTC line comment style
     */
    void ToggleLineComment(const wxString& commentSymbol, int commentStyle) override;

    /**
     * @brief block comment the selection
     */
    void CommentBlockSelection(const wxString& commentBlockStart, const wxString& commentBlockEnd) override;

    /**
     * @brief is this editor modified?
     */
    bool IsEditorModified() const override { return GetModify(); }

    // User clicked Ctrl+.
    void GotoDefinition();
    /**
     * find declaration file for the current expression
     */
    void FindDeclarationFile();

    // return the EOL according to the content
    int GetEOLByContent();

    int GetEOLByOS();
    /**
     * Return true if editor definition contains more
     * on its stack
     */
    bool CanGotoPreviousDefintion() { return NavMgr::Get()->CanPrev(); }

    // Callback function for UI events
    void OnUpdateUI(wxUpdateUIEvent& event);

    // Callback function for menu command events
    void OnMenuCommand(wxCommandEvent& event);

    // try to match a brace from the current caret pos and select the region
    void MatchBraceAndSelect(bool selRegion);

    // Popup a find/replace dialog
    void DoFindAndReplace(bool isReplaceDlg);

    // set this page as active, this usually happened when user changed the notebook
    // page to this one
    void SetActive() override;

    // Ditto, but asynchronously
    void DelayedSetActive() override;

    // Perform FindNext operation based on the data stored in the FindReplaceData class
    void FindNext(const FindReplaceData& data);

    /**
     * @brief display functions' calltip from the current position of the caret
     */
    void ShowFunctionTipFromCurrentPos();

    /**
     * Change the document's syntax highlight
     * @param lexerName the syntax highlight's lexer name (as appear in the liteeditor.xml file)
     */
    void SetSyntaxHighlight(const wxString& lexerName) override;

    /**
     * Change the document's syntax highlight - use the file name to determine lexer name
     */
    virtual void SetSyntaxHighlight(bool bUpdateColors = true);

    /**
     * Return the document context object
     */
    ContextBasePtr GetContext() const { return m_context; }

    /**
     * If word-wrap isn't on, and forceDelay is false, this calls DoEnsureCaretIsVisible() immediately. Otherwise it
     * stores a position for OnScnPainted() to ensure-is-visible in the next scintilla paint event
     * This doesn't happen until scintilla painting is complete, so it isn't ruined by e.g. word-wrap
     * @param position the position to ensure is visible
     * @param preserveSelection preserve any selection
     * @param forceDelay wait for the next paint event even if word-wrap is off
     */
    void SetEnsureCaretIsVisible(int pos, bool preserveSelection = true, bool forceDelay = false);

    /**
     * Does the necessary things to ensure that the destination line of a GoTo is visible
     * @param position the position to ensure is visible
     * @param preserveSelection cache and reapply any selection, which would otherwise be cleared by scintilla
     */
    void DoEnsureCaretIsVisible(int pos, bool preserveSelection);

    // Bookmark API
    //-----------------------------------------
    /**
     * @brief return true if this editor has at least one compiler
     * marker (warning or error)
     */
    bool HasCompilerMarkers();

    /**
     * @brief center the line in the editor
     */
    void CenterLine(int line, int col = wxNOT_FOUND) override;

    /**
     * @brief center the editor at a given line, but preserve the selection
     */
    void CenterLinePreserveSelection(int line) override;

    /**
     * @brief Center line if needed (i.e. only if the line is not visible)
     */
    void CenterLineIfNeeded(int line, bool force = false);

    /**
     * @brief convert the editor indentation to spaces
     */
    void ConvertIndentToSpaces();

    /**
     * @brief convert the editor indentation to tabs
     */
    void ConvertIndentToTabs();

    // Is there currently a marker at the current line?
    bool LineIsMarked(enum marker_mask_type mask);
    // Toggle marker at the current line
    void ToggleMarker();

    /**
     * Delete markers from the current document
     * @param which_type if >0, delete the matching bookmark type; 0 delete the currently-active type; -1 delete all
     * types
     */
    void DelAllMarkers(int which_type);

    // Find next marker and move cursor to that line
    void FindNextMarker();
    // Find previous marker and move cursor to that line
    void FindPrevMarker();

    /**
     * @brief return list of bookmarks (active type)
     */
    size_t GetFindMarkers(std::vector<std::pair<int, wxString>>& bookmarksVector) override;

    /**
     * @brief similar to wxStyledTextCtrl::GetColumn(), but treat TAB as a single char
     * width
     */
    int GetColumnInChars(int pos) override;

    /**
     * Sets whether the currently-active bookmark level is Find', or the current standard-bookmark type
     */
    void SetFindBookmarksActive(bool findBookmarksActive) { m_findBookmarksActive = findBookmarksActive; }
    /**
     * Returns true if the currently-active bookmark level is 'Find'
     */
    bool IsFindBookmarksActive() const { return m_findBookmarksActive; }

    /**
     * The user is changing the currently-active bookmark type
     */
    void OnChangeActiveBookmarkType(wxCommandEvent& event);

    /**
     * Returns the currently-active bookmark level
     */
    int GetActiveBookmarkType() const;

    /**
     * Returns the mask for the currently-active bookmark level
     */
    enum marker_mask_type GetActiveBookmarkMask() const;

    /**
     * Returns the label for the passed bookmark type, or its type as a string
     */
    static wxString GetBookmarkLabel(sci_marker_types type);

    /**
     * Returns a tooltip for the most significant bookmark on the passed line
     */
    void GetBookmarkTooltip(int lineno, wxString& tip, wxString& title);

    // Replace all
    bool ReplaceAll();
    bool ReplaceAllExactMatch(const wxString& what, const wxString& replaceWith);
    // mark all occurances
    bool MarkAllFinds();

    // Folding API
    //-----------------------------------------
    void ToggleCurrentFold();
    void FoldAll();
    /**
     * Toggles *all* folds within the selection, not just the outer one of each function
     */
    void ToggleAllFoldsInSelection();
    void DoRecursivelyExpandFolds(bool expand, int startline,
                                  int endline); // Helper function for ToggleAllFoldsInSelection()
                                                /**
                                                 *  Find the topmost fold level within the selection, and toggle all selected folds of that level
                                                 */
    void ToggleTopmostFoldsInSelection();
    /**
     * Load collapsed folds from a vector
     */
    void LoadCollapsedFoldsFromArray(const clEditorStateLocker::VecInt_t& folds);
    /**
     * Store any collapsed folds to a vector, so they can be serialised
     */
    void StoreCollapsedFoldsToArray(clEditorStateLocker::VecInt_t& folds) const;

    static FindReplaceDialog* GetFindReplaceDialog() { return m_findReplaceDlg; }

    // Util function
    int SafeGetChar(int pos);
    wxString PreviousWord(int pos, int& foundPos);
    wxChar NextChar(const int& pos, int& foundPos);

    int FindString(const wxString& str, int flags, const bool down, long pos);

    /**
     * @brief find the current selection and select without removing the current selection
     */
    void QuickAddNext();

    /**
     * @brief find all occurances of the selected text and select
     */
    void QuickFindAll();

    bool FindAndSelect();
    bool SelectRange(const LSP::Range& range) override;
    bool SelectLocation(const LSP::Location& range) override;
    bool FindAndSelect(const FindReplaceData& data);
    bool FindAndSelect(const wxString& pattern, const wxString& name);
    void FindAndSelectV(const wxString& pattern, const wxString& name, int pos = 0,
                        NavMgr* unused = NULL) override; // The same but returns void, so usable with CallAfter()
    void DoFindAndSelectV(const wxArrayString& strings, int pos); // Called with CallAfter()

    bool Replace();
    bool Replace(const FindReplaceData& data);

    void RecalcHorizontalScrollbar();

    /**
     * Get editor options. Takes any workspace/project overrides into account
     */
    virtual OptionsConfigPtr GetOptions() override { return m_options; }

    /**
     * Add marker to the current line
     */
    void AddMarker();

    /**
     * Delete a marker from the current line
     */
    void DelMarker();

    /**
     * @brief return true if there is a breakpoint marker on the given line
     */
    bool HasBreakpointMarker(int line_number = wxNOT_FOUND) override;

    /**
     * @brief delete all breakpoint markers from the given line
     * @param line_number if set to wxNOT_FOUND(-1), delete all breakpoints from the editor
     */
    void DeleteBreakpointMarkers(int line_number = wxNOT_FOUND) override;

    /**
     * @brief return all lines that have breakpoint marker
     */
    size_t GetBreakpointMarkers(std::vector<int>* lines) override;

    /**
     * @brief delete all breakpoint markers from the given line
     */
    void SetBreakpointMarker(int line_number = wxNOT_FOUND, const wxString& tooltip = wxEmptyString) override;

    /**
     * Store all bookmarks in a wxArrayString
     */
    void StoreMarkersToArray(wxArrayString& bookmarks);

    /**
     * Load bookmarks from a wxArrayString
     */
    void LoadMarkersFromArray(const wxArrayString& bookmarks);

    /**
     * Attempt to match brace backward
     * @param chCloseBrace the closing brace character (can be one of: '}' ')' ']')
     * @param pos position to start the match
     * @param matchedPos output, the position of the matched brace
     * \return true on match false otherwise
     */
    bool MatchBraceBack(const wxChar& chCloseBrace, const long& pos, long& matchedPos);

    /**
     * Open file and clear the undo buffer
     */
    virtual void Create(const wxString& project, const wxFileName& fileName);

    /**
     * Insert text to the editor and keeping the page indentation
     * @param text text to enter
     * @param pos position to insert the text
     */
    void InsertTextWithIndentation(const wxString& text, int pos);

    BrowseRecord CreateBrowseRecord() override;

    bool IsContextMenuOn() const { return m_popupIsOn; }

    bool IsDragging() const { return m_isDragging; }

    /**
     * @brief return an approximation of the line height
     * @return line height
     */
    int GetCurrLineHeight();

    /**
     * toggle break point at the current line & file
     */
    void ToggleBreakpoint(int lineno = -1);

    /**
     * add a temporary or conditional break point at the current line & file
     */
    void AddOtherBreakpointType(wxCommandEvent& event);

    /**
     * Ignore the break point at the current line & file
     */
    void OnIgnoreBreakpoint();

    /**
     * Edit a breakpoint in the BreakptProperties dialog
     */
    void OnEditBreakpoint();

    /**
     * Add a breakpoint at the current line & file
     * Optionally make it temporary, disabled or conditional
     */
    void AddBreakpoint(int lineno = -1, const wxString& conditions = wxT(""), const bool is_temp = false,
                       const bool is_disabled = false);

    /**
     * Delete the breakpoint at the current line & file, or lineno if from ToggleBreakpoint()
     */
    void DelBreakpoint(int lineno = -1);

    /**
     * @brief change the breakpoint at the current line to disable or enable
     */
    void ToggleBreakpointEnablement();

    /**
     * @brief search the editor for pattern. If pattern is found, the editor will then search for 'what'
     * inside the pattern and will select it
     * @param pattern pattern to search in the editor
     * @param what    sub string of pattern to select
     * @param pos     start the search from 'pos'
     * @param navmgr  Navigation manager to place browsing recrods
     * @return return true if a match was found, false otherwise
     */
    bool FindAndSelect(const wxString& pattern, const wxString& what, int pos, NavMgr* navmgr) override;

    virtual void UpdateBreakpoints();

    //--------------------------------
    // breakpoint visualisation
    //--------------------------------
    virtual void SetBreakpointMarker(int lineno, BreakpointType bptype, bool is_disabled,
                                     const std::vector<clDebuggerBreakpoint>& li);
    virtual void DelAllBreakpointMarkers();

    void HighlightLine(int lineno) override;
    void UnHighlightAll() override;

    // compiler warnings and errors
    void SetWarningMarker(int lineno, const wxString& annotationText) override;
    void SetErrorMarker(int lineno, const wxString& annotationText) override;
    void DelAllCompilerMarkers() override;

    void DoShowCalltip(int pos, const wxString& title, const wxString& tip, bool strip_html_tags = true);
    /**
     * @brief adjust calltip window position to fit into the display screen
     */
    void DoAdjustCalltipPos(wxPoint& pt) const;
    void DoCancelCalltip();
    void DoCancelCodeCompletionBox();
    int DoGetOpenBracePos();

    //----------------------------------
    // File modifications
    //----------------------------------

    /**
     * return the last modification time (on disk) of editor's underlying file
     */
    time_t GetFileLastModifiedTime() const;

    /**
     * return/set the last modification time that was made by the editor
     */
    time_t GetEditorLastModifiedTime() const { return m_modifyTime; }
    void SetEditorLastModifiedTime(time_t modificationTime) { m_modifyTime = modificationTime; }

    /**
     * @brief Get the editor's modification count
     */
    wxUint64 GetModificationCount() const override { return m_modificationCount; }

    /**
     * @brief run through the file content and update colours for the
     * functions / locals
     */
    void UpdateColours();

    /**
     * @brief display completion box. This function also moves the completion box to the current position
     * @param tags list of tags to work with
     * @param word part of the word
     */
    void ShowCompletionBox(const std::vector<TagEntryPtr>& tags, const wxString& word);

    /**
     * @brief return true if the completion box is visible
     */
    bool IsCompletionBoxShown() override;

    /**
     * @brief highlight the word where the cursor is at
     * @param highlight
     */
    void HighlightWord(bool highlight = true);

    /**
     * @brief Trim trailing whitespace and/or ensure the file ends with a newline
     * @param trim trim trailing whitespace
     * @param appendLf append a newline to the file if none is present
     */
    void TrimText(bool trim, bool appendLf);

    /**
     *--------------------------------------------------
     * Implemetation for IEditor interace
     *--------------------------------------------------
     */
    wxStyledTextCtrl* GetCtrl() override { return static_cast<wxStyledTextCtrl*>(this); }

    /**
     * @brief set a code completion annotation at the given line. code completion
     * annotations are automatically cleared on the next char added
     * @param text
     * @param lineno
     */
    void SetCodeCompletionAnnotation(const wxString& text, int lineno) override;

    wxString GetEditorText() override { return GetText(); }
    size_t GetEditorTextRaw(std::string& text) override;
    void SetEditorText(const wxString& text) override;
    void OpenFile() override;
    void ReloadFromDisk(bool keepUndoHistory = false) override;
    void SetCaretAt(long pos) override;
    long GetCurrentPosition() override { return GetCurrentPos(); }
    const wxFileName& GetFileName() const override { return m_fileName; }
    const wxString& GetProjectName() const override { return m_project; }

    int GetPosAtMousePointer() override;

    /**
     * @brief
     * @return
     */
    wxString GetWordAtCaret(bool wordCharsOnly = true) override;
    /**
     * @brief
     * @return
     */
    void GetWordAtMousePointer(wxString& word, wxRect& wordRect) override;
    /**
     * @brief get word at a given position
     * @param pos word's position
     * @param wordCharsOnly when set to false, return the string between the nearest whitespaces
     */
    wxString GetWordAtPosition(int pos, bool wordCharsOnly = true) override;
    /**
     * @brief
     * @param text
     */
    void AppendText(const wxString& text) override { wxStyledTextCtrl::AppendText(text); }
    void InsertText(int pos, const wxString& text) override { wxStyledTextCtrl::InsertText(pos, text); }
    int GetLength() override { return wxStyledTextCtrl::GetLength(); }

    bool Save() override { return SaveFile(); }
    bool SaveAs(const wxString& defaultName = wxEmptyString, const wxString& savePath = wxEmptyString) override
    {
        return SaveFileAs(defaultName, savePath);
    }

    int GetEOL() override { return wxStyledTextCtrl::GetEOLMode(); }
    int GetCurrentLine() override;
    void ReplaceSelection(const wxString& text) override;
    wxString GetSelection() override;
    int GetSelectionStart() override;
    int GetSelectionEnd() override;
    void SelectText(int startPos, int len) override;

    void SetLexerName(const wxString& lexerName) override;

    // User Indicators API
    void SetUserIndicatorStyleAndColour(int style, const wxColour& colour) override;
    void SetUserIndicator(int startPos, int len) override;
    void ClearUserIndicators() override;
    int GetUserIndicatorStart(int pos) override;
    int GetUserIndicatorEnd(int pos) override;
    int GetLexerId() override;
    int GetStyleAtPos(int pos) override;

    /**
     * @brief Get position of start of word.
     * @param pos from position
     * @param onlyWordCharacters
     */
    int WordStartPos(int pos, bool onlyWordCharacters) override;

    /**
     * @brief  Get position of end of word.
     * @param pos from position
     * @param onlyWordCharacters
     */
    int WordEndPos(int pos, bool onlyWordCharacters) override;

    /**
     * Prepend the indentation level found at line at 'pos' to each line in the input string
     * @param text text to enter
     * @param pos position to insert the text
     * @param flags formatting flags
     */
    wxString FormatTextKeepIndent(const wxString& text, int pos, size_t flags = 0) override;

    /**
     * @brief return the line numebr containing 'pos'
     * @param pos the position
     */
    int LineFromPos(int pos) override;

    /**
     * @brief return the start pos of line nummber
     * @param line the line number
     * @return line nummber or 0 if the document is empty
     */
    int PosFromLine(int line) override;

    /**
     * @brief return the END position of 'line'
     * @param line the line number
     */
    int LineEnd(int line) override;

    /**
     * @brief return text from a given pos -> endPos
     * @param startPos
     * @param endPos
     * @return
     */
    wxString GetTextRange(int startPos, int endPos) override;

    /**
     * @brief display a calltip at the current position
     */
    void ShowCalltip(clCallTipPtr tip) override;

    wxChar PreviousChar(const int& pos, int& foundPos, bool wantWhitespace = false) override;
    int PositionAfterPos(int pos) override;
    int PositionBeforePos(int pos) override;
    int GetCharAtPos(int pos) override;

    /**
     * @brief apply editor configuration (TAB vs SPACES, tab size, EOL mode etc)
     */
    void ApplyEditorConfig() override;

    /**
     * @brief return true if the current editor is detached from the mainbook
     */
    bool IsDetached() const;

    /**
     * @brief display a tooltip (title + tip)
     * @param tip tip text
     * @param pos position for the tip. If wxNOT_FOUND the tip is positioned at mouse cursor position
     */
    void ShowTooltip(const wxString& tip, const wxString& title = wxEmptyString, int pos = wxNOT_FOUND) override;

    /**
     * @brief display a rich tooltip (title + tip)
     * @param tip tip text
     * @param pos position for the tip. If wxNOT_FOUND the tip is positioned at mouse cursor position
     */
    void ShowRichTooltip(const wxString& tip, const wxString& title, int pos = wxNOT_FOUND) override;

    /**
     * @brief return the first selection (in case there are multiple selections enabled)
     */
    wxString GetFirstSelection();

    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------

    void SetIsVisible(const bool& isVisible) { this->m_isVisible = isVisible; }
    const bool& GetIsVisible() const { return m_isVisible; }

    wxString GetEolString();
    void HighlightWord(StringHighlightOutput* highlightOutput);

    /**
     * Get a vector of relevant position changes. Used for 'GoTo next/previous FindInFiles match'
     */
    void GetChanges(std::vector<int>& changes);

    /**
     * Tells the EditorDeltasHolder that there's been (another) FindInFiles call
     */
    void OnFindInFiles();

    /**
     * @brief paste the clipboard content one line above the caret position
     */
    void PasteLineAbove();

    /**
     * @brief update editor options based on the global + workspace settings
     */
    void UpdateOptions();

    /**
     * @brief incase this editor represents a remote file, return its remote path
     */
    wxString GetRemotePath() const override;
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
    wxString GetRemotePathOrLocal() const override;

    /**
     * @brief return true if this file represents a remote file
     */
    bool IsRemoteFile() const override;

    /**
     * @brief return a pointer to the remote data
     * @return remote file info, or null if this file is not a remote a file
     */
    SFTPClientData* GetRemoteData() const override;

private:
    void UpdateLineNumberMarginWidth();
    void DoUpdateTLWTitle(bool raise);
    void DoWrapPrevSelectionWithChars(wxChar first, wxChar last);
    int GetFirstSingleLineCommentPos(int from, int commentStyle);
    void DoSelectRange(const LSP::Range& range);

    /**
     * @brief set the zoom factor
     */
    void SetZoomFactor(int zoom_factor);

    /**
     * @brief return number of whitespace characters in the beginning of the line
     */
    int GetNumberFirstSpacesInLine(int line);

    void FillBPtoMarkerArray();
    BPtoMarker GetMarkerForBreakpt(enum BreakpointType bp_type);
    void SetProperties();
    void DefineMarker(int marker, int markerType, wxColor fore, wxColor back);
    bool SaveToFile(const wxFileName& fileName);
    void BraceMatch(const bool& bSelRegion);
    void BraceMatch(long pos);
    void DoHighlightWord();
    bool IsOpenBrace(int position);
    bool IsCloseBrace(int position);
    size_t GetCodeNavModifier();
    // Conevert FindReplaceDialog flags to wxSD flags
    size_t SearchFlags(const FindReplaceData& data);

    void AddDebuggerContextMenu(wxMenu* menu);
    void RemoveDebuggerContextMenu(wxMenu* menu);
    void DoBreakptContextMenu(wxPoint clientPt);
    void DoMarkHyperlink(wxMouseEvent& event, bool isMiddle);
    void DoQuickJump(wxMouseEvent& event, bool isMiddle);
    bool DoFindAndSelect(const wxString& pattern, const wxString& what, int start_pos, NavMgr* navmgr);
    void DoSaveMarkers();
    void DoRestoreMarkers();
    int GetFirstNonWhitespacePos(bool backward = false);
    wxMenu* DoCreateDebuggerWatchMenu(const wxString& word);

    wxFontEncoding DetectEncoding(const wxString& filename);
    void DoToggleFold(int line, const wxString& textTag);

    // Line numbers drawings
    void DoUpdateLineNumbers(bool relative_numbers);
    void UpdateLineNumbers();
    void UpdateDefaultTextWidth();

    // Event handlers
    void OnIdle(wxIdleEvent& event);
    void OpenURL(wxCommandEvent& event);
    void OnHighlightWordChecked(wxCommandEvent& e);
    void OnRemoveMatchInidicator(wxCommandEvent& e);
    void OnSavePoint(wxStyledTextEvent& event);
    void OnCharAdded(wxStyledTextEvent& event);
    void OnMarginClick(wxStyledTextEvent& event);
    void OnChange(wxStyledTextEvent& event);
    void OnZoom(wxStyledTextEvent& event);
    void OnDwellStart(wxStyledTextEvent& event);
    void OnDwellEnd(wxStyledTextEvent& event);
    void OnCallTipClick(wxStyledTextEvent& event);
    void OnScnPainted(wxStyledTextEvent& event);
    void OnSciUpdateUI(wxStyledTextEvent& event);
    void OnFindDialog(wxCommandEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnRightDown(wxMouseEvent& event);
    void OnMotion(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void OnLeaveWindow(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnFocusLost(wxFocusEvent& event);
    void OnFocus(wxFocusEvent& event);
    void OnLeftDClick(wxStyledTextEvent& event);
    void OnPopupMenuUpdateUI(wxUpdateUIEvent& event);
    void OnDbgAddWatch(wxCommandEvent& event);
    void OnDbgCustomWatch(wxCommandEvent& event);
    void OnDragStart(wxStyledTextEvent& e);
    void OnDragEnd(wxStyledTextEvent& e);
    void DoSetCaretAt(long pos);
    static void DoSetCaretAt(wxStyledTextCtrl* ctrl, long pos);
    void OnFileFormatDone(wxCommandEvent& e);
    void OnFileFormatStarting(wxCommandEvent& e);
    void OnTimer(wxTimerEvent& event);
    void OnEditorConfigChanged(wxCommandEvent& event);
    void OnColoursAndFontsUpdated(clCommandEvent& event);
};

#endif // LITEEDITOR_EDITOR_H
