//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : context_cpp.cpp
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


#include "pluginmanager.h"
#include "cl_editor_tip_window.h"
#include "implement_parent_virtual_functions.h"
#include "debuggerasciiviewer.h"
#include <wx/file.h>
#include "threebuttondlg.h"
#include "precompiled_header.h"
#include "debuggerconfigtool.h"
#include "debuggersettings.h"
#include "parse_thread.h"
#include "cc_box.h"
#include <wx/progdlg.h>
#include "renamesymboldlg.h"
#include "cpptoken.h"
#include "globals.h"
#include "commentconfigdata.h"
#include "editor_config.h"
#include "movefuncimpldlg.h"
#include "context_cpp.h"
#include "cl_editor.h"
#include "ctags_manager.h"
#include "manager.h"
#include "symbols_dialog.h"
#include "editor_config.h"
#include "wx/xrc/xmlres.h"
#include "algorithm"
#include "language.h"
#include "browse_record.h"
#include "wx/tokenzr.h"
#include "setters_getters_dlg.h"
#include "navigationmanager.h"
#include "wx/regex.h"
#include <wx/choicdlg.h>
#include "frame.h"
#include "debuggermanager.h"
#include "addincludefiledlg.h"
#include "workspacetab.h"
#include "fileview.h"
#include "refactorindexbuildjob.h"
#include "new_quick_watch_dlg.h"
#include "code_completion_api.h"

//#define __PERFORMANCE
#include "performance.h"

static bool IsSource(const wxString &ext)
{
	wxString e(ext);
	e = e.MakeLower();
	return e == wxT("cpp") || e == wxT("cxx") || e == wxT("c") || e == wxT("c++") || e == wxT("cc");
}

static bool IsHeader(const wxString &ext)
{
	wxString e(ext);
	e = e.MakeLower();
	return e == wxT("hpp") || e == wxT("h") || e == wxT("hxx");
}

#define VALIDATE_PROJECT(ctrl)\
	if(ctrl.GetProject().IsEmpty())\
	{\
		return;\
	}

#define VALIDATE_WORKSPACE()\
	if(ManagerST::Get()->IsWorkspaceOpen() == false)\
	{\
		return;\
	}

struct SFileSort {
	bool operator()(const wxFileName &one, const wxFileName &two) {
		return two.GetFullName().Cmp(one.GetFullName()) > 0;
	}
};

//----------------------------------------------------------------------------------

struct RefactorSource {
	wxString name;
	wxString scope;
	bool isClass;

	RefactorSource() : name(wxEmptyString), scope(wxEmptyString), isClass(false) {
	}

	void Reset() {
		name.clear();
		scope.clear();
		isClass = false;
	}
};

//----------------------------------------------------------------------------------

//Images initialization
wxBitmap ContextCpp::m_classBmp = wxNullBitmap;
wxBitmap ContextCpp::m_structBmp = wxNullBitmap;
wxBitmap ContextCpp::m_namespaceBmp = wxNullBitmap;
wxBitmap ContextCpp::m_variableBmp = wxNullBitmap;
wxBitmap ContextCpp::m_tpyedefBmp = wxNullBitmap;
wxBitmap ContextCpp::m_memberPrivateBmp = wxNullBitmap;
wxBitmap ContextCpp::m_memberPublicBmp = wxNullBitmap;
wxBitmap ContextCpp::m_memberProtectedeBmp = wxNullBitmap;
wxBitmap ContextCpp::m_functionPrivateBmp = wxNullBitmap;
wxBitmap ContextCpp::m_functionPublicBmp = wxNullBitmap;
wxBitmap ContextCpp::m_functionProtectedeBmp = wxNullBitmap;
wxBitmap ContextCpp::m_macroBmp = wxNullBitmap;
wxBitmap ContextCpp::m_enumBmp = wxNullBitmap;
wxBitmap ContextCpp::m_enumeratorBmp = wxNullBitmap;
wxBitmap ContextCpp::m_cppFileBmp = wxNullBitmap;
wxBitmap ContextCpp::m_hFileBmp = wxNullBitmap;
wxBitmap ContextCpp::m_otherFileBmp = wxNullBitmap;

BEGIN_EVENT_TABLE(ContextCpp, wxEvtHandler)
	EVT_UPDATE_UI(XRCID("find_decl"),               ContextCpp::OnUpdateUI)
	EVT_UPDATE_UI(XRCID("find_impl"),               ContextCpp::OnUpdateUI)
	EVT_UPDATE_UI(XRCID("go_to_function_start"),    ContextCpp::OnUpdateUI)
	EVT_UPDATE_UI(XRCID("go_to_next_function"),     ContextCpp::OnUpdateUI)
	EVT_UPDATE_UI(XRCID("insert_doxy_comment"),     ContextCpp::OnUpdateUI)
	EVT_UPDATE_UI(XRCID("setters_getters"),         ContextCpp::OnUpdateUI)
	EVT_UPDATE_UI(XRCID("move_impl"),               ContextCpp::OnUpdateUI)

	EVT_MENU(XRCID("swap_files"),                   ContextCpp::OnSwapFiles)
	EVT_MENU(XRCID("comment_selection"),            ContextCpp::OnCommentSelection)
	EVT_MENU(XRCID("comment_line"),                 ContextCpp::OnCommentLine)
	EVT_MENU(XRCID("find_decl"),                    ContextCpp::OnFindDecl)
	EVT_MENU(XRCID("find_impl"),                    ContextCpp::OnFindImpl)
	EVT_MENU(XRCID("go_to_function_start"),         ContextCpp::OnGotoFunctionStart)
	EVT_MENU(XRCID("go_to_next_function"),          ContextCpp::OnGotoNextFunction)
	EVT_MENU(XRCID("insert_doxy_comment"),          ContextCpp::OnInsertDoxyComment)
	EVT_MENU(XRCID("move_impl"),                    ContextCpp::OnMoveImpl)
	EVT_MENU(XRCID("add_impl"),                     ContextCpp::OnAddImpl)
	EVT_MENU(XRCID("add_multi_impl"),               ContextCpp::OnAddMultiImpl)
	EVT_MENU(XRCID("add_virtual_impl"),             ContextCpp::OnOverrideParentVritualFunctions)
	EVT_MENU(XRCID("add_pure_virtual_impl"),        ContextCpp::OnOverrideParentVritualFunctions)
	EVT_MENU(XRCID("setters_getters"),              ContextCpp::OnGenerateSettersGetters)
	EVT_MENU(XRCID("add_include_file"),             ContextCpp::OnAddIncludeFile)
	EVT_MENU(XRCID("rename_function"),              ContextCpp::OnRenameFunction)
	EVT_MENU(XRCID("retag_file"), ContextCpp::OnRetagFile)
END_EVENT_TABLE()

ContextCpp::ContextCpp(LEditor *container)
		: ContextBase(container)
		, m_rclickMenu(NULL)
{
	Initialize();
}


ContextCpp::ContextCpp()
		: ContextBase(wxT("C++"))
		, m_rclickMenu(NULL)
{
}

ContextCpp::~ContextCpp()
{
	if (m_rclickMenu) {
		delete m_rclickMenu;
		m_rclickMenu = NULL;
	}
}

ContextBase *ContextCpp::NewInstance(LEditor *container)
{
	return new ContextCpp(container);
}

void ContextCpp::OnDwellEnd(wxScintillaEvent &event)
{
	LEditor &rCtrl = GetCtrl();
	rCtrl.DoCancelCalltip();
	event.Skip();
}

void ContextCpp::OnDwellStart(wxScintillaEvent &event)
{
	LEditor &rCtrl = GetCtrl();

	VALIDATE_PROJECT(rCtrl);

	//before we start, make sure we are the visible window
	if (Frame::Get()->GetMainBook()->GetActiveEditor() != &rCtrl) {
		event.Skip();
		return;
	}

	long pos = event.GetPosition();
	int  end = rCtrl.WordEndPosition(pos, true);
	int  word_start = rCtrl.WordStartPosition(pos, true);

	// get the expression we are standing on it
	if ( IsCommentOrString( pos ) )
		return;

	// get the token
	wxString word = rCtrl.GetTextRange(word_start, end);
	if (word.IsEmpty()) {
		return;
	}

	//get the expression we are hovering over
	wxString expr = GetExpression(end, false);
	// get the full text of the current page
	wxString text = rCtrl.GetTextRange(0, pos);
	// now we are ready to process the scope and build our tips
	std::vector<wxString> tips;
	int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPosition())+1;
	TagsManagerST::Get()->GetHoverTip(rCtrl.GetFileName(), line, expr, word, text, tips);

	// display a tooltip
	wxString tooltip;
	if (tips.size() > 0) {
		tooltip << tips[0];
		for ( size_t i=1; i<tips.size(); i++ )
			tooltip << wxT("\n") << tips[i];

		// cancel any old calltip and display the new one
		rCtrl.DoCancelCalltip();
		rCtrl.DoShowCalltip(event.GetPosition(), tooltip, ct_function_hover);
	}
}

wxString ContextCpp::GetFileImageString(const wxString &ext)
{
	if (IsSource(ext)) {
		return wxT("?15");
	}
	if (IsHeader(ext)) {
		return wxT("?16");
	}
	return wxT("?17");
}

wxString ContextCpp::GetImageString(const TagEntry &entry)
{
	if (entry.GetKind() == wxT("class"))
		return wxT("?1");

	if (entry.GetKind() == wxT("struct"))
		return wxT("?2");

	if (entry.GetKind() == wxT("namespace"))
		return wxT("?3");

	if (entry.GetKind() == wxT("variable"))
		return wxT("?4");

	if (entry.GetKind() == wxT("typedef"))
		return wxT("?5");

	if (entry.GetKind() == wxT("member") && entry.GetAccess().Contains(wxT("private")))
		return wxT("?6");

	if (entry.GetKind() == wxT("member") && entry.GetAccess().Contains(wxT("public")))
		return wxT("?7");

	if (entry.GetKind() == wxT("member") && entry.GetAccess().Contains(wxT("protected")))
		return wxT("?8");

	//member with no access? (maybe part of namespace??)
	if (entry.GetKind() == wxT("member"))
		return wxT("?7");

	if ((entry.GetKind() == wxT("function") || entry.GetKind() == wxT("prototype")) && entry.GetAccess().Contains(wxT("private")))
		return wxT("?9");

	if ((entry.GetKind() == wxT("function") || entry.GetKind() == wxT("prototype")) && (entry.GetAccess().Contains(wxT("public")) || entry.GetAccess().IsEmpty()))
		return wxT("?10");

	if ((entry.GetKind() == wxT("function") || entry.GetKind() == wxT("prototype")) && entry.GetAccess().Contains(wxT("protected")))
		return wxT("?11");

	if (entry.GetKind() == wxT("macro"))
		return wxT("?12");

	if (entry.GetKind() == wxT("enum"))
		return wxT("?13");

	if (entry.GetKind() == wxT("enumerator"))
		return wxT("?14");

	return wxEmptyString;
}

void ContextCpp::AutoIndent(const wxChar &nChar)
{
	LEditor &rCtrl = GetCtrl();

	if(rCtrl.GetDisableSmartIndent()) {
		/*ContextBase::AutoIndent(nChar);*/
		return;
	}

	int curpos = rCtrl.GetCurrentPos();
	if (IsComment(curpos) && nChar == wxT('\n')) {
		AutoAddComment();
		return;
	}

	if (IsCommentOrString(curpos)) {
		ContextBase::AutoIndent(nChar);
		return;
	}

	int line = rCtrl.LineFromPosition(curpos);
	if (nChar == wxT('\n')) {

		int      prevpos (wxNOT_FOUND);
		int      foundPos(wxNOT_FOUND);
		wxString word;

		wxChar ch = rCtrl.PreviousChar(curpos, prevpos );
		word      = rCtrl.PreviousWord(curpos, foundPos);

		// user hit ENTER after 'else'
		if ( word == wxT("else") ) {
			int prevLine = rCtrl.LineFromPosition(prevpos);
			rCtrl.SetLineIndentation(line, rCtrl.GetIndent() + rCtrl.GetLineIndentation(prevLine));
			rCtrl.SetCaretAt(rCtrl.GetLineIndentPosition(line));
			rCtrl.ChooseCaretX(); // set new column as "current" column
			return;
		}

		// User typed 'ENTER' immediatly after closing brace ')'
		if ( prevpos != wxNOT_FOUND && ch == wxT(')') ) {

			long openBracePos          (wxNOT_FOUND);
			int  posWordBeforeOpenBrace(wxNOT_FOUND);

			if (rCtrl.MatchBraceBack(wxT(')'), prevpos, openBracePos)) {
				rCtrl.PreviousChar(openBracePos, posWordBeforeOpenBrace);
				if (posWordBeforeOpenBrace != wxNOT_FOUND) {
					word = rCtrl.PreviousWord(posWordBeforeOpenBrace, foundPos);

					// c++ expression with single line and should be treated separatly
					if ( word == wxT("if") || word == wxT("while") || word == wxT("for")) {
						int prevLine = rCtrl.LineFromPosition(prevpos);
						rCtrl.SetLineIndentation(line, rCtrl.GetIndent() + rCtrl.GetLineIndentation(prevLine));
						rCtrl.SetCaretAt(rCtrl.GetLineIndentPosition(line));
						rCtrl.ChooseCaretX(); // set new column as "current" column
						return;
					}
				}
			}
		}

		// User typed 'ENTER' immediatly after colons ':'
		if ( prevpos != wxNOT_FOUND && ch == wxT(':') ) {
			int  posWordBeforeColons(wxNOT_FOUND);

			rCtrl.PreviousChar(prevpos, posWordBeforeColons);
			if (posWordBeforeColons != wxNOT_FOUND) {
				word = rCtrl.PreviousWord(posWordBeforeColons, foundPos);
				int prevLine = rCtrl.LineFromPosition(posWordBeforeColons);

				// If we found one of the following keywords, un-indent their line by (foldLevel - 1)*indentSize
				if ( word == wxT("public") || word == wxT("private") || word == wxT("protected")) {

					ContextBase::AutoIndent(nChar);

					// Indent this line according to the block indentation level
					int foldLevel = (rCtrl.GetFoldLevel(prevLine) & wxSCI_FOLDLEVELNUMBERMASK) - wxSCI_FOLDLEVELBASE;
					if (foldLevel) {
						rCtrl.SetLineIndentation(prevLine, ((foldLevel-1)*rCtrl.GetIndent()) );
						rCtrl.ChooseCaretX();
					}
					return;
				}
			}
		}

		// use the previous line indentation level
		if (prevpos == wxNOT_FOUND || ch != wxT('{') || IsCommentOrString(prevpos)) {
			ContextBase::AutoIndent(nChar);
			return;
		}

		// Open brace? increase indent size
		int prevLine = rCtrl.LineFromPosition(prevpos);
		rCtrl.SetLineIndentation(line, rCtrl.GetIndent() + rCtrl.GetLineIndentation(prevLine));
		rCtrl.SetCaretAt(rCtrl.GetLineIndentPosition(line));

	} else if (nChar == wxT('}')) {

		long matchPos = wxNOT_FOUND;
		if (!rCtrl.MatchBraceBack(wxT('}'), rCtrl.PositionBefore(curpos), matchPos))
			return;
		int secondLine = rCtrl.LineFromPosition(matchPos);
		if (secondLine == line)
			return;
		rCtrl.SetLineIndentation(line, rCtrl.GetLineIndentation(secondLine));

	} else if (nChar == wxT('{')) {
		wxString lineString = rCtrl.GetLine(line);
		lineString.Trim().Trim(false);

		int matchPos = wxNOT_FOUND;
		wxChar previousChar = rCtrl.PreviousChar(rCtrl.PositionBefore(curpos), matchPos);
		if(previousChar != wxT('{') && lineString == wxT("{")) {
			// indent this line accroding to the previous line
			int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPos());
			rCtrl.SetLineIndentation(line, rCtrl.GetLineIndentation(line-1));
			rCtrl.ChooseCaretX();
		}
	}

	// set new column as "current" column
	rCtrl.ChooseCaretX();
}

bool ContextCpp::IsCommentOrString(long pos)
{
	int style;
	style = GetCtrl().GetStyleAt(pos);
	return (style == wxSCI_C_COMMENT                ||
	        style == wxSCI_C_COMMENTLINE            ||
	        style == wxSCI_C_COMMENTDOC             ||
	        style == wxSCI_C_COMMENTLINEDOC         ||
	        style == wxSCI_C_COMMENTDOCKEYWORD      ||
	        style == wxSCI_C_COMMENTDOCKEYWORDERROR ||
	        style == wxSCI_C_STRING                 ||
	        style == wxSCI_C_STRINGEOL              ||
	        style == wxSCI_C_CHARACTER);
}

//=============================================================================
// >>>>>>>>>>>>>>>>>>>>>>>> CodeCompletion API - START
//=============================================================================

//user pressed ., -> or ::
void ContextCpp::CodeComplete(long pos)
{
	VALIDATE_WORKSPACE();
	long from = pos;
	if (from == wxNOT_FOUND) {
		from = GetCtrl().GetCurrentPos();
	}
	DoCodeComplete(from);
}

void ContextCpp::RemoveDuplicates(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target)
{
	for (size_t i=0; i<src.size(); i++) {
		if (i == 0) {
			target.push_back(src.at(0));
		} else {
			if (src.at(i)->GetName() != target.at(target.size()-1)->GetName()) {
				target.push_back(src.at(i));
			}
		}
	}
}

wxString ContextCpp::GetExpression(long pos, bool onlyWord, LEditor *editor, bool forCC)
{
	bool cont(true);
	int depth(0);

	LEditor *ctrl(NULL);
	if (!editor) {
		ctrl = &GetCtrl();
	} else {
		ctrl = editor;
	}

	int position( pos );
	int at(position);
	bool prevGt(false);
	bool prevColon(false);
	while (cont && depth >= 0) {
		wxChar ch =ctrl->PreviousChar(position, at, true);
		position = at;
		//Eof?
		if (ch == 0) {
			at = 0;
			break;
		}

		//Comment?
		int style = ctrl->GetStyleAt(pos);
		if (style == wxSCI_C_COMMENT				||
		        style == wxSCI_C_COMMENTLINE			||
		        style == wxSCI_C_COMMENTDOC				||
		        style == wxSCI_C_COMMENTLINEDOC			||
		        style == wxSCI_C_COMMENTDOCKEYWORD		||
		        style == wxSCI_C_COMMENTDOCKEYWORDERROR ||
		        style == wxSCI_C_STRING					||
		        style == wxSCI_C_STRINGEOL				||
		        style == wxSCI_C_CHARACTER) {
			continue;
		}

		switch (ch) {
		case wxT(';'):
						// dont include this token
						at = ctrl->PositionAfter(at);
			cont = false;
			prevColon = false;
			break;
		case wxT('-'):
						if (prevGt) {
					prevGt = false;
					//if previous char was '>', we found an arrow so reduce the depth
					//which was increased
					depth--;
				} else {
					if (depth <= 0) {
						//dont include this token
						at =ctrl->PositionAfter(at);
						cont = false;
					}
				}
			prevColon = false;
			break;
		case wxT(' '):
					case wxT('\n'):
						case wxT('\v'):
							case wxT('\t'):
								case wxT('\r'):
										prevGt = false;
			prevColon = false;
			if (depth <= 0) {
				cont = false;
				break;
			}
			break;
		case wxT('{'):
					case wxT('='):
							prevGt = false;
			prevColon = false;
			cont = false;
			break;
		case wxT('('):
					case wxT('['):
							depth--;
			prevGt = false;
			prevColon = false;
			if (depth < 0) {
				//dont include this token
				at =ctrl->PositionAfter(at);
				cont = false;
			}
			break;
		case wxT(','):
					case wxT('*'):
						case wxT('&'):
							case wxT('!'):
								case wxT('~'):
									case wxT('+'):
										case wxT('^'):
											case wxT('|'):
												case wxT('%'):
													case wxT('?'):
															prevGt = false;
			prevColon = false;
			if (depth <= 0) {

				//dont include this token
				at =ctrl->PositionAfter(at);
				cont = false;
			}
			break;
		case wxT('>'):
						prevGt = true;
			prevColon = false;
			depth++;
			break;
		case wxT('<'):
						prevGt = false;
			prevColon = false;
			depth--;
			if (depth < 0) {

				//dont include this token
				at =ctrl->PositionAfter(at);
				cont = false;
			}
			break;
		case wxT(')'):
					case wxT(']'):
							prevGt = false;
			prevColon = false;
			depth++;
			break;
		default:
			prevGt = false;
			prevColon = false;
			break;
		}
	}

	if (at < 0) at = 0;
	wxString expr = ctrl->GetTextRange(at, pos);
	if ( !forCC ) {
		// If we do not require the expression for CodeCompletion
		// return the un-touched buffer
		return expr;
	}

	//remove comments from it
	CppScanner sc;
	sc.SetText(_C(expr));
	wxString expression;
	int type=0;
	while ( (type = sc.yylex()) != 0 ) {
		wxString token = _U(sc.YYText());
		expression += token;
		expression += wxT(" ");
	}
	return expression;
}

wxString ContextCpp::GetWordUnderCaret()
{
	LEditor &rCtrl = GetCtrl();
	// Get the partial word that we have
	long pos = rCtrl.GetCurrentPos();
	long start = rCtrl.WordStartPosition(pos, true);
	long end   = rCtrl.WordEndPosition(pos, true);
	return rCtrl.GetTextRange(start, end);
}

void ContextCpp::OnContextOpenDocument(wxCommandEvent &event)
{
	wxUnusedVar(event);
	DoOpenWorkspaceFile();
}

void ContextCpp::RemoveMenuDynamicContent(wxMenu *menu)
{
	std::vector<wxMenuItem*>::iterator iter = m_dynItems.begin();
	for (; iter != m_dynItems.end(); iter++) {
		menu->Destroy((*iter));
	}
	m_dynItems.clear();
	m_selectedWord.Empty();
}

void ContextCpp::AddMenuDynamicContent(wxMenu *menu)
{
	//if we are placed over an include statement,
	//add an option in the menu to open it
	wxString fileName;

	LEditor &rCtrl = GetCtrl();
//	VALIDATE_PROJECT(rCtrl);

	wxString menuItemText;
	wxString line = rCtrl.GetCurLine();
	menuItemText.Clear();

	if (IsIncludeStatement(line, &fileName)) {

		PrependMenuItemSeparator(menu);
		menuItemText << wxT("Open Include File \"") << fileName << wxT("\"");

		PrependMenuItem(menu, menuItemText, wxCommandEventHandler(ContextCpp::OnContextOpenDocument));
		m_selectedWord = fileName;

	} else {
		int pos = rCtrl.GetCurrentPos();
		if (IsCommentOrString(pos)) {
			return;
		}

		wxString word = rCtrl.GetWordAtCaret();
		if (word.IsEmpty() == false) {
			PrependMenuItemSeparator(menu);
			menuItemText << wxT("Add Include File for \"") << word << wxT("\"");
			PrependMenuItem(menu, menuItemText, XRCID("add_include_file"));
			m_selectedWord = word;
		}
	}
}

void ContextCpp::OnAddIncludeFile(wxCommandEvent &e)
{
	wxUnusedVar(e);
	LEditor &rCtrl = GetCtrl();

	//get expression
	int pos = rCtrl.GetCurrentPos();

	if (IsCommentOrString(pos))
		return;

	int word_end = rCtrl.WordEndPosition(pos, true);
	wxString expr = GetExpression(word_end, false);

	// get the scope
	wxString text = rCtrl.GetTextRange(0, word_end);

	wxString word = m_selectedWord;
	if (word.IsEmpty()) {
		//try the word under the caret
		word = rCtrl.GetWordAtCaret();
		if (word.IsEmpty()) {
			return;
		}
	}

	std::vector<TagEntryPtr> tags;
	int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPosition())+1;
	TagsManagerST::Get()->FindImplDecl(rCtrl.GetFileName(), line, expr, word, text, tags, false);
	if (tags.empty())
		return;

	std::map<wxString, bool> tmpmap;

	wxArrayString options;

	//remove duplicate file entries
	for (std::vector<TagEntryPtr>::size_type i=0; i< tags.size(); i++) {
		tmpmap[tags.at(i)->GetFile()] = true;
	}

	//convert the map to wxArrayString
	std::map<wxString, bool>::iterator iter = tmpmap.begin();
	for (; iter != tmpmap.end(); iter++) {
		options.Add(iter->first);
	}

	//we now got list of tags that matches 'word'
	wxString choice;
	if (options.GetCount() > 1) {
		//multiple matches
		choice = wxGetSingleChoice(wxT("Select File to Include:"), wxT("Add Include File"), options, &GetCtrl());
	} else {
		choice = options.Item(0);
	}

	if (choice.IsEmpty()) {
		return;
	}

	//check to see if this file is a workspace file
	AddIncludeFileDlg *dlg = new AddIncludeFileDlg(NULL, choice, rCtrl.GetText(), FindLineToAddInclude());
	if (dlg->ShowModal() == wxID_OK) {
		//add the line to the current document
		wxString lineToAdd = dlg->GetLineToAdd();
		int line = dlg->GetLine();

		long pos = rCtrl.PositionFromLine(line);
		rCtrl.InsertText(pos, lineToAdd + rCtrl.GetEolString());
	}
	dlg->Destroy();
}

bool ContextCpp::IsIncludeStatement(const wxString &line, wxString *fileName)
{
	wxString tmpLine(line);
	wxString tmpLine1(line);

	//If we are on an include statement, popup a file list
	//completion list
	tmpLine = tmpLine.Trim();
	tmpLine = tmpLine.Trim(false);
	tmpLine.Replace(wxT("\t"), wxT(" "));

	static wxRegEx reIncludeFile(wxT("include *[\\\"\\<]{1}([a-zA-Z0-9_/\\.\\+\\-]*)"));
	if (tmpLine.StartsWith(wxT("#"), &tmpLine1)) {
		if (reIncludeFile.Matches(tmpLine1)) {
			if (fileName) {
				*fileName = reIncludeFile.GetMatch(tmpLine1, 1);
			}
			return true;
		}
	}
	return false;
}

void ContextCpp::CompleteWord()
{
	LEditor &rCtrl = GetCtrl();

	VALIDATE_WORKSPACE();

	std::vector<TagEntryPtr> tags;
	wxString scope;
	wxString scopeName;
	wxString word;
	wxString fileName;

	wxString line = rCtrl.GetCurLine();
	if (IsIncludeStatement(line, &fileName)) {
		DisplayFilesCompletionBox(fileName);
		return;
	}

	//	Make sure we are not on a comment section
	if (IsCommentOrString(rCtrl.GetCurrentPos()))
		return;

	// Get the partial word that we have
	long pos = rCtrl.GetCurrentPos();
	long start = rCtrl.WordStartPosition(pos, true);
	word = rCtrl.GetTextRange(start, pos);

	if (word.IsEmpty()) {
		// incase the 'word' is empty, test the word to the left of the current pos
		wxChar ch1 = rCtrl.SafeGetChar(pos - 1);
		wxChar ch2 = rCtrl.SafeGetChar(pos - 2);

		if (ch1 == wxT('.') || (ch2 == wxT('-') && ch1 == wxT('>')) ) {
			CodeComplete();
		}
		return;
	}

	TagsManager *mgr = TagsManagerST::Get();

	//get the current expression
	wxString expr = GetExpression(rCtrl.GetCurrentPos(), true);

	std::vector<TagEntryPtr> candidates;
	//get the full text of the current page
	wxString text = rCtrl.GetTextRange(0, rCtrl.GetCurrentPos());
	int lineNum = rCtrl.LineFromPosition(rCtrl.GetCurrentPosition())+1;
	if (mgr->WordCompletionCandidates(rCtrl.GetFileName(), lineNum, expr, text, word, candidates)) {
		DisplayCompletionBox(candidates, word, false);
	}
}

void ContextCpp::DisplayCompletionBox(const std::vector<TagEntryPtr> &tags, const wxString &word, bool showFullDecl)
{
	// calculate the position to display the completion box
	GetCtrl().ShowCompletionBox(tags, word, showFullDecl);
}

void ContextCpp::DisplayFilesCompletionBox(const wxString &word)
{
	wxString list;

	wxString fileName(word);
	fileName.Replace(wxT("\\"), wxT("/"));
	fileName = fileName.AfterLast(wxT('/'));

	std::vector<wxFileName> files;
	TagsManagerST::Get()->GetFiles(fileName, files);

	std::sort(files.begin(), files.end(), SFileSort());

	if ( files.empty() == false ) {
		GetCtrl().RegisterImageForKind(wxT("FileCpp"),    m_cppFileBmp);
		GetCtrl().RegisterImageForKind(wxT("FileHeader"), m_hFileBmp  );
		std::vector<TagEntryPtr> tags;

		for (size_t i=0; i<files.size(); i++) {
			TagEntryPtr t(new TagEntry());
			t->SetName(files.at(i).GetFullName());
			t->SetKind(IsSource(files.at(i).GetExt()) ? wxT("FileCpp") : wxT("FileHeader"));
			tags.push_back(t);
		}
		GetCtrl().ShowCompletionBox(tags, fileName, false, true);
	}
}

//=============================================================================
// <<<<<<<<<<<<<<<<<<<<<<<<<<< CodeCompletion API - END
//=============================================================================

void ContextCpp::GotoPreviousDefintion()
{
	NavMgr::Get()->NavigateBackward(PluginManager::Get());
}

TagEntryPtr ContextCpp::GetTagAtCaret(bool scoped, bool impl)
{
	if (!ManagerST::Get()->IsWorkspaceOpen())
		return NULL;

	LEditor &rCtrl = GetCtrl();

	//	Make sure we are not on a comment section
	if (IsCommentOrString(rCtrl.GetCurrentPos()))
		return NULL;

	// Get the word under the cursor OR the selected word
	int word_start = -1, word_end = -1;
	rCtrl.wxScintilla::GetSelection(&word_start, &word_end);
	if (word_start == word_end) {
		word_start = rCtrl.WordStartPos(word_start, true);
		word_end = rCtrl.WordEndPos(word_end, true);
	}
	wxString word = rCtrl.GetTextRange(word_start, word_end);
	if (word.IsEmpty())
		return NULL;

	std::vector<TagEntryPtr> tags;
	if (scoped) {
		// get tags that make sense in current scope and expression
		wxFileName fname = rCtrl.GetFileName();
		wxString expr = GetExpression(word_end, false);
		wxString text = rCtrl.GetTextRange(0, word_end);
		int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPosition())+1;
		TagsManagerST::Get()->FindImplDecl(fname, line, expr, word, text, tags, impl);
		if (!impl && tags.empty()) {
			// try again, this time allow impls
			// this will find inline definitions, which have no separate declaration
			TagsManagerST::Get()->FindImplDecl(fname, line, expr, word, text, tags, true);
		}
	} else {
		// get all tags that match the name (ignore scope)
		TagsManagerST::Get()->FindSymbol(word, tags);
	}
	if (tags.empty())
		return NULL;

	if (tags.size() == 1) // only one tag found
		return tags[0];

	// popup a dialog offering the results to the user
	SymbolsDialog dlg(&rCtrl);
	dlg.AddSymbols(tags, 0);
	return dlg.ShowModal() == wxID_OK ? dlg.GetTag() : TagEntryPtr(NULL);
}

void ContextCpp::DoGotoSymbol(TagEntryPtr tag)
{
	if (tag) {
		LEditor *editor = Frame::Get()->GetMainBook()->OpenFile(tag->GetFile(), wxEmptyString, tag->GetLine()-1);
		if (editor) {
			editor->FindAndSelect(tag->GetPattern(), tag->GetName());
		}
	}
}

void ContextCpp::GotoDefinition()
{
	DoGotoSymbol(GetTagAtCaret(false, false));
}

void ContextCpp::SwapFiles(const wxFileName &fileName)
{
	wxFileName otherFile(fileName);
	wxString ext = fileName.GetExt();
	wxArrayString exts;

	//replace the file extension
	if (IsSource(ext)) {
		//try to find a header file
		exts.Add(wxT("h"));
		exts.Add(wxT("hpp"));
		exts.Add(wxT("hxx"));
		exts.Add(wxT("hh"));
		exts.Add(wxT("h++"));
	} else {
		//try to find a implementation file
		exts.Add(wxT("cpp"));
		exts.Add(wxT("cxx"));
		exts.Add(wxT("c++"));
		exts.Add(wxT("cc"));
		exts.Add(wxT("c"));
	}

	for (size_t i=0; i<exts.GetCount(); i++) {
		otherFile.SetExt(exts.Item(i));
		if (TryOpenFile(otherFile))
			return;
	}

	long res(wxNOT_FOUND);

	// we failed to locate matched file, offer the user to create one
	// check to see if user already provided an answer
	otherFile.SetExt(exts.Item(0));

	bool userAnsweredBefore = EditorConfigST::Get()->GetLongValue(wxT("CreateSwappedFile"), res);
	if (!userAnsweredBefore) {
		// prompt the user with an "annoying" dialog
		ThreeButtonDlg dlg(Frame::Get(), _("No matched file was found, would you like to create one?"), wxT("CodeLite"));
		res = dlg.ShowModal();
		if (dlg.GetDontAskMeAgain() && res != wxID_CANCEL) {
			// the user is not interested of creating file, so dont bot
			EditorConfigST::Get()->SaveLongValue(wxT("CreateSwappedFile"), res);
		}
	}

	switch (res) {
	case wxID_NO:
	case wxID_CANCEL:
		return;
	case wxID_OK:
	default:
		DoCreateFile(otherFile);
		break;
	}
}

bool ContextCpp::FindSwappedFile(const wxFileName &rhs, wxString &lhs)
{
	wxFileName otherFile(rhs);
	wxString ext = rhs.GetExt();
	wxArrayString exts;

	//replace the file extension
	if (IsSource(ext)) {
		//try to find a header file
		exts.Add(wxT("h"));
		exts.Add(wxT("hpp"));
		exts.Add(wxT("hxx"));
	} else {
		//try to find a implementation file
		exts.Add(wxT("cpp"));
		exts.Add(wxT("cxx"));
		exts.Add(wxT("cc"));
		exts.Add(wxT("c"));
	}


	std::vector<wxFileName> files;
	ManagerST::Get()->GetWorkspaceFiles(files, true);

	for (size_t j=0; j<exts.GetCount(); j++) {
		otherFile.SetExt(exts.Item(j));
		if (otherFile.FileExists()) {
			//we got a match
			lhs = otherFile.GetFullPath();
			return true;
		}

		for (size_t i=0; i<files.size(); i++) {
			if (files.at(i).GetFullName() == otherFile.GetFullName()) {
				lhs = files.at(i).GetFullPath();
				return true;
			}
		}
	}
	return false;
}

bool ContextCpp::TryOpenFile(const wxFileName &fileName)
{
	if (fileName.FileExists()) {
		//we got a match
		wxString proj = ManagerST::Get()->GetProjectNameByFile(fileName.GetFullPath());
		return Frame::Get()->GetMainBook()->OpenFile(fileName.GetFullPath(), proj);
	}

	//ok, the file does not exist in the current directory, try to find elsewhere
	//whithin the workspace files
	std::vector<wxFileName> files;
	ManagerST::Get()->GetWorkspaceFiles(files, true);

	for (size_t i=0; i<files.size(); i++) {
		if (files.at(i).GetFullName() == fileName.GetFullName()) {
			wxString proj = ManagerST::Get()->GetProjectNameByFile(files.at(i).GetFullPath());
			return Frame::Get()->GetMainBook()->OpenFile(files.at(i).GetFullPath(), proj);
		}
	}
	return false;
}

//-----------------------------------------------
// Menu event handlers
//-----------------------------------------------
void ContextCpp::OnSwapFiles(wxCommandEvent &event)
{
	wxUnusedVar(event);
	SwapFiles(GetCtrl().GetFileName());
}


void ContextCpp::DoMakeDoxyCommentString(DoxygenComment& dc)
{
	LEditor &editor = GetCtrl();
	CommentConfigData data;
	EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

	wxString blockStart(wxT("/**\n"));
	if (!data.GetUseSlash2Stars()) {
		blockStart = wxT("/*!\n");
	}

	//prepend the prefix to the
	wxString classPattern = data.GetClassPattern();
	wxString funcPattern  = data.GetFunctionPattern();

	//replace $(Name) here **before** the call to ExpandAllVariables()
	classPattern.Replace(wxT("$(Name)"), dc.name);
	funcPattern.Replace(wxT("$(Name)"), dc.name);

	classPattern = ExpandAllVariables(classPattern, WorkspaceST::Get(), editor.GetProjectName(), wxEmptyString, editor.GetFileName().GetFullPath());
	funcPattern = ExpandAllVariables(funcPattern, WorkspaceST::Get(), editor.GetProjectName(), wxEmptyString, editor.GetFileName().GetFullPath());

	dc.comment.Replace(wxT("$(ClassPattern)"), classPattern);
	dc.comment.Replace(wxT("$(FunctionPattern)"), funcPattern);

	//close the comment
	dc.comment << wxT(" */\n");
	dc.comment.Prepend(blockStart);
}

void ContextCpp::OnInsertDoxyComment(wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor &editor = GetCtrl();

	VALIDATE_WORKSPACE();

	//get the current line text
	int lineno = editor.LineFromPosition(editor.GetCurrentPos());

	CommentConfigData data;
	EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

	//get doxygen comment based on file and line
	wxChar keyPrefix(wxT('\\'));
	if (data.GetUseShtroodel()) {
		keyPrefix = wxT('@');
	}

	DoxygenComment dc = TagsManagerST::Get()->GenerateDoxygenComment(editor.GetFileName().GetFullPath(), lineno, keyPrefix);
	//do we have a comment?
	if (dc.comment.IsEmpty())
		return;

	DoMakeDoxyCommentString(dc);

	editor.InsertTextWithIndentation(dc.comment, lineno);

	//since we just inserted a text to the document, we force a save on the
	//document, or else the parser will lose sync with the database
	//but avoid saving it, if it not part of the workspace
	editor.SaveFile();
}

void ContextCpp::OnCommentSelection(wxCommandEvent &event)
{
	wxUnusedVar(event);

	LEditor &editor = GetCtrl();
	int start = editor.GetSelectionStart();
	int end   = editor.GetSelectionEnd();
	if (editor.LineFromPosition(editor.PositionBefore(end)) != editor.LineFromPosition(end)) {
		end = editor.PositionBefore(end);
	}
	if (start == end)
		return;

	editor.SetCurrentPos(end);

	editor.BeginUndoAction();
	editor.InsertText(end, wxT("*/"));
	editor.InsertText(start, wxT("/*"));
	editor.EndUndoAction();

	editor.CharRight();
	editor.CharRight();
	editor.ChooseCaretX();
}

void ContextCpp::OnCommentLine(wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor &editor = GetCtrl();

	int start = editor.GetSelectionStart();
	int end   = editor.GetSelectionEnd();
	if (editor.LineFromPosition(editor.PositionBefore(end)) != editor.LineFromPosition(end)) {
		end = std::max(start, editor.PositionBefore(end));
	}

	bool doingComment = editor.GetStyleAt(start) != wxSCI_C_COMMENTLINE;

	int line_start = editor.LineFromPosition(start);
	int line_end   = editor.LineFromPosition(end);

	editor.BeginUndoAction();
	for (; line_start <= line_end; line_start++) {
		start = editor.PositionFromLine(line_start);
		if (doingComment) {
			editor.InsertText(start, wxT("//"));
		} else if (editor.GetStyleAt(start) == wxSCI_C_COMMENTLINE) {
			editor.SetAnchor(start);
			editor.SetCurrentPos(editor.PositionAfter(editor.PositionAfter(start)));
			editor.DeleteBackNotLine();
		}
	}
	editor.EndUndoAction();

	editor.SetCaretAt(editor.PositionFromLine(line_end+1));
	editor.ChooseCaretX();
}

void ContextCpp::OnGenerateSettersGetters(wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor &editor = GetCtrl();

	VALIDATE_WORKSPACE();

	long pos = editor.GetCurrentPos();

	if (IsCommentOrString(pos)) {
		return;
	}

	TagsManager *tagmgr = TagsManagerST::Get();
	std::vector<TagEntryPtr> tags;
	//get the scope name that the caret is currently at

	wxString text = editor.GetTextRange(0, pos);
	wxString scopeName = tagmgr->GetScopeName(text);
	tagmgr->TagsByScope(scopeName, wxT("member"), tags);
	if (tags.empty()) {
		return;
	}

	std::vector<TagEntryPtr> classtags;
	tagmgr->FindByPath(scopeName, classtags);
	if (classtags.empty() || classtags.size() > 1)
		return;

	TagEntryPtr tag = classtags.at(0);
	if (tag->GetFile().CmpNoCase(editor.GetFileName().GetFullPath()) != 0) {

		wxString msg;
		msg << wxT("This file does not seem to contain the declaration for '") << tag->GetName() << wxT("'\n");
		msg << wxT("The declaration of '") << tag->GetName() << wxT("' is located at '") << tag->GetFile() << wxT("'\n");
		msg << wxT("Would you like CodeLite to open this file for you?");

		if (wxMessageBox(msg, wxT("CodeLite"), wxYES_NO) == wxYES) {
			wxString projectName = ManagerST::Get()->GetProjectNameByFile(tag->GetFile());
			Frame::Get()->GetMainBook()->OpenFile(tag->GetFile(), projectName, tag->GetLine());
		}
		return;
	}

	int lineno = editor.LineFromPosition(editor.GetCurrentPos()) + 1;

	//get the file name and line where to insert the setters getters
	static SettersGettersDlg *s_dlg = NULL;
	if (!s_dlg) {
		s_dlg = new SettersGettersDlg(Frame::Get());
	}

	s_dlg->Init(tags, tag->GetFile(), lineno);
	if (s_dlg->ShowModal() == wxID_OK) {
		wxString code = s_dlg->GetGenCode();
		if (code.IsEmpty() == false) {
			editor.InsertTextWithIndentation(code, lineno);
		}

		if ( s_dlg->GetFormatText() )
			DoFormatActiveEditor();
	}
}

void ContextCpp::OnKeyDown(wxKeyEvent &event)
{
	LEditor &ctrl = GetCtrl();
	if (ctrl.GetFunctionTip()->IsActive()) {
		switch (event.GetKeyCode()) {
		case WXK_UP:
			ctrl.GetFunctionTip()->SelectPrev(DoGetCalltipParamterIndex());
			return;
		
		case WXK_DOWN:
			ctrl.GetFunctionTip()->SelectNext(DoGetCalltipParamterIndex());
			return;
		}
	}
	event.Skip();
}

void ContextCpp::OnFindImpl(wxCommandEvent &event)
{
	DoGotoSymbol(GetTagAtCaret(true, true));
}

void ContextCpp::OnFindDecl(wxCommandEvent &event)
{
	DoGotoSymbol(GetTagAtCaret(true, false));
}

void ContextCpp::OnUpdateUI(wxUpdateUIEvent &event)
{
	bool workspaceOpen = ManagerST::Get()->IsWorkspaceOpen();
	bool projectAvailable = (GetCtrl().GetProjectName().IsEmpty() == false);

	if (event.GetId() == XRCID("insert_doxy_comment")) {
		event.Enable(projectAvailable);
	} else if (event.GetId() == XRCID("setters_getters")) {
		event.Enable(projectAvailable);
	} else if (event.GetId() == XRCID("go_to_function_start")) {
		event.Enable(workspaceOpen);
	} else if (event.GetId() == XRCID("go_to_next_function")) {
		event.Enable(workspaceOpen);
	} else if (event.GetId() == XRCID("find_decl")) {
		event.Enable(workspaceOpen);
	} else if (event.GetId() == XRCID("find_impl")) {
		event.Enable(workspaceOpen);
	} else if (event.GetId() == XRCID("move_impl")) {
		event.Enable(projectAvailable && GetCtrl().GetSelectedText().IsEmpty() == false );
	} else {
		event.Skip();
	}
}

void ContextCpp::SetActive()
{
	wxScintillaEvent dummy;
	OnSciUpdateUI(dummy);
}

void ContextCpp::OnSciUpdateUI(wxScintillaEvent &event)
{
	wxUnusedVar(event);
	LEditor &ctrl = GetCtrl();

	static long lastPos(wxNOT_FOUND);
	static long lastLine(wxNOT_FOUND);

	//get the current position
	long curpos = ctrl.GetCurrentPos();
	if (curpos != lastPos) {
		lastPos = curpos;

		// update the calltip highlighting if needed
		DoUpdateCalltipHighlight();

		// update navigation bar, but do this only if it visible
		if ( !Frame::Get()->GetMainBook()->IsNavBarShown() )
			return;

		// we know that the position position has changed, make sure that the line
		// number has changed also
		if (ctrl.LineFromPosition(curpos) != lastLine) {
			lastLine = ctrl.LineFromPosition(curpos);
			Frame::Get()->GetMainBook()->UpdateNavBar(&ctrl);

		}
	}
}

void ContextCpp::OnDbgDwellEnd(wxScintillaEvent &event)
{
	wxUnusedVar(event);
	// remove the debugger indicator
	GetCtrl().SetIndicatorCurrent(DEBUGGER_INDICATOR);
	GetCtrl().IndicatorClearRange(0, GetCtrl().GetLength());
}

void ContextCpp::OnDbgDwellStart(wxScintillaEvent & event)
{
	static wxRegEx reCppIndentifier(wxT("[a-zA-Z_][a-zA-Z0-9_]*"));

	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract()) {
		if ( ManagerST::Get()->GetDisplayVariableDialog()->IsShown() ) {
			// a 'Quick Show dialog' is already shown!
			// dont show another tip
			return;
		}
	}

	wxPoint pt;
	wxString word;
	pt.x = event.GetX();
	pt.y = event.GetY();
	LEditor &ctrl = GetCtrl();
	int pos = event.GetPosition();
	if (pos != wxNOT_FOUND) {

		if (IsCommentOrString(pos)) {
			return;
		}

		long end(0);
		long sel_start(0), sel_end(0);

		end = ctrl.WordEndPosition  (pos, true);

		// if thers is no selected text, use the word calculated from the caret position
		if (!ctrl.GetSelectedText().IsEmpty()) {
			// selection is not empty, use it
			sel_start = ctrl.GetSelectionStart();
			sel_end   = ctrl.GetSelectionEnd  ();
			word = ctrl.GetTextRange(sel_start, sel_end);

			// Mark the code we are going to try and show tip for
			GetCtrl().SetIndicatorCurrent(DEBUGGER_INDICATOR);
			GetCtrl().IndicatorFillRange(sel_start, sel_end - sel_start);

		} else {
			word = GetExpression(end, false, &GetCtrl(), false);
			word.Trim().Trim(false);

			// Mark the code we are going to try and show tip for
			GetCtrl().SetIndicatorCurrent(DEBUGGER_INDICATOR);
			GetCtrl().IndicatorFillRange(end - word.length(), word.Length());
		}

		if (word.IsEmpty()) {
			return;
		}
	} else {
		return;
	}

	dbgr->CreateVariableObject( word, DBG_USERR_QUICKWACTH );
}

int ContextCpp::FindLineToAddInclude()
{
	LEditor &ctrl = GetCtrl();

	int maxLineToScan = ctrl.GetLineCount();
	if (maxLineToScan > 100) {
		maxLineToScan = 100;
	}

	for (int i=0; i<maxLineToScan; i++) {
		if (IsIncludeStatement(ctrl.GetLine(i))) {
			return i;
		}
	}
	return wxNOT_FOUND;
}

void ContextCpp::OnMoveImpl(wxCommandEvent &e)
{
	wxUnusedVar(e);
	LEditor &rCtrl = GetCtrl();
	VALIDATE_WORKSPACE();

	//get expression
	int pos = rCtrl.GetCurrentPos();
	int word_end = rCtrl.WordEndPosition(pos, true);
	int word_start = rCtrl.WordStartPosition(pos, true);

	// get the scope
	wxString word = rCtrl.GetTextRange(word_start, word_end);

	if (word.IsEmpty())
		return;

	std::vector<TagEntryPtr> tags;
	int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPosition())+1;
	TagsManagerST::Get()->FindSymbol(word, tags);
	if (tags.empty())
		return;


	//get this scope name
	int startPos(0);
	wxString scopeText = rCtrl.GetTextRange(startPos, rCtrl.GetCurrentPos());

	//get the scope name from the text
	wxString scopeName = TagsManagerST::Get()->GetScopeName(scopeText);
	if (scopeName.IsEmpty()) {
		scopeName = wxT("<global>");
	}

	TagEntryPtr tag;
	bool match(false);
	for (std::vector< TagEntryPtr >::size_type i=0; i< tags.size(); i++) {
		if (tags.at(i)->GetName() == word &&
		        tags.at(i)->GetLine() == line &&
		        tags.at(i)->GetKind() == wxT("function") &&
		        tags.at(i)->GetScope() == scopeName) {
			//we got a match
			tag = tags.at(i);
			match = true;
			break;
		}
	}

	if (match) {

		long curPos = word_end;
		long blockEndPos(wxNOT_FOUND);
		long blockStartPos(wxNOT_FOUND);
		wxString content;

		if (DoGetFunctionBody(curPos, blockStartPos, blockEndPos, content)) {

			//create the functions body
			wxString body = TagsManagerST::Get()->FormatFunction(tag, true);
			//remove the empty content provided by this function
			body = body.BeforeLast(wxT('{'));
			body = body.Trim().Trim(false);
			body.Prepend(wxT("\n"));
			body << content << wxT("\n");

			wxString targetFile;
			FindSwappedFile(rCtrl.GetFileName(), targetFile);
			MoveFuncImplDlg *dlg = new MoveFuncImplDlg(NULL, body, targetFile);
			if (dlg->ShowModal() == wxID_OK) {
				//get the updated data
				targetFile = dlg->GetFileName();
				body = dlg->GetText();
				if (OpenFileAndAppend(targetFile, body)) {
					//remove the current body and replace it with ';'
					rCtrl.SetTargetEnd(blockEndPos);
					rCtrl.SetTargetStart(blockStartPos);
					rCtrl.ReplaceTarget(wxT(";"));
				}
			}
			dlg->Destroy();
		}
	}
}

bool ContextCpp::DoGetFunctionBody(long curPos, long &blockStartPos, long &blockEndPos, wxString &content)
{
	LEditor &rCtrl = GetCtrl();
	blockStartPos  = wxNOT_FOUND;
	blockEndPos = wxNOT_FOUND;

	//scan for the functions' start block
	while ( true ) {
		curPos = rCtrl.PositionAfter(curPos);

		//eof?
		if (curPos == rCtrl.GetLength()) {
			break;
		}

		//comment?
		if (IsCommentOrString(curPos)) {
			continue;
		}

		//valid character
		wxChar ch = rCtrl.GetCharAt(curPos);
		if (ch == wxT(';')) {
			//no implementation to move
			break;
		}

		if (ch == wxT('{')) {
			blockStartPos = curPos;
			break;
		}
	}

	//collect the functions' block
	if (blockStartPos != wxNOT_FOUND) {
		int depth(1);
		content << wxT("{");
		while (depth > 0) {
			curPos = rCtrl.PositionAfter(curPos);
			//eof?
			if (curPos == rCtrl.GetLength()) {
				break;
			}

			//comment?
			wxChar ch = rCtrl.GetCharAt(curPos);
			if (IsCommentOrString(curPos)) {
				content << ch;
				continue;
			}

			switch (ch) {
			case wxT('{'):
							depth++;
				break;
			case wxT('}'):
							depth--;
				break;
			}
			content << ch;
		}

		if (depth == 0) {
			blockEndPos = rCtrl.PositionAfter(curPos);
		}
	}

	return 	(blockEndPos > blockStartPos) 	&&
	        (blockEndPos != wxNOT_FOUND) 	&&
	        (blockStartPos != wxNOT_FOUND);
}

void ContextCpp::OnOverrideParentVritualFunctions(wxCommandEvent& e)
{
	LEditor &rCtrl = GetCtrl();
	VALIDATE_WORKSPACE();

	// Get the text from the file start point until the current position
	int      pos      = rCtrl.GetCurrentPos();
	wxString context  = rCtrl.GetTextRange(0, pos);
	bool     onlyPure = e.GetId() == XRCID("add_pure_virtual_impl");

	wxString scopeName = TagsManagerST::Get()->GetScopeName(context);
	if (scopeName.IsEmpty() || scopeName == wxT("<global>")) {
		wxMessageBox(_("Cant resolve scope properly. Found <") + scopeName + wxT(">"), wxT("CodeLite"), wxICON_INFORMATION|wxOK);
		return;
	}

	// get map of all unimlpemented methods
	std::vector<TagEntryPtr> protos;
	TagsManagerST::Get()->GetUnOverridedParentVirtualFunctions(scopeName, onlyPure, protos);

	// No methods to add?
	if (protos.empty())
		return;

	// Locate the swapped file
	wxString targetFile(rCtrl.GetFileName().GetFullPath());
	FindSwappedFile(rCtrl.GetFileName(), targetFile);

	CommentConfigData data;
	EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

	//get doxygen comment based on file and line
	wxChar keyPrefix(wxT('\\'));
	if (data.GetUseShtroodel()) {
		keyPrefix = wxT('@');
	}

	ImplementParentVirtualFunctionsDialog dlg(wxTheApp->GetTopWindow(), scopeName, protos, keyPrefix, this);
	dlg.m_textCtrlImplFile->SetValue(targetFile);
	if (dlg.ShowModal() == wxID_OK) {
		wxString implFile = dlg.m_textCtrlImplFile->GetValue();
		wxString impl     = dlg.GetImpl();
		wxString decl     = dlg.GetDecl();
		rCtrl.InsertText(rCtrl.GetCurrentPos(), decl);
		if (dlg.m_checkBoxFormat->IsChecked())
			DoFormatActiveEditor();

		// Open teh implementation file and format it if needed
		OpenFileAppendAndFormat(implFile, impl, dlg.m_checkBoxFormat->IsChecked());
	}

	// Restore this file to be the active one
	Frame::Get()->GetMainBook()->OpenFile(GetCtrl().GetFileName().GetFullPath());
}

void ContextCpp::OnAddMultiImpl(wxCommandEvent &e)
{
	wxUnusedVar(e);
	LEditor &rCtrl = GetCtrl();
	VALIDATE_WORKSPACE();

	//get the text from the file start point until the current position
	int pos = rCtrl.GetCurrentPos();
	wxString context = rCtrl.GetTextRange(0, pos);

	wxString scopeName = TagsManagerST::Get()->GetScopeName(context);
	if (scopeName.IsEmpty() || scopeName == wxT("<global>")) {
		wxMessageBox(_("'Add Functions Implementation' can only work inside valid scope, got (") + scopeName + wxT(")"), wxT("CodeLite"), wxICON_INFORMATION|wxOK);
		return;
	}

	// get map of all unimlpemented methods
	std::map<wxString, TagEntryPtr> protos;
	TagsManagerST::Get()->GetUnImplementedFunctions( scopeName, protos );

	// the map now consist only with functions without implementation
	// create body for all of those functions
	//create the functions body
	wxString body;
	std::map<wxString, TagEntryPtr>::iterator iter = protos.begin();

	for (; iter != protos.end(); iter ++ ) {
		TagEntryPtr tag = iter->second;
		//use normalize function signature rather than the default one
		//this will ensure that default values are removed
		tag->SetSignature(TagsManagerST::Get()->NormalizeFunctionSig( tag->GetSignature(), Normalize_Func_Name ));
		body << TagsManagerST::Get()->FormatFunction(tag, true);
		body << wxT("\n");
	}

	wxString targetFile;
	FindSwappedFile(rCtrl.GetFileName(), targetFile);

	//if no swapped file is found, use the current file
	if (targetFile.IsEmpty()) {
		targetFile = rCtrl.GetFileName().GetFullPath();
	}

	MoveFuncImplDlg dlg(NULL, body, targetFile);
	dlg.SetTitle(wxT("Implement All Un-Implemented Functions"));
	if (dlg.ShowModal() == wxID_OK) {
		//get the updated data
		targetFile = dlg.GetFileName();
		body = dlg.GetText();
		OpenFileAndAppend(targetFile, body);
	}
}

void ContextCpp::OnAddImpl(wxCommandEvent &e)
{
	wxUnusedVar(e);
	LEditor &rCtrl = GetCtrl();
	VALIDATE_WORKSPACE();

	//get expression
	int pos = rCtrl.GetCurrentPos();
	int word_end = rCtrl.WordEndPosition(pos, true);
	int word_start = rCtrl.WordStartPosition(pos, true);

	// get the scope
	wxString word = rCtrl.GetTextRange(word_start, word_end);

	if (word.IsEmpty())
		return;

	std::vector<TagEntryPtr> tags;
	int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPosition())+1;

	//get this scope name
	int startPos(0);
	wxString scopeText = rCtrl.GetTextRange(startPos, rCtrl.GetCurrentPos());

	//get the scope name from the text
	wxString scopeName = TagsManagerST::Get()->GetScopeName(scopeText);
	if (scopeName.IsEmpty()) {
		scopeName = wxT("<global>");
	}

	TagsManagerST::Get()->FindSymbol(word, tags);
	if (tags.empty())
		return;

	TagEntryPtr tag;
	bool match(false);
	for (std::vector< TagEntryPtr >::size_type i=0; i< tags.size(); i++) {
		if (tags.at(i)->GetName() == word &&
		        tags.at(i)->GetLine() == line &&
		        tags.at(i)->GetKind() == wxT("prototype") &&
		        tags.at(i)->GetScope() == scopeName) {
			//we got a match
			tag = tags.at(i);
			match = true;
			break;
		}
	}

	if (match) {

		long curPos = word_end;
		long blockEndPos(wxNOT_FOUND);
		long blockStartPos(wxNOT_FOUND);
		wxString content;

		if (DoGetFunctionBody(curPos, blockStartPos, blockEndPos, content)) {
			//function already has body ...
			wxMessageBox(_("Function '") + tag->GetName() + wxT("' already has a body"), wxT("CodeLite"), wxICON_WARNING|wxOK);
			return;
		}

		//create the functions body
		//replace the function signature with the normalized one, so default values
		//will not appear in the function implementation
		wxString newSig = TagsManagerST::Get()->NormalizeFunctionSig( tag->GetSignature(), Normalize_Func_Name );
		tag->SetSignature( newSig );
		wxString body = TagsManagerST::Get()->FormatFunction(tag, true);

		wxString targetFile;
		FindSwappedFile(rCtrl.GetFileName(), targetFile);

		//if no swapped file is found, use the current file
		if (targetFile.IsEmpty()) {
			targetFile = rCtrl.GetFileName().GetFullPath();
		}

		MoveFuncImplDlg dlg(NULL, body, targetFile);
		dlg.SetTitle(wxT("Add Function Implmentation"));
		if (dlg.ShowModal() == wxID_OK) {
			//get the updated data
			targetFile = dlg.GetFileName();
			body       = dlg.GetText();
			OpenFileAndAppend(targetFile, body);
		}
	}
}

void ContextCpp::DoFormatActiveEditor()
{
	IPlugin *formatter = PluginManager::Get()->GetPlugin(wxT("CodeFormatter"));
	if (formatter) {
		// code formatter is available, format the current source file
		wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("format_source"));
		Frame::Get()->GetEventHandler()->ProcessEvent(e);
	}
}

bool ContextCpp::OpenFileAppendAndFormat(const wxString& fileName, const wxString& text, bool format)
{
	OpenFileAndAppend(fileName, text);
	if (format)
		DoFormatActiveEditor();
	return true;
}

bool ContextCpp::OpenFileAndAppend ( const wxString &fileName, const wxString &text )
{
	LEditor *editor = Frame::Get()->GetMainBook()->OpenFile(fileName, wxEmptyString, 0);
	if (!editor)
		return false;

	// if needed, append EOL
	if (editor->GetText().EndsWith(editor->GetEolString()) == false) {
		editor->AppendText(editor->GetEolString());
	}

	int lineNum = editor->GetLineCount();
	editor->GotoLine ( lineNum-1 );
	editor->AppendText ( text );
	return true;
}

void ContextCpp::OnFileSaved()
{
	PERF_FUNCTION();

	VariableList var_list;
	std::map< std::string, Variable > var_map;
	std::map< wxString, TagEntryPtr> foo_map;
	std::map<std::string, std::string> ignoreTokens;

	wxArrayString varList;
	wxArrayString projectTags;

	LEditor &rCtrl = GetCtrl();
	VALIDATE_WORKSPACE();

	// if there is nothing to color, go ahead and return
	if ( !(TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_COLOUR_WORKSPACE_TAGS) && !(TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_COLOUR_VARS) ) {
		return;
	}

	// wxSCI_C_WORD2
	if (TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_COLOUR_WORKSPACE_TAGS) {

		// get list of all tags from the workspace
		TagsManagerST::Get()->GetAllTagsNames(projectTags);
	}
	// wxSCI_C_GLOBALCLASS
	if (TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_COLOUR_VARS) {
		//---------------------------------------------------------------------
		// Colour local variables
		//---------------------------------------------------------------------
		PERF_BLOCK("Getting Locals") {

			const wxCharBuffer patbuf = _C(rCtrl.GetText());

			// collect list of variables
			get_variables( patbuf.data(), var_list, ignoreTokens, false);

		}

		// list all functions of this file
		std::vector< TagEntryPtr > tags;
		TagsManagerST::Get()->GetFunctions(tags, rCtrl.GetFileName().GetFullPath());

		PERF_BLOCK("Adding Functions") {

			VariableList::iterator viter = var_list.begin();
			for (; viter != var_list.end(); viter++ ) {
				Variable vv = *viter;
				varList.Add(_U(vv.m_name.c_str()));
			}

			// parse all function's arguments and add them as well
			for (size_t i=0; i<tags.size(); i++) {
				wxString sig = tags.at(i)->GetSignature();
				const wxCharBuffer cb = _C(sig);
				VariableList vars_list;
				get_variables(cb.data(), vars_list, ignoreTokens, true);
				VariableList::iterator it = vars_list.begin();
				for (; it != vars_list.end(); it++ ) {
					Variable var = *it;
					wxString name = _U(var.m_name.c_str());
					if (varList.Index(name) == wxNOT_FOUND) {
						// add it
						varList.Add(name);
					}
				}
			}

		}
	}
	PERF_BLOCK("Setting Keywords") {

		size_t cc_flags = TagsManagerST::Get()->GetCtagsOptions().GetFlags();
		if (cc_flags & CC_COLOUR_WORKSPACE_TAGS) {
			wxString flatStr;
			for (size_t i=0; i< projectTags.GetCount(); i++) {
				// add only entries that does not appear in the variable list
				//if (varList.Index(projectTags.Item(i)) == wxNOT_FOUND) {
				flatStr << projectTags.Item(i) << wxT(" ");
			}
			rCtrl.SetKeyWords(1, flatStr);
		} else {
			rCtrl.SetKeyWords(1, wxEmptyString);
		}

		if (cc_flags & CC_COLOUR_VARS) {
			// convert it to space delimited string
			wxString varFlatStr;
			for (size_t i=0; i< varList.GetCount(); i++) {
				varFlatStr << varList.Item(i) << wxT(" ");
			}
			rCtrl.SetKeyWords(3, varFlatStr);
		} else {
			rCtrl.SetKeyWords(3, wxEmptyString);
		}
	}
}

void ContextCpp::ApplySettings()
{
	//-----------------------------------------------
	// Load laguage settings from configuration file
	//-----------------------------------------------
	SetName(wxT("C++"));

	// Set the key words and the lexer
	LexerConfPtr lexPtr;
	// Read the configuration file
	if (EditorConfigST::Get()->IsOk()) {
		lexPtr = EditorConfigST::Get()->GetLexer(wxT("C++"));
	}

	// Update the control
	LEditor &rCtrl = GetCtrl();
	rCtrl.SetLexer((int)lexPtr->GetLexerId());

	wxString keyWords = lexPtr->GetKeyWords(0);
	keyWords.Replace(wxT("\n"), wxT(" "));
	keyWords.Replace(wxT("\r"), wxT(" "));
	rCtrl.SetKeyWords(0, keyWords);

	DoApplySettings( lexPtr );

	//create all images used by the cpp context
	wxImage img;
	if (m_classBmp.IsOk() == false) {
		m_classBmp = wxXmlResource::Get()->LoadBitmap(wxT("class"));
		m_structBmp = wxXmlResource::Get()->LoadBitmap(wxT("struct"));
		m_namespaceBmp = wxXmlResource::Get()->LoadBitmap(wxT("namespace"));
		m_variableBmp = wxXmlResource::Get()->LoadBitmap(wxT("member_public"));
		m_tpyedefBmp = wxXmlResource::Get()->LoadBitmap(wxT("typedef"));
		m_tpyedefBmp.SetMask(new wxMask(m_tpyedefBmp, wxColor(0, 128, 128)));

		m_memberPrivateBmp = wxXmlResource::Get()->LoadBitmap(wxT("member_private"));
		m_memberPublicBmp = wxXmlResource::Get()->LoadBitmap(wxT("member_public"));
		m_memberProtectedeBmp = wxXmlResource::Get()->LoadBitmap(wxT("member_protected"));
		m_functionPrivateBmp = wxXmlResource::Get()->LoadBitmap(wxT("func_private"));
		m_functionPublicBmp = wxXmlResource::Get()->LoadBitmap(wxT("func_public"));
		m_functionProtectedeBmp = wxXmlResource::Get()->LoadBitmap(wxT("func_protected"));
		m_macroBmp = wxXmlResource::Get()->LoadBitmap(wxT("typedef"));
		m_macroBmp.SetMask(new wxMask(m_macroBmp, wxColor(0, 128, 128)));

		m_enumBmp = wxXmlResource::Get()->LoadBitmap(wxT("enum"));
		m_enumBmp.SetMask(new wxMask(m_enumBmp, wxColor(0, 128, 128)));

		m_enumeratorBmp = wxXmlResource::Get()->LoadBitmap(wxT("enumerator"));

		//Initialise the file bitmaps
		m_cppFileBmp = wxXmlResource::Get()->LoadBitmap(wxT("page_white_cplusplus"));
		m_hFileBmp = wxXmlResource::Get()->LoadBitmap(wxT("page_white_h"));
		m_otherFileBmp = wxXmlResource::Get()->LoadBitmap(wxT("page_white_text"));
	}

	//register the images
	rCtrl.ClearRegisteredImages();
	rCtrl.RegisterImage(1, m_classBmp);
	rCtrl.RegisterImage(2, m_structBmp);
	rCtrl.RegisterImage(3, m_namespaceBmp);
	rCtrl.RegisterImage(4, m_variableBmp);
	rCtrl.RegisterImage(5, m_tpyedefBmp);
	rCtrl.RegisterImage(6, m_memberPrivateBmp);
	rCtrl.RegisterImage(7, m_memberPublicBmp);
	rCtrl.RegisterImage(8, m_memberProtectedeBmp);
	rCtrl.RegisterImage(9, m_functionPrivateBmp);
	rCtrl.RegisterImage(10, m_functionPublicBmp);
	rCtrl.RegisterImage(11, m_functionProtectedeBmp);
	rCtrl.RegisterImage(12, m_macroBmp);
	rCtrl.RegisterImage(13, m_enumBmp);
	rCtrl.RegisterImage(14, m_enumeratorBmp);
	rCtrl.RegisterImage(15, m_cppFileBmp);
	rCtrl.RegisterImage(16, m_hFileBmp);
	rCtrl.RegisterImage(17, m_otherFileBmp);

	//delete uneeded commands
	rCtrl.CmdKeyClear('/', wxSCI_SCMOD_CTRL);
	rCtrl.CmdKeyClear('/', wxSCI_SCMOD_CTRL|wxSCI_SCMOD_SHIFT);

	// update word characters to allow '~' as valid word character
	rCtrl.SetWordChars(wxT("~_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"));
}

void ContextCpp::Initialize()
{
	//load the context menu from the resource manager
	m_rclickMenu = wxXmlResource::Get()->LoadMenu(wxT("editor_right_click"));
}

void ContextCpp::AutoAddComment()
{
	LEditor &rCtrl = GetCtrl();

	CommentConfigData data;
	EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

	int curpos = rCtrl.GetCurrentPos();
	int line = rCtrl.LineFromPosition(curpos);
	int cur_style = rCtrl.GetStyleAt(curpos);

	bool dontadd = false;
	switch (cur_style) {
	case wxSCI_C_COMMENTLINE:
		dontadd = rCtrl.GetLine(line-1).Trim().Trim(false) == wxT("//") || !data.GetContinueCppComment();
		break;
	case wxSCI_C_COMMENT:
	case wxSCI_C_COMMENTDOC:
		dontadd = !data.GetAddStarOnCComment();
		break;
	default:
		dontadd = true;
		break;
	}
	if (dontadd) {
		ContextBase::AutoIndent(wxT('\n'));
		return;
	}

	wxString toInsert;
	switch (cur_style) {
	case wxSCI_C_COMMENTLINE:
		if (rCtrl.GetStyleAt(rCtrl.PositionAfter(curpos)) != wxSCI_C_COMMENTLINE) {
			toInsert = wxT("// ");
		}
		break;
	case wxSCI_C_COMMENT:
	case wxSCI_C_COMMENTDOC:
		if (rCtrl.GetStyleAt(rCtrl.PositionBefore(rCtrl.PositionBefore(curpos))) == cur_style) {
			toInsert = rCtrl.GetCharAt(rCtrl.GetLineIndentPosition(line-1)) == wxT('*') ? wxT("* ") : wxT(" * ");
		}
		break;
	}
	rCtrl.SetLineIndentation(line, rCtrl.GetLineIndentation(line-1));
	int insertPos = rCtrl.GetLineIndentPosition(line);
	rCtrl.InsertText(insertPos, toInsert);
	rCtrl.SetCaretAt(insertPos + toInsert.Length());
	rCtrl.ChooseCaretX(); // set new column as "current" column
}

bool ContextCpp::IsComment(long pos)
{
	int style;
	style = GetCtrl().GetStyleAt(pos);
	return (style == wxSCI_C_COMMENT				||
	        style == wxSCI_C_COMMENTLINE			||
	        style == wxSCI_C_COMMENTDOC				||
	        style == wxSCI_C_COMMENTLINEDOC			||
	        style == wxSCI_C_COMMENTDOCKEYWORD		||
	        style == wxSCI_C_COMMENTDOCKEYWORDERROR   );
}

void ContextCpp::OnRenameFunction(wxCommandEvent& e)
{
	VALIDATE_WORKSPACE();

	LEditor &rCtrl = GetCtrl();
	CppTokensMap l;

	//get expression
	int pos = rCtrl.GetCurrentPos();
	int word_end = rCtrl.WordEndPosition(pos, true);
	int word_start = rCtrl.WordStartPosition(pos, true);

	// get the scope
	wxString word = rCtrl.GetTextRange(word_start, word_end);
	if (word.IsEmpty())
		return;

	// search to see if we are on a valid tag
	RefactorSource source;
	if (!ResolveWord(&rCtrl, word_start, word, &source)) {
		// parsing of the initial expression failed, abort
		return;
	}

	if (!Frame::Get()->GetMainBook()->SaveAll(true, false))
		return;

	wxLogMessage(wxT("Refactoring: ") + source.name + wxT(" of scope: ") + source.scope);

	// load all tokens, first we need to parse the workspace files...
	BuildRefactorDatabase(word, l);
	std::list<CppToken> tokens;

	// incase no tokens were found (possibly cause of user pressing cancel
	// abort this operation
	l.findTokens(word, tokens);
	if (tokens.empty()) {
		return;
	}

	wxString msg;
	msg << wxT("Found ") << tokens.size() << wxT(" instances of ") << word;
	wxLogMessage(msg);

	// create an empty hidden instance of LEditor
	LEditor *editor = new LEditor(rCtrl.GetParent());
	editor->Show(false);
	editor->SetIsVisible(false);

	// Get expressions for the CC to work with:
	RefactorSource target;
	std::list<CppToken> candidates;
	std::list<CppToken> possibleCandidates;

	wxProgressDialog* prgDlg = new wxProgressDialog (wxT("Parsing matches..."), wxT("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"), (int)tokens.size(), NULL, wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_AUTO_HIDE | wxPD_CAN_ABORT);
	prgDlg->GetSizer()->Fit(prgDlg);
	prgDlg->Layout();
	prgDlg->Centre();

	std::list<CppToken>::iterator iter = tokens.begin();
	int counter(0);

	for (; iter != tokens.end(); iter++) {
		CppToken token = *iter;
		editor->Create(wxEmptyString, token.getFilename());
		token.setLineNo( editor->LineFromPosition( (int)token.getOffset() ) + 1 );
		token.setLine( editor->GetLine( token.getLineNo()-1 ) );

		wxString msg;
		wxFileName f(token.getFilename());
		msg << wxT("Parsing expression ") << counter << wxT("/") << tokens.size() << wxT(" in file: ") << f.GetFullName();
		if ( !prgDlg->Update(counter, msg) ) {
			// user clicked 'Cancel'
			prgDlg->Destroy();
			editor->Destroy();
			return;
		}

		counter++;

		// reset the result
		target.Reset();
		if (ResolveWord(editor, token.getOffset(), word, &target)) {

			if (target.name == source.name && target.scope == source.scope) {
				// full match
				candidates.push_back( token );
			} else if (target.name == source.scope && !source.isClass) {
				// source is function, and target is class
				candidates.push_back( token );
			} else if (target.name == source.name && source.isClass) {
				// source is class, and target is ctor
				candidates.push_back( token );
			} else {
				// add it to the possible match list
				possibleCandidates.push_back( token );
			}
		} else {
			// resolved word failed, add it to the possible list
			possibleCandidates.push_back( token );
		}
	}

	editor->Destroy();
	prgDlg->Destroy();

	// display the refactor dialog
	RenameSymbol *dlg = new RenameSymbol(&rCtrl, candidates, possibleCandidates, source.name);
	if (dlg->ShowModal() == wxID_OK) {
		std::list<CppToken> matches;

		dlg->GetMatches( matches );
		if (matches.empty() == false) {
			ReplaceInFiles(dlg->GetWord(), matches);
		}
	}
	dlg->Destroy();
}

void ContextCpp::BuildRefactorDatabase ( const wxString& word, CppTokensMap &l )
{
	wxArrayString projects;
	ManagerST::Get()->GetProjectList ( projects );
	std::vector<wxFileName> files;

	for ( size_t i=0; i<projects.GetCount(); i++ ) {
		ProjectPtr proj = ManagerST::Get()->GetProject ( projects.Item ( i ) );
		if ( proj ) {
			proj->GetFiles ( files, true );
		}
	}
	RefactorIndexBuildJob job ( files, word.c_str() );
	job.Parse ( word, l );
}

void ContextCpp::ReplaceInFiles ( const wxString &word, std::list<CppToken> &li )
{
	int off = 0;
	wxString file_name ( wxEmptyString );

	for ( std::list<CppToken>::iterator iter = li.begin(); iter != li.end(); iter++ ) {
		CppToken &token = *iter;
		if ( file_name == token.getFilename() ) {
			// update next token offset in case we are still in the same file
			token.setOffset ( token.getOffset() + off );
		} else {
			// switched file
			off = 0;
			file_name = token.getFilename();
		}
		LEditor *editor = Frame::Get()->GetMainBook()->OpenFile(token.getFilename(), wxEmptyString, 0);
		if (editor != NULL && (editor->GetFileName().GetFullPath().CmpNoCase(token.getFilename()) == 0) ) {
			editor->SetSelection ( token.getOffset(), token.getOffset()+token.getName().Len() );
			if ( editor->GetSelectionStart() != editor->GetSelectionEnd() ) {
				editor->ReplaceSelection ( word );
				off += word.Len() - token.getName().Len();
			}
		}
	}
}

bool ContextCpp::ResolveWord(LEditor *ctrl, int pos, const wxString &word, RefactorSource *rs)
{
	std::vector<TagEntryPtr> tags;
	// try to process the current expression
	wxString expr = GetExpression(pos + word.Len(), false, ctrl);

	// get the scope
	//Optimize the text for large files
	int line = ctrl->LineFromPosition(pos)+1;
	wxString text = ctrl->GetTextRange(0, pos + word.Len());

	// we simply collect declarations & implementations

	//try implemetation first
	bool found(false);
	TagsManagerST::Get()->FindImplDecl(ctrl->GetFileName(), line, expr, word, text, tags, true, true);
	if (tags.empty() == false) {
		// try to see if we got a function and not class/struct

		for (size_t i=0; i<tags.size(); i++) {
			TagEntryPtr tag = tags.at(i);
			// find first non class/struct tag
			if (tag->GetKind() != wxT("class") && tag->GetKind() != wxT("struct")) {

				// if there is no match, add it anyways
				if (!found) {
					rs->isClass = (tag->GetKind() == wxT("class") ||tag->GetKind() == wxT("struct"));
					rs->name = tag->GetName();
					rs->scope = tag->GetScope();
					found = true;
				} else if (rs->scope == wxT("<global>") && rs->isClass == false) {
					// give predecense to <global> variables
					rs->isClass = (tag->GetKind() == wxT("class") ||tag->GetKind() == wxT("struct"));
					rs->name = tag->GetName();
					rs->scope = tag->GetScope();
					found = true;
				}
				found = true;
			}
		}

		// if no match was found, keep the first result but keep searching
		if ( !found ) {
			TagEntryPtr tag = tags.at(0);
			rs->scope = tag->GetScope();
			rs->name = tag->GetName();
			rs->isClass = (tag->GetKind() == wxT("class") ||tag->GetKind() == wxT("struct"));
			found = true;
		} else {
			return true;
		}
	}

	// Ok, the "implementation" search did not yield definite results, try declaration
	tags.clear();
	TagsManagerST::Get()->FindImplDecl(ctrl->GetFileName(), line, expr, word, text, tags, false, true);
	if (tags.empty() == false) {
		// try to see if we got a function and not class/struct
		for (size_t i=0; i<tags.size(); i++) {
			TagEntryPtr tag = tags.at(i);
			// find first non class/struct tag
			if (tag->GetKind() != wxT("class") && tag->GetKind() != wxT("struct")) {
				rs->name = tag->GetName();
				rs->scope = tag->GetScope();
				return true;
			}
		}

		// if no match was found, keep the first result but keep searching
		if ( !found ) {
			TagEntryPtr tag = tags.at(0);
			rs->scope = tag->GetScope();
			rs->name = tag->GetName();
			rs->isClass = (tag->GetKind() == wxT("class") ||tag->GetKind() == wxT("struct"));
		}
		return true;
	}

	// if we got so far, CC failed to parse the expression
	return false;
}

void ContextCpp::OnRetagFile(wxCommandEvent& e)
{
	VALIDATE_WORKSPACE();

	wxUnusedVar(e);
	LEditor &ctrl = GetCtrl();
	if ( ctrl.GetModify() ) {
		wxMessageBox(wxString::Format(wxT("Please save the file before retagging it")));
		return;
	}

	RetagFile();
	ctrl.SetActive();
}

void ContextCpp::RetagFile()
{
	LEditor &ctrl = GetCtrl();
	ManagerST::Get()->RetagFile(ctrl.GetFileName().GetFullPath());

	// incase this file is not cache this function does nothing
	TagsManagerST::Get()->ClearCachedFile(ctrl.GetFileName().GetFullPath());

//	// clear all the queries which holds reference to this file
//	TagsManagerST::Get()->GetWorkspaceTagsCache()->DeleteByFilename(ctrl.GetFileName().GetFullPath());
//
//	// clear also the swapped file
//	wxString targetFile;
//	if (FindSwappedFile(ctrl.GetFileName(), targetFile) && targetFile.IsEmpty() == false) {
//		TagsManagerST::Get()->GetWorkspaceTagsCache()->DeleteByFilename(targetFile);
//	}
}

void ContextCpp::OnUserTypedXChars(const wxString &word)
{
	// user typed more than 3 chars, display completion box with C++ keywords
	if ( IsCommentOrString(GetCtrl().GetCurrentPos()) ) {
		return;
	}

	if (TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_CPP_KEYWORD_ASISST) {
		std::vector<TagEntryPtr> tags;
		MakeCppKeywordsTags(word, tags);
		if ( tags.empty() == false ) {
			GetCtrl().ShowCompletionBox(tags, 		// list of tags
			                            word, 		// partial word
			                            false, 		// dont show full declaration
			                            true, 		// auto hide if there is no match in the list
			                            false);		// do not automatically insert word if there is only single choice
		}
	}
}

void ContextCpp::MakeCppKeywordsTags(const wxString &word, std::vector<TagEntryPtr>& tags)
{
	wxString cppWords = wxT("and and_eq asm auto bitand bitor bool break case catch char class compl const const_cast continue default delete "
	                        "do double dynamic_cast else enum explicit export extern false float for friend goto if inline int long mutable namespace "
	                        "new not not_eq operator or or_eq private protected public register reinterpret_cast return short signed sizeof size_t static "
	                        "static_cast struct switch template this throw true try typedef typeid typename union unsigned using virtual void volatile "
	                        "wchar_t while xor xor_eq ");

	// add preprocessors
	cppWords << wxT("ifdef undef define defined include endif elif ifndef ");

	wxString s1(word);
	wxArrayString wordsArr = wxStringTokenize(cppWords, wxT(" "));
	for (size_t i=0; i<wordsArr.GetCount(); i++) {

		wxString s2(wordsArr.Item(i));
		if (s2.StartsWith(s1) || s2.Lower().StartsWith(s1.Lower())) {
			TagEntryPtr tag ( new TagEntry() );
			tag->SetName(wordsArr.Item(i));
			tag->SetKind(wxT("cpp_keyword"));
			tags.push_back(tag);
		}
	}
}

wxString ContextCpp::CallTipContent()
{
	// if we have an active call tip, return its content
	if (GetCtrl().GetFunctionTip()->IsActive())
		return GetCtrl().GetFunctionTip()->GetText();

	return wxEmptyString;
}

void ContextCpp::DoCodeComplete(long pos)
{
	long currentPosition = pos;
	bool showFuncProto = false;
	int pos1, pos2, end;
	LEditor &rCtrl = GetCtrl();
	wxChar ch = rCtrl.PreviousChar(pos, pos1);

	//	Make sure we are not on a comment section
	if (IsCommentOrString(rCtrl.PositionBefore(pos))) {
		return;
	}

	// Search for first non-whitespace wxChar

	bool showFullDecl(false);

	switch (ch) {
	case '.':
		// Class / Struct completion
		rCtrl.PreviousChar(pos1, end);
		break;
	case '>':
		// Check previous character if is '-'
		// We open drop box as well
		if (rCtrl.PreviousChar(pos1, pos2) == '-') {
			rCtrl.PreviousChar(pos2, end);
		} else {
			return;
		}
		break;
	case ':':
		// Check previous character if is ':'
		// We open drop box as well
		if (rCtrl.PreviousChar(pos1, pos2) == wxT(':')) {
			rCtrl.PreviousChar(pos2, end);
			showFullDecl = true;
		} else {
			return;
		}
		break;
	case '(':
		showFuncProto = true;
		//is this setting is on?
		if (!(TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_DISP_FUNC_CALLTIP)) {
			return;
		}
		rCtrl.PreviousChar(pos1, end);
		break;
	default:
		return;
	}

	//get expression
	wxString expr = GetExpression(currentPosition, false);

	// get the scope
	// Optimize the text for large files
	int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPosition())+1;
	int startPos(0);

	// enable faster scope name resolving if needed
	if ( !(TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_ACCURATE_SCOPE_RESOLVING) ) {
		TagEntryPtr t = TagsManagerST::Get()->FunctionFromFileLine(rCtrl.GetFileName(), line);
		if ( t ) {
			startPos = rCtrl.PositionFromLine( t->GetLine() - 1);
			if ( startPos > currentPosition ) {
				startPos = 0;
			}
		}
	}

	wxString text = rCtrl.GetTextRange(startPos, currentPosition);

	// collect all text from 0 - first scope found
	// this will help us detect statements like 'using namespace foo;'
	if (startPos) { //> 0
		//get the first function on this file
		int endPos(0);
		int endPos1(0);
		int endPos2(0);
		TagEntryPtr t2 = TagsManagerST::Get()->FirstFunctionOfFile(rCtrl.GetFileName());
		if ( t2 ) {
			endPos1 = rCtrl.PositionFromLine( t2->GetLine() - 1);
			if (endPos1 > 0 && endPos1 <= startPos) {
				endPos = endPos1;
			}
		}

		TagEntryPtr t3 = TagsManagerST::Get()->FirstScopeOfFile(rCtrl.GetFileName());
		if ( t3 ) {
			endPos2 = rCtrl.PositionFromLine( t3->GetLine() - 1);
			if (endPos2 > 0 && endPos2 <= startPos && endPos2 < endPos1) {
				endPos = endPos2;
			}
		}

		wxString globalText = rCtrl.GetTextRange(0, endPos);
		globalText.Append(wxT(";"));
		text.Prepend(globalText);
	}

	std::vector<TagEntryPtr> candidates;
	if ( showFuncProto ) {
		//for function prototype, the last char entered was '(', this will break
		//the logic of the Getexpression() method to workaround this, we search for
		//expression one char before the current position
		expr = GetExpression(rCtrl.PositionBefore(currentPosition), false);

		//display function tooltip
		int word_end   = rCtrl.WordEndPosition(end, true);
		int word_start = rCtrl.WordStartPosition(end, true);

		// get the token
		wxString word = rCtrl.GetTextRange(word_start, word_end);
		rCtrl.GetFunctionTip()->Add( TagsManagerST::Get()->GetFunctionTip(rCtrl.GetFileName(), line, expr, text, word) );
		rCtrl.GetFunctionTip()->Highlight(DoGetCalltipParamterIndex());
		
		// In an ideal world, we would like our tooltip to be placed 
		// on top of the caret.
		wxPoint pt = rCtrl.PointFromPosition(currentPosition);
		rCtrl.GetFunctionTip()->Activate(pt, rCtrl.GetCurrLineHeight(), rCtrl.StyleGetBackground(wxSCI_C_DEFAULT));
		
	} else {

		if (TagsManagerST::Get()->AutoCompleteCandidates(rCtrl.GetFileName(), line, expr, text, candidates)) {
			DisplayCompletionBox(candidates, wxEmptyString, showFullDecl);
		}
	}

}

int ContextCpp::GetHyperlinkRange(int pos, int &start, int &end)
{
	LEditor &rCtrl = GetCtrl();
	int lineNum = rCtrl.LineFromPosition(pos);
	wxString fileName;
	wxString line = rCtrl.GetLine(lineNum);
	if (IsIncludeStatement(line, &fileName)) {
		start = rCtrl.PositionFromLine(lineNum)+line.find(fileName);
		end = start + fileName.size();
		return start <= pos && pos <= end ? XRCID("open_include_file") : wxID_NONE;
	}
	return ContextBase::GetHyperlinkRange(pos, start, end);
}

void ContextCpp::GoHyperlink(int start, int end, int type, bool alt)
{
	if (type == XRCID("open_include_file")) {
		m_selectedWord = GetCtrl().GetTextRange(start, end);
		DoOpenWorkspaceFile();
	} else {
		if (type == XRCID("find_tag")) {
			wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED,
			                 alt ? XRCID("find_impl") : XRCID("find_decl"));
			Frame::Get()->AddPendingEvent(e);
		}
	}
}

void ContextCpp::DoOpenWorkspaceFile()
{
	wxFileName fileName(m_selectedWord);
	wxString tmpName(m_selectedWord);

	tmpName.Replace(wxT("\\"), wxT("/"));
	if (tmpName.Contains(wxT("..")))
		tmpName = fileName.GetFullName();

	std::vector<wxFileName> files, files2;
	TagsManagerST::Get()->GetFiles(fileName.GetFullName(), files);
	//filter out the all files that does not have an exact match
	for (size_t i=0; i<files.size(); i++) {
		wxString curFileName = files.at(i).GetFullPath();
		curFileName.Replace(wxT("\\"), wxT("/"));
		if (curFileName.EndsWith(tmpName)) {
			files2.push_back(files.at(i));
		}
	}

	wxString fileToOpen;
	if (files2.size() > 1) {
		wxArrayString choices;
		for (size_t i=0; i<files2.size(); i++) {
			choices.Add(files2.at(i).GetFullPath());
		}

		fileToOpen = wxGetSingleChoice(wxT("Select file to open:"), wxT("Select file"), choices, &GetCtrl());
	} else if (files2.size() == 1) {
		fileToOpen = files2.at(0).GetFullPath();
	}


	if (fileToOpen.IsEmpty() == false) {
		Frame::Get()->GetMainBook()->OpenFile(fileToOpen);
	}
}

void ContextCpp::DoCreateFile(const wxFileName& fn)
{
	// get the file name from the user
	wxString new_file = wxGetTextFromUser(_("New File Name:"), _("Create File"), fn.GetFullPath(), Frame::Get());
	if (new_file.IsEmpty()) {
		// user clicked cancel
		return;
	}

	// if the project is part of a project, add this file to the same project
	// (under the same virtual folder as well)
	if (GetCtrl().GetProject().IsEmpty() == false) {
		ProjectPtr p = ManagerST::Get()->GetProject(GetCtrl().GetProject());
		if (p) {
			wxString vd = p->GetVDByFileName(GetCtrl().GetFileName().GetFullPath());
			vd.Prepend(p->GetName() + wxT(":"));

			if (vd.IsEmpty() == false) {
				Frame::Get()->GetWorkspaceTab()->GetFileView()->CreateAndAddFile(new_file, vd);
			}
		}
	} else {
		// just a plain file
		wxFile file;
		if ( !file.Create ( new_file.GetData(), true ) )
			return;

		if ( file.IsOpened() ) {
			file.Close();
		}
	}

	TryOpenFile(wxFileName(new_file));
}

void ContextCpp::OnGotoFunctionStart(wxCommandEvent& event)
{
	int line_number = GetCtrl().LineFromPosition(GetCtrl().GetCurrentPos());
	TagEntryPtr tag = TagsManagerST::Get()->FunctionFromFileLine(GetCtrl().GetFileName(), line_number);
	if (tag) {
		// move the caret to the function start
		BrowseRecord jumpfrom = GetCtrl().CreateBrowseRecord();
		GetCtrl().SetCaretAt(GetCtrl().PositionFromLine(tag->GetLine()-1));
		// add an entry to the navigation manager
		NavMgr::Get()->AddJump(jumpfrom, GetCtrl().CreateBrowseRecord());
	}
}

void ContextCpp::OnGotoNextFunction(wxCommandEvent& event)
{
	int line_number = GetCtrl().LineFromPosition(GetCtrl().GetCurrentPos());
	TagEntryPtr tag = TagsManagerST::Get()->FunctionFromFileLine(GetCtrl().GetFileName(), line_number+1, true);
	if (tag) {
		// move the caret to the function start
		BrowseRecord jumpfrom = GetCtrl().CreateBrowseRecord();
		GetCtrl().SetCaretAt(GetCtrl().PositionFromLine(tag->GetLine()-1));
		// add an entry to the navigation manager
		NavMgr::Get()->AddJump(jumpfrom, GetCtrl().CreateBrowseRecord());
	}
}

void ContextCpp::OnCallTipClick(wxScintillaEvent& e)
{
	wxUnusedVar(e);
}

void ContextCpp::OnCalltipCancel()
{
}

int ContextCpp::DoGetCalltipParamterIndex()
{
	int index(0);
	LEditor &ctrl =  GetCtrl();
	int pos = ctrl.DoGetOpenBracePos();
	if (pos != wxNOT_FOUND) {

		// loop over the text from pos -> current position and count the number of commas found
		int depth(0);
		bool exit_loop(false);

		while ( pos < ctrl.GetCurrentPos() && !exit_loop ) {
			wxChar ch        = ctrl.SafeGetChar(pos);
			wxChar ch_before = ctrl.SafeGetChar(ctrl.PositionBefore(pos));

			if (IsCommentOrString(pos)) {
				pos = ctrl.PositionAfter(pos);
				continue;
			}

			switch (ch) {
			case wxT(','):
							if (depth == 0) index++;
				break;
			case wxT('{'):
						case wxT('}'):
							case wxT(';'):
									// error?
									exit_loop = true;
				break;
			case wxT('('):
						case wxT('<'):
							case wxT('['):
									depth++;
				break;
			case wxT('>'):
							if ( ch_before == wxT('-') ) {
						// operator noting to do
						break;
					}
				// fall through
			case wxT(')'):
						case wxT(']'):
								depth--;
				break;
			default:
				break;
			}
			pos = ctrl.PositionAfter(pos);
		}
	}
	return index;
}

void ContextCpp::DoUpdateCalltipHighlight()
{
	LEditor &ctrl = GetCtrl();
	if (ctrl.GetFunctionTip()->IsActive()) {
		ctrl.GetFunctionTip()->Highlight(DoGetCalltipParamterIndex());
	}
}

void ContextCpp::SemicolonShift()
{
	int foundPos    (wxNOT_FOUND);
	int semiColonPos(wxNOT_FOUND);
	LEditor &ctrl = GetCtrl();
	if (ctrl.NextChar(ctrl.GetCurrentPos(), semiColonPos) == wxT(')')) {

		// test to see if we are inside a 'for' statement
		long openBracePos          (wxNOT_FOUND);
		int  posWordBeforeOpenBrace(wxNOT_FOUND);

		if (ctrl.MatchBraceBack(wxT(')'), semiColonPos, openBracePos)) {
			ctrl.PreviousChar(openBracePos, posWordBeforeOpenBrace);
			if (posWordBeforeOpenBrace != wxNOT_FOUND) {
				wxString word = ctrl.PreviousWord(posWordBeforeOpenBrace, foundPos);

				// c++ expression with single line and should be treated separatly
				if ( word == wxT("for"))
					return;

				// At the current pos, we got a ';'
				// at semiColonPos we got ;
				// switch
				ctrl.DeleteBack();
				ctrl.SetCurrentPos(semiColonPos);
				ctrl.InsertText(semiColonPos, wxT(";"));
				ctrl.SetCaretAt(semiColonPos+1);
				ctrl.GetFunctionTip()->Deactivate();
			}
		}
	}
}
