#ifndef LITEEDITOR_EDITOR_H
#define LITEEDITOR_EDITOR_H

#include <wx/wxscintilla.h>
#include <stack>
#include <map>
#include "entry.h"
#include "cl_calltip.h"
#include "wx/filename.h"
#include "findreplacedlg.h"
#include "context_base.h"
#include "wx/menu.h"
#include "browse_record.h"
#include "navigationmanager.h"
#include "debuggermanager.h"
#include "plugin.h"

class wxFindReplaceDialog;
class CCBox;

//incase we are using DLL build of wxWdigets, we need to make this class to export its 
//classes 
#ifndef WXDLLIMPEXP_LE
	#ifdef WXMAKINGDLL
	#    define WXDLLIMPEXP_LE WXEXPORT
	#elif defined(WXUSINGDLL)
	#    define WXDLLIMPEXP_LE WXIMPORT
	#else 
	#    define WXDLLIMPEXP_LE
	#endif // WXDLLIMPEXP_LE
#endif

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
	wxFileName m_fileName;
	wxString m_project;
	wxStopWatch m_watch;
	ContextBasePtr m_context;
	wxMenu *m_rightClickMenu;
	std::vector<wxMenuItem*> m_dynItems;

	// static cache among editors to keep track of jumping between editors
	static FindReplaceDialog *m_findReplaceDlg;
	static FindReplaceData m_findReplaceData;
	int m_lastMatchPos;
	static std::map<wxString, int> ms_bookmarkShapes;
	bool m_popupIsOn;
	time_t m_modifyTime;
	std::map<int, wxString> m_customCmds;
	bool m_resetSearch;
	CCBox *m_ccBox;
	
public:
	static FindReplaceData &GetFindReplaceData(){return m_findReplaceData;}

public:
	/// Construct a LEditor object
	LEditor(wxWindow* parent, wxWindowID id, const wxSize& size, const wxString& fileName, const wxString& project, bool hidden = false);

	/// Default destructor
	virtual ~LEditor();
	
	// Save the editor data into file
	virtual bool SaveFile();

	// Save content of the editor to a given file (Save As...)
	// this function prompts the user for selecting file name
	bool SaveFileAs();

	void CompleteWord();
	
	// set this editor file name
	void SetFileName(const wxFileName &name) { m_fileName = name; }

	// Return the project name
	const wxString &GetProject() const { return m_project; }
	// Set the project name
	void SetProject(const wxString &proj) { m_project = proj; }

	// Attempt to display a list of members 
	// after a '.' or '->' operator has been inserted into 
	// the code
	void CodeComplete();

	// User clicked Ctrl+.
	void GotoDefinition();

	// User clicked Ctrl+,
	void GotoPreviousDefintion();

	/**
	 * Return true if editor definition contains more
	 * on its stack
	 */
	bool CanGotoPreviousDefintion() { return NavMgr::Get()->CanPrev(); }

	// Load a file 
	void OpenFile(const wxString& fileName, const wxString& project);
	
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
	 * Change the document's syntax highlight
	 * \param lexerName the syntax highlight's lexer name (as appear in the liteeditor.xml file)
	 */
	virtual void SetSyntaxHighlight(const wxString &lexerName);

	/**
	 * Change the document's syntax highlight - use the file name to determine lexer name
	 */
	virtual void SetSyntaxHighlight();

	/**
	 * Reset the editor to its default state:
	 * -# Syntax highlight is set according to file extension
	 * -# Setproperties() is called
	 */
	void RestoreDefaults();

	/** 
	 * Return the document context object
	 */
	ContextBasePtr GetContext() const { return m_context; }

	// Bookmark API
	//-----------------------------------------

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
	// mark all occurances
	bool MarkAll();
	
	static FindReplaceDialog* GetFindReplaceDialog() { return m_findReplaceDlg; }

	// Util function
	int SafeGetChar(int pos);
	wxChar PreviousChar(const int& pos, int &foundPos, bool wantWhitespace = false);
	wxChar NextChar(const int& pos, int &foundPos);
	int  FindString (const wxString &str, int flags, const bool down, long pos);
	void SetDirty(bool dirty);
	
	bool FindAndSelect();
	bool FindAndSelect(const FindReplaceData &data);

	bool Replace();
	bool Replace(const FindReplaceData &data);

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

	/**
	 * \brief create browsing crecord from the editor's current position
	 * \return browsing record
	 */
	BrowseRecord CreateBrowseRecord();
	bool IsContextMenuOn() const {return m_popupIsOn;}
	
	/**
	 * @brief return an approximation of the line height
	 * @return line height
	 */
	int GetCurrLineHeight();
	
	/**
	 * toggle break point at the current line & file
	 * the second version is using BreakpointInfo
	 */
	void ToggleBreakpoint();
	void ToggleBreakpoint(const BreakpointInfo &bp);

	/**
	 * remove breakpoint from current file and 
	 * from the carte line number
	 */
	void DelBreakpoint();

	/**
	 * remove breakpoint from current file and 
	 * from the carte line number
	 */
	virtual void DelBreakpoint(const BreakpointInfo &bp);
	virtual void UpdateBreakpoints();
	
	//--------------------------------
	// breakpoint visualisation
	//--------------------------------
	void SetBreakpointMarker(int lineno);
	virtual void DelBreakpointMarker(int lineno);
	virtual void DelAllBreakpointMarkers();
	
	virtual void HighlightLine(int lineno);
	virtual void UnHighlightAll();
	
	//----------------------------------
	//File modifications
	//----------------------------------

	/**
	 * return/set the last modification time that was made by the editor
	 */
	time_t GetEditorLastModifiedTime() const {return m_modifyTime;}
	void SetEditorLastModifiedTime(time_t modificationTime) {m_modifyTime = modificationTime;}
	
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
	void ShowCompletionBox(const std::vector<TagEntryPtr> &tags, const wxString &word, bool showFullDecl);
	
	/**
	 * @brief hide the completion box if it is active.
	 */
	void HideCompletionBox();
	
	/**
	 * @brief highlight the word where the cursor is at
	 * @param highlight  
	 */
	void HighlightWord(bool highlight = true);
	
	/**
	 *--------------------------------------------------
	 * Implemetation for IEditor interace
	 *--------------------------------------------------
	 */
	virtual wxString GetEditorText() {return GetText();}
	virtual void SetEditorText(const wxString &text);
	virtual void ReloadFile();
	virtual void SetCaretAt(long pos);	
	virtual long GetCurrentPosition(){return GetCurrentPos();}
	virtual const wxFileName& GetFileName() const { return m_fileName; }
	virtual const wxString &GetProjectName() const { return m_project; }
	virtual wxString GetWordAtCaret() ;

private:
	void DoSetBreakpoint(const BreakpointInfo &bp);
	void SetProperties();
	void DefineMarker(int marker, int markerType, wxColor fore, wxColor back);
	void SetLineNumberWidth();
	bool SaveToFile(const wxFileName &fileName);
	void BraceMatch(const bool& bSelRegion);
	void BraceMatch(long pos);
	void DoHighlightWord();
	
	// Conevert FindReplaceDialog flags to wxSD flags
	size_t SearchFlags(const FindReplaceData &data);
	
	void AddDebuggerContextMenu(wxMenu *menu);
	void RemoveDebuggerContextMenu(wxMenu *menu);

	DECLARE_EVENT_TABLE()
	void OnCharAdded(wxScintillaEvent& event);
	void OnMarginClick(wxScintillaEvent& event);
	void OnChange(wxScintillaEvent& event);
	void OnDwellStart(wxScintillaEvent& event);
	void OnDwellEnd(wxScintillaEvent& event);
	void OnCallTipClick(wxScintillaEvent& event);
	void OnModified(wxScintillaEvent& event);
	void OnSciUpdateUI(wxScintillaEvent &event);
	void OnFindDialog(wxCommandEvent &event);
	void OnContextMenu(wxContextMenuEvent &event);
	void OnKeyDown(wxKeyEvent &event);
	void OnLeftDown(wxMouseEvent &event);
	void OnPopupMenuUpdateUI(wxUpdateUIEvent &event);
	void OnDbgQuickWatch(wxCommandEvent &event);
	void OnDbgAddWatch(wxCommandEvent &event);
	void OnDbgCustomWatch(wxCommandEvent &event);
	void OnDragStart(wxScintillaEvent &e);
	void OnDragEnd(wxScintillaEvent &e);
};

#endif // LITEEDITOR_EDITOR_H
