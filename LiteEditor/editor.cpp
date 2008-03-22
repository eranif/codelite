#include "editor.h"
#include "precompiled_header.h"
#include "colourrequest.h"
#include "colourthread.h"
#include <wx/xrc/xmlres.h>
#include <wx/ffile.h>
#include <wx/tooltip.h>
#include <wx/settings.h>
#include "parse_thread.h"
#include "ctags_manager.h"
#include "manager.h"
#include "menumanager.h"
#include <wx/fdrepdlg.h>
#include "findreplacedlg.h"
#include <wx/wxFlatNotebook/renderer.h>
#include "context_manager.h"
#include "editor_config.h"
#include "filedroptarget.h"
#include "fileutils.h"
#include "wx/tokenzr.h"
#include "simpletable.h"
#include "debuggerpane.h"
#include "frame.h"
#include "pluginmanager.h"
#include "breakpointdlg.h"
#include <sys/stat.h>
#include <sys/types.h>
#include "debuggersettings.h"
#include "globals.h"
#include "threebuttondlg.h"
#include "debuggerconfigtool.h"
#include "addincludefiledlg.h"

// fix bug in wxscintilla.h
#ifdef EVT_SCI_CALLTIP_CLICK
#undef EVT_SCI_CALLTIP_CLICK
#define EVT_SCI_CALLTIP_CLICK(id, fn)          DECLARE_EVENT_TABLE_ENTRY (wxEVT_SCI_CALLTIP_CLICK,          id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxScintillaEventFunction, & fn ), (wxObject *) NULL),
#endif

#define NUMBER_MARGIN_ID 	0
#define SEP_MARGIN_ID 		1
#define SYMBOLS_MARGIN_ID 	2
#define FOLD_MARGIN_ID 		3

//debugger line marker xpm
extern char *arrow_right_green_xpm[];
extern char *stop_xpm[];

BEGIN_EVENT_TABLE(LEditor, wxScintilla)
	EVT_SCI_CHARADDED(wxID_ANY, LEditor::OnCharAdded)
	EVT_SCI_MARGINCLICK(wxID_ANY, LEditor::OnMarginClick)
	EVT_SCI_DWELLSTART(wxID_ANY, LEditor::OnDwellStart)
	EVT_SCI_CALLTIP_CLICK(wxID_ANY, LEditor::OnCallTipClick)
	EVT_SCI_DWELLEND(wxID_ANY, LEditor::OnDwellEnd)
	EVT_SCI_MODIFIED(wxID_ANY, LEditor::OnModified)
	EVT_SCI_UPDATEUI(wxID_ANY, LEditor::OnSciUpdateUI)
	EVT_CONTEXT_MENU(LEditor::OnContextMenu)
	EVT_KEY_DOWN(LEditor::OnKeyDown)
	EVT_LEFT_DOWN(LEditor::OnLeftDown)

	//C++ contex menu update UI events
	EVT_UPDATE_UI(XRCID("insert_doxy_comment"), LEditor::OnPopupMenuUpdateUI)
	EVT_UPDATE_UI(XRCID("setters_getters"), LEditor::OnPopupMenuUpdateUI)
	EVT_UPDATE_UI(XRCID("find_decl"), LEditor::OnPopupMenuUpdateUI)
	EVT_UPDATE_UI(XRCID("find_impl"), LEditor::OnPopupMenuUpdateUI)
	//	EVT_UPDATE_UI(XRCID("move_impl"), LEditor::OnPopupMenuUpdateUI)

	// Find and replace dialog
	EVT_COMMAND(wxID_ANY, wxEVT_FRD_FIND_NEXT, LEditor::OnFindDialog)
	EVT_COMMAND(wxID_ANY, wxEVT_FRD_REPLACE, LEditor::OnFindDialog)
	EVT_COMMAND(wxID_ANY, wxEVT_FRD_REPLACEALL, LEditor::OnFindDialog)
	EVT_COMMAND(wxID_ANY, wxEVT_FRD_BOOKMARKALL, LEditor::OnFindDialog)
	EVT_COMMAND(wxID_ANY, wxEVT_FRD_CLOSE, LEditor::OnFindDialog)
END_EVENT_TABLE()

// Instantiate statics
FindReplaceDialog* LEditor::m_findReplaceDlg = NULL;
FindReplaceData LEditor::m_findReplaceData;
std::map<wxString, int> LEditor::ms_bookmarkShapes;

time_t GetFileModificationTime(const wxString &filename)
{
	struct stat buff;
	const wxCharBuffer cname = _C(filename);
	if (stat(cname.data(), &buff) < 0) {
		return 0;
	}
	return buff.st_mtime;
}

LEditor::LEditor(wxWindow* parent, wxWindowID id, const wxSize& size, const wxString& fileName, const wxString& project, bool hidden)
		: wxScintilla(parent, id, wxDefaultPosition, size)
		, m_fileName(fileName)
		, m_project(project)
		, m_rightClickMenu(NULL)
		, m_lastMatchPos(0)
		, m_popupIsOn(false)
		, m_modifyTime(0)
		, m_resetSearch(true)
{
	Show(!hidden);
	ms_bookmarkShapes[wxT("Small Rectangle")] = wxSCI_MARK_SMALLRECT;
	ms_bookmarkShapes[wxT("Rounded Rectangle")] = wxSCI_MARK_ROUNDRECT;
	ms_bookmarkShapes[wxT("Small Arrow")] = wxSCI_MARK_ARROW;
	ms_bookmarkShapes[wxT("Circle")] = wxSCI_MARK_CIRCLE;

	m_fileName.MakeAbsolute();

	// If file name is provided, open it
	wxString tmpFilename(m_fileName.GetName());
	if (	!tmpFilename.IsEmpty() ) {
		OpenFile(m_fileName.GetFullPath(), m_project);
	}
	SetDropTarget(new FileDropTarget());
	RestoreDefaults();
}

void LEditor::RestoreDefaults()
{
	//the order is important, SetSyntaxHighlight must be called before SetProperties
	//do not change it
	SetSyntaxHighlight();
}

void LEditor::SetSyntaxHighlight()
{
	ClearDocumentStyle();
	m_context = ManagerST::Get()->NewContextByFileName(m_fileName, this);
	SetProperties();
	Colourise(0, wxSCI_INVALID_POSITION);
}

LEditor::~LEditor()
{
}

void LEditor::SetCaretAt(long pos)
{
	SetCurrentPos(pos);
	SetSelectionStart(pos);
	SetSelectionEnd(pos);
	EnsureCaretVisible();
}

/// Setup some scintilla properties
void LEditor::SetProperties()
{
	m_rightClickMenu = m_context->GetMenu();
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
	CallTipUseStyle(1);

	SetMouseDwellTime(250);
	SetProperty(wxT("fold"), wxT("1"));
	SetProperty(wxT("fold.html"), wxT("1"));
	SetProperty(wxT("styling.within.preprocessor"), wxT("1"));

	// Fold and comments as well
	SetProperty(wxT("fold.comment"), wxT("1"));
	SetModEventMask (wxSCI_MOD_DELETETEXT | wxSCI_MOD_INSERTTEXT  | wxSCI_PERFORMED_UNDO  | wxSCI_PERFORMED_REDO );

	int caretSlop = 1;
	int caretZone = 20;
	int caretStrict = 0;
	int caretEven = 0;
	int caretJumps = 0;

	SetXCaretPolicy(caretStrict | caretSlop | caretEven | caretJumps, caretZone);

	caretSlop = 1;
	caretZone = 1;
	caretStrict = 4;
	caretEven = 8;
	caretJumps = 0;
	SetYCaretPolicy(caretStrict | caretSlop | caretEven | caretJumps, caretZone);
	SetCaretWidth(1);
	SetMarginLeft(1);
	SetMarginRight(0);

	// Mark current line
	SetCaretLineVisible(options->GetHighlightCaretLine());
	SetCaretLineBackground(options->GetCaretLineColour());

#ifdef __WXMSW__
	SetCaretLineBackgroundAlpha(30);
#endif

	SetFoldFlags(options->GetUnderlineFoldLine() ? 16 : 0);

	//------------------------------------------
	// Margin settings
	//------------------------------------------
	// symbol margin
	SetMarginType(SYMBOLS_MARGIN_ID, wxSCI_MARGIN_SYMBOL);
	// Line numbes
	SetMarginType(NUMBER_MARGIN_ID, wxSCI_MARGIN_NUMBER);

	// line number margin displays every thing but folding, bookmarks (256) and breakpoint (512)
	SetMarginMask(NUMBER_MARGIN_ID, ~(256 | 512 | 128 | wxSCI_MASK_FOLDERS));

	// Separators
	SetMarginType(SEP_MARGIN_ID, wxSCI_MARGIN_FORE);
	SetMarginMask(SEP_MARGIN_ID, 0);

	// Fold margin - allow only folder symbols to display
	SetMarginMask(FOLD_MARGIN_ID, wxSCI_MASK_FOLDERS);

	// Set margins' width
	SetMarginWidth(SYMBOLS_MARGIN_ID, options->GetDisplayBookmarkMargin() ? 16 : 0);	// Symbol margin
	// allow everything except for the folding symbols
	SetMarginMask(SYMBOLS_MARGIN_ID, ~(wxSCI_MASK_FOLDERS));

	//sets the caret colour
	SetCaretForeground(options->GetCaretColour());

	// Line number margin
	int pixelWidth = 4 + 5*TextWidth(wxSCI_STYLE_LINENUMBER, wxT("9"));
	SetMarginWidth(NUMBER_MARGIN_ID, options->GetDisplayLineNumbers() ? pixelWidth : 0);

	SetMarginWidth(SEP_MARGIN_ID, 1);	// Symbol margin which acts as separator

	int ww = options->GetDisplayFoldMargin() ? 16 : 0;
	SetMarginWidth(FOLD_MARGIN_ID, ww);	// Fold margin
	StyleSetForeground(wxSCI_STYLE_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));

	// Mark fold margin & symbols margins as sensetive
	SetMarginSensitive(FOLD_MARGIN_ID, true);
	SetMarginSensitive(SYMBOLS_MARGIN_ID, true);

	//---------------------------------------------------
	// Fold settings
	//---------------------------------------------------
	// Define the folding style to be square
	if ( options->GetFoldStyle() == wxT("Flatten Tree Square Headers") ) {

		DefineMarker(wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_BOXMINUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDER, wxSCI_MARK_BOXPLUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_VLINE, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_LCORNER, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_BOXPLUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_BOXMINUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_TCORNER, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));

	} else if ( options->GetFoldStyle() == wxT("Flatten Tree Circular Headers") ) {

		DefineMarker(wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_CIRCLEMINUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDER, wxSCI_MARK_CIRCLEPLUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_VLINE, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_LCORNERCURVE, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_CIRCLEPLUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_CIRCLEMINUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_TCORNER, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));

	} else if ( options->GetFoldStyle() == wxT("Simple") ) {

		DefineMarker(wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_MINUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDER, wxSCI_MARK_PLUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_PLUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_MINUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));

	} else if ( options->GetFoldStyle() == wxT("Arrows") ) {

		DefineMarker(wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_ARROWDOWN, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDER, wxSCI_MARK_ARROW, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_ARROW, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_ARROWDOWN, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
		DefineMarker(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_BACKGROUND, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));

	}

	// Bookmark
	int marker = wxSCI_MARK_ARROW;
	std::map<wxString, int>::iterator iter = ms_bookmarkShapes.find(options->GetBookmarkShape());
	if ( iter != ms_bookmarkShapes.end() ) {
		marker = iter->second;
	}

	MarkerDefine(0x7, marker);
	MarkerSetBackground(0x7, options->GetBookmarkBgColour());
	MarkerSetForeground(0x7, options->GetBookmarkFgColour());

	wxImage imgbp(stop_xpm);
	wxBitmap bmpbp(imgbp);
	MarkerDefineBitmap(0x8, bmpbp);

	//debugger line marker
	wxImage img(arrow_right_green_xpm);
	wxBitmap bmp(img);
	MarkerDefineBitmap(0x9, bmp);
	MarkerSetBackground(0x9, wxT("LIME GREEN"));
	MarkerSetForeground(0x9, wxT("BLACK"));

	// calltip settings
	/*	CallTipUseStyle(1);
		wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
		StyleSetFont(wxSCI_STYLE_CALLTIP, font);*/

	CallTipSetBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
	CallTipSetForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));

	SetTabWidth(4);
	SetIndent(4);
	SetTabIndents(true);
	SetBackSpaceUnIndents (true);
	SetUseTabs (options->GetIndentUsesTabs());
	SetLayoutCache(wxSCI_CACHE_DOCUMENT);

	size_t frame_flags = Frame::Get()->GetFrameGeneralInfo().GetFlags();
	int eolMode (wxSCI_EOL_LF);	//default it to unix
	if (frame_flags & CL_USE_EOL_CR) {
		eolMode = wxSCI_EOL_CR;
	} else if (frame_flags & CL_USE_EOL_CRLF) {
		eolMode = wxSCI_EOL_CRLF;
	}

	SetEOLMode(eolMode);
	SetViewEOL(frame_flags & CL_SHOW_EOL ? true : false);

	//if no right click menu is provided by the context, use scintilla default
	//right click menu
	UsePopUp(m_rightClickMenu ? false : true);
	StyleSetForeground(wxSCI_STYLE_LINENUMBER, wxColour(0, 63, 125));
	SetIndentationGuides(options->GetShowIndentationGuidelines());

	SetFoldMarginColour (true, wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
}

void LEditor::SetDirty(bool dirty)
{
	if ( dirty ) {
		if ( !ManagerST::Get()->GetPageTitle(this).StartsWith(wxT("*")) ) {
			ManagerST::Get()->SetPageTitle(this, wxT("*") + ManagerST::Get()->GetPageTitle(this));
		}
	} else {
		ManagerST::Get()->SetPageTitle(this, GetFileName().GetFullName());
	}
}

void LEditor::OnCharAdded(wxScintillaEvent& event)
{
	// set the page title as dirty
	SetDirty(true);

	// make sure line is visible
	int curLine = LineFromPosition(GetCurrentPos());
	if ( !GetFoldExpanded(curLine) ) {
		ToggleFold(curLine);
	}

	// Always do auto-indentation
	if (event.GetKey() == ':' || event.GetKey() == '}' || event.GetKey() == '\n')
		m_context->AutoIndent(event.GetKey());

	if (false == GetProjectName().IsEmpty()) {
		switch ( event.GetKey() ) {
		case ':':
		case '.':
		case '>':
		case '(':
			CodeComplete();
			break;
		case ')': {
				m_context->CallTipCancel();
				break;
			}
		case '\n': {
				// incase we are typing in a folded line, make sure it is visible
				int  nLineNumber = LineFromPosition(GetCurrentPos());
				EnsureVisible(nLineNumber+1);
				break;
			}
		default:
			break;
		}
	}
	event.Skip();
}

void LEditor::OnSciUpdateUI(wxScintillaEvent &event)
{
	// Get current position
	long pos = GetCurrentPos();

	if ((GetCharAt(pos) == '{' ||
	        GetCharAt(pos) == '[' ||
	        GetCharAt(pos) == '<' ||
	        GetCharAt(pos) == '(')
	        && !m_context->IsCommentOrString(pos)) {
		BraceMatch((long)pos);
	} else
		if ((GetCharAt(PositionBefore(pos)) == '{' ||
		        GetCharAt(PositionBefore(pos)) == '<' ||
		        GetCharAt(PositionBefore(pos)) == '[' ||
		        GetCharAt(PositionBefore(pos)) == '(') && !m_context->IsCommentOrString(PositionBefore(pos))) {
			BraceMatch((long)PositionBefore(pos));
		} else
			if ((GetCharAt(pos) == '}' ||
			        GetCharAt(pos) == ']' ||
			        GetCharAt(pos) == '>' ||
			        GetCharAt(pos) == ')') && !m_context->IsCommentOrString(pos)) {
				BraceMatch((long)pos);
			} else
				if ((GetCharAt(PositionBefore(pos)) == '}' ||
				        GetCharAt(PositionBefore(pos)) == '>' ||
				        GetCharAt(PositionBefore(pos)) == ']' ||
				        GetCharAt(PositionBefore(pos)) == ')') && !m_context->IsCommentOrString(PositionBefore(pos))) {
					BraceMatch((long)PositionBefore(pos));
				} else {
					wxScintilla::BraceHighlight(wxSCI_INVALID_POSITION, wxSCI_INVALID_POSITION);
				}
	//update line number
	wxString message;
	message << wxT("Ln ") << LineFromPosition(pos)+1 << wxT("    Col ") << GetColumn(pos) << wxT("    Pos ") << pos;
	ManagerST::Get()->SetStatusMessage(message, 3);

	//let the context handle this as well
	m_context->OnSciUpdateUI(event);
}

void LEditor::OnMarginClick(wxScintillaEvent& event)
{
	switch (event.GetMargin()) {
	case SYMBOLS_MARGIN_ID:
		//symbols / breakpoints margin
		{
			int lineno = LineFromPosition(event.GetPosition());
			BreakpointInfo bp;
			bp.file = GetFileName().GetFullPath();
			bp.lineno = lineno+1;
			ToggleBreakpoint(bp);
		}
		break;
	case FOLD_MARGIN_ID:
		//fold margin
		{
			int nLine = LineFromPosition(event.GetPosition());
			ToggleFold(nLine);
		}
		break;
	default:
		break;
	}
}

void LEditor::DefineMarker(int marker, int markerType, wxColor fore, wxColor back)
{
	MarkerDefine(marker, markerType);
	MarkerSetForeground(marker, fore);
	MarkerSetBackground(marker, back);
}

bool LEditor::SaveFile()
{
	if (this->GetModify()) {
		if (GetFileName().GetFullName().Find(wxT("Untitled")) != -1 || GetFileName().GetFullName().IsEmpty()) {
			return SaveFileAs();
		}

		// first save the file content
		if ( !SaveToFile(m_fileName) )
			return false;

		if ( GetProjectName().IsEmpty() )
			return true;

		//-------------------------------------------------------------------
		// Using the CodeParser library, enforces us to notify the parsing
		// thread once the file is saved. ctags accepts file name, so we
		// notify him once the file on the disk is changed, there is no
		// point in notifying the parsing thread on, for example, OnCharAdded
		// event, since the actual file on the disk was not modified
		//-------------------------------------------------------------------
		if (TagsManagerST::Get()->IsValidCtagsFile(m_fileName)) {
			ParseRequest *req = new ParseRequest();
			// Put a request on the parsing thread to update the GUI tree for this file
			wxFileName fn = TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName();
			req->dbfile = fn.GetFullPath();
			// Construct an absolute file name for ctags
			wxFileName absFile( m_fileName);
			absFile.MakeAbsolute();
			req->file = absFile.GetFullPath();

			//the previous call 'stole' the focus from us...
			ParseThreadST::Get()->Add(req);

			UpdateColours();
			SetActive();
		}
	}
	return true;
}

bool LEditor::SaveFileAs()
{
	// Prompt the user for a new file name
	const wxString ALL(wxT("All Files (*)|*"));
	wxFileDialog *dlg = new wxFileDialog(this, wxT("Save As"), m_fileName.GetPath(), m_fileName.GetFullName(), ALL,
	                                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT ,
	                                     wxDefaultPosition);

	if (dlg->ShowModal() == wxID_OK) {
		// get the path
		wxFileName name(dlg->GetPath());
		if ( !SaveToFile(name) ) {
			wxMessageBox(wxT("Failed to save file"), wxT("Error"), wxOK | wxICON_ERROR);
			return false;
		}
		m_fileName = name;
		delete dlg;
		return true;
	}

	delete dlg;
	return false;
}

// an internal function that does the actual file writing to disk
bool LEditor::SaveToFile(const wxFileName &fileName)
{
	wxFFile file(fileName.GetFullPath().GetData(), wxT("wb"));
	if (file.IsOpened() == false) {
		// Nothing to be done
		wxString msg = wxString::Format(wxT("Failed to open file %s"), fileName.GetFullPath().GetData());
		wxMessageBox( msg );
		return false;
	}

	file.Write(GetText());
	file.Close();

	//update the modification time of the file
	m_modifyTime = GetFileModificationTime(fileName.GetFullPath());
	SetSavePoint();

	//fire a wxEVT_FILE_SAVED event
	wxString file_name = fileName.GetFullPath();
	SendCmdEvent(wxEVT_FILE_SAVED, (void*)&file_name);

	// update the file name (remove the star from the file name)
	ManagerST::Get()->SetPageTitle(this, fileName.GetFullName());

	// Update context if needed
	if (fileName.GetExt() != m_fileName.GetExt()) {
		// new context is required
		ClearDocumentStyle();
		m_context = ManagerST::Get()->NewContextByFileName(fileName, this);
		Colourise(0, wxSCI_INVALID_POSITION);
		SetProperties();
	}
	return true;
}

void LEditor::SetSyntaxHighlight(const wxString &lexerName)
{
	ClearDocumentStyle();
	m_context = ContextManager::Get()->NewContext(this, lexerName);
	SetProperties();
	UpdateColours();
}

void LEditor::OpenFile(const wxString &fileName, const wxString &project)
{
	if (fileName.IsEmpty() == true)
		return;

	wxString text;
	ReadFileWithConversion(fileName, text);
	SetText( text );

	//get the modification time of the file
	m_modifyTime = GetFileModificationTime(fileName);

	// make sure user can not undo this operation
	EmptyUndoBuffer();

	// Keep the file name and the project
	m_fileName = fileName;
	m_project = project;

	//update breakpoints
	UpdateBreakpoints();
	SetCaretAt(0);

	UpdateColours();
}

//this function is called before the debugger startup
void LEditor::UpdateBreakpoints()
{
	//remove all break points associated with this file
	DebuggerMgr::Get().DelBreakpoints(GetFileName().GetFullPath());

	//collect the actual breakpoint according to the markers set
	int mask(0);
	mask |= 256;
	int lineno = MarkerNext(0, mask);
	while (lineno >= 0) {
		BreakpointInfo bp;
		bp.file = GetFileName().GetFullPath();
		bp.lineno = lineno + 1;
		DebuggerMgr::Get().AddBreakpoint(bp);

		lineno = MarkerNext(lineno+1, mask);
	}
	Frame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
}

void LEditor::DoSetBreakpoint(const BreakpointInfo &bp)
{
	if (!DebuggerMgr::Get().AddBreakpoint(bp)) {
		wxMessageBox(wxT("Failed to insert breakpoint"));
		return;
	}
	if (bp.lineno-1 < 0) {
		return;
	}
	SetBreakpointMarker(bp.lineno-1);
	Frame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
}

wxString LEditor::GetWordAtCaret()
{
	// Get the partial word that we have
	long pos = GetCurrentPos();
	long start = WordStartPosition(pos, true);
	long end   = WordEndPosition(pos, true);
	return GetTextRange(start, end);
}

//---------------------------------------------------------------------------
// Most of the functionality for this functionality
// is done in the Language & TagsManager objects, however,
// as you can see below, much work still needs to be done in the application
// layer (outside of the library) to provide the input arguments for
// the CodeParser library
//---------------------------------------------------------------------------
void LEditor::CompleteWord()
{
	m_context->CompleteWord();
}

//------------------------------------------------------------------
// AutoCompletion, by far the nicest feature of a modern IDE
// This function attempts to resolve the string to the left of
// the '.', '->' operator and to display a popup menu with
// list of possible matches
//------------------------------------------------------------------
void LEditor::CodeComplete()
{
	m_context->CodeComplete();
}


//----------------------------------------------------------------
// Demonstrate how to achieve symbol browsing using the CodeLite
// library, in addition we implements here a memory for allowing
// user to go back and forward
//----------------------------------------------------------------
void LEditor::GotoDefinition()
{
	m_context->GotoDefinition();
}

void LEditor::GotoPreviousDefintion()
{
	m_context->GotoPreviousDefintion();
}

void LEditor::OnDwellStart(wxScintillaEvent & event)
{
	Manager *mgr = ManagerST::Get();
	if (mgr->DbgCanInteract()) {
		//debugger is running and responsive, query it about the current token
		if (!IsContextMenuOn()) {
			m_context->OnDbgDwellStart(event);
		}
	} else {
		if (TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_DISP_TYPE_INFO) {
			//if context menu is on, dont allow it
			if (!IsContextMenuOn()) {
				m_context->OnDwellStart(event);
			}
		}
	}
}

void LEditor::OnDwellEnd(wxScintillaEvent & event)
{
	m_context->OnDwellEnd(event);
	m_context->OnDbgDwellEnd(event);
}

void LEditor::OnCallTipClick(wxScintillaEvent& event)
{
	m_context->OnCallTipClick(event);
}

void LEditor::OnModified(wxScintillaEvent& event)
{
	if (event.GetModificationType() & wxSCI_MOD_INSERTTEXT) {
		SetDirty(true);
	}

	if (event.GetModificationType() & wxSCI_MOD_DELETETEXT) {
		SetDirty(true);
	}

	if (event.GetModificationType() & wxSCI_PERFORMED_UNDO) {
		if (GetModify() == false) {
			SetDirty(false);
		}
	}

	if (event.GetModificationType() & wxSCI_PERFORMED_REDO) {
		SetDirty(true);
	}
}

void LEditor::OnMenuCommand(wxCommandEvent &event)
{
	MenuEventHandlerPtr handler = MenuManager::Get()->GetHandler(event.GetId());

	if ( handler ) {
		handler->ProcessCommandEvent(this, event);
	}
}

void LEditor::OnUpdateUI(wxUpdateUIEvent &event)
{
	MenuEventHandlerPtr handler = MenuManager::Get()->GetHandler(event.GetId());

	if ( handler ) {
		handler->ProcessUpdateUIEvent(this, event);
	}
}

//-----------------------------------------------------------------------
// Misc functions
//-----------------------------------------------------------------------

wxChar LEditor::PreviousChar(const int& pos, int &foundPos, bool wantWhitespace)
{
	wxChar ch = 0;
	long curpos = PositionBefore( pos );
	if (curpos == 0) {
		foundPos = curpos;
		return ch;
	}

	while ( true ) {
		ch = GetCharAt( curpos );
		if (ch == wxT('\t') || ch == wxT(' ') || ch == wxT('\r') || ch == wxT('\v') || ch == wxT('\n')) {
			//if the caller is intrested in whitepsaces,
			//simply return it
			if (wantWhitespace) {
				foundPos = curpos;
				return ch;
			}

			long tmpPos = curpos;
			curpos = PositionBefore( curpos );
			if (curpos == 0 && tmpPos == curpos)
				break;
		} else {
			foundPos = curpos;
			return ch;
		}
	}
	foundPos = -1;
	return ch;
}

wxChar LEditor::NextChar( const int &pos, int &foundPos )
{
	wxChar ch = 0;
	long nextpos = pos;
	while ( true ) {
		if ( nextpos == GetLength() )
			break; // eof
		ch = GetCharAt( nextpos );
		if (ch == wxT('\t') || ch == wxT(' ') || ch == wxT('\r') || ch == wxT('\v') || ch == wxT('\n')) {
			nextpos = PositionAfter( nextpos );
			continue;
		} else {
			foundPos = nextpos;
			return ch;
		}
	}
	foundPos = -1;
	return ch;
}


int LEditor::FindString (const wxString &str, int flags, const bool down, long pos)
{
	// initialize direction
	if ( down ) {
		SetTargetStart (pos);
		SetTargetEnd(GetLength());
	} else {
		SetTargetStart (pos);
		SetTargetEnd(0);
	}
	SetSearchFlags(flags);

	// search string
	int _pos = SearchInTarget(str);
	if (_pos >= 0) return _pos;
	else return -1;
}

bool LEditor::MatchBraceBack(const wxChar& chCloseBrace, const long &pos, long &matchedPos)
{
	if (pos<=0)
		return false;

	wxChar chOpenBrace;

	switch (chCloseBrace) {
	case '}':
		chOpenBrace = '{';
		break;
	case ')':
		chOpenBrace = '(';
		break;
	case ']':
		chOpenBrace = '[';
		break;
	case '>':
		chOpenBrace = '<';
		break;
	default:
		return false;
	}

	long nPrevPos = pos;
	wxChar ch;
	int depth = 1;

	// We go backward
	while (true) {
		if (nPrevPos == 0)
			break;
		nPrevPos = PositionBefore(nPrevPos);

		// Make sure we are not in a comment
		if (m_context->IsCommentOrString(nPrevPos))
			continue;

		ch = GetCharAt(nPrevPos);
		if (ch == chOpenBrace) {
			// Dec the depth level
			depth--;
			if (depth == 0) {
				matchedPos = nPrevPos;
				return true;
			}
		} else if (ch == chCloseBrace) {
			// Inc depth level
			depth++;
		}
	}
	return false;
}

//--------------------------------------------------------
// Brace match
//--------------------------------------------------------
void LEditor::MatchBraceAndSelect(bool selRegion)
{
	// Get current position
	long pos = GetCurrentPos();

	if (GetCharAt(pos) == '{' && !m_context->IsCommentOrString(pos)) {
		BraceMatch(selRegion);
		return;
	}

	if (GetCharAt(PositionBefore(pos)) == '{' && !m_context->IsCommentOrString(PositionBefore(pos))) {
		SetCurrentPos(PositionBefore(pos));
		BraceMatch(selRegion);
		return;
	}

	if (GetCharAt(pos) == '}' && !m_context->IsCommentOrString(pos)) {
		BraceMatch(selRegion);
		return;
	}

	if (GetCharAt(PositionBefore(pos)) == '}' && !m_context->IsCommentOrString(PositionBefore(pos))) {
		SetCurrentPos(PositionBefore(pos));
		BraceMatch(selRegion);
		return;
	}
}

void LEditor::BraceMatch(long pos)
{
	// Check if we have a match
	long endPos = wxScintilla::BraceMatch(pos);
	if (endPos != wxSCI_INVALID_POSITION) {
		// Highlight indent guide if exist
		wxScintilla::BraceHighlight(pos, endPos);
	} else {
		wxScintilla::BraceBadLight(pos);
	}
}

void LEditor::BraceMatch(const bool& bSelRegion)
{
	// Check if we have a match
	long endPos = wxScintilla::BraceMatch(GetCurrentPos());
	if (endPos != wxSCI_INVALID_POSITION) {
		// Highlight indent guide if exist
		long startPos = GetCurrentPos();
		if (bSelRegion) {
			// Select the range
			if (endPos > startPos) {
				SetSelectionEnd(PositionAfter(endPos));
				SetSelectionStart(startPos);
			} else {
				SetSelectionEnd(PositionAfter(startPos));
				SetSelectionStart(endPos);
			}
		} else {
			SetSelectionEnd(endPos);
			SetSelectionStart(endPos);
			SetCurrentPos(endPos);
		}
		EnsureCaretVisible();
	}
}

void LEditor::SetActive()
{
	// if the find and replace dialog is opened, set ourself
	// as the event owners
	if ( m_findReplaceDlg ) {
		m_findReplaceDlg->SetEventOwner(GetEventHandler());
	}

	SetFocus();
//	SetSCIFocus(true);
}

// Popup a Find/Replace dialog
void LEditor::DoFindAndReplace(bool isReplaceDlg)
{
	if ( m_findReplaceDlg == NULL ) {
		// Create the dialog
		m_findReplaceDlg = new FindReplaceDialog(ManagerST::Get()->GetMainFrame(), m_findReplaceData);
		m_findReplaceDlg->SetEventOwner(this->GetEventHandler());
	}

	if ( m_findReplaceDlg->IsShown() ) {
		// make sure that dialog has focus and that this instace
		// of LEditor is the owner for the events
		m_findReplaceDlg->SetEventOwner(this->GetEventHandler());
		m_findReplaceDlg->SetFocus();
		return;
	}

	// the search always starts from the current line
	m_lastMatchPos = GetCurrentPos();
	//if there is a selection, set it
	if (GetSelectedText().IsEmpty() == false) {
		//if this string does not exist in the array add it
		m_findReplaceDlg->GetData().SetFindString(GetSelectedText());
	}
	m_findReplaceDlg->Show(isReplaceDlg ? REPLACE_DLG : FIND_DLG);
}

void LEditor::OnFindDialog(wxCommandEvent& event)
{
	EditorConfig *conf = EditorConfigST::Get();
	wxEventType type = event.GetEventType();
	bool dirDown = ! (m_findReplaceDlg->GetData().GetFlags() & wxFRD_SEARCHUP ? true : false);

	if ( type == wxEVT_FRD_FIND_NEXT ) {
		FindNext(m_findReplaceDlg->GetData());
	} else if ( type == wxEVT_FRD_REPLACE ) {
		if ( !Replace() ) {
			if (dirDown) {
				long res;
				if (!conf->GetLongValue(wxT("ReplaceWrapAroundAnswer"), res)) {
					ThreeButtonDlg *dlg = new ThreeButtonDlg(NULL,
					        wxT("CodeLite reached the end of the document, Search again from the start?"),
					        wxT("CodeLite"));
					res = dlg->ShowModal();
					if (dlg->GetDontAskMeAgain()) {
						//save this answer
						conf->SaveLongValue(wxT("ReplaceWrapAroundAnswer"), res);
					}
					dlg->Destroy();
				}

			} else {
				long res;
				if (!conf->GetLongValue(wxT("ReplaceWrapAroundAnswer"), res)) {
					ThreeButtonDlg *dlg = new ThreeButtonDlg(NULL,
					        wxT("CodeLite reached the end of the document, Search again from the start?"),
					        wxT("CodeLite"));
					res = dlg->ShowModal();
					if (dlg->GetDontAskMeAgain()) {
						//save this answer
						conf->SaveLongValue(wxT("ReplaceWrapAroundAnswer"), res);
					}
					dlg->Destroy();
				}

				if (res == wxID_OK) {
					Replace();
				}
			}
		}
	}

	else if (type == wxEVT_FRD_REPLACEALL) {
		ReplaceAll();
	} else if (type == wxEVT_FRD_BOOKMARKALL) {
		MarkAll();
	}
	event.Skip();
}

void LEditor::FindNext(const FindReplaceData &data)
{
	EditorConfig *conf = EditorConfigST::Get();

	bool dirDown = ! (data.GetFlags() & wxFRD_SEARCHUP ? true : false);
	if ( !FindAndSelect(data) ) {
		if (dirDown) {
			long res;
			if (!conf->GetLongValue(wxT("FindNextWrapAroundAnswer"), res)) {
				ThreeButtonDlg *dlg = new ThreeButtonDlg(NULL,
				        wxT("CodeLite reached the end of the document, Search again from the start?"),
				        wxT("CodeLite"));
				res = dlg->ShowModal();
				if (dlg->GetDontAskMeAgain()) {
					//save this answer
					conf->SaveLongValue(wxT("FindNextWrapAroundAnswer"), res);
				}
				dlg->Destroy();
			}
			if (res == wxID_OK) {
				FindAndSelect(data);
			}
		} else {
			long res;
			if (!conf->GetLongValue(wxT("FindNextWrapAroundAnswer"), res)) {
				ThreeButtonDlg *dlg = new ThreeButtonDlg(NULL,
				        wxT("CodeLite reached the end of the document, Search again from the start?"),
				        wxT("CodeLite"));
				res = dlg->ShowModal();
				if (dlg->GetDontAskMeAgain()) {
					//save this answer
					conf->SaveLongValue(wxT("FindNextWrapAroundAnswer"), res);
				}
				dlg->Destroy();
			}
			if (res == wxID_OK) {
				FindAndSelect(data);
			}
		}
	}
}

bool LEditor::Replace()
{
	return Replace(m_findReplaceDlg->GetData());
}

bool LEditor::FindAndSelect()
{
	return FindAndSelect(m_findReplaceDlg->GetData());
}

bool LEditor::FindAndSelect(const FindReplaceData &data)
{
	bool dirDown = ! (data.GetFlags() & wxFRD_SEARCHUP ? true : false);
	int flags = GetSciSearchFlag(data);

	if(m_resetSearch) {
		m_lastMatchPos = GetCurrentPos();
		m_resetSearch = false;
	}
	int pos = FindString(data.GetFindString(), flags, dirDown, m_lastMatchPos);
	if (pos >= 0) {
		EnsureCaretVisible();
		SetSelection (pos, pos + (int)data.GetFindString().Length());

		if ( !dirDown ) {
			m_lastMatchPos = GetCurrentPos() - data.GetFindString().Length();
		} else {
			m_lastMatchPos = GetCurrentPos();
		}

		//adjust the dialog position
		if (m_findReplaceDlg && m_findReplaceDlg->IsShown()) {
			wxPoint pt = PointFromPosition(pos);
			//pt is in wxScintilla coordinates, need to convert them into
			//the frame coordinates
			wxPoint displayPt = ClientToScreen(pt);

			//check if this point is placed inside the dialog area
			wxRect rr = m_findReplaceDlg->GetScreenRect();
			if (rr.Contains(displayPt)) {
				//move the dialog a bit upward
				m_findReplaceDlg->Move(rr.x, displayPt.y - rr.GetHeight());
			}
		}
		return true;
	} else {
		if ( dirDown ) {
			m_lastMatchPos = 0;
		} else {
			m_lastMatchPos = GetLength();
		}
		return false;
	}
}

bool LEditor::Replace(const FindReplaceData &data)
{
	wxString replaceString = data.GetReplaceString();
	wxString findString    = data.GetFindString();
	wxString selection = GetSelectedText();

	SetSearchFlags( GetSciSearchFlag(data));
	TargetFromSelection();

	if (SearchInTarget( findString ) != -1) {
		// the selection contains the searched string
		// do the replace
		ReplaceTarget( replaceString );
		m_lastMatchPos += replaceString.Length();
	}

	//  and find another match in the document
	return FindAndSelect();
}

int LEditor::GetSciSearchFlag(const FindReplaceData &data)
{
	size_t flags = 0;
	size_t wxflags = data.GetFlags();
	wxflags & wxFRD_MATCHWHOLEWORD ? flags |= wxSCI_FIND_WHOLEWORD : flags = flags;
	wxflags & wxFRD_MATCHCASE ? flags |= wxSCI_FIND_MATCHCASE : flags = flags;
	wxflags & wxFRD_REGULAREXPRESSION ? flags |= wxSCI_FIND_REGEXP : flags = flags;
	return static_cast<int>(flags);
}

//----------------------------------------------
// Bookmarks
//----------------------------------------------
void LEditor::AddMarker()
{
	int nPos = GetCurrentPos();
	int nLine = LineFromPosition(nPos);
	MarkerAdd(nLine, 0x7);
}

void LEditor::DelMarker()
{
	int nPos = GetCurrentPos();
	int nLine = LineFromPosition(nPos);
	MarkerDelete(nLine, 0x7);
}

void LEditor::ToggleMarker()
{
	// Add/Remove marker
	// First we check if we already have a marker
	int nPos = GetCurrentPos();
	int nLine = LineFromPosition(nPos);
	int nBits = MarkerGet(nLine);
	bool bHasMraker = nBits & 128 ? true : false;

	if ( !bHasMraker )
		//Delete it
		AddMarker();
	else
		//Add one
		DelMarker();
}

void LEditor::DelAllMarkers()
{
	// Delete all markers from the view
	MarkerDeleteAll(0x7);
}

void LEditor::FindNextMarker()
{
	int nPos = GetCurrentPos();
	int nLine = LineFromPosition(nPos);
	int mask = 128;
	int nFoundLine = MarkerNext(nLine + 1, mask);
	if (nFoundLine >= 0) {
		// mark this place before jumping to next marker
		GotoLine(nFoundLine);
	} else {
		//We reached the last marker, try again from top
		int nLine = LineFromPosition(0);
		int nFoundLine = MarkerNext(nLine, mask);
		if (nFoundLine >= 0) {
			GotoLine(nFoundLine);
		}
	}
}

void LEditor::FindPrevMarker()
{
	int nPos = GetCurrentPos();
	int nLine = LineFromPosition(nPos);
	int mask = 128;
	int nFoundLine = MarkerPrevious(nLine - 1, mask);
	if (nFoundLine >= 0) {
		GotoLine(nFoundLine);
	} else {
		//We reached first marker, try again from button
		int nFileSize = GetLength();
		int nLine = LineFromPosition(nFileSize);
		int nFoundLine = MarkerPrevious(nLine, mask);
		if (nFoundLine >= 0) {
			GotoLine(nFoundLine);
		}
	}
}

bool LEditor::ReplaceAll()
{
	int occur = 0;
	wxString findWhat = m_findReplaceDlg->GetData().GetFindString();
	wxString replaceWith = m_findReplaceDlg->GetData().GetReplaceString();
	int flags = GetSciSearchFlag(m_findReplaceDlg->GetData());

	if (findWhat.IsEmpty()) {
		return false;
	}

	BeginUndoAction();
	long pos = 0;

	// Save the caret position
	long savedPos = GetCurrentPos();

	SetCaretAt(0);
	while ((pos = FindString(findWhat, flags, true, pos)) >= 0) {
		occur++;
		ReplaceTarget (replaceWith);
		pos += (int)replaceWith.Length();
		SetCaretAt(pos);
	}

	// Restore the caret
	SetCaretAt(savedPos);

	EndUndoAction();

	wxString message;
	message << wxT("Replacements: ") << occur;
	m_findReplaceDlg->SetReplacementsMessage(message);
	return occur > 0;
}

bool LEditor::MarkAll()
{
	long pos = wxSCI_INVALID_POSITION;
	long savedPos = GetCurrentPos();
	wxString findWhat = m_findReplaceDlg->GetData().GetFindString();
	int flags = GetSciSearchFlag(m_findReplaceDlg->GetData());

	if (findWhat.IsEmpty())
		return false;

	// Set the cursor to start
	SetCaretAt(0);

	while ((pos = FindString (findWhat, flags, true, pos)) >= 0) {
		MarkerAdd(LineFromPosition(pos), 0x7);
		pos = PositionAfter(pos);
		SetCaretAt(pos);
	}

	// Resttore the caret
	SetCaretAt(savedPos);
	return true;
}

void LEditor::ReloadFile()
{
	if (m_fileName.GetFullPath().IsEmpty() == true || m_fileName.GetFullPath().StartsWith(wxT("Untitled")))
		return;

	wxString text;
	ReadFileWithConversion(m_fileName.GetFullPath(), text);
	SetText( text );

	m_modifyTime = GetFileModificationTime(m_fileName.GetFullPath());
	SetDirty(false);
	SetSavePoint();
	EmptyUndoBuffer();

	//update breakpoints
	UpdateBreakpoints();
	UpdateColours();
}

void LEditor::SetEditorText(const wxString &text)
{
	SetText( text );
	SetDirty(true);

	//update breakpoints
	UpdateBreakpoints();
}

void LEditor::Create(const wxString &project, const wxFileName &fileName)
{
	// set the file name
	SetFileName(fileName);
	// set the project name
	SetProject(project);
	// reload the file from disk
	ReloadFile();
	// mark this editor as non-modified to avoid non-needed confirm dialogs
	SetSavePoint();
	EmptyUndoBuffer();
	// let the editor choose the syntax highlight to use according to file extension
	// and set the editor properties to default
	RestoreDefaults();
}

void LEditor::InsertTextWithIndentation(const wxString &text, int lineno)
{
	//keep the page idnetation level
	wxString textToInsert(text);
	int lineStartPos = PositionFromLine(lineno);
	int indentSize = GetIndent();
	int indent = GetLineIndentation(lineno);
	if (GetTabIndents()) {
		indent = indent / indentSize;
	}

	wxStringTokenizer tkz(textToInsert, wxT("\n"));
	textToInsert.Clear();
	while (tkz.HasMoreTokens()) {
		for (int i=0; i<indent; i++)
			textToInsert << wxT("\t");
		textToInsert << tkz.NextToken() << wxT("\n");
	}
	InsertText(lineStartPos, textToInsert);
}

void LEditor::OnContextMenu(wxContextMenuEvent &event)
{
	if (m_rightClickMenu) {
		CallTipCancel();
		wxString selectText = GetSelectedText();
		wxPoint pt = event.GetPosition();
		wxPoint clientPt = ScreenToClient(pt);
		int closePos = PositionFromPointClose(clientPt.x, clientPt.y);
		if (closePos != wxNOT_FOUND) {
			if (!selectText.IsEmpty()) {
				//If the selection text is placed under the cursor,
				//keep it selected, else, unselect the text
				//and place the caret to be under cursor
				int selStart = GetSelectionStart();
				int selEnd = GetSelectionEnd();
				if (closePos < selStart || closePos > selEnd) {
					//cursor is not over the selected text, unselect and re-position caret
					SetCaretAt(closePos);
				}
			} else {
				//no selection, just place the caret
				SetCaretAt(closePos);
			}
		}

		//Let the context add it dynamic content
		m_context->AddMenuDynamicContent(m_rightClickMenu);

		//add the debugger (if currently running) to add its dynamic content
		IDebugger *debugger = DebuggerMgr::Get().GetActiveDebugger();
		if (debugger && debugger->IsRunning()) {
			AddDebuggerContextMenu(m_rightClickMenu);
		}

		//turn the popupIsOn value to avoid annoying
		//calltips from firing while our menu is popped
		m_popupIsOn = true;

		//let the plugins hook their content
		PluginManager::Get()->HookPopupMenu(m_rightClickMenu, MenuTypeEditor);

		//Popup the menu
		PopupMenu(m_rightClickMenu);

		//let the plugins remove their hooked content
		PluginManager::Get()->UnHookPopupMenu(m_rightClickMenu, MenuTypeEditor);

		m_popupIsOn = false;

		//Let the context remove the dynamic content
		m_context->RemoveMenuDynamicContent(m_rightClickMenu);
		RemoveDebuggerContextMenu(m_rightClickMenu);
	}
	event.Skip();
}

void LEditor::OnKeyDown(wxKeyEvent &event)
{
	//let the context process it as well
	m_context->OnKeyDown(event);
	if (event.GetKeyCode() == WXK_NUMPAD_DELETE) {
		event.Skip(false);
	}
}

void LEditor::OnLeftDown(wxMouseEvent &event)
{
	m_resetSearch = true;
	//emulate here VS like selection with mouse and ctrl key
	if (event.m_controlDown) {
		long pos = PositionFromPointClose(event.GetX(), event.GetY());
		if (pos == wxSCI_INVALID_POSITION) {
			event.Skip();
			return;
		}

		long start = WordStartPosition(pos, true);
		long end   = WordEndPosition(pos, true);
		if (start == end) {
			//pass it on
			event.Skip();
			return;
		}
		//select the word
		SetSelectionStart(start);
		SetSelectionEnd(end);
		//make the caret visible (if not, scroll to it)
		EnsureCaretVisible();
		return;
	}
	event.Skip();
}

void LEditor::OnPopupMenuUpdateUI(wxUpdateUIEvent &event)
{
	//pass it to the context
	m_context->ProcessEvent(event);
}

BrowseRecord LEditor::CreateBrowseRecord()
{
	// Remember this position before skipping to the next one
	BrowseRecord record;
	record.lineno = LineFromPosition(GetCurrentPos())+1; // scintilla counts from zero, while tagentry from 1
	record.filename = GetFileName().GetFullPath();
	record.project = GetProject();

	//if the file is part of the workspace set the project name
	//else, open it with empty project
	record.position = GetCurrentPos();
	return record;
}

void LEditor::DelBreakpoint(const BreakpointInfo &bp)
{
	if (DebuggerMgr::Get().DelBreakpoint(bp)) {
		DelBreakpointMarker(bp.lineno-1);
	}
	Frame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
}

void LEditor::DelBreakpoint()
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning() && !ManagerST::Get()->DbgCanInteract()) {
		return;
	}

	BreakpointInfo bp;
	bp.file = GetFileName().GetFullPath();
	bp.lineno = LineFromPosition(GetCurrentPos())+1;
	DelBreakpoint(bp);
}

void LEditor::SetBreakpointMarker(int lineno)
{
	MarkerAdd(lineno, 0x8);
	Frame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
}

void LEditor::DelBreakpointMarker(int lineno)
{
	MarkerDelete(lineno, 0x8);
	Frame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
}

void LEditor::DelAllBreakpointMarkers()
{
	int mask(0);
	mask |= 256;
	int lineno = MarkerNext(0, mask);
	while (lineno >= 0) {
		MarkerDelete(lineno, 0x8);
		long startPos = PositionFromLine(lineno);
		long endPos   = GetLineEndPosition(lineno);
		Colourise(startPos, endPos);
		lineno = MarkerNext(0, mask);
	}
}

void LEditor::HighlightLine(int lineno)
{
	int sci_line = lineno - 1;
	if (GetLineCount() < sci_line -1) {
		sci_line = GetLineCount() - 1;
	}
	MarkerAdd(sci_line, 0x9);
}

void LEditor::UnHighlightAll()
{
	MarkerDeleteAll(0x9);
}

void LEditor::ToggleBreakpoint()
{
	BreakpointInfo bp;
	bp.file = GetFileName().GetFullPath();
	bp.lineno = LineFromPosition(GetCurrentPos())+1;
	ToggleBreakpoint(bp);
}

void LEditor::ToggleBreakpoint(const BreakpointInfo &bp)
{
	bool contIsNeeded(false);

	Manager *mgr = ManagerST::Get();
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning() && !mgr->DbgCanInteract()) {
		mgr->DbgDoSimpleCommand(DBG_PAUSE);
		contIsNeeded = true;
	}

	//we are assuming that this call is made from a caller,
	//so the line numbers needs to be adjusted (reduce by 1)
	int mask = MarkerGet(bp.lineno-1);
	if (mask & 256) {
		//we have breakpoint
		DelBreakpoint(bp);
	} else {
		//no breakpoint at this line, add one
		DoSetBreakpoint(bp);
	}

	if (contIsNeeded) {
		//resume execution of the debugger
		mgr->DbgStart();
	}
}

void LEditor::AddDebuggerContextMenu(wxMenu *menu)
{
	if (!ManagerST::Get()->DbgCanInteract()) {
		return;
	}

	wxString word = GetSelectedText();
	if (word.IsEmpty()) {
		word = GetWordAtCaret();
		if (word.IsEmpty()) {
			return;
		}
	}

	m_customCmds.clear();

	wxMenuItem *item;
	item = new wxMenuItem(menu, wxID_SEPARATOR);
	menu->Prepend(item);
	m_dynItems.push_back(item);


	wxString menuItemText;

	menuItemText.Clear();
	menuItemText << wxT("Quick Watch '") << word << wxT("'");
	item = new wxMenuItem(menu, wxNewId(), menuItemText);
	menu->Prepend(item);
	menu->Connect(item->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(LEditor::OnDbgQuickWatch), NULL, this);
	m_dynItems.push_back(item);

	menuItemText.Clear();
	menuItemText << wxT("Add Watch '") << word << wxT("'");
	item = new wxMenuItem(menu, wxNewId(), menuItemText);
	menu->Prepend(item);
	menu->Connect(item->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(LEditor::OnDbgAddWatch), NULL, this);
	m_dynItems.push_back(item);

	//---------------------------------------------
	//add custom commands
	//---------------------------------------------
	DebuggerSettingsData data;
	DebuggerConfigTool::Get()->ReadObject(wxT("DebuggerCommands"), &data);
	std::vector<DebuggerCmdData> cmds = data.GetCmds();

	for (size_t i=0; i<cmds.size(); i++) {
		if (i == 0) {
			item = new wxMenuItem(menu, wxID_SEPARATOR);
			menu->Prepend(item);
			m_dynItems.push_back(item);
		}

		DebuggerCmdData cmd = cmds.at(i);
		menuItemText.Clear();
		menuItemText << wxT("Watch '") << word << wxT("' as '") << cmd.GetName() << wxT("'");
		item = new wxMenuItem(menu, wxNewId(), menuItemText);
		menu->Prepend(item);
		menu->Connect(item->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(LEditor::OnDbgCustomWatch), NULL, this);
		m_dynItems.push_back(item);
		m_customCmds[item->GetId()] = cmd.GetCommand();
	}
}

void LEditor::RemoveDebuggerContextMenu(wxMenu *menu)
{
	std::vector<wxMenuItem*>::iterator iter = m_dynItems.begin();
	for (; iter != m_dynItems.end(); iter++) {
		menu->Destroy(*iter);
	}
	m_dynItems.clear();
	m_customCmds.clear();
}

void LEditor::OnDbgQuickWatch(wxCommandEvent &event)
{
	wxUnusedVar(event);

	wxString word = GetSelectedText();
	if (word.IsEmpty()) {
		word = GetWordAtCaret();
		if (word.IsEmpty()) {
			return;
		}
	}
	ManagerST::Get()->DbgQuickWatch(word);
}

void LEditor::OnDbgAddWatch(wxCommandEvent &event)
{
	wxUnusedVar(event);

	wxString word = GetSelectedText();
	if (word.IsEmpty()) {
		word = GetWordAtCaret();
		if (word.IsEmpty()) {
			return;
		}
	}
	Frame::Get()->GetDebuggerPane()->GetWatchesTable()->AddExpression(word);
	Frame::Get()->GetDebuggerPane()->SelectTab(DebuggerPane::WATCHES);
	Frame::Get()->GetDebuggerPane()->GetWatchesTable()->RefreshValues();
}

void LEditor::OnDbgCustomWatch(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxString word = GetSelectedText();
	if (word.IsEmpty()) {
		word = GetWordAtCaret();
		if (word.IsEmpty()) {
			return;
		}
	}

	//find the custom command to run
	std::map<int, wxString>::iterator iter = m_customCmds.find(event.GetId());
	if (iter != m_customCmds.end()) {
		//Replace $(Variable) with the actual string
		wxString command = iter->second;
		command.Replace(wxT("$(Variable)"), word);

		Frame::Get()->GetDebuggerPane()->GetWatchesTable()->AddExpression(command);
		Frame::Get()->GetDebuggerPane()->SelectTab(DebuggerPane::WATCHES);
		Frame::Get()->GetDebuggerPane()->GetWatchesTable()->RefreshValues();
	}
}

void LEditor::UpdateColours()
{
	if ( TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_COLOUR_VARS ||
	        TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_COLOUR_PROJ_TAGS) {
		m_context->OnFileSaved();
	} else {
		SetKeyWords(1, wxEmptyString);
		SetKeyWords(2, wxEmptyString);
		SetKeyWords(3, wxEmptyString);
	}

	//colourise the document
	Colourise(0, wxSCI_INVALID_POSITION);
}
