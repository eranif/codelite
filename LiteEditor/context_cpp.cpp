#include "precompiled_header.h"
#include "movefuncimpldlg.h"
#include "context_cpp.h"
#include "editor.h"
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

struct SFileSort {
	bool operator()(const wxFileName &one, const wxFileName &two) {
		return two.GetFullName().Cmp(one.GetFullName()) > 0;
	}
};

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
END_EVENT_TABLE()

ContextCpp::ContextCpp(LEditor *container)
		: ContextBase(container)
		, m_tipKind(TipNone)
		, m_rclickMenu(NULL)
{
	ApplySettings();
	
	//load the context menu from the resource manager
	m_rclickMenu = wxXmlResource::Get()->LoadMenu(wxT("editor_right_click"));
	m_rclickMenu->Connect(XRCID("swap_files"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ContextCpp::OnSwapFiles), NULL, this);
	m_rclickMenu->Connect(XRCID("insert_doxy_comment"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ContextCpp::OnInsertDoxyComment), NULL, this);
	m_rclickMenu->Connect(XRCID("comment_selection"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ContextCpp::OnCommentSelection), NULL, this);
	m_rclickMenu->Connect(XRCID("comment_line"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ContextCpp::OnCommentLine), NULL, this);
	m_rclickMenu->Connect(XRCID("setters_getters"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ContextCpp::OnGenerateSettersGetters), NULL, this);
	m_rclickMenu->Connect(XRCID("find_decl"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ContextCpp::OnFindDecl), NULL, this);
	m_rclickMenu->Connect(XRCID("find_impl"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ContextCpp::OnFindImpl), NULL, this);
	m_rclickMenu->Connect(XRCID("move_impl"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ContextCpp::OnMoveImpl), NULL, this);
	m_rclickMenu->Connect(XRCID("add_impl"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ContextCpp::OnAddImpl), NULL, this);
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
	wxString expr = GetExpression(end);
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

	if (IsCommentOrString(rCtrl.GetCurrentPos())) {
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
		if (rCtrl.GetTabIndents()) {
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

	VALIDATE_PROJECT(rCtrl);

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
	wxString expr = GetExpression(rCtrl.GetCurrentPos());

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
			if (endPos1 > 0 && endPos1 <= startPos) {endPos = endPos1;}
		}

		TagEntryPtr t3 = TagsManagerST::Get()->FirstScopeOfFile(rCtrl.GetFileName());
		if ( t3 ) {
			endPos2 = rCtrl.PositionFromLine( t3->GetLine() - 1);
			if (endPos2 > 0 && endPos2 <= startPos && endPos2 < endPos1) {endPos = endPos2;}
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
		expr = GetExpression(rCtrl.PositionBefore(rCtrl.GetCurrentPos()));

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

wxString ContextCpp::GetExpression(long pos)
{
	bool cont(true);
	int depth(0);
	LEditor &rCtrl = GetCtrl();

	int position( pos );
	int at(position);
	bool prevGt(false);
	while (cont) {
		wxChar ch = rCtrl.PreviousChar(position, at, true);
		position = at;
		//Eof?
		if (ch == 0) {
			at = 0;
			break;
		}

		//Comment?
		if (IsCommentOrString(at))
			continue;

		switch (ch) {
		case wxT('-'):
						if (prevGt) {
					prevGt = false;
					//if previous char was '>', we found an arrow so reduce the depth
					//which was increased
					depth--;
				} else {
					if (depth <= 0) {
						//dont include this token
						at = rCtrl.PositionAfter(at);
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
						case wxT(';'):
								prevGt = false;
			cont = false;
			break;
		case wxT('('):
					case wxT('['):
							depth--;
			prevGt = false;
			if (depth < 0) {
				//dont include this token
				at = rCtrl.PositionAfter(at);
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
				at = rCtrl.PositionAfter(at);
				cont = false;
			}
			break;
		case wxT('>'):
						prevGt = true;
			depth++;
			break;
		case wxT('<'):
						prevGt = true;
			depth--;
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
	wxString expr = rCtrl.GetTextRange(at, pos);

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
			PrependMenuItem(menu, menuItemText, wxCommandEventHandler(ContextCpp::OnAddIncludeFile));
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
	wxString expr = GetExpression(word_end);

	// get the scope
	wxString text = rCtrl.GetTextRange(0, word_end);

	if (m_selectedWord.IsEmpty())
		return;

	std::vector<TagEntryPtr> tags;
	int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPosition())+1;
	TagsManagerST::Get()->FindImplDecl(rCtrl.GetFileName(), line, expr, m_selectedWord, text, tags, false);
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

	VALIDATE_PROJECT(rCtrl);

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

	// get the word under the cursor
	word = GetWordUnderCaret();

	if (word.IsEmpty())
		return;

	TagsManager *mgr = TagsManagerST::Get();

	//get the current expression
	wxString expr = GetExpression(rCtrl.GetCurrentPos());

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
	LEditor &rCtrl = GetCtrl();
	wxString list;
	size_t i=0;
	//Assumption (which is always true..): the tags are sorted
	wxString lastName;
	if (tags.empty() == false) {
		for (; i<tags.size()-1; i++) {
			if (lastName != tags.at(i)->GetName()) {
				list.Append(tags.at(i)->GetName() + GetImageString(*tags.at(i)) + wxT("@"));
				lastName = tags.at(i)->GetName();
			}
			if (showFullDecl) {
				//collect only declarations
				if (tags.at(i)->GetKind() == wxT("prototype")) {
					list.Append(tags.at(i)->GetName() + tags.at(i)->GetSignature() + GetImageString(*tags[i]) + wxT("@"));
				}
			}
		}

		if (lastName != tags.at(i)->GetName()) {
			list.Append(tags.at(i)->GetName() + GetImageString(*tags.at(i)) + wxT("@"));
		}
		list = list.BeforeLast(wxT('@'));

		rCtrl.AutoCompSetSeparator((int)('@'));	// set the separator to be non valid language wxChar
		rCtrl.AutoCompSetChooseSingle(true);					// If only one match, insert it automatically
		rCtrl.AutoCompSetIgnoreCase(false);
		rCtrl.AutoCompSetDropRestOfWord(true);
		rCtrl.AutoCompSetAutoHide(false);
		rCtrl.AutoCompShow((int)word.Length(), list);
		rCtrl.AutoCompSetFillUps(wxT("<( \t"));
	}
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
	if (!NavMgr::Get()->CanPrev())
		return;

	// Get the last tag visited
	BrowseRecord record = NavMgr::Get()->Prev();

	wxString msg;
	msg << wxT("GotoPreviousDefintion ") << record.filename << wxT(" ") << record.lineno;
	ManagerST::Get()->OpenFile(record);
}

void ContextCpp::GotoDefinition()
{
	LEditor &rCtrl = GetCtrl();

	VALIDATE_PROJECT(rCtrl);

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
		// Just open the file and set the cursor on the match we found
		wxString projectName = ManagerST::Get()->GetProjectNameByFile(tags[0]->GetFile());
		ManagerST::Get()->OpenFile(tags[0]->GetFile(), projectName, tags[0]->GetLine()-1);
	} else {
		// popup a dialog offering the results to the user
		SymbolsDialog *dlg = new SymbolsDialog(&GetCtrl());
		dlg->AddSymbols( tags, 0 );
		if (dlg->ShowModal() == wxID_OK) {
			ManagerST::Get()->OpenFile(dlg->GetFile(), dlg->GetProject(), dlg->GetLine()-1);
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

	VALIDATE_PROJECT(editor);

	//get the current line text
	int lineno = editor.LineFromPosition(editor.GetCurrentPos());

	//get doxygen comment based on file and line
	TagsManager *mgr = TagsManagerST::Get();
	wxString comment = mgr->GenerateDoxygenComment(editor.GetFileName().GetFullPath(), lineno);
	//do we have a comment?
	if (comment.IsEmpty())
		return;

	editor.InsertTextWithIndentation(comment, lineno);

	//since we just inserted a text to the document, we force a save on the
	//document, or else the parser will lose sync with the database
	//but avoid saving it, if it not part of the workspace
	wxString project = ManagerST::Get()->GetProjectNameByFile(editor.GetFileName().GetFullPath());
	if (project.IsEmpty() == false) {
		editor.SaveFile();
	}
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
	editor.InsertText(editor.PositionAfter(editor.PositionAfter(end)), wxT("*/"));
	editor.EndUndoAction();
}

void ContextCpp::OnCommentLine(wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor &editor = GetCtrl();

	int lineno = editor.LineFromPosition(editor.GetCurrentPos());
	int start  = editor.PositionFromLine(lineno);

	//createa C block comment
	editor.BeginUndoAction();
	editor.InsertText(start, wxT("//"));
	editor.EndUndoAction();
}

void ContextCpp::OnGenerateSettersGetters(wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor &editor = GetCtrl();

	VALIDATE_PROJECT(editor);
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
	/*
		//validate project is open for the container editor
		if (GetCtrl().GetProject().IsEmpty()) {
			event.Skip();
			return;
		}

		if (m_tipKind == TipFuncProto && GetCtrl().CallTipActive() && m_ct) {
			if (event.GetKeyCode() == WXK_DOWN) {
				GetCtrl().CallTipCancel();
				GetCtrl().CallTipShow(GetCtrl().GetCurrentPos(), m_ct->Next());
				return;
			} else if (event.GetKeyCode() == WXK_UP) {
				GetCtrl().CallTipCancel();
				GetCtrl().CallTipShow(GetCtrl().GetCurrentPos(), m_ct->Prev());
				return;
			}
		}
	*/
	event.Skip();
}

void ContextCpp::OnFindImpl(wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor &rCtrl = GetCtrl();
	VALIDATE_PROJECT(rCtrl);

	//get expression
	int pos = rCtrl.GetCurrentPos();
	int word_end = rCtrl.WordEndPosition(pos, true);
	int word_start = rCtrl.WordStartPosition(pos, true);
	wxString expr = GetExpression(word_end);

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
	VALIDATE_PROJECT(rCtrl);

	//get expression
	//get expression
	int pos = rCtrl.GetCurrentPos();
	int word_end = rCtrl.WordEndPosition(pos, true);
	int word_start = rCtrl.WordStartPosition(pos, true);
	wxString expr = GetExpression(word_end);

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

	LEditor &ctrl = GetCtrl();
	if (event.GetId() == XRCID("insert_doxy_comment")) {
		event.Enable(ctrl.GetProject().IsEmpty() == false);
	} else
		if (event.GetId() == XRCID("setters_getters")) {
			event.Enable(ctrl.GetProject().IsEmpty() == false);
		} else
			if (event.GetId() == XRCID("find_decl")) {
				event.Enable(ctrl.GetProject().IsEmpty() == false);
			} else
				if (event.GetId() == XRCID("find_impl")) {
					event.Enable(ctrl.GetProject().IsEmpty() == false);
				} else
					if (event.GetId() == XRCID("move_impl")) {
						event.Enable(ctrl.GetProject().IsEmpty() == false && ctrl.GetSelectedText().IsEmpty() == false );
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
			//we need to update the navigation bar of the main frame
			TagsManager *tags = TagsManagerST::Get();
			Frame::Get()->GetMainBook()->UpdateScope(tags->FunctionFromFileLine(ctrl.GetFileName(), lastLine+1));
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
	VALIDATE_PROJECT(rCtrl);

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

	clFunction foo;
	TagEntryPtr tag;
	bool match(false);
	for (std::vector< TagEntryPtr >::size_type i=0; i< tags.size(); i++) {
		if (tags.at(i)->GetName() == word && tags.at(i)->GetLine() == line && tags.at(i)->GetKind() == wxT("function")) {
			//we got a match
			tag = tags.at(i);
			LanguageST::Get()->FunctionFromPattern(tags.at(i)->GetPattern(), foo);
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
			wxString body;
			blockEndPos = rCtrl.PositionAfter(blockEndPos);

			if (foo.m_retrunValusConst.empty() == false) {
				body << _U(foo.m_retrunValusConst.c_str()) << wxT(" ");
			}

			if (foo.m_returnValue.m_typeScope.empty() == false) {
				body << _U(foo.m_returnValue.m_typeScope.c_str()) << wxT("::");
				body << _U(foo.m_returnValue.m_templateDecl.c_str());
			}

			body << _U(foo.m_returnValue.m_type.c_str())
			<< _U(foo.m_returnValue.m_starAmp.c_str())
			<< wxT(" ") << tag->GetScope() << wxT("::") << tag->GetName() << tag->GetSignature();
			body << wxT("\n");
			body << content;
			body << wxT("\n");
			body << wxT("\n");

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
			blockEndPos = curPos;
		}
	}

	return 	(blockEndPos > blockStartPos) 	&&
	        (blockEndPos != wxNOT_FOUND) 	&&
	        (blockStartPos != wxNOT_FOUND);
}

void ContextCpp::OnAddImpl(wxCommandEvent &e)
{
	wxUnusedVar(e);
	LEditor &rCtrl = GetCtrl();
	VALIDATE_PROJECT(rCtrl);

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
	if(scopeName.IsEmpty()) {scopeName = wxT("<global>");}
	
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
		wxString body = TagsManagerST::Get()->FormatFunction(tag, true);
		
		wxString targetFile;
		FindSwappedFile(rCtrl.GetFileName(), targetFile);
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
	VALIDATE_PROJECT(rCtrl);
	
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
	for(size_t i=0; i< tags.size(); i++) {
		foo_map[tags.at(i)->GetName()] = tags.at(i);
	}
	
	//cross reference between the two lists, if any of the tokens exist in 
	//both lists, then it will be considered as function

	//remove duplicates
	VariableList::iterator viter = var_list.begin();
	for(; viter != var_list.end(); viter++ ) {
		Variable var = *viter;
		wxString name = _U(var.m_name.c_str());
		if(foo_map.find(name) == foo_map.end()) {
			var_map[var.m_name] = var;
		}
	}

	//create to word list
	//functions
	wxString fooList;
	wxString varList;
	
	std::map< wxString, TagEntryPtr >::iterator it1 = foo_map.begin();
	for(; it1 != foo_map.end(); it1++ ) {
		fooList << it1->second->GetName() << wxT(" ");
	}
	
	std::map< std::string, Variable >::iterator it2 = var_map.begin();
	for(; it2 != var_map.end(); it2++ ) {
		varList << _U(it2->second.m_name.c_str()) << wxT(" ");
	}
	
	//wxSCI_C_WORD2
	rCtrl.SetKeyWords(1, fooList);
	
	//wxSCI_C_GLOBALCLASS
	rCtrl.SetKeyWords(3, varList);
		
	//try to colourse only visible scope
	rCtrl.Colourise(0, wxSCI_INVALID_POSITION);
}

void ContextCpp::ApplySettings() 
{
	//-----------------------------------------------
	// Load laguage settings from configuration file
	//-----------------------------------------------
	SetName(wxT("C++"));

	// Set the key words and the lexer
	std::list<StyleProperty> styles;
	LexerConfPtr lexPtr;
	// Read the configuration file
	if (EditorConfigST::Get()->IsOk()) {
		lexPtr = EditorConfigST::Get()->GetLexer(wxT("C++"));
	}

	// Update the control
	LEditor &rCtrl = GetCtrl();
	rCtrl.SetLexer(lexPtr->GetLexerId());

	wxString keyWords = lexPtr->GetKeyWords();
	keyWords.Replace(wxT("\n"), wxT(" "));
	keyWords.Replace(wxT("\r"), wxT(" "));
	rCtrl.SetKeyWords(0, keyWords);
	rCtrl.StyleClearAll();

	styles = lexPtr->GetProperties();
	std::list<StyleProperty>::iterator iter = styles.begin();
	for (; iter != styles.end(); iter++) {
		StyleProperty st = (*iter);
		int size = st.GetFontSize();
		wxString face = st.GetFaceName();
		bool bold = st.IsBold();

		wxFont font;
		if (st.GetId() == wxSCI_STYLE_CALLTIP) {
			font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
		} else {
			font = wxFont(size, wxFONTFAMILY_TELETYPE, wxNORMAL, bold ? wxBOLD : wxNORMAL, false, face);
		}

		if (st.GetId() == 0) { //default
			rCtrl.StyleSetFont(wxSCI_STYLE_DEFAULT, font);
			rCtrl.StyleSetSize(wxSCI_STYLE_DEFAULT, (*iter).GetFontSize());
			rCtrl.StyleSetForeground(wxSCI_STYLE_DEFAULT, (*iter).GetFgColour());
			rCtrl.StyleSetBackground(wxSCI_STYLE_DEFAULT, (*iter).GetBgColour());
			rCtrl.StyleSetBackground(wxSCI_STYLE_LINENUMBER, (*iter).GetBgColour());
			rCtrl.StyleSetSize(wxSCI_STYLE_LINENUMBER, (*iter).GetFontSize());
			rCtrl.SetFoldMarginColour(true, (*iter).GetBgColour());
			rCtrl.SetFoldMarginHiColour(true, (*iter).GetBgColour());
		}

		rCtrl.StyleSetFont(st.GetId(), font);
		rCtrl.StyleSetSize(st.GetId(), (*iter).GetFontSize());
		rCtrl.StyleSetForeground(st.GetId(), (*iter).GetFgColour());
		rCtrl.StyleSetBackground(st.GetId(), (*iter).GetBgColour());
	}

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
}
