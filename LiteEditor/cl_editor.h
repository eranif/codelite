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

#include <wx/wxscintilla.h>
#include <stack>
#include <vector>
#include <map>
#include "entry.h"
#include "stringhighlighterjob.h"
#include "cl_calltip.h"
#include "wx/filename.h"
#include "findreplacedlg.h"
#include "context_base.h"
#include "wx/menu.h"
#include "browse_record.h"
#include "navigationmanager.h"
#include "debuggermanager.h"
#include "breakpointsmgr.h"
#include "plugin.h"

#define DEBUGGER_INDICATOR          11
#define MATCH_INDICATOR             10
#define USER_INDICATOR              3
#define HYPERLINK_INDICATOR         4

class wxFindReplaceDialog;
class CCBox;
class clEditorTipWindow;
class DisplayVariableDlg;

enum sci_annotation_styles {
	eAnnotationStyleError = 128, eAnnotationStyleWarning
};

// NB The following are sci markers, which are zero based. So smt_bookmark is actually the eighth of them (important when masking it!)
// If you add another type here, watch out for smt_LAST_BP_TYPE; and you need also to add to the enum 'marker_mask_type' below
// The higher the value, the nearer the top of the pecking order displaywise. So keep the most important breakpoint at the top i.e. smt_breakpoint,
// but have smt_breakpointsmt_indicator above it, so you can see the indicator when there's a breakpt too
enum sci_marker_types { smt_bookmark=7, smt_FIRST_BP_TYPE=8, smt_cond_bp_disabled = smt_FIRST_BP_TYPE, smt_bp_cmdlist_disabled, smt_bp_disabled,
                        smt_bp_ignored, smt_cond_bp, smt_bp_cmdlist, smt_breakpoint, smt_LAST_BP_TYPE = smt_breakpoint, smt_indicator, smt_warning, smt_error
                      };

// These are bitmap masks of the various margin markers.
// So 256 == 0x100 == 100000000, 2^9, and masks the ninth marker, smt_cond_bp_disabled==8 (as the markers are zero-based)
// 0x7f00 is binary 111111100000000 and masks all the 7 current breakpoint types. If you add others, change it
enum marker_mask_type { mmt_folds=wxSCI_MASK_FOLDERS, mmt_bookmarks=128, mmt_FIRST_BP_TYPE=0x100, mmt_cond_bp_disabled=mmt_FIRST_BP_TYPE, mmt_bp_cmdlist_disabled=0x200, mmt_bp_disabled=0x400,
                        mmt_bp_ignored=0x800,  mmt_cond_bp=0x1000,mmt_bp_cmdlist=0x2000, mmt_breakpoint=0x4000, mmt_LAST_BP_TYPE=mmt_breakpoint,  mmt_all_breakpoints=0x7f00,   mmt_indicator=0x8000,
                        mmt_compiler=0x30000 /* masks compiler errors/warnings */
                      };

enum calltip_type { ct_function_hover, ct_debugger, ct_function_proto, ct_breakpoint, ct_compiler_msg, ct_none};

/**
* @class BPtoMarker
* Holds which marker and mask are associated with each breakpoint type
*/
typedef struct _BPtoMarker {
	enum BreakpointType bp_type;	// An enum of possible break/watchpoint types. In debugger.h
	sci_marker_types marker;
	marker_mask_type mask;
	sci_marker_types marker_disabled;
	marker_mask_type mask_disabled;
} BPtoMarker;

extern const wxEventType wxCMD_EVENT_REMOVE_MATCH_INDICATOR;
/**
 * \ingroup LiteEditor
 * LEditor CodeLite editing component based on Scintilla
 * LEditor provides most of the C++/C editing capablities including:
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
class LEditor : public wxScintilla, public IEditor
{
	wxFileName                                  m_fileName;
	wxString                                    m_project;
	wxStopWatch                                 m_watch;
	ContextBasePtr                              m_context;
	wxMenu *                                    m_rightClickMenu;
	std::vector<wxMenuItem*>                    m_dynItems;
	std::vector<BPtoMarker>                     m_BPstoMarkers;
	static FindReplaceDialog *                  m_findReplaceDlg;
	static FindReplaceData                      m_findReplaceData;
	int                                         m_lastMatchPos;
	static std::map<wxString, int>              ms_bookmarkShapes;
	bool                                        m_popupIsOn;
	time_t                                      m_modifyTime;
	std::map<int, wxString>                     m_customCmds;
	CCBox *                                     m_ccBox;
	bool                                        m_isVisible;
	int                                         m_hyperLinkIndicatroStart;
	int                                         m_hyperLinkIndicatroEnd;
	int                                         m_hyperLinkType;
	bool                                        m_hightlightMatchedBraces;
	bool                                        m_autoAddMatchedBrace;
	std::map<int, std::vector<BreakpointInfo> > m_breakpointsInfo;
	bool                                        m_autoAdjustHScrollbarWidth;
	calltip_type                                m_calltipType;
	bool                                        m_reloadingFile;
	bool                                        m_disableSmartIndent;
	bool                                        m_disableSemicolonShift;
	clEditorTipWindow*                          m_functionTip;
	wxChar                                      m_lastCharEntered;
	int                                         m_lastCharEnteredPos;

public:
	static bool                                 m_ccShowPrivateMembers;
	static bool                                 m_ccShowItemsComments;
	static bool                                 m_ccInitialized;

public:
	static FindReplaceData &GetFindReplaceData() {
		return m_findReplaceData;
	}

	void SetReloadingFile(const bool& reloadingFile) {
		this->m_reloadingFile = reloadingFile;
	}
	const bool& GetReloadingFile() const {
		return m_reloadingFile;
	}

	clEditorTipWindow* GetFunctionTip() {
		return m_functionTip;
	}

public:
	/// Construct a LEditor object
	LEditor(wxWindow* parent);

	/// Default destructor
	virtual ~LEditor();

	// Save the editor data into file
	virtual bool SaveFile();

	// Save content of the editor to a given file (Save As...)
	// this function prompts the user for selecting file name
	bool SaveFileAs();

	void SetDisableSmartIndent(bool disableSmartIndent) {
		this->m_disableSmartIndent = disableSmartIndent;
	}

	bool GetDisableSmartIndent() const {
		return m_disableSmartIndent;
	}
	/**
	 * @brief set the EOL mode of the file by applying this logic:
	 * - if the file has content, use the current cotext EOL
	 * - if the file is empty and the EOL mode is set to Default, make it EOL of the hosting OS
	 * - Use the setting provided by the user
	 */
	void SetEOL();

	void CompleteWord();

	/**
	 * \brief chage the case of the current selection. If selection is empty,
	 * change the selection of the character to the right of the cart. In case of changing
	 * the char to the right, move the caret to the right as well.
	 * \param toLower change to lower case.
	 */
	void ChangeCase(bool toLower);

	// set this editor file name
	void SetFileName(const wxFileName &name) {
		m_fileName = name;
	}

	// Return the project name
	const wxString &GetProject() const {
		return m_project;
	}
	// Set the project name
	void SetProject(const wxString &proj) {
		m_project = proj;
	}

	// Attempt to display a list of members
	// after a '.' or '->' operator has been inserted into
	// the code
	void CodeComplete();

	// User clicked Ctrl+.
	void GotoDefinition();

	// User clicked Ctrl+,
	void GotoPreviousDefintion();

	// return the EOL according to the content
	int GetEOLByContent();

	int GetEOLByOS();
	/**
	 * Return true if editor definition contains more
	 * on its stack
	 */
	bool CanGotoPreviousDefintion() {
		return NavMgr::Get()->CanPrev();
	}

	// Callback function for UI events
	void OnUpdateUI(wxUpdateUIEvent &event);

	// Callback function for menu command events
	void OnMenuCommand(wxCommandEvent &event);

	// try to match a brace from the current caret pos and select the region
	void MatchBraceAndSelect(bool selRegion);

	// Popup a find/replace dialog
	void DoFindAndReplace(bool isReplaceDlg);

	// set this page as active, this usually happened when user changed the notebook
	// page to this one
	virtual void SetActive();

	// Perform FindNext operation based on the data stored in the FindReplaceData class
	void FindNext(const FindReplaceData &data);

	/**
	 * \brief diaply functions' calltip from the current position of the caret
	 */
	void ShowFunctionTipFromCurrentPos();

	/**
	 * Change the document's syntax highlight
	 * \param lexerName the syntax highlight's lexer name (as appear in the liteeditor.xml file)
	 */
	virtual void SetSyntaxHighlight(const wxString &lexerName);

	/**
	 * Change the document's syntax highlight - use the file name to determine lexer name
	 */
	virtual void SetSyntaxHighlight();

	/**
	 * Return the document context object
	 */
	ContextBasePtr GetContext() const {
		return m_context;
	}

	// Bookmark API
	//-----------------------------------------

	// Is there currently a marker at the current line?
	bool LineIsMarked(enum marker_mask_type mask);
	// Toggle marker at the current line
	void ToggleMarker();
	// Delete all markers from the current document
	void DelAllMarkers();
	// Find next marker and move cursor to that line
	void FindNextMarker();
	// Find previous marker and move cursor to that line
	void FindPrevMarker();

	// Replace all
	bool ReplaceAll();
	bool ReplaceAllExactMatch(const wxString &what, const wxString &replaceWith);
	// mark all occurances
	bool MarkAll();

	void ToggleCurrentFold();
	void FoldAll();

	static FindReplaceDialog* GetFindReplaceDialog() {
		return m_findReplaceDlg;
	}

	// Util function
	int SafeGetChar(int pos);
	wxChar   PreviousChar(const int& pos, int &foundPos, bool wantWhitespace = false);
	wxString PreviousWord(int pos, int &foundPos);
	wxChar   NextChar    (const int& pos, int &foundPos);

	int  FindString (const wxString &str, int flags, const bool down, long pos);

	bool FindAndSelect();
	bool FindAndSelect(const FindReplaceData &data);
	bool FindAndSelect(const wxString &pattern, const wxString &name);

	bool Replace();
	bool Replace(const FindReplaceData &data);

	void RecalcHorizontalScrollbar();

	/**
	 * Add marker to the current line
	 */
	void AddMarker();

	/**
	 * Delete a marker from the current line
	 */
	void DelMarker();

	/**
	 * Attempt to match brace backward
	 * \param chCloseBrace the closing brace character (can be one of: '}' ')' ']')
	 * \param pos position to start the match
	 * \param matchedPos output, the position of the matched brace
	 * \return true on match false otherwise
	 */
	bool MatchBraceBack(const wxChar& chCloseBrace, const long &pos, long &matchedPos);

	/**
	 * Open file and clear the undo buffer
	 */
	virtual void Create(const wxString &project, const wxFileName &fileName);

	/**
	 * Insert text to the editor and keeping the page indentation
	 * \param text text to enter
	 * \param pos position to insert the text
	 */
	void InsertTextWithIndentation(const wxString &text, int pos);

	virtual BrowseRecord CreateBrowseRecord();

	bool IsContextMenuOn() const {
		return m_popupIsOn;
	}

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
	void AddOtherBreakpointType(wxCommandEvent &event);

	/**
	 * toggle whether the break point at the current line & file is enabled
	 */
	void ToggleBreakpointEnablement();

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
	 * Optionally make it temporary or conditional
	 */
	void AddBreakpoint(int lineno = -1, const wxString& conditions = wxT(""), const bool is_temp = false);

	/**
	 * Delete the breakpoint at the current line & file, or lineno if from ToggleBreakpoint()
	 */
	void DelBreakpoint(int lineno = -1);

	/**
	 * @brief search the editor for pattern. If pattern is found, the editor will then search for 'what'
	 * inside the pattern and will select it
	 * @param pattern pattern to search in the editor
	 * @param what    sub string of pattern to select
	 * @param pos     start the search from 'pos'
	 * @param navmgr  Navigation manager to place browsing recrods
	 * @return return true if a match was found, false otherwise
	 */
	virtual bool FindAndSelect(const wxString &pattern, const wxString &what, int pos, NavMgr *navmgr);

	virtual void UpdateBreakpoints();

	//--------------------------------
	// breakpoint visualisation
	//--------------------------------
	virtual void SetBreakpointMarker(int lineno, BreakpointType bptype, bool is_disabled, const std::vector<BreakpointInfo>& li);
	virtual void DelAllBreakpointMarkers();

	virtual void HighlightLine(int lineno);
	virtual void UnHighlightAll();

	// compiler warnings and errors
	void SetWarningMarker(int lineno);
	void SetErrorMarker(int lineno);
	void DelAllCompilerMarkers();
	void DoShowCalltip(int pos, const wxString &tip, calltip_type type, int hltPos = wxNOT_FOUND, int hltLen = wxNOT_FOUND);
	void DoCancelCalltip();
	int  DoGetOpenBracePos();

	calltip_type GetCalltipType() const {
		return m_calltipType;
	}

	//----------------------------------
	//File modifications
	//----------------------------------

	/**
	 * return the last modification time (on disk) of editor's underlying file
	 */
	time_t GetFileLastModifiedTime() const;

	/**
	 * return/set the last modification time that was made by the editor
	 */
	time_t GetEditorLastModifiedTime() const {
		return m_modifyTime;
	}
	void SetEditorLastModifiedTime(time_t modificationTime) {
		m_modifyTime = modificationTime;
	}

	/**
	 * \brief run through the file content and update colours for the
	 * functions / locals
	 */
	void UpdateColours();

	/**
	 * @brief display completion box. This function also moves the completion box to the current position
	 * @param tags list of tags to work with
	 * @param word part of the word
	 * @param showFullDecl display full function declaration
	 */
	void ShowCompletionBox(const std::vector<TagEntryPtr> &tags, const wxString &word, bool showFullDecl, bool autoHide = false, bool autoInsertSingleChoice = true);

	/**
	 * @brief displays teh code completion box. Unlike the previous metho, this method accepts owner and sends an event once selection is made
	 * @param tags list if tags to display
	 * @param word part of the word
	 * @param owner event handler to be notified once a selection is made
	 */
	virtual void ShowCompletionBox(const std::vector<TagEntryPtr> &tags, const wxString &word, wxEvtHandler *owner);

	/**
	 * @brief register new user image fot TagEntry kind
	 * @param kind the kind string that will be associated with the bitmap (TagEntry::GetKind())
	 * @param bmp 16x16 bitmap
	 */
	virtual void RegisterImageForKind(const wxString &kind, const wxBitmap &bmp);

	/**
	 * @brief return true if the completion box is visible
	 */
	virtual bool IsCompletionBoxShown();

	/**
	 * @brief hide the completion box if it is active.
	 */
	virtual void HideCompletionBox();

	/**
	 * @brief highlight the word where the cursor is at
	 * @param highlight
	 */
	void HighlightWord(bool highlight = true);

	void TrimText();

	/**
	 *--------------------------------------------------
	 * Implemetation for IEditor interace
	 *--------------------------------------------------
	 */
	virtual wxString GetEditorText() {
		return GetText();
	}
	virtual void SetEditorText(const wxString &text);
	virtual void ReloadFile();
	virtual void SetCaretAt(long pos);
	virtual long GetCurrentPosition() {
		return GetCurrentPos();
	}
	virtual const wxFileName& GetFileName() const {
		return m_fileName;
	}
	virtual const wxString &GetProjectName() const {
		return m_project;
	}
	virtual wxString GetWordAtCaret() ;
	virtual void AppendText(const wxString &text) {
		wxScintilla::AppendText(text);
	}
	virtual void InsertText(int pos, const wxString &text) {
		wxScintilla::InsertText(pos, text);
	}
	virtual int GetLength() {
		return wxScintilla::GetLength();
	}
	virtual bool IsModified() {
		return wxScintilla::GetModify();
	}
	virtual int GetEOL() {
		return wxScintilla::GetEOLMode();
	}
	virtual int GetCurrentLine();
	virtual void ReplaceSelection(const wxString &text);
	virtual wxString GetSelection();
	virtual int GetSelectionStart();
	virtual int GetSelectionEnd();
	virtual void SelectText(int startPos, int len);

	virtual void SetLexerName(const wxString &lexerName);

	// User Indicators API
	virtual void SetUserIndicatorStyleAndColour(int style, const wxColour &colour);
	virtual void SetUserIndicator(int startPos, int len);
	virtual void ClearUserIndicators();
	virtual int  GetUserIndicatorStart(int pos);
	virtual int  GetUserIndicatorEnd(int pos);
	virtual int GetLexerId();
	virtual int GetStyleAtPos(int pos);

	/**
	 * @brief Get position of start of word.
	 * @param pos from position
	 * @param onlyWordCharacters
	 */
	virtual int WordStartPos (int pos, bool onlyWordCharacters);

	/**
	 * @brief  Get position of end of word.
	 * @param pos from position
	 * @param onlyWordCharacters
	 */
	virtual int WordEndPos (int pos, bool onlyWordCharacters);

	/**
	 * Prepend the indentation level found at line at 'pos' to each line in the input string
	 * \param text text to enter
	 * \param pos position to insert the text
	 * \param flags formatting flags
	 */
	virtual wxString FormatTextKeepIndent(const wxString &text, int pos, size_t flags = 0);


	/**
	 * @brief return the line numebr containing 'pos'
	 * @param pos the position
	 */
	virtual int LineFromPos(int pos);

	/**
	 * @brief return the start pos of line nummber
	 * @param line the line number
	 * @return line nummber or 0 if the document is empty
	 */
	virtual int PosFromLine(int line);

	/**
	 * @brief return the END position of 'line'
	 * @param line the line number
	 */
	virtual int LineEnd(int line);

	/**
	 * @brief return text from a given pos -> endPos
	 * @param startPos
	 * @param endPos
	 * @return
	 */
	virtual wxString GetTextRange(int startPos, int endPos);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------

	/**
	 * Get editor options. Takes any workspace/project overrides into account
	 */
	OptionsConfigPtr GetOptions();

	void SetIsVisible(const bool& isVisible) {
		this->m_isVisible = isVisible;
	}
	const bool& GetIsVisible() const {
		return m_isVisible;
	}

	wxString GetEolString();
	void HighlightWord(StringHighlightOutput *highlightOutput);

private:
	void FillBPtoMarkerArray();
	BPtoMarker GetMarkerForBreakpt(enum BreakpointType bp_type);
	void SetProperties();
	void DefineMarker(int marker, int markerType, wxColor fore, wxColor back);
	bool SaveToFile(const wxFileName &fileName);
	void BraceMatch(const bool& bSelRegion);
	void BraceMatch(long pos);
	void DoHighlightWord();
	void DoSetStatusMessage(const wxString &msg, int col);
	bool IsOpenBrace (int position);
	bool IsCloseBrace(int position);

	// Conevert FindReplaceDialog flags to wxSD flags
	size_t SearchFlags(const FindReplaceData &data);

	void    AddDebuggerContextMenu(wxMenu *menu);
	void    RemoveDebuggerContextMenu(wxMenu *menu);
	void    DoBreakptContextMenu(wxPoint clientPt);
	void    DoMarkHyperlink(wxMouseEvent &event, bool isMiddle);
	void    DoQuickJump(wxMouseEvent &event, bool isMiddle);
	bool    DoFindAndSelect(const wxString &pattern, const wxString &what, int start_pos, NavMgr *navmgr);
	wxMenu* DoCreateDebuggerWatchMenu(const wxString &word);

	DECLARE_EVENT_TABLE()
	void OnRemoveMatchInidicator(wxCommandEvent &e);
	void OnSavePoint(wxScintillaEvent &event);
	void OnCharAdded(wxScintillaEvent& event);
	void OnMarginClick(wxScintillaEvent& event);
	void OnChange(wxScintillaEvent& event);
	void OnDwellStart(wxScintillaEvent& event);
	void OnDwellEnd(wxScintillaEvent& event);
	void OnCallTipClick(wxScintillaEvent& event);
	void OnSciUpdateUI(wxScintillaEvent &event);
	void OnFindDialog(wxCommandEvent &event);
	void OnContextMenu(wxContextMenuEvent &event);
	void OnKeyDown(wxKeyEvent &event);
	void OnLeftDown(wxMouseEvent &event);
	void OnMiddleDown(wxMouseEvent &event);
	void OnMiddleUp(wxMouseEvent &event);
	void OnLeftUp(wxMouseEvent &event);
	void OnLeaveWindow(wxMouseEvent &event);
	void OnFocusLost(wxFocusEvent &event);
	void OnLeftDClick(wxScintillaEvent &event);
	void OnPopupMenuUpdateUI(wxUpdateUIEvent &event);
	void OnDbgAddWatch(wxCommandEvent &event);
	void OnDbgRunToCursor(wxCommandEvent &event);
	void OnDbgCustomWatch(wxCommandEvent &event);
	void OnDragStart(wxScintillaEvent &e);
	void OnDragEnd(wxScintillaEvent &e);
	void DoSetCaretAt(long pos);
};

#endif // LITEEDITOR_EDITOR_H
