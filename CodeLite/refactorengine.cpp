#include "refactorengine.h"
#include "cppwordscanner.h"
#include "entry.h"
#include "ctags_manager.h"
#include <wx/progdlg.h>
#include <wx/sizer.h>

RefactoringEngine::RefactoringEngine()
{
}

RefactoringEngine::~RefactoringEngine()
{
}

RefactoringEngine* RefactoringEngine::Instance()
{
	static RefactoringEngine ms_instance;
	return &ms_instance;
}


void RefactoringEngine::Clear()
{
	m_possibleCandidates.clear();
	m_candidates.clear();
}

void RefactoringEngine::RenameGlobalSymbol(const wxString& symname, const wxFileName& fn, int line, int pos, const wxFileList& files)
{
	// Clear previous results
	Clear();

	// Container for the results found in the 'files'
	CppTokensMap l;

	// Load the file and get a state map + the text from the scanner
	CppWordScanner scanner(fn.GetFullPath());

	// get the current file states
	TextStatesPtr states = scanner.states();
	if(!states)
		return;

	// Attempt to understand the expression that the caret is currently located at (using line:pos:file)
	RefactorSource rs;
	if(!DoResolveWord(states, fn, pos + symname.Len(), line, symname, &rs))
		return;

	wxProgressDialog* prgDlg = CreateProgressDialog(wxT("Stage 1/2: Gathering required information..."), (int)files.size());

	// Search the provided input files for the symbol to rename and prepare
	// a CppTokensMap
	for (size_t i=0; i<files.size(); i++) {
		wxFileName curfile = files.at(i);

		wxString msg;
		msg << wxT("Parsing: ") << curfile.GetFullName();
		// update the progress bar
		if (!prgDlg->Update(i, msg)){
			prgDlg->Destroy();
			Clear();
			return;
		}

		CppWordScanner tmpScanner(curfile.GetFullPath());
		tmpScanner.Match(symname, l);
	}
	prgDlg->Destroy();

	// load all tokens, first we need to parse the workspace files...
	std::list<CppToken> tokens;

	// incase no tokens were found (possibly cause of user pressing cancel
	// abort this operation
	l.findTokens(symname, tokens);
	if (tokens.empty())
		return;

	// sort the tokens
	tokens.sort();

	RefactorSource target;
	std::list<CppToken>::iterator iter = tokens.begin();
	int counter(0);

	TextStatesPtr statesPtr(NULL);
	wxString      statesPtrFileName;
	prgDlg = CreateProgressDialog(wxT("Stage 2/2: Parsing matches..."), (int) tokens.size());
	for (; iter != tokens.end(); iter++) {

		// TODO :: send an event here to report our progress
		wxFileName f(iter->getFilename());
		wxString   msg;
		msg << wxT("Parsing expression ") << counter << wxT("/") << tokens.size() << wxT(" in file: ") << f.GetFullName();
		if ( !prgDlg->Update(counter, msg) ) {
			// user clicked 'Cancel'
			Clear();
			prgDlg->Destroy();
			return;
		}

		counter++;
		// reset the result
		target.Reset();

		if(!statesPtr || statesPtrFileName != iter->getFilename()) {
			// Create new statesPtr
			CppWordScanner sc(iter->getFilename());
			statesPtr         = sc.states();
			statesPtrFileName = iter->getFilename();
		}

		if(!statesPtr)
			continue;

		if (DoResolveWord(statesPtr, wxFileName(iter->getFilename()), iter->getOffset(), line, symname, &target)) {

			if (target.name == rs.name && target.scope == rs.scope) {
				// full match
				m_candidates.push_back( *iter );
			} else if (target.name == rs.scope && !rs.isClass) {
				// source is function, and target is class
				m_candidates.push_back( *iter );

			} else if (target.name == rs.name && rs.isClass) {
				// source is class, and target is ctor
				m_candidates.push_back( *iter );

			} else {
				// add it to the possible match list
				m_possibleCandidates.push_back( *iter );
			}
		} else {
			// resolved word failed, add it to the possible list
			m_possibleCandidates.push_back( *iter );
		}
	}

	prgDlg->Destroy();
}

void RefactoringEngine::RenameLocalSymbol(const wxString& symname, const wxFileName& fn, int line, int pos)
{
	// Clear previous results
	Clear();

	// Load the file and get a state map + the text from the scanner
	CppWordScanner scanner(fn.GetFullPath());

	// get the current file states
	TextStatesPtr states = scanner.states();
	if( !states ) {
		return;
	}


	// get the local by scanning from the current function's
	TagEntryPtr tag = TagsManagerST::Get()->FunctionFromFileLine(fn, line + 1);
	if( !tag ) {
		return;
	}

	// Get the line number of the function
	int funcLine = tag->GetLine() - 1;

	// Convert the line number to offset
	int from = states->LineToPos     (funcLine);
	int to   = states->FunctionEndPos(from);

	if(to == wxNOT_FOUND)
		return;

	// search for matches in the given range
	CppTokensMap l;
	scanner.Match(symname, l, from, to);

	std::list<CppToken> tokens;
	l.findTokens(symname, tokens);
	if (tokens.empty())
		return;

	// Loop over the matches
	// Incase we did manage to resolve the word, it means that it is NOT a local variable (DoResolveWord only wors for globals NOT for locals)
	RefactorSource target;
	std::list<CppToken>::iterator iter = tokens.begin();
	for (; iter != tokens.end(); iter++) {
		wxFileName f(iter->getFilename());
		if (!DoResolveWord(states, wxFileName(iter->getFilename()), iter->getOffset(), line, symname, &target)) {
			m_candidates.push_back( *iter );
		}
	}
}

bool RefactoringEngine::DoResolveWord(TextStatesPtr states, const wxFileName& fn, int pos, int line, const wxString &word, RefactorSource *rs)
{
	std::vector<TagEntryPtr> tags;

	// try to process the current expression
	wxString expr = GetExpression(pos, states);

	// get the scope
	// Optimize the text for large files
	wxString text = states->text.Left(pos + 1);

	// we simply collect declarations & implementations

	//try implemetation first
	bool found(false);
	TagsManagerST::Get()->FindImplDecl(fn, line, expr, word, text, tags, true, true);
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
			rs->scope   = tag->GetScope();
			rs->name    = tag->GetName();
			rs->isClass = tag->IsClass() || tag->IsStruct();
			found = true;

		} else {
			return true;

		}
	}

	// Ok, the "implementation" search did not yield definite results, try declaration
	tags.clear();
	TagsManagerST::Get()->FindImplDecl(fn, line, expr, word, text, tags, false, true);
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
			rs->scope    = tag->GetScope();
			rs->name     = tag->GetName();
			rs->isClass  = tag->IsClass() || tag->IsStruct();
		}
		return true;
	}

	// if we got so far, CC failed to parse the expression
	return false;
}

wxString RefactoringEngine::GetExpression(int pos, TextStatesPtr states)
{
	bool     cont(true);
	int      depth(0);
	bool     prevGt   (false);
	bool     prevColon(false);
	wxString expression;

	states->SetPosition(pos);
	while (cont && depth >= 0) {
		wxChar ch = states->Previous();

		// eof?
		if (ch == 0) {
			break;
		}

		switch (ch) {
		case wxT(';'):
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

		if(cont) {
			expression.Prepend(ch);
		}
	}
	return expression;
}

wxProgressDialog* RefactoringEngine::CreateProgressDialog(const wxString& title, int maxValue)
{
	wxProgressDialog* prgDlg = NULL;
	// Create a progress dialog
	prgDlg = new wxProgressDialog (title,
								   wxT("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"),
								   maxValue,
								   NULL,
								   wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_AUTO_HIDE | wxPD_CAN_ABORT);
	prgDlg->GetSizer()->Fit(prgDlg);
	prgDlg->Layout();
	prgDlg->Centre();

	prgDlg->Update(0, title);
	return prgDlg;
}

void RefactoringEngine::FindReferences(const wxString& symname, const wxFileName& fn, int line, int pos, const wxFileList& files)
{
	// Clear previous results
	Clear();

	// Container for the results found in the 'files'
	CppTokensMap l;

	// Load the file and get a state map + the text from the scanner
	CppWordScanner scanner(fn.GetFullPath());

	// get the current file states
	TextStatesPtr states = scanner.states();
	if(!states)
		return;

	// Attempt to understand the expression that the caret is currently located at (using line:pos:file)
	RefactorSource rs;
	if(!DoResolveWord(states, fn, pos + symname.Len(), line, symname, &rs))
		return;

	wxProgressDialog* prgDlg = CreateProgressDialog(wxT("Stage 1/2: Gathering required information..."), (int)files.size());

	// Search the provided input files for the symbol to rename and prepare
	// a CppTokensMap
	for (size_t i=0; i<files.size(); i++) {
		wxFileName curfile = files.at(i);

		wxString msg;
		msg << wxT("Parsing: ") << curfile.GetFullName();
		// update the progress bar
		if (!prgDlg->Update(i, msg)){
			prgDlg->Destroy();
			Clear();
			return;
		}

		CppWordScanner tmpScanner(curfile.GetFullPath());
		tmpScanner.Match(symname, l);
	}
	prgDlg->Destroy();

	// load all tokens, first we need to parse the workspace files...
	std::list<CppToken> tokens;

	// incase no tokens were found (possibly cause of user pressing cancel
	// abort this operation
	l.findTokens(symname, tokens);
	if (tokens.empty())
		return;

	// sort the tokens
	tokens.sort();

	RefactorSource target;
	std::list<CppToken>::iterator iter = tokens.begin();
	int counter(0);

	TextStatesPtr statesPtr(NULL);
	wxString      statesPtrFileName;
	prgDlg = CreateProgressDialog(wxT("Stage 2/2: Parsing matches..."), (int) tokens.size());
	for (; iter != tokens.end(); iter++) {

		// TODO :: send an event here to report our progress
		wxFileName f(iter->getFilename());
		wxString   msg;
		msg << wxT("Parsing expression ") << counter << wxT("/") << tokens.size() << wxT(" in file: ") << f.GetFullName();
		if ( !prgDlg->Update(counter, msg) ) {
			// user clicked 'Cancel'
			Clear();
			prgDlg->Destroy();
			return;
		}

		counter++;
		// reset the result
		target.Reset();

		if(!statesPtr || statesPtrFileName != iter->getFilename()) {
			// Create new statesPtr
			CppWordScanner sc(iter->getFilename());
			statesPtr         = sc.states();
			statesPtrFileName = iter->getFilename();
		}

		if(!statesPtr)
			continue;

		if (DoResolveWord(statesPtr, wxFileName(iter->getFilename()), iter->getOffset(), iter->getLineNumber(), symname, &target)) {
			
			// set the line number
			if(statesPtr->states.size() > iter->getOffset())
				iter->setLineNumber( statesPtr->states[iter->getOffset()].lineNo );
			
			if (target.name == rs.name && target.scope == rs.scope) {
				// full match
				m_candidates.push_back( *iter );

			} else if (target.name == rs.scope && !rs.isClass) {
				// source is function, and target is class
				m_candidates.push_back( *iter );

			} else if (target.name == rs.name && rs.isClass) {
				// source is class, and target is ctor
				m_candidates.push_back( *iter );

			}
		}
	}

	prgDlg->Destroy();
}
