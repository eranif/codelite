//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cl_editor.cpp
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

#include "cl_editor.h"
#include "jobqueue.h"
#include "stringhighlighterjob.h"
#include "job.h"
#include "drawingutils.h"
#include "cc_box.h"
#include "stringsearcher.h"
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
#include "quickfindbar.h"

// fix bug in wxscintilla.h
#ifdef EVT_SCI_CALLTIP_CLICK
#undef EVT_SCI_CALLTIP_CLICK
#define EVT_SCI_CALLTIP_CLICK(id, fn)          DECLARE_EVENT_TABLE_ENTRY (wxEVT_SCI_CALLTIP_CLICK,          id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxScintillaEventFunction, & fn ), (wxObject *) NULL),
#endif

#define NUMBER_MARGIN_ID 	0
#define SEP_MARGIN_ID 		1
#define SYMBOLS_MARGIN_ID 	2
#define FOLD_MARGIN_ID 		3

#define USER_INDICATOR 				3
#define HYPERLINK_INDICATOR 		4

const wxEventType wxEVT_CMD_UPDATE_STATUS_BAR = wxNewEventType();

#ifndef wxScintillaEventHandler
#define wxScintillaEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxScintillaEventFunction, &func)
#endif

//debugger line marker xpm
extern char *arrow_right_green_xpm[];
extern char *stop_xpm[];

extern unsigned int UTF8Length(const wchar_t *uptr, unsigned int tlen);

BEGIN_EVENT_TABLE(LEditor, wxScintilla)

	EVT_SCI_CHARADDED(wxID_ANY, LEditor::OnCharAdded)
	EVT_SCI_MARGINCLICK(wxID_ANY, LEditor::OnMarginClick)
	EVT_SCI_CALLTIP_CLICK(wxID_ANY, LEditor::OnCallTipClick)
	EVT_SCI_DWELLEND(wxID_ANY, LEditor::OnDwellEnd)
	EVT_SCI_MODIFIED(wxID_ANY, LEditor::OnModified)
	EVT_SCI_UPDATEUI(wxID_ANY, LEditor::OnSciUpdateUI)
	EVT_CONTEXT_MENU(LEditor::OnContextMenu)
	EVT_KEY_DOWN(LEditor::OnKeyDown)
	EVT_LEFT_DOWN(LEditor::OnLeftDown)
	EVT_MIDDLE_DOWN(LEditor::OnMiddleDown)
	EVT_MIDDLE_UP(LEditor::OnMiddleUp)
	EVT_LEFT_UP(LEditor::OnLeftUp)
	EVT_LEAVE_WINDOW(LEditor::OnLeaveWindow)
	EVT_SCI_DOUBLECLICK(wxID_ANY, LEditor::OnLeftDClick)
	EVT_COMMAND(wxID_ANY, wxEVT_FRD_FIND_NEXT, LEditor::OnFindDialog)
	EVT_COMMAND(wxID_ANY, wxEVT_FRD_REPLACE, LEditor::OnFindDialog)
	EVT_COMMAND(wxID_ANY, wxEVT_FRD_REPLACEALL, LEditor::OnFindDialog)
	EVT_COMMAND(wxID_ANY, wxEVT_FRD_BOOKMARKALL, LEditor::OnFindDialog)
	EVT_COMMAND(wxID_ANY, wxEVT_FRD_CLOSE, LEditor::OnFindDialog)
	EVT_COMMAND(wxID_ANY, wxEVT_FRD_CLEARBOOKMARKS, LEditor::OnFindDialog)
	EVT_COMMAND(wxID_ANY, wxEVT_CMD_JOB_STATUS_VOID_PTR, LEditor::OnHighlightThread)
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
		, m_popupIsOn(false)
		, m_modifyTime(0)
		, m_ccBox(NULL)
		, m_isVisible(true)
		, m_hyperLinkIndicatroStart(wxNOT_FOUND)
		, m_hyperLinkIndicatroEnd(wxNOT_FOUND)
		, m_hyperLinkType(wxID_NONE)
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

//	SetDropTarget(new FileDropTarget());
	RestoreDefaults();

	Connect(wxEVT_SCI_DWELLSTART, wxScintillaEventHandler(LEditor::OnDwellStart), NULL, this);

	// clear Ctrl+D
	CmdKeyClear(wxT('D'), wxSCI_SCMOD_CTRL);
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
	DoSetCaretAt(pos);
	EnsureCaretVisible();
}

/// Setup some scintilla properties
void LEditor::SetProperties()
{
	m_rightClickMenu = m_context->GetMenu();
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
	CallTipUseStyle(1);

	m_hightlightMatchedBraces = options->GetHighlightMatchedBraces();
	if(!m_hightlightMatchedBraces)
		wxScintilla::BraceHighlight(wxSCI_INVALID_POSITION, wxSCI_INVALID_POSITION);

	SetViewWhiteSpace(options->GetShowWhitspaces());
	SetMouseDwellTime(250);
	SetProperty(wxT("fold"), wxT("1"));
	SetProperty(wxT("fold.html"), wxT("1"));
	SetProperty(wxT("styling.within.preprocessor"), wxT("1"));
	SetProperty(wxT("fold.comment"), wxT("1"));

	SetProperty(wxT("fold.at.else"), options->GetFoldAtElse() ? wxT("1") : wxT("0"));
	SetProperty(wxT("fold.preprocessor"), options->GetFoldPreprocessor() ? wxT("1") : wxT("0"));
	SetProperty(wxT("fold.compact"), options->GetFoldCompact() ? wxT("1") : wxT("0"));

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

	// Line number margin
	int pixelWidth = 4 + 5*TextWidth(wxSCI_STYLE_LINENUMBER, wxT("9"));
	SetMarginWidth(NUMBER_MARGIN_ID, options->GetDisplayLineNumbers() ? pixelWidth : 0);

	SetMarginWidth(SEP_MARGIN_ID, 1);	// Symbol margin which acts as separator

	int ww = options->GetDisplayFoldMargin() ? 16 : 0;
	SetMarginWidth(FOLD_MARGIN_ID, ww);	// Fold margin
//	StyleSetForeground(wxSCI_STYLE_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));

	// Mark fold margin & symbols margins as sensetive
	SetMarginSensitive(FOLD_MARGIN_ID, true);
	SetMarginSensitive(SYMBOLS_MARGIN_ID, true);

	// Right margin
	SetEdgeMode(options->GetEdgeMode());
	SetEdgeColumn(options->GetEdgeColumn());
	SetEdgeColour(options->GetEdgeColour());

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

	CallTipSetBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
	CallTipSetForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));

#ifdef __WXMAC__
	// turning off these two greatly improves performance
	// on Mac
	SetTwoPhaseDraw(false);
	SetBufferedDraw(false);
#else
	SetTwoPhaseDraw(true);
	SetBufferedDraw(true);
#endif

    //indentation settings
	SetTabIndents(true);
	SetBackSpaceUnIndents (true);
	SetUseTabs(options->GetIndentUsesTabs());
	SetTabWidth(options->GetTabWidth());
	SetIndent(options->GetIndentWidth());
	SetIndentationGuides(options->GetShowIndentationGuidelines());
    
	SetLayoutCache(wxSCI_CACHE_DOCUMENT);

	size_t frame_flags = Frame::Get()->GetFrameGeneralInfo().GetFlags();
	SetViewEOL(frame_flags & CL_SHOW_EOL ? true : false);

	//if no right click menu is provided by the context, use scintilla default
	//right click menu
	UsePopUp(m_rightClickMenu ? false : true);

#ifdef __WXMAC__
	IndicatorSetUnder(1, false);
	IndicatorSetUnder(2, false);
	IndicatorSetUnder(HYPERLINK_INDICATOR, false);
#else
	IndicatorSetUnder(1, true);
	IndicatorSetUnder(2, true);
	IndicatorSetUnder(HYPERLINK_INDICATOR, true);
#endif

	SetUserIndicatorStyleAndColour(wxSCI_INDIC_SQUIGGLE, wxT("RED"));

	wxColour col2(wxT("LIGHT BLUE"));
	wxString val2 = EditorConfigST::Get()->GetStringValue(wxT("WordHighlightColour"));
	if (val2.IsEmpty() == false) {
		col2 = wxColour(val2);
	}

	IndicatorSetForeground(1, options->GetBookmarkBgColour());
	IndicatorSetForeground(2, col2);
	IndicatorSetStyle(HYPERLINK_INDICATOR, wxSCI_INDIC_PLAIN);
}

void LEditor::SetDirty(bool dirty)
{
	if ( dirty ) {
		if ( GetIsVisible() ) {
			if ( !ManagerST::Get()->GetPageTitle(this).StartsWith(wxT("*")) ) {

				ManagerST::Get()->SetPageTitle(this, wxT("*") + ManagerST::Get()->GetPageTitle(this));

				// update the main frame's title as well
				if (ManagerST::Get()->GetActiveEditor() == this) {
					Frame::Get()->SetFrameTitle(this);
				}
			}
		}
	} else {
		if ( GetIsVisible() ) {
			ManagerST::Get()->SetPageTitle(this, GetFileName().GetFullName());

			if (ManagerST::Get()->GetActiveEditor() == this) {
				Frame::Get()->SetFrameTitle(this);
			}
		}
	}
}

void LEditor::OnCharAdded(wxScintillaEvent& event)
{
	// set the page title as dirty
	SetDirty(true);

	// get the word and select it in the completion box
	if (IsCompletionBoxShown()) {
		int pos = WordStartPosition(GetCurrentPos(), true);
		wxString word = GetTextRange(pos, GetCurrentPos());

		if ( word.IsEmpty() ) {
			HideCompletionBox();
		} else {
			m_ccBox->SelectWord(word);
		}
	}

	// make sure line is visible
	int curLine = LineFromPosition(GetCurrentPos());
	if ( !GetFoldExpanded(curLine) ) {
		ToggleFold(curLine);
	}

	// Always do auto-indentation
	if (event.GetKey() == ':' || event.GetKey() == '}' || event.GetKey() == '\n')
		m_context->AutoIndent(event.GetKey());

	switch ( event.GetKey() ) {
	case ':':
	case '.':
	case '>':
	case '(': {
			if (m_context->IsCommentOrString(GetCurrentPos()) == false) {
				CodeComplete();
			}
			break;
		}
	case ')': {
			// dismiss the current calltip
			m_context->CallTipCancel();
			// display the next one
			ShowFunctionTipFromCurrentPos();
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

	if ( IsCompletionBoxShown() == false ) {
		// display the keywords completion box only if user typed more than 2
		// chars && the caret is placed at the end of that word
		long startPos = WordStartPosition(GetCurrentPos(), true);
		if (GetWordAtCaret().Len() >= 2 && GetCurrentPos() - startPos >= 2 ) {
			m_context->OnUserTypedXChars(GetWordAtCaret());
		}
	}
	event.Skip();
}

void LEditor::OnSciUpdateUI(wxScintillaEvent &event)
{
	// Get current position
	long pos = GetCurrentPos();

	//ignore << and >>
	int charAfter  = SafeGetChar(PositionAfter(pos));
	int charBefore = SafeGetChar(PositionBefore(pos));
	int beforeBefore = SafeGetChar(PositionBefore(PositionBefore(pos)));
	int charCurrnt = SafeGetChar(pos);

	wxString sel_text = GetSelectedText();
	if (m_hightlightMatchedBraces) {
		if ( sel_text.IsEmpty() == false) {
			wxScintilla::BraceHighlight(wxSCI_INVALID_POSITION, wxSCI_INVALID_POSITION);
		} else if (	charCurrnt == '<' && charAfter == '<' 	||	//<<
		            charCurrnt == '<' && charBefore == '<' 	||	//<<
		            charCurrnt == '>' && charAfter == '>' 	||	//>>
		            charCurrnt == '>' && charBefore == '>'  ||	//>>
		            beforeBefore == '<' && charBefore == '<'||	//<<
		            beforeBefore == '>' && charBefore == '>'||	//>>
		            beforeBefore == '-' && charBefore == '>'||	//->
		            charCurrnt == '>' && charBefore == '-'	) {	//->
			wxScintilla::BraceHighlight(wxSCI_INVALID_POSITION, wxSCI_INVALID_POSITION);
		} else {

			if ((charCurrnt == '{' || charCurrnt == '[' || GetCharAt(pos) == '<' || charCurrnt == '(') && !m_context->IsCommentOrString(pos)) {
				BraceMatch((long)pos);
			} else if ((charBefore == '{' || charBefore == '<' || charBefore == '[' || charBefore == '(') && !m_context->IsCommentOrString(PositionBefore(pos))) {
				BraceMatch((long)PositionBefore(pos));
			} else if ((charCurrnt == '}' || charCurrnt == ']' || charCurrnt == '>' || charCurrnt == ')') && !m_context->IsCommentOrString(pos)) {
				BraceMatch((long)pos);
			} else if ((charBefore == '}' || charBefore == '>' || charBefore == ']' ||charBefore == ')') && !m_context->IsCommentOrString(PositionBefore(pos))) {
				BraceMatch((long)PositionBefore(pos));
			} else {
				wxScintilla::BraceHighlight(wxSCI_INVALID_POSITION, wxSCI_INVALID_POSITION);
			}
		}
	}
	//update line number
	wxString message;
	message << wxT("Ln ") << LineFromPosition(pos)+1 << wxT(",  Col ") << GetColumn(pos) << wxT(",  Pos ") << pos << wxT(",  Style ") << GetStyleAt(pos);
	SetStatusBarMessage(message, 2);

	switch ( GetEOLMode() ) {
	case wxSCI_EOL_CR:
		SetStatusBarMessage(wxT("EOL Mode: Mac"), 3);
		break;
	case wxSCI_EOL_CRLF:
		SetStatusBarMessage(wxT("EOL Mode: Dos/Windows"), 3);
		break;
	default:
		SetStatusBarMessage(wxT("EOL Mode: Unix"), 3);
		break;
	}

	if (sel_text.IsEmpty()) {
		// remove indicators
		SetIndicatorCurrent(2);
		IndicatorClearRange(0, GetLength());

#ifdef __WXMAC__
		Refresh();
#endif

	}

	RecalcHorizontalScrollbar();

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

		SetDirty(false);

		wxString projName = GetProjectName();
		if ( projName.Trim().Trim(false).IsEmpty() )
			return true;

		// clear cached file, this function does nothing if the file is not cached
		TagsManagerST::Get()->ClearCachedFile(GetFileName().GetFullPath());

		// clear all the queries which holds reference to this file
		TagsManagerST::Get()->GetWorkspaceTagsCache()->DeleteByFilename(GetFileName().GetFullPath());

		if (TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_DISABLE_AUTO_PARSING) {
			return true;
		}

		m_context->RetagFile();
	} else {
		SetDirty(false);
	}
	return true;
}

bool LEditor::SaveFileAs()
{
	// Prompt the user for a new file name
	const wxString ALL(wxT("All Files (*)|*"));
	wxFileDialog dlg(this, wxT("Save As"), m_fileName.GetPath(), m_fileName.GetFullName(), ALL,
	                 wxFD_SAVE | wxFD_OVERWRITE_PROMPT ,
	                 wxDefaultPosition);

	if (dlg.ShowModal() == wxID_OK) {
		// get the path
		wxFileName name(dlg.GetPath());
		if ( !SaveToFile(name) ) {
			wxMessageBox(_("Failed to save file"), wxT("Error"), wxOK | wxICON_ERROR);
			return false;
		}
		m_fileName = name;
		SetDirty(false);
		return true;
	}
	return false;
}

// an internal function that does the actual file writing to disk
bool LEditor::SaveToFile(const wxFileName &fileName)
{

#if defined(__WXMSW__)
	DWORD dwAttrs = GetFileAttributes(fileName.GetFullPath().c_str());
	if (dwAttrs != INVALID_FILE_ATTRIBUTES) {
		if (dwAttrs & FILE_ATTRIBUTE_READONLY) {
			if (wxMessageBox(wxString::Format(wxT("'%s' has read-only attribute set\nWould you like CodeLite to try and remove it?"), fileName.GetFullPath().c_str()), wxT("CodeLite"),
			                 wxYES_NO|wxICON_QUESTION|wxCENTER) == wxYES) {
				// try to clear the read-only flag from the file
				if ( SetFileAttributes(fileName.GetFullPath().c_str(), dwAttrs & ~(FILE_ATTRIBUTE_READONLY)) == FALSE ) {
					wxMessageBox(wxString::Format(wxT("Failed to open file '%s' for write"), fileName.GetFullPath().c_str()), wxT("CodeLite"), wxOK|wxCENTER|wxICON_WARNING);
					return false;
				}
			} else {
				return false;
			}
		}
	}
#endif

	wxString tmp_file;
	wxFFile file(fileName.GetFullPath().GetData(), wxT("wb"));
	if (file.IsOpened() == false) {
		// Nothing to be done
		if (wxMessageBox(wxString::Format(wxT("Failed to open file '%s' for write, Override it?"), fileName.GetFullPath().GetData()), wxT("CodeLite"), wxYES_NO|wxICON_WARNING) == wxYES) {
			// try to override it
			time_t curt = GetFileModificationTime(fileName.GetFullPath());
			tmp_file << fileName.GetFullPath() << curt;
			if (file.Open(tmp_file.c_str(), wxT("wb")) == false) {
				wxMessageBox(wxString::Format(wxT("Failed to open file '%s' for write"), tmp_file.c_str()), wxT("CodeLite"), wxOK|wxICON_WARNING);
				return false;
			}
		} else {
			return false;
		}
	}

	// save the file using the user's defined encoding
	wxCSConv fontEncConv(EditorConfigST::Get()->GetOptions()->GetFileFontEncoding());
	
	// trim lines / append LF if needed
	TrimText();

	// write the content
	file.Write(GetText(), fontEncConv);
	file.Close();

	// if the saving was done to a temporary file, override it
	if (tmp_file.IsEmpty() == false) {
		if (wxRenameFile(tmp_file, fileName.GetFullPath(), true) == false) {
			wxMessageBox(wxString::Format(wxT("Failed to override read-only file")), wxT("CodeLite"), wxOK|wxICON_WARNING);
			return false;
		}
	}

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
		wxMessageBox(_("Failed to insert breakpoint"));
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

	if (event.GetModificationType() & wxSCI_PERFORMED_UNDO ||event.GetModificationType() & wxSCI_PERFORMED_REDO ) {
		if (GetModify() == false) {
			SetDirty(false);
		} else {
			SetDirty(true);
		}
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

void LEditor::RecalcHorizontalScrollbar()
{
	// recalculate and set the length of horizontal scrollbar
	int maxPixel = 0;
	int startLine = GetFirstVisibleLine();
	int endLine =  startLine + LinesOnScreen();
	if (endLine >= (GetLineCount() - 1))
		endLine--;

	for (int i = startLine; i <= endLine; i++) {
		int visibleLine = (int) DocLineFromVisible(i);         //get actual visible line, folding may offset lines
		int endPosition = GetLineEndPosition(visibleLine);      //get character position from begin
		int beginPosition = PositionFromLine(visibleLine);      //and end of line

		wxPoint beginPos = PointFromPosition(beginPosition);
		wxPoint endPos = PointFromPosition(endPosition);

		int curLen = endPos.x - beginPos.x;

		if (maxPixel < curLen) //If its the largest line yet
			maxPixel = curLen;
	}

	if (maxPixel == 0)
		maxPixel++;                                 //make sure maxPixel is valid

	int currentLength = GetScrollWidth();               //Get current scrollbar size
	if (currentLength != maxPixel) {
		//And if it is not the same, update it
		SetScrollWidth(maxPixel);
	}
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
	SetSCIFocus(true);

	m_context->SetActive();
}

// Popup a Find/Replace dialog
/**
 * \brief
 * \param isReplaceDlg
 */
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
	//if there is a selection, set it
	if (GetSelectedText().IsEmpty() == false) {
		//if this string does not exist in the array add it
		wxString Selection(GetSelectedText());
		if (isReplaceDlg) {
			if ( !Selection.Contains(wxT("\n")) ) {
				// Don't try to use a multiline selection as the 'find' token. It looks ugly and
				// it won't be what the user wants (it'll be the 'Replace in Selection' selection)
				m_findReplaceDlg->GetData().SetFindString(GetSelectedText());
			}
		} else {
			// always set the find string in 'Find' dialog
			m_findReplaceDlg->GetData().SetFindString(GetSelectedText());
		}
	}

	if (isReplaceDlg) { // Zeroise
		m_findReplaceDlg->ResetReplacedCount();
		m_findReplaceDlg->SetReplacementsMessage(frd_dontshowzeros);
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

		// Perform a "Replace" operation

		if ( !Replace() ) {
			long res(wxNOT_FOUND);

			wxString msg;
			if ( dirDown ) {
				msg << wxT("CodeLite reached the end of the document, Search again from the start?");
			} else {
				msg << wxT("CodeLite reached the end of the document, Search again from the bottom?");
			}

			if (!conf->GetLongValue(wxT("ReplaceWrapAroundAnswer"), res)) {
				ThreeButtonDlg *dlg = new ThreeButtonDlg(NULL, msg, wxT("CodeLite"));
				res = dlg->ShowModal();
				if (dlg->GetDontAskMeAgain() && res != wxID_CANCEL) {
					//save this answer
					conf->SaveLongValue(wxT("ReplaceWrapAroundAnswer"), res);
				}
				dlg->Destroy();
			}

			if ( res == wxID_OK ) {
				int saved_pos = GetCurrentPos();

				// place the caret at the new position
				if (dirDown) {
					SetCaretAt(0);
				} else {
					SetCaretAt(GetLength());
				}

				// replace again
				if ( !Replace() ) {
					// restore the caret
					SetCaretAt( saved_pos );

					// popup a message
					wxMessageBox(_("Can not find the string '") + m_findReplaceDlg->GetData().GetFindString() + wxT("'"),
					             wxT("CodeLite"),
					             wxICON_WARNING);
				}
			}
		}
	} else if (type == wxEVT_FRD_REPLACEALL) {
		ReplaceAll();
	} else if (type == wxEVT_FRD_BOOKMARKALL) {
		MarkAll();
	} else if (type == wxEVT_FRD_CLEARBOOKMARKS) {
		DelAllMarkers();
	}
}

void LEditor::FindNext(const FindReplaceData &data)
{
	EditorConfig *conf = EditorConfigST::Get();

	bool dirDown = ! (data.GetFlags() & wxFRD_SEARCHUP ? true : false);
	if ( !FindAndSelect(data) ) {
		long res(wxNOT_FOUND);
		wxString msg;
		if ( dirDown ) {
			msg << wxT("CodeLite reached the end of the document, Search again from the start?");
		} else {
			msg << wxT("CodeLite reached the end of the document, Search again from the bottom?");
		}

		if (!conf->GetLongValue(wxT("FindNextWrapAroundAnswer"), res)) {
			ThreeButtonDlg *dlg = new ThreeButtonDlg(NULL, msg, wxT("CodeLite"));
			res = dlg->ShowModal();
			if (dlg->GetDontAskMeAgain() && res != wxID_CANCEL) {
				//save this answer
				conf->SaveLongValue(wxT("FindNextWrapAroundAnswer"), res);
			}
			dlg->Destroy();
		}

		if (res == wxID_OK) {
			int saved_pos = GetCurrentPos();
			if (dirDown) {
				DoSetCaretAt(0);
			} else {
				DoSetCaretAt(GetLength());
			}

			if ( !FindAndSelect(data) ) {
				// restore the caret
				DoSetCaretAt( saved_pos );

				wxMessageBox(_("Can not find the string '") + data.GetFindString() + wxT("'"),
				             wxT("CodeLite"),
				             wxICON_WARNING);
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
	wxString findWhat = data.GetFindString();
	size_t flags = SearchFlags(data);
	int offset = GetCurrentPos();

	int dummy, dummy_len(0), dummy_c, dummy_len_c(0);
	if ( GetSelectedText().IsEmpty() == false) {
		if (flags & wxSD_SEARCH_BACKWARD) {
			// searching up
			if (StringFindReplacer::Search(GetSelectedText(), GetSelectedText().Len(), findWhat, flags, dummy, dummy_len, dummy_c, dummy_len_c) && dummy_len_c == (int)GetSelectedText().Len()) {
				// place the caret at the start of the selection so the search will skip this selected text
				int sel_start = GetSelectionStart();
				int sel_end = GetSelectionEnd();
				sel_end > sel_start ? offset = sel_start : offset = sel_end;
			}
		} else {
			// searching down
			if (StringFindReplacer::Search(GetSelectedText(), 0, findWhat, flags, dummy, dummy_len, dummy_c, dummy_len_c) && dummy_len_c == (int)GetSelectedText().Len()) {
				// place the caret at the end of the selection so the search will skip this selected text
				int sel_start = GetSelectionStart();
				int sel_end = GetSelectionEnd();
				sel_end > sel_start ? offset = sel_end : offset = sel_start;
			}
		}
	}

	int pos(0);
	int match_len(0);

	if ( StringFindReplacer::Search(GetText(), offset, findWhat, flags, pos, match_len) ) {

		int line = LineFromPosition(pos);
		if ( line >= 0 ) EnsureVisible(line);
		if ( flags & wxSD_SEARCH_BACKWARD ) {
			SetSelection(pos + match_len, pos);
		} else {
			SetSelection(pos, pos + match_len);
		}

		return true;
	}
	return false;
}

bool LEditor::Replace(const FindReplaceData &data)
{
	// the string to be replaced should be selected
	if ( GetSelectedText().IsEmpty() == false ) {
		int pos(0);
		int match_len(0);
		size_t flags = SearchFlags(data);
		if ( StringFindReplacer::Search(GetSelectedText(), 0, data.GetFindString(), flags, pos, match_len) ) {
			ReplaceSelection(data.GetReplaceString());
			m_findReplaceDlg->IncReplacedCount();
			m_findReplaceDlg->SetReplacementsMessage();
		}
	}

	//  and find another match in the document
	return FindAndSelect();
}

size_t LEditor::SearchFlags(const FindReplaceData &data)
{
	size_t flags = 0;
	size_t wxflags = data.GetFlags();
	wxflags & wxFRD_MATCHWHOLEWORD ? flags |= wxSD_MATCHWHOLEWORD : flags = flags;
	wxflags & wxFRD_MATCHCASE ? flags |= wxSD_MATCHCASE : flags = flags;
	wxflags & wxFRD_REGULAREXPRESSION ? flags |= wxSD_REGULAREXPRESSION : flags = flags;
	wxflags & wxFRD_SEARCHUP ? flags |= wxSD_SEARCH_BACKWARD : flags = flags;
	return flags;
}

//----------------------------------------------
// Folds
//----------------------------------------------
void LEditor::ToggleCurrentFold()
{
	int line = GetCurrentLine();
	if ( line >= 0 ) ToggleFold( line );
}

// If the cursor is on/in/below an open fold, collapse all. Otherwise expand all
void LEditor::FoldAll()
{
	// Colourise(0,-1);  SciTE did this here, but it doesn't seem to accomplish anything

	// First find the current fold-point, and ask it whether or not it's folded
	int lineSeek = GetCurrentLine();
	while ( true ) {
		if ( GetFoldLevel(lineSeek) & wxSCI_FOLDLEVELHEADERFLAG )  break;
		int parentline = GetFoldParent( lineSeek );  // See if we're inside a fold area
		if ( parentline >= 0 ) {
			lineSeek = parentline;
			break;
		} else lineSeek--; // Must have been between folds
		if ( lineSeek < 0 ) return;
	}
	bool expanded = GetFoldExpanded(lineSeek);

	// Now go through the whole document, toggling folds that match the original one
	int maxLine = GetLineCount();
	for (int line = 0; line < maxLine; line++) {  // For every line
		int level = GetFoldLevel(line);
		// The next statement means: If this level is a Fold start
		if ((level & wxSCI_FOLDLEVELHEADERFLAG) &&
		        (wxSCI_FOLDLEVELBASE == (level & wxSCI_FOLDLEVELNUMBERMASK))) {
			if ( GetFoldExpanded(line) == expanded ) ToggleFold( line );
		}
	}
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

	// delete all markers as well
	SetIndicatorCurrent(1);
	IndicatorClearRange(0, GetLength());

	SetIndicatorCurrent(2);
	IndicatorClearRange(0, GetLength());

	SetIndicatorCurrent(HYPERLINK_INDICATOR);
	IndicatorClearRange(0, GetLength());
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
	if (nFoundLine >= 0) EnsureVisible(nFoundLine);
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
	if (nFoundLine >= 0) EnsureVisible(nFoundLine);
}

bool LEditor::ReplaceAll()
{
	int offset( 0 );

	wxString findWhat = m_findReplaceDlg->GetData().GetFindString();
	wxString replaceWith = m_findReplaceDlg->GetData().GetReplaceString();
	size_t flags = SearchFlags(m_findReplaceDlg->GetData());

	int pos(0);
	int match_len(0);
	int posInChars(0);
	int match_lenInChars(0);

	wxString txt;
	if ( m_findReplaceDlg->GetData().GetFlags() & wxFRD_SELECTIONONLY ) {
		txt = GetSelectedText();
	} else {
		txt = GetText();
	}

	while ( StringFindReplacer::Search(txt, offset, findWhat, flags, pos, match_len, posInChars, match_lenInChars) ) {
		txt.Remove(posInChars, match_lenInChars);
		txt.insert(posInChars, replaceWith);
		m_findReplaceDlg->IncReplacedCount();
		offset = pos + UTF8Length(replaceWith, replaceWith.length()); // match_len;
	}

	// replace the buffer
	BeginUndoAction();
	long savedPos = GetCurrentPos();

	if ( m_findReplaceDlg->GetData().GetFlags() & wxFRD_SELECTIONONLY ) {
		// replace the selection
		ReplaceSelection(txt);

		// place the caret at the end of the selection
		SetCurrentPos(GetSelectionEnd());
		EnsureCaretVisible();
	} else {
		SetText(txt);
		// Restore the caret
		SetCaretAt(savedPos);
	}

	EndUndoAction();

	if ( m_findReplaceDlg->GetData().GetFlags() & wxFRD_SELECTIONONLY ) {
		m_findReplaceDlg->ResetSelectionOnlyFlag();
	}
	m_findReplaceDlg->SetReplacementsMessage();
	return m_findReplaceDlg->GetReplacedCount() > 0;
}

bool LEditor::MarkAll()
{
	wxString findWhat = m_findReplaceDlg->GetData().GetFindString();

	if (findWhat.IsEmpty()) {
		return false;
	}

	// Save the caret position
	long savedPos = GetCurrentPos();
	size_t flags = SearchFlags(m_findReplaceDlg->GetData());

	int pos(0);
	int match_len(0);

	// remove reverse search
	flags &= ~ wxSD_SEARCH_BACKWARD;
	int offset(0);

	wxString txt;
	int fixed_offset(0);
	if ( m_findReplaceDlg->GetData().GetFlags() & wxFRD_SELECTIONONLY ) {
		txt = GetSelectedText();
		fixed_offset = GetSelectionStart();
	} else {
		txt = GetText();
	}

	DelAllMarkers();

	// set the active indicator to be 1
	SetIndicatorCurrent(1);

	while ( StringFindReplacer::Search(txt, offset, findWhat, flags, pos, match_len) ) {
		MarkerAdd(LineFromPosition(fixed_offset + pos), 0x7);

		// add indicator as well
		IndicatorFillRange(fixed_offset + pos, match_len);
		offset = pos + match_len;
	}

	// Restore the caret
	SetCurrentPos(savedPos);
	EnsureCaretVisible();
	return true;
}

void LEditor::ReloadFile()
{
	HideCompletionBox();

	if (m_fileName.GetFullPath().IsEmpty() == true || m_fileName.GetFullPath().StartsWith(wxT("Untitled"))) {
		SetEOLMode(GetEOLByOS());
		return;
	}

	wxString text;
	ReadFileWithConversion(m_fileName.GetFullPath(), text);
	SetText( text );

	m_modifyTime = GetFileModificationTime(m_fileName.GetFullPath());

	SetSavePoint();
	EmptyUndoBuffer();
	SetDirty(false);

	//update breakpoints
	UpdateBreakpoints();
	UpdateColours();

	// set the EOL mode
	int eol = GetEOLByContent();
	if (eol == wxNOT_FOUND) {
		eol = GetEOLByOS();
	}
	SetEOLMode(eol);
}

void LEditor::SetEditorText(const wxString &text)
{
	HideCompletionBox();

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
	wxString textTag = FormatTextKeepIndent(text, PositionFromLine(lineno));
	InsertText(PositionFromLine(lineno), textTag);
}

wxString LEditor::FormatTextKeepIndent(const wxString &text, int pos)
{
	//keep the page idnetation level
	wxString textToInsert(text);

	int indentSize = GetIndent();
	int indent = GetLineIndentation(LineFromPosition(pos));

	wxString indentBlock;
	if (GetUseTabs()) {
		indent = indent / indentSize;
		for (int i=0; i<indent; i++) {
			indentBlock << wxT("\t");
		}
	} else {
		for (int i=0; i<indent; i++) {
			indentBlock << wxT(" ");
		}
	}

	wxString eol = GetEolString();

	textToInsert.Replace(wxT("\r"), wxT("\n"));
	wxArrayString lines = wxStringTokenize(textToInsert, wxT("\n"), wxTOKEN_STRTOK);

	textToInsert.Clear();
	for (size_t i=0; i<lines.GetCount(); i++) {
		textToInsert << indentBlock;
		textToInsert << lines.Item(i) << eol;
	}

	return textToInsert;
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
	if (IsCompletionBoxShown()) {
		switch (event.GetKeyCode()) {
		case WXK_NUMPAD_ENTER:
		case WXK_RETURN:
		case WXK_TAB:
			m_ccBox->InsertSelection();
			HideCompletionBox();
			return;

		case WXK_ESCAPE:
		case WXK_LEFT:
		case WXK_RIGHT:
		case WXK_HOME:
		case WXK_END:
		case WXK_DELETE:
		case WXK_NUMPAD_DELETE:
			HideCompletionBox();
			return;
		case WXK_UP:
			m_ccBox->Previous();
			return;
		case WXK_DOWN:
			m_ccBox->Next();
			return;
		case WXK_PAGEUP:
			m_ccBox->PreviousPage();
			return;
		case WXK_PAGEDOWN:
			m_ccBox->NextPage();
			return;
		case WXK_BACK: {

			if (event.ControlDown()) {
				HideCompletionBox();
			} else {

				wxString word = GetWordAtCaret();
				if (word.IsEmpty()) {
					HideCompletionBox();
				} else {
					word.RemoveLast();
					m_ccBox->SelectWord(word);
				}
			}
			break;
		}
		default:
			break;
		}
	}
	m_context->OnKeyDown(event);
}

void LEditor::OnLeftUp(wxMouseEvent& event)
{
	long value(0);
	EditorConfigST::Get()->GetLongValue(wxT("QuickCodeNavigationUsesMouseMiddleButton"), value);

	if (!value) {
		DoQuickJump(event, false);
	}
	event.Skip();
}

void LEditor::OnLeaveWindow(wxMouseEvent& event)
{
	m_hyperLinkIndicatroStart = wxNOT_FOUND;
	m_hyperLinkIndicatroEnd = wxNOT_FOUND;
	m_hyperLinkType = wxID_NONE;

	SetIndicatorCurrent(HYPERLINK_INDICATOR);
	IndicatorClearRange(0, GetLength());
	event.Skip();
}

void LEditor::OnMiddleUp(wxMouseEvent& event)
{
	long value(0);
	EditorConfigST::Get()->GetLongValue(wxT("QuickCodeNavigationUsesMouseMiddleButton"), value);

	if (value) {
		long pos = PositionFromPointClose(event.GetX(), event.GetY());
		if (pos != wxNOT_FOUND) {
			DoSetCaretAt(pos);
		}
		DoQuickJump(event, true);
	}
	event.Skip();
}

void LEditor::OnMiddleDown(wxMouseEvent& event)
{
	long value(0);
	EditorConfigST::Get()->GetLongValue(wxT("QuickCodeNavigationUsesMouseMiddleButton"), value);
	if (value) {
		DoMarkHyperlink(event, true);
		return;
	}
}

void LEditor::OnLeftDown(wxMouseEvent &event)
{
	// hide completion box
	HideCompletionBox();

	long value(0);
	EditorConfigST::Get()->GetLongValue(wxT("QuickCodeNavigationUsesMouseMiddleButton"), value);

	if (!value) {
		DoMarkHyperlink(event, false);
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

	item = new wxMenuItem(menu, wxNewId(), _("Run to cursor"));
	menu->Prepend(item);
	menu->Connect(item->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(LEditor::OnDbgRunToCursor), NULL, this);
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
	        TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_COLOUR_WORKSPACE_TAGS) {
		m_context->OnFileSaved();
	} else {
		if (m_context->GetName() == wxT("C++")) {
			SetKeyWords(1, wxEmptyString);
			SetKeyWords(2, wxEmptyString);
			SetKeyWords(3, wxEmptyString);
			SetKeyWords(4, wxEmptyString);
		}
	}

	//colourise the document
	int startLine = GetFirstVisibleLine();
	int endLine =  startLine + LinesOnScreen();
	if (endLine >= (GetLineCount() - 1))
		endLine--;
	
	Colourise(0, wxSCI_INVALID_POSITION);
}

int LEditor::SafeGetChar(int pos)
{
	if (pos < 0 || pos >= GetLength()) {
		return 0;
	}
	return GetCharAt(pos);
}

void LEditor::OnDragEnd(wxScintillaEvent& e)
{
//	wxUnusedVar(e);
//	wxLogMessage(wxT("OnDragEnd"));
	e.Skip();
}

void LEditor::OnDragStart(wxScintillaEvent& e)
{
//	wxUnusedVar(e);
//	wxLogMessage(wxT("OnDragStart"));
	e.Skip();
}

void LEditor::ShowCompletionBox(const std::vector<TagEntryPtr>& tags, const wxString& word, wxEvtHandler* owner)
{
	if ( m_ccBox == NULL ) {
		// create new completion box
		m_ccBox = new CCBox(this);
	}

	m_ccBox->SetAutoHide(false);
	m_ccBox->SetInsertSingleChoice(false);

	m_ccBox->Adjust();
	m_ccBox->Show(tags, word, false, owner);
}

void LEditor::ShowCompletionBox(const std::vector<TagEntryPtr>& tags, const wxString& word, bool showFullDecl, bool autoHide, bool autoInsertSingleChoice)
{
	if ( m_ccBox == NULL ) {
		// create new completion box
		m_ccBox = new CCBox(this);
	}

	m_ccBox->SetAutoHide(autoHide);
	m_ccBox->SetInsertSingleChoice(autoInsertSingleChoice);

	m_ccBox->Adjust();
	m_ccBox->Show(tags, word, showFullDecl, NULL);
}

void LEditor::HideCompletionBox()
{
	if (IsCompletionBoxShown()) {
		m_ccBox->Hide();
	}
}

int LEditor::GetCurrLineHeight()
{
	int point = GetCurrentPos();
	wxPoint pt = PointFromPosition(point);

	// calculate the line height
	int curline = LineFromPosition(point);
	int ll;
	int hh(0);
	if (curline > 0) {
		ll = curline - 1;
		int pp = PositionFromLine(ll);
		wxPoint p = PointFromPosition(pp);
		hh =  pt.y - p.y;
	} else {
		ll = curline + 1;
		int pp = PositionFromLine(ll);
		wxPoint p = PointFromPosition(pp);
		hh =  p.y - pt.y;
	}

	if (hh == 0) {
		hh = 12; // default height on most OSs
	}

	return hh;
}

void LEditor::DoHighlightWord()
{
	wxString word = GetSelectedText();
	if ( word.IsEmpty() ) {
		return;
	}

	// to make the code "smoother" we move the search task to different thread
	StringHighlighterJob *j = new StringHighlighterJob(this, GetText().c_str(), word.c_str());
	JobQueueSingleton::Instance()->PushJob( j );
}

void LEditor::HighlightWord(bool highlight)
{
	if (highlight) {
		DoHighlightWord();
	} else {
		SetIndicatorCurrent(2);
		IndicatorClearRange(0, GetLength());
	}
}

void LEditor::OnLeftDClick(wxScintillaEvent& event)
{
	long highlight_word(0);
	EditorConfigST::Get()->GetLongValue(wxT("highlight_word"), highlight_word);
	if ( GetSelectedText().IsEmpty() == false && highlight_word) {
		DoHighlightWord();
	}
	event.Skip();
}

void LEditor::OnHighlightThread(wxCommandEvent& e)
{
	// the search highlighter thread has completed the calculations, fetch the results and mark them in the editor
	std::vector<std::pair<int, int> > *matches = (std::vector<std::pair<int, int> >*) e.GetClientData();

	SetIndicatorCurrent(2);

	// clear the old markers
	IndicatorClearRange(0, GetLength());

	for (size_t i=0; i<matches->size(); i++) {
		std::pair<int, int> p = matches->at(i);
		IndicatorFillRange(p.first, p.second);
	}

	delete matches;
}

bool LEditor::IsCompletionBoxShown()
{
	return m_ccBox && m_ccBox->IsShown();
}

int LEditor::GetCurrentLine()
{
	// return the current line number
	int pos = GetCurrentPos();
	return LineFromPosition(pos);
}

void LEditor::DoSetCaretAt(long pos)
{
	SetCurrentPos(pos);
	SetSelectionStart(pos);
	SetSelectionEnd(pos);
}

int LEditor::GetEOLByContent()
{
	if (GetLength() == 0) {
		return wxNOT_FOUND;
	}

	// locate the first EOL
	wxString txt = GetText();
	size_t pos1 = static_cast<size_t>(txt.Find(wxT("\n")));
	size_t pos2 = static_cast<size_t>(txt.Find(wxT("\r\n")));
	size_t pos3 = static_cast<size_t>(txt.Find(wxT("\r")));

	size_t max_size_t = static_cast<size_t>(-1);
	// the buffer is not empty but it does not contain any EOL as well
	if (pos1 == max_size_t && pos2 == max_size_t && pos3 == max_size_t ) {
		return wxNOT_FOUND;
	}

	size_t first_eol_pos(0);
	pos2 < pos1 ? first_eol_pos = pos2 : first_eol_pos = pos1;
	pos3 < first_eol_pos ? first_eol_pos = pos3 : first_eol_pos = first_eol_pos;

	// get the EOL at first_eol_pos
	wxChar ch = SafeGetChar(first_eol_pos);
	if (ch == wxT('\n')) {
		return wxSCI_EOL_LF;
	}

	if ( ch == wxT('\r') ) {
		wxChar secondCh = SafeGetChar(first_eol_pos+1);
		if (secondCh == wxT('\n')) {
			return wxSCI_EOL_CRLF;
		} else {
			return wxSCI_EOL_CR;
		}
	}
	return wxNOT_FOUND;
}

int LEditor::GetEOLByOS()
{
#if defined(__WXMAC__)
	return wxSCI_EOL_CR;
#elif defined(__WXGTK__)
	return wxSCI_EOL_LF;
#else
	return wxSCI_EOL_CRLF;
#endif
}

void LEditor::SetStatusBarMessage(const wxString& msg, int field)
{
	// send event to main frame to perform the update
	wxCommandEvent e(wxEVT_CMD_UPDATE_STATUS_BAR);
	e.SetInt(field);
	e.SetString(msg);
	Frame::Get()->AddPendingEvent(e);
}

void LEditor::ShowFunctionTipFromCurrentPos()
{
	// determine the closest open brace from the current caret position
	int pos = PositionBefore( GetCurrentPos() );
	int depth(0);

	bool exit_loop(false);
	while ( pos > 0 ) {
		wxChar ch = SafeGetChar(pos);
		if (m_context->IsCommentOrString(pos)) {
			pos = PositionBefore(pos);
			continue;
		}

		switch (ch) {
		case wxT('{'):
					case wxT('}'):
						case wxT(';'):
								exit_loop = true;
			break;
		case wxT('('):
						depth ++;
			if (depth == 1) {
				pos = PositionAfter(pos);
				exit_loop = true;
			} else {
				pos = PositionBefore(pos);
			}
			break;
		case wxT(')'):
						depth--;
			// fall through
		default:
			pos = PositionBefore(pos);
			break;
		}

		if (exit_loop)
			break;
	}

	if (depth == 1 && pos >= 0) {
		m_context->CodeComplete(pos);
	}
}

wxString LEditor::GetSelection()
{
	return wxScintilla::GetSelectedText();
}

void LEditor::ReplaceSelection(const wxString& text)
{
	wxScintilla::ReplaceSelection(text);
}

void LEditor::ClearUserIndicators()
{
	SetIndicatorCurrent(USER_INDICATOR);
	IndicatorClearRange(0, GetLength());
}

int LEditor::GetUserIndicatorEnd(int pos)
{
	return wxScintilla::IndicatorEnd(USER_INDICATOR, pos);
}

int LEditor::GetUserIndicatorStart(int pos)
{
	return wxScintilla::IndicatorStart(USER_INDICATOR, pos);
}

void LEditor::SelectText(int startPos, int len)
{
	SetSelectionStart(startPos);
	SetSelectionEnd(startPos + len);
}

void LEditor::SetUserIndicator(int startPos, int len)
{
	SetIndicatorCurrent(USER_INDICATOR);
	IndicatorFillRange(startPos, len);
}

void LEditor::SetUserIndicatorStyleAndColour(int style, const wxColour& colour)
{
	IndicatorSetForeground(USER_INDICATOR, colour);
	IndicatorSetStyle(USER_INDICATOR, style);
	IndicatorSetUnder(USER_INDICATOR, true);
}

int LEditor::GetLexerId()
{
	return GetLexer();
}

int LEditor::GetStyleAtPos(int pos)
{
	return GetStyleAt(pos);
}

void LEditor::RegisterImageForKind(const wxString& kind, const wxBitmap& bmp)
{
	if ( m_ccBox == NULL ) {
		// create new completion box
		m_ccBox = new CCBox(this);
	}

	m_ccBox->RegisterImageForKind(kind, bmp);
}

int LEditor::WordStartPos(int pos, bool onlyWordCharacters)
{
	return wxScintilla::WordStartPosition(pos, onlyWordCharacters);
}

int LEditor::WordEndPos(int pos, bool onlyWordCharacters)
{
	return wxScintilla::WordEndPosition(pos, onlyWordCharacters);
}

void LEditor::DoMarkHyperlink(wxMouseEvent& event, bool isMiddle)
{
	if (event.m_controlDown || isMiddle) {
		SetIndicatorCurrent(HYPERLINK_INDICATOR);
		long pos = PositionFromPointClose(event.GetX(), event.GetY());

		IndicatorSetForeground(HYPERLINK_INDICATOR, wxT("NAVY"));

		if (pos != wxSCI_INVALID_POSITION) {
			m_hyperLinkType = m_context->GetHyperlinkRange(pos, m_hyperLinkIndicatroStart, m_hyperLinkIndicatroEnd);
			if (m_hyperLinkType != wxID_NONE) {
				IndicatorFillRange(m_hyperLinkIndicatroStart, m_hyperLinkIndicatroEnd - m_hyperLinkIndicatroStart);
			} else {
				m_hyperLinkIndicatroStart = wxNOT_FOUND;
				m_hyperLinkIndicatroEnd = wxNOT_FOUND;
			}
		}
	}
}
void LEditor::DoQuickJump(wxMouseEvent& event, bool isMiddle)
{
	if (m_hyperLinkIndicatroStart != wxNOT_FOUND && m_hyperLinkIndicatroEnd != wxNOT_FOUND) {
		// indicator is highlighted
		long pos = PositionFromPointClose(event.GetX(), event.GetY());
		if (m_hyperLinkIndicatroStart <= pos && pos <= m_hyperLinkIndicatroEnd) {
			bool altLink = isMiddle && event.m_controlDown || !isMiddle && event.m_altDown;
			m_context->GoHyperlink(m_hyperLinkIndicatroStart, m_hyperLinkIndicatroEnd,
			                       m_hyperLinkType, altLink);
		}
	}

	// clear the hyper link indicators
	m_hyperLinkIndicatroStart = wxNOT_FOUND;
	m_hyperLinkIndicatroEnd = wxNOT_FOUND;

	SetIndicatorCurrent(HYPERLINK_INDICATOR);
	IndicatorClearRange(0, GetLength());
	event.Skip();
}

void LEditor::TrimText()
{
	long trim(0);
	long appendLf(0);
	EditorConfigST::Get()->GetLongValue(wxT("EditorTrimEmptyLines"), trim);
	EditorConfigST::Get()->GetLongValue(wxT("EditorAppendLf"), appendLf);

	if (!trim && !appendLf) {
		return;
	}

	if (trim) {
		int maxLines = GetLineCount();
		for (int line = 0; line < maxLines; line++) {
			int lineStart = PositionFromLine(line);
			int lineEnd = GetLineEndPosition(line);
			int i = lineEnd-1;
			wxChar ch = (wxChar)(GetCharAt(i));
			while ((i >= lineStart) && ((ch == _T(' ')) || (ch == _T('\t')))) {
				i--;
				ch = (wxChar)(GetCharAt(i));
			}
			if (i < (lineEnd-1)) {
				SetTargetStart(i+1);
				SetTargetEnd(lineEnd);
				ReplaceTarget(_T(""));
			}
		}
	}

	if (appendLf) {
		// The following code was adapted from the SciTE sourcecode
		int maxLines = GetLineCount();
		int enddoc = PositionFromLine(maxLines);
		if (maxLines <= 1 || enddoc > PositionFromLine(maxLines-1))
			InsertText(enddoc,GetEolString());

	}
}

wxString LEditor::GetEolString()
{
	wxString eol;
	switch (this->GetEOLMode()) {
	case wxSCI_EOL_CR:
		eol = wxT("\r");
		break;
	case wxSCI_EOL_CRLF:
		eol = wxT("\r\n");
		break;
	case wxSCI_EOL_LF:
		eol = wxT("\n");
		break;
	}
	return eol;
}

void LEditor::GetEditorState(LEditorState& s)
{
	int mask(0);
	mask |= 256;

	// collect breakpoints
	int lineno = MarkerNext(0, mask);
	while (lineno >= 0) {
		s.breakpoints.push_back(lineno);
		lineno = MarkerNext(lineno+1, mask);
	}

	// collect all bookmarks
	mask = 128;
	lineno = MarkerNext(0, mask);
	while (lineno >= 0) {
		s.markers.push_back(lineno);
		lineno = MarkerNext(lineno+1, mask);
	}

	s.caretPosition = GetCurrentPos();
}

void LEditor::SetEditorState(const LEditorState& s)
{
	for (size_t i=0; i<s.markers.size(); i++) {
		int line_number = s.markers.at(i);
		MarkerAdd(line_number, 0x7);
	}

	for (size_t i=0; i<s.breakpoints.size(); i++) {
		int line_number = s.breakpoints.at(i);
		MarkerAdd(line_number, 0x8);
	}
	SetCaretAt(s.caretPosition);
}

void LEditor::OnDbgRunToCursor(wxCommandEvent& event)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	
	if (dbgr && dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract()){
		dbgr->Break(GetFileName().GetFullPath(), GetCurrentLine()+1, true);
		dbgr->Continue();
	}
}
