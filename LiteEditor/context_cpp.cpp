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


#include "precompiled_header.h"
#include "cc_box.h"
#include <wx/progdlg.h>
#include "renamesymboldlg.h"
#include "cpptoken.h"
#include "tokendb.h"
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
#include "variable.h"
#include "function.h"

extern void get_variables(const std::string &in, VariableList &li, const std::map<std::string, bool> &ignoreTokens);
extern void get_functions(const std::string &in, FunctionList &li, const std::map<std::string, bool> &ignoreTokens);

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
	EVT_UPDATE_UI(XRCID("find_decl"), ContextCpp::OnUpdateUI)
	EVT_UPDATE_UI(XRCID("find_impl"), ContextCpp::OnUpdateUI)
	EVT_UPDATE_UI(XRCID("insert_doxy_comment"), ContextCpp::OnUpdateUI)
	EVT_UPDATE_UI(XRCID("setters_getters"), ContextCpp::OnUpdateUI)
	EVT_UPDATE_UI(XRCID("move_impl"), ContextCpp::OnUpdateUI)

	EVT_MENU(XRCID("swap_files"), ContextCpp::OnSwapFiles)
	EVT_MENU(XRCID("comment_selection"), ContextCpp::OnCommentSelection)
	EVT_MENU(XRCID("comment_line"), ContextCpp::OnCommentLine)
	EVT_MENU(XRCID("find_decl"), ContextCpp::OnFindDecl)
	EVT_MENU(XRCID("find_impl"), ContextCpp::OnFindImpl)
	EVT_MENU(XRCID("insert_doxy_comment"), ContextCpp::OnInsertDoxyComment)
	EVT_MENU(XRCID("move_impl"), ContextCpp::OnMoveImpl)
	EVT_MENU(XRCID("add_impl"), ContextCpp::OnAddImpl)
	EVT_MENU(XRCID("add_multi_impl"), ContextCpp::OnAddMultiImpl)
	EVT_MENU(XRCID("setters_getters"), ContextCpp::OnGenerateSettersGetters)
	EVT_MENU(XRCID("add_include_file"), ContextCpp::OnAddIncludeFile)
	EVT_MENU(XRCID("rename_function"), ContextCpp::OnRenameFunction)
END_EVENT_TABLE()

ContextCpp::ContextCpp(LEditor *container)
		: ContextBase(container)
		, m_tipKind(TipNone)
		, m_rclickMenu(NULL)
{
	ApplySettings();
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
	rCtrl.CallTipCancel();
	m_tipKind = TipNone;
	event.Skip();
}

void ContextCpp::OnDwellStart(wxScintillaEvent &event)
{
	LEditor &rCtrl = GetCtrl();

	VALIDATE_PROJECT(rCtrl);

	//before we start, make sure we are the visible window
	Manager *mgr = ManagerST::Get();
	if (mgr->GetActiveEditor() != &rCtrl) {
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
		rCtrl.CallTipCancel();
		rCtrl.CallTipShow(event.GetPosition(), tooltip);
		m_tipKind = TipHover;
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
	int indentSize = rCtrl.GetIndent();
	int pos = wxNOT_FOUND;
	long matchPos = wxNOT_FOUND;
	int curpos = rCtrl.GetCurrentPos();
	if (IsComment(curpos) && nChar == wxT('\n')) {

		//enter was hit in comment section
		AutoAddComment();
		return;

	} else if (IsCommentOrString(rCtrl.GetCurrentPos())) {

		ContextBase::AutoIndent(nChar);
		return;

	}

	// enter was pressed
	int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPos());
	if (nChar == wxT('\n')) {
		wxChar ch = rCtrl.PreviousChar(rCtrl.GetCurrentPos(), pos);
		if (pos != wxNOT_FOUND && ch == wxT('{')) {
			if (IsCommentOrString(pos)) {
				return;
			}

			//open brace?
			//increase indent size
			int prevLine = rCtrl.LineFromPosition(pos);

			int prevLineIndet = rCtrl.GetLineIndentation(prevLine);
			rCtrl.SetLineIndentation(line, indentSize + prevLineIndet);
		} else {
			//just copy the previous line indentation
			ContextBase::AutoIndent(nChar);
			return;
		}

		int dummy = rCtrl.GetLineIndentation(line);
		if (rCtrl.GetUseTabs()) {
			dummy = dummy / indentSize;
		}
		rCtrl.SetCaretAt(rCtrl.GetCurrentPos() + dummy);
	} else if (nChar == wxT('}') && rCtrl.MatchBraceBack(wxT('}'), rCtrl.GetCurrentPos()-1, matchPos)) {
		int secondLine = rCtrl.LineFromPosition(matchPos);
		if (secondLine == line) {
			return;
		}
		int secondLineIndent = rCtrl.GetLineIndentation(secondLine);
		rCtrl.SetLineIndentation(line, secondLineIndent);
	}
}

bool ContextCpp::IsCommentOrString(long pos)
{
	int style;
	style = GetCtrl().GetStyleAt(pos);
	return (style == wxSCI_C_COMMENT				||
	        style == wxSCI_C_COMMENTLINE			||
	        style == wxSCI_C_COMMENTDOC				||
	        style == wxSCI_C_COMMENTLINEDOC			||
	        style == wxSCI_C_COMMENTDOCKEYWORD		||
	        style == wxSCI_C_COMMENTDOCKEYWORDERROR ||
	        style == wxSCI_C_STRING					||
	        style == wxSCI_C_STRINGEOL				||
	        style == wxSCI_C_CHARACTER);
}

void ContextCpp::CallTipCancel()
{
	LEditor &rCtrl = GetCtrl();
	rCtrl.CallTipCancel();
	m_tipKind = TipNone;
}

void ContextCpp::OnCallTipClick(wxScintillaEvent &event)
{
	LEditor &rCtrl = GetCtrl();
	switch ( event.GetPosition() ) {
	case ArrowUp:
		rCtrl.CallTipCancel();
		rCtrl.CallTipShow(rCtrl.GetCurrentPos(), m_ct->Prev());
		break;
	case ArrowDown:
		rCtrl.CallTipCancel();
		rCtrl.CallTipShow(rCtrl.GetCurrentPos(), m_ct->Next());
		break;
	case Elsewhere:
		break;
	}
}

//=============================================================================
// >>>>>>>>>>>>>>>>>>>>>>>> CodeCompletion API - START
//=============================================================================

//user pressed ., -> or ::
void ContextCpp::CodeComplete()
{
	LEditor &rCtrl = GetCtrl();

	VALIDATE_WORKSPACE();

	long pos = rCtrl.GetCurrentPos();
	bool showFuncProto = false;

	wxChar ch;

	//	Make sure we are not on a comment section
	if (IsCommentOrString(pos))
		return;

	// Search for first non-whitespace wxChar
	int pos1, pos2, end;
	ch = rCtrl.PreviousChar(pos, pos1);
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
	wxString expr = GetExpression(rCtrl.GetCurrentPos(), false);

	// get the scope
	//Optimize the text for large files
	int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPosition())+1;
	int startPos(0);
	TagEntryPtr t = TagsManagerST::Get()->FunctionFromFileLine(rCtrl.GetFileName(), line);
	if ( t ) {
		startPos = rCtrl.PositionFromLine( t->GetLine() - 1);
		if ( startPos > rCtrl.GetCurrentPos() ) {
			startPos = 0;
		}
	}

	wxString text = rCtrl.GetTextRange(startPos, rCtrl.GetCurrentPos());
	//hack #2
	//collect all text from 0 - first scope found
	//this will help us detect statements like 'using namespace foo;'
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
		expr = GetExpression(rCtrl.PositionBefore(rCtrl.GetCurrentPos()), false);

		//display function tooltip
		int word_end = rCtrl.WordEndPosition(end, true);
		int word_start = rCtrl.WordStartPosition(end, true);

		// get the token
		wxString word = rCtrl.GetTextRange(word_start, word_end);
		m_ct = TagsManagerST::Get()->GetFunctionTip(rCtrl.GetFileName(), line, expr, text, word);
		if (m_ct && m_ct->Count() > 0) {
			rCtrl.CallTipCancel();
			rCtrl.CallTipShow(rCtrl.GetCurrentPos(), m_ct->All());
			m_tipKind = TipFuncProto;
		}
	} else {
		if (TagsManagerST::Get()->AutoCompleteCandidates(rCtrl.GetFileName(), line, expr, text, candidates)) {
			DisplayCompletionBox(candidates, wxEmptyString, showFullDecl);
		}
	}
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

wxString ContextCpp::GetExpression(long pos, bool onlyWord, LEditor *editor)
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
			break;
		case wxT(' '):
					case wxT('\n'):
						case wxT('\v'):
							case wxT('\t'):
								case wxT('\r'):
										prevGt = false;
			if (depth <= 0) {
				cont = false;
				break;
			}
			break;
		case wxT('{'):
					case wxT('='):
							prevGt = false;
			cont = false;
			break;
		case wxT('('):
					case wxT('['):
							depth--;
			prevGt = false;
			if (depth < 0) {
				//dont include this token
				at =ctrl->PositionAfter(at);
				cont = false;
				break;
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
														prevGt = false;
			if (depth <= 0) {

				//dont include this token
				at =ctrl->PositionAfter(at);
				cont = false;
			}
			break;
		case wxT('>'):
						prevGt = true;
			depth++;
			break;
		case wxT('<'):
						prevGt = false;
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
			depth++;
			break;
		default:
			prevGt = false;
			break;
		}
	}

	if (at < 0) at = 0;
	wxString expr = ctrl->GetTextRange(at, pos);

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

	wxFileName fileName(m_selectedWord);
	wxString tmpName(m_selectedWord);
	tmpName.Replace(wxT("\\"), wxT("/"));

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

		fileToOpen = wxGetSingleChoice(wxT("Select file to open:"), wxT("Select file"), choices);
	} else if (files2.size() == 1) {
		fileToOpen = files2.at(0).GetFullPath();
	}


	if (fileToOpen.IsEmpty() == false) {
		//we got a match
		LEditor &rCtrl = GetCtrl();

		//only provide the file name to the manager and let him
		//decide what is the correct project name
		ManagerST::Get()->OpenFile(fileToOpen, wxEmptyString);

		// Keep the current position as well
		NavMgr::Get()->Push(rCtrl.CreateBrowseRecord());
	}
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
	VALIDATE_PROJECT(rCtrl);

	wxString menuItemText;
	wxString line = rCtrl.GetCurLine();
	menuItemText.Clear();

	if (IsIncludeStatement(line, &fileName)) {

		PrependMenuItemSeparator(menu);
		menuItemText << wxT("Open Workspace File \"") << fileName << wxT("\"");

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

void ContextCpp::PrependMenuItem(wxMenu *menu, const wxString &text, wxObjectEventFunction func)
{
	wxMenuItem *item;
	wxString menuItemText;
	item = new wxMenuItem(menu, wxNewId(), text);
	menu->Prepend(item);
	menu->Connect(item->GetId(), wxEVT_COMMAND_MENU_SELECTED, func, NULL, this);
	m_dynItems.push_back(item);
}

void ContextCpp::PrependMenuItem(wxMenu *menu, const wxString &text, int id)
{
	wxMenuItem *item;
	wxString menuItemText;
	item = new wxMenuItem(menu, id, text);
	menu->Prepend(item);
	m_dynItems.push_back(item);
}

void ContextCpp::PrependMenuItemSeparator(wxMenu *menu)
{
	wxMenuItem *item;
	item = new wxMenuItem(menu, wxID_SEPARATOR);
	menu->Prepend(item);
	m_dynItems.push_back(item);
}

void ContextCpp::OnAddIncludeFile(wxCommandEvent &e)
{
	wxUnusedVar(e);
	LEditor &rCtrl = GetCtrl();
	VALIDATE_PROJECT(rCtrl);

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
		choice = wxGetSingleChoice(wxT("Select File to Include:"), wxT("Add Include File"), options);
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
		rCtrl.InsertText(pos, lineToAdd + wxT("\n"));
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

	static wxRegEx reIncludeFile(wxT("include *[\\\"\\<]{1}([a-zA-Z0-9_/\\.]*)"));
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

	if (word.IsEmpty())
		return;

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
	LEditor &rCtrl = GetCtrl();
	wxString list;
	size_t i=0;

	wxString fileName(word);
	fileName.Replace(wxT("\\"), wxT("/"));
	fileName = fileName.AfterLast(wxT('/'));

	std::vector<wxFileName> files;
	TagsManagerST::Get()->GetFiles(fileName, files);
	std::sort(files.begin(), files.end(), SFileSort());

	if ( files.empty() == false )	{
		for (; i<files.size()-1; i++) {
			list.Append(files.at(i).GetFullName() + GetFileImageString(files.at(i).GetExt()) + wxT("@"));
		}

		list.Append(files.at(i).GetFullName()+GetFileImageString(files.at(i).GetExt()));
		rCtrl.AutoCompSetSeparator((int)('@'));	// set the separator to be non valid language wxChar
		rCtrl.AutoCompSetChooseSingle(true);					// If only one match, insert it automatically
		rCtrl.AutoCompSetDropRestOfWord(true);
		rCtrl.AutoCompSetIgnoreCase(false);
		rCtrl.AutoCompSetAutoHide(false);
		rCtrl.AutoCompShow((int)fileName.Length(), list);
		rCtrl.AutoCompSetFillUps(wxT(">\" \t"));
	}
}

//=============================================================================
// <<<<<<<<<<<<<<<<<<<<<<<<<<< CodeCompletion API - END
//=============================================================================

void ContextCpp::GotoPreviousDefintion()
{
	NavMgr::Get()->NavigateBackward();
}

void ContextCpp::GotoDefinition()
{
	LEditor &rCtrl = GetCtrl();

	VALIDATE_WORKSPACE();

	std::vector<TagEntryPtr> tags;

	//	Make sure we are not on a comment section
	if (IsCommentOrString(rCtrl.GetCurrentPos()))
		return;

	// Get the word under the cursor OR the selected word
	wxString word = rCtrl.GetSelectedText();
	if (word.IsEmpty()) {
		// No selection, try to find the word under the cursor
		long pos = rCtrl.GetCurrentPos();
		long end = rCtrl.WordEndPosition(pos, true);
		long start = rCtrl.WordStartPosition(pos, true);

		// Get the word
		word = rCtrl.GetTextRange(start, end);
		if (word.IsEmpty())
			return;
	}

	// get all tags that matches the name (we use exact match)
	TagsManagerST::Get()->FindSymbol(word, tags);
	if (tags.empty())
		return;

	DoGotoSymbol(tags);
}

void ContextCpp::DoGotoSymbol(const std::vector<TagEntryPtr> &tags)
{
	LEditor &rCtrl = GetCtrl();
	// Keep the current position as well
	NavMgr::Get()->Push(rCtrl.CreateBrowseRecord());

	// Did we get a single match?
	if (tags.size() == 1) {
		ManagerST::Get()->OpenFile(	tags[0]->GetFile(),
		                            wxEmptyString,
		                            tags[0]->GetLine()-1);
	} else {
		// popup a dialog offering the results to the user
		SymbolsDialog *dlg = new SymbolsDialog(&GetCtrl());
		dlg->AddSymbols( tags, 0 );
		if (dlg->ShowModal() == wxID_OK) {
			ManagerST::Get()->OpenFile(dlg->GetFile(), wxEmptyString, dlg->GetLine()-1);
		}
		dlg->Destroy();
	}
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
	} else {
		//try to find a implementation file
		exts.Add(wxT("cpp"));
		exts.Add(wxT("cxx"));
		exts.Add(wxT("cc"));
		exts.Add(wxT("c"));
	}

	for (size_t i=0; i<exts.GetCount(); i++) {
		otherFile.SetExt(exts.Item(i));
		if (TryOpenFile(otherFile)) {
			//keep the current location, and return
			NavMgr::Get()->Push(GetCtrl().CreateBrowseRecord());
			return;
		}
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
		ManagerST::Get()->OpenFile(fileName.GetFullPath(), proj);
		return true;
	}

	//ok, the file does not exist in the current directory, try to find elsewhere
	//whithin the workspace files
	std::vector<wxFileName> files;
	ManagerST::Get()->GetWorkspaceFiles(files, true);

	for (size_t i=0; i<files.size(); i++) {
		if (files.at(i).GetFullName() == fileName.GetFullName()) {
			wxString proj = ManagerST::Get()->GetProjectNameByFile(files.at(i).GetFullPath());
			ManagerST::Get()->OpenFile(files.at(i).GetFullPath(), proj);
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

	wxString blockStart(wxT("/**\n"));
	if (!data.GetUseSlash2Stars()) {
		blockStart = wxT("/*!\n");
	}

	DoxygenComment dc = TagsManagerST::Get()->GenerateDoxygenComment(editor.GetFileName().GetFullPath(), lineno, keyPrefix);
	//do we have a comment?
	if (dc.comment.IsEmpty())
		return;

	//prepend the prefix to the
	wxString classPattern = data.GetClassPattern();
	wxString funcPattern  = data.GetFunctionPattern();

	//replace $(Name) here **before** the call to ExpandAllVariables()
	classPattern.Replace(wxT("$(Name)"), dc.name);
	funcPattern.Replace(wxT("$(Name)"), dc.name);

	classPattern = ExpandAllVariables(classPattern, WorkspaceST::Get(), editor.GetProjectName(), editor.GetFileName().GetFullPath());
	funcPattern = ExpandAllVariables(funcPattern, WorkspaceST::Get(), editor.GetProjectName(), editor.GetFileName().GetFullPath());

	dc.comment.Replace(wxT("$(ClassPattern)"), classPattern);
	dc.comment.Replace(wxT("$(FunctionPattern)"), funcPattern);

	//close the comment
	dc.comment << wxT(" */\n");
	dc.comment.Prepend(blockStart);

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

	if (start == end)
		return;

	//createa C block comment
	editor.BeginUndoAction();
	editor.InsertText(start, wxT("/*"));

	//advance the end selection by 2
	end = editor.PositionAfter(editor.PositionAfter(end));
	editor.InsertText(end, wxT("*/"));

	end = editor.PositionAfter(editor.PositionAfter(end));
	editor.SetCaretAt(end);

	editor.EndUndoAction();
}

void ContextCpp::OnCommentLine(wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor &editor = GetCtrl();

	int line_start = editor.LineFromPosition(editor.GetCurrentPos());
	int line_end(line_start);

	if (editor.GetSelectedText().IsEmpty() == false) {
		//we have a selection
		//calculate the line number and start point using the selection
		line_start = editor.LineFromPosition(editor.GetSelectionStart());
		line_end   = editor.LineFromPosition(editor.GetSelectionEnd());
	}

	editor.BeginUndoAction();
	//comment all the lines
	int i(line_start);
	for (i=line_start; i<= line_end; i++) {
		int start = editor.PositionFromLine(i);
		editor.InsertText(start, wxT("//"));
	}

	//place the caret at the end of the commented line
	int endPos = editor.PositionFromLine(line_end) + editor.LineLength(line_end);
	editor.SetCaretAt(endPos);

	editor.EndUndoAction();
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
	if (tag->GetFile() != editor.GetFileName().GetFullPath()) {
		wxString msg;
		msg << wxT("This file does not seem to contain the declaration for '") << tag->GetName() << wxT("'\n");
		msg << wxT("The declaration of '") << tag->GetName() << wxT("' is located at '") << tag->GetFile() << wxT("'\n");
		msg << wxT("Would you like CodeLite to open this file for you?");

		if (wxMessageBox(msg, wxT("CodeLite"), wxYES_NO) == wxYES) {
			wxString projectName = ManagerST::Get()->GetProjectNameByFile(tag->GetFile());
			ManagerST::Get()->OpenFile(tag->GetFile(), projectName, tag->GetLine());
		}
		return;
	}

	int lineno = editor.LineFromPosition(editor.GetCurrentPos()) + 1;

	//get the file name and line where to insert the setters getters
	static SettersGettersDlg *s_dlg = NULL;
	if (!s_dlg) {
		s_dlg = new SettersGettersDlg(ManagerST::Get()->GetMainFrame());
	}

	s_dlg->Init(tags, tag->GetFile(), lineno);
	if (s_dlg->ShowModal() == wxID_OK) {
		wxString code = s_dlg->GetGenCode();
		if (code.IsEmpty() == false) {
			editor.InsertTextWithIndentation(code, lineno);
		}
	}
}

void ContextCpp::OnKeyDown(wxKeyEvent &event)
{
	event.Skip();
}

void ContextCpp::OnFindImpl(wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor &rCtrl = GetCtrl();
	VALIDATE_WORKSPACE();

	//get expression
	int pos = rCtrl.GetCurrentPos();
	int word_end = rCtrl.WordEndPosition(pos, true);
	int word_start = rCtrl.WordStartPosition(pos, true);
	wxString expr = GetExpression(word_end, false);

	// get the scope
	wxString text = rCtrl.GetTextRange(0, word_end);

	//the word

	wxString word = rCtrl.GetTextRange(word_start, word_end);

	if (word.IsEmpty())
		return;

	std::vector<TagEntryPtr> tags;
	int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPosition())+1;
	TagsManagerST::Get()->FindImplDecl(rCtrl.GetFileName(), line, expr, word, text, tags, true);
	if (tags.empty())
		return;

	DoGotoSymbol(tags);

}

void ContextCpp::OnFindDecl(wxCommandEvent &event)
{

	wxUnusedVar(event);
	LEditor &rCtrl = GetCtrl();

	VALIDATE_WORKSPACE();

	//get expression
	//get expression
	int pos = rCtrl.GetCurrentPos();
	int word_end = rCtrl.WordEndPosition(pos, true);
	int word_start = rCtrl.WordStartPosition(pos, true);
	wxString expr = GetExpression(word_end, false);

	// get the scope
	wxString text = rCtrl.GetTextRange(0, word_end);

	//the word
	wxString word = rCtrl.GetTextRange(word_start, word_end);

	if (word.IsEmpty())
		return;

	std::vector<TagEntryPtr> tags;
	int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPosition())+1;
	TagsManagerST::Get()->FindImplDecl(rCtrl.GetFileName(), line, expr, word, text, tags, false);
	if (tags.empty())
		return;

	DoGotoSymbol(tags);
}

void ContextCpp::OnUpdateUI(wxUpdateUIEvent &event)
{
	bool workspaceOpen = ManagerST::Get()->IsWorkspaceOpen();
	bool projectAvailable = (GetCtrl().GetProjectName().IsEmpty() == false);

	if (event.GetId() == XRCID("insert_doxy_comment")) {
		event.Enable(projectAvailable);
	} else if (event.GetId() == XRCID("setters_getters")) {
		event.Enable(projectAvailable);
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

void ContextCpp::OnSciUpdateUI(wxScintillaEvent &event)
{
	wxUnusedVar(event);

	//update the navigation toolbar only if the toolbar is visible
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("Navigation Toolbar"));
	if (!info.IsOk() || !info.IsShown()) {
		return;
	}

	LEditor &ctrl = GetCtrl();

	static long lastPos(wxNOT_FOUND);
	static long lastLine(wxNOT_FOUND);

	//get the current position
	long curpos = ctrl.GetCurrentPos();
	if (curpos != lastPos) {
		lastPos = curpos;
		//position has changed, compare line numbers
		if (ctrl.LineFromPosition(curpos) != lastLine) {
			
			lastLine = ctrl.LineFromPosition(curpos);
			Frame::Get()->GetMainBook()->UpdateScope( TagsManagerST::Get()->FunctionFromFileLine(ctrl.GetFileName(), lastLine+1) );
			
		}
	}
}

void ContextCpp::OnDbgDwellEnd(wxScintillaEvent &event)
{
	wxUnusedVar(event);
	Manager *mgr = ManagerST::Get();
	mgr->DbgCancelQuickWatchTip();
}

void ContextCpp::OnDbgDwellStart(wxScintillaEvent & event)
{
	static wxRegEx reCppIndentifier(wxT("[a-zA-Z_][a-zA-Z0-9_]*"));
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

		long start = ctrl.WordStartPosition(pos, true);
		long end   = ctrl.WordEndPosition(pos, true);
		word = ctrl.GetTextRange(start, end);
		if (word.IsEmpty()) {
			return;
		}

		//make sure that this is indeed a variable name
		if (!reCppIndentifier.Matches(word)) {
			return;
		}
	} else {
		return;
	}

	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	wxString cmd;
	wxString output;
	cmd << wxT("print ") << word;
	if (dbgr && dbgr->ExecSyncCmd(cmd, output)) {
		// cancel any old calltip and display the new one
		ctrl.CallTipCancel();

		// GDB HACK BEGIN::
		//gdb displays the variable name as $<NUMBER>,
		//we simply replace it with the actual string
		output = output.Trim().Trim(false);
		static wxRegEx reGdbVar(wxT("^\\$[0-9]+"));
		reGdbVar.ReplaceFirst(&output, word);
		// GDB HACK END::

		ctrl.CallTipShow(event.GetPosition(), output);
		m_tipKind = TipHover;
	}
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
				if (ManagerST::Get()->OpenFileAndAppend(targetFile, body)) {
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
		wxMessageBox(wxT("'Add Functions Implementation' can only work inside valid scope, got (") + scopeName + wxT(")"), wxT("CodeLite"), wxICON_INFORMATION|wxOK);
		return;
	}

	//get list of all prototype functions from the database
	std::vector< TagEntryPtr > vproto;
	std::vector< TagEntryPtr > vimpl;

	//currently we want to add implementation only for workspace classes
	TagsManagerST::Get()->TagsByScope(scopeName, wxT("prototype"), vproto, true, true);
	TagsManagerST::Get()->TagsByScope(scopeName, wxT("function"), vimpl, true, true);

	//filter out functions which already has implementation
	std::map<wxString, TagEntryPtr> protos;
	for ( size_t i=0; i < vproto.size() ; i++ ) {
		TagEntryPtr tag = vproto.at(i);
		wxString key = tag->GetName();

		//override the scope to be our scope...
		tag->SetScope( scopeName );

		key << TagsManagerST::Get()->NormalizeFunctionSig( tag->GetSignature() );
		protos[key] = tag;
	}

	//remove from the map all the functions that already has a body
	for ( size_t i=0; i < vimpl.size() ; i++ ) {
		TagEntryPtr tag = vimpl.at(i);
		wxString key = tag->GetName();
		key << TagsManagerST::Get()->NormalizeFunctionSig( tag->GetSignature() );
		std::map<wxString, TagEntryPtr>::iterator iter = protos.find(key);
		if ( iter != protos.end() ) {
			protos.erase( iter );
		}
	}

	// the map now consist only with functions without implementation
	// create body for all of those functions
	//create the functions body
	wxString body;
	std::map<wxString, TagEntryPtr>::iterator iter = protos.begin();

	for (; iter != protos.end(); iter ++ ) {
		TagEntryPtr tag = iter->second;
		//use normalize function signature rather than the default one
		//this will ensure that default values are removed
		tag->SetSignature(TagsManagerST::Get()->NormalizeFunctionSig( tag->GetSignature(), true ));
		body << TagsManagerST::Get()->FormatFunction(tag, true);
		body << wxT("\n");
	}

	wxString targetFile;
	FindSwappedFile(rCtrl.GetFileName(), targetFile);

	//if no swapped file is found, use the current file
	if (targetFile.IsEmpty()) {
		targetFile = rCtrl.GetFileName().GetFullPath();
	}

	MoveFuncImplDlg *dlg = new MoveFuncImplDlg(NULL, body, targetFile);
	if (dlg->ShowModal() == wxID_OK) {
		//get the updated data
		targetFile = dlg->GetFileName();
		body = dlg->GetText();
		ManagerST::Get()->OpenFileAndAppend(targetFile, body);
	}
	dlg->Destroy();
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
			wxMessageBox(wxT("Function '") + tag->GetName() + wxT("' already has a body"), wxT("CodeLite"), wxICON_WARNING|wxOK);
			return;
		}

		//create the functions body
		//replace the function signature with the normalized one, so default values
		//will not appear in the function implementation
		wxString newSig = TagsManagerST::Get()->NormalizeFunctionSig( tag->GetSignature(), true );
		tag->SetSignature( newSig );
		wxString body = TagsManagerST::Get()->FormatFunction(tag, true);

		wxString targetFile;
		FindSwappedFile(rCtrl.GetFileName(), targetFile);

		//if no swapped file is found, use the current file
		if (targetFile.IsEmpty()) {
			targetFile = rCtrl.GetFileName().GetFullPath();
		}

		MoveFuncImplDlg *dlg = new MoveFuncImplDlg(NULL, body, targetFile);
		if (dlg->ShowModal() == wxID_OK) {
			//get the updated data
			targetFile = dlg->GetFileName();
			body = dlg->GetText();
			ManagerST::Get()->OpenFileAndAppend(targetFile, body);
		}
		dlg->Destroy();
	}
}

void ContextCpp::OnFileSaved()
{
	LEditor &rCtrl = GetCtrl();
	VALIDATE_WORKSPACE();

	VariableList var_list;
	std::map< std::string, Variable > var_map;
	std::map< wxString, TagEntryPtr> foo_map;

	std::map<std::string, bool> ignoreTokens;

	const wxCharBuffer patbuf = _C(rCtrl.GetText());

	//collect list of variables
	get_variables( patbuf.data(), var_list, ignoreTokens );

	//list all functions of this file
	std::vector< TagEntryPtr > tags;
	TagsManagerST::Get()->GetFunctions(tags, rCtrl.GetFileName().GetFullPath());
	for (size_t i=0; i< tags.size(); i++) {
		foo_map[tags.at(i)->GetName()] = tags.at(i);
	}

	//cross reference between the two lists, if any of the tokens exist in
	//both lists, then it will be considered as function

	//remove duplicates
	VariableList::iterator viter = var_list.begin();
	for (; viter != var_list.end(); viter++ ) {
		Variable var = *viter;
		wxString name = _U(var.m_name.c_str());
		if (foo_map.find(name) == foo_map.end()) {
			var_map[var.m_name] = var;
		}
	}

	//create to word list
	//functions
	wxString projectTags;
	wxString varList;

	std::map< std::string, Variable >::iterator it2 = var_map.begin();
	for (; it2 != var_map.end(); it2++ ) {
		varList << _U(it2->second.m_name.c_str()) << wxT(" ");
	}

	//get list of all tags from the workspace
	TagsManagerST::Get()->GetAllTagsNameAsSpaceDelimString(projectTags);

	//wxSCI_C_WORD2
	if (TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_COLOUR_PROJ_TAGS) {
		rCtrl.SetKeyWords(1, projectTags);
	} else {
		rCtrl.SetKeyWords(1, wxEmptyString);
	}

	//wxSCI_C_GLOBALCLASS
	if (TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_COLOUR_VARS) {
		rCtrl.SetKeyWords(3, varList);
	} else {
		rCtrl.SetKeyWords(3, wxEmptyString);
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
	rCtrl.SetLexer(lexPtr->GetLexerId());

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
	int cur_style;
	int next_style;
	int prepre_style;

	LEditor &rCtrl = GetCtrl();
	int curpos = rCtrl.GetCurrentPos();
	int prevpos = rCtrl.PositionBefore(curpos);

	//get the style for the previous line we where
	cur_style = rCtrl.GetStyleAt(curpos);
	next_style = rCtrl.GetStyleAt(rCtrl.PositionAfter(curpos));
	prepre_style = rCtrl.GetStyleAt(rCtrl.PositionBefore(prevpos));

	CommentConfigData data;
	EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

	if (cur_style == wxSCI_C_COMMENTLINE) {

		//C++ comment style was in the previous line
		//just copy the previous line indentation

		int indentSize = rCtrl.GetIndent();
		int line = rCtrl.LineFromPosition(curpos);
		int prevLine = line - 1;

		if (rCtrl.GetLine(prevLine).Trim().Trim(false) == wxT("//") || data.GetContinueCppComment() == false) {
			//dont add new comment
			ContextBase::AutoIndent(wxT('\n'));
			return;
		}

		//take the previous line indentation size
		int prevLineIndet = rCtrl.GetLineIndentation(prevLine);
		rCtrl.SetLineIndentation(line, prevLineIndet);
		//place the caret at the end of the line
		int dummy = rCtrl.GetLineIndentation(line);
		if (rCtrl.GetUseTabs()) {
			dummy = dummy / indentSize;
		}

		if (next_style != wxSCI_C_COMMENTLINE) {
			rCtrl.InsertText(curpos + dummy, wxT("//"));
			rCtrl.SetCaretAt(curpos + dummy + 2);
		}

	} else if (cur_style == wxSCI_C_COMMENT || cur_style == wxSCI_C_COMMENTDOC) {

		if (data.GetAddStarOnCComment() == false) {
			ContextBase::AutoIndent(wxT('\n'));
			return;
		}

		// we are in C style comment
		int indentSize = rCtrl.GetIndent();
		int line = rCtrl.LineFromPosition(curpos);
		int prevLine = line - 1;

		//take the previous line indentation size
		int prevLineIndet = rCtrl.GetLineIndentation(prevLine);
		rCtrl.SetLineIndentation(line, prevLineIndet);
		//place the caret at the end of the line
		int dummy = rCtrl.GetLineIndentation(line);
		if (rCtrl.GetUseTabs()) {
			dummy = dummy / indentSize;
		}

		if (prepre_style == wxSCI_C_COMMENT || prepre_style == wxSCI_C_COMMENTDOC) {
			rCtrl.InsertText(curpos + dummy, wxT(" *"));
			rCtrl.SetCaretAt(curpos + dummy + 2);
		}
	}
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
	
	// make sure there are no un-saved files before refactoring
	Notebook *book = Frame::Get()->GetNotebook();
	for(size_t i=0; i<book->GetPageCount(); i++){
		LEditor *editor = dynamic_cast<LEditor*>(book->GetPage(i));
		if(editor && editor->GetModify()) {
			wxMessageBox(wxT("Please save on all un-saved files before refactoring"));
			return;
		}
	}
	
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

	wxLogMessage(wxT("Refactoring: ") + source.name + wxT(" of scope: ") + source.scope);

	// load all tokens, first we need to parse the workspace files...
	ManagerST::Get()->BuildRefactorDatabase(word, l);
	std::list<CppToken> tokens;

	// incase no tokens were found (possibly cause of user pressing cancel
	// abort this operation
	l.findTokens(word, tokens);
	if(tokens.empty()){
		return;
	}
	
	wxString msg;
	msg << wxT("Found ") << tokens.size() << wxT(" instances of ") << word;
	wxLogMessage(msg);

	// create an empty hidden instance of LEditor
	LEditor *editor = new LEditor(Frame::Get()->GetNotebook(), wxID_ANY, wxSize(1, 1), wxEmptyString, wxEmptyString, true);

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
		token.setLine( editor->GetLine( editor->LineFromPosition( (int)token.getOffset() ) ) );
		
		wxString msg;
		wxFileName f(token.getFilename());
		msg << wxT("Parsing expression ") << counter << wxT("/") << tokens.size() << wxT(" in file: ") << f.GetFullName();
		if( !prgDlg->Update(counter, msg) ){
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
	RenameSymbol *dlg = new RenameSymbol(&rCtrl, candidates, possibleCandidates);
	if(dlg->ShowModal() == wxID_OK){
		std::list<CppToken> matches;
		
		dlg->GetMatches( matches );
		if(matches.empty() == false) {
			ManagerST::Get()->ReplaceInFiles(dlg->GetWord(), matches);
		}
	}
	dlg->Destroy();
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
				}else if(rs->scope == wxT("<global>") && rs->isClass == false) {
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

//void ContextCpp::OnScopeUpdate(wxCommandEvent& e)
//{
//	//we need to update the navigation bar of the main frame
//	ScopeJobResult *result = reinterpret_cast<ScopeJobResult*>(e.GetClientData());
//	if( result ) {
//		LEditor &ctrl = GetCtrl();
//		
//		// do we still need to update?
//		TagEntryPtr tag( result->tag );
//		
//		if(ctrl.GetCurrentLine() == result->last_line) {
//			// update the scope
//			Frame::Get()->GetMainBook()->UpdateScope( tag );
//		}
//
//		// delete the result
//		delete result;
//	}
//}
