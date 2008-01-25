#include "precompiled_header.h"
#include "ctags_manager.h"
#include <wx/txtstrm.h>
#include <wx/file.h>
#include <algorithm> 
#include <wx/progdlg.h>
#include "dirtraverser.h"
#include "wx/tokenzr.h"
#include "wx/filename.h"
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include "cpp_comment_creator.h"
#include "tags_options_data.h"
#include <wx/busyinfo.h>
#include "wx/timer.h"
#include "variable_entry.h"
#include "procutils.h"

//---------------------------------------------------------------------------
// Misc

// Descending sorting function
struct SDescendingSort {
	bool operator()(const TagEntryPtr &rStart, const TagEntryPtr &rEnd) {
		return rStart->GetName().Cmp(rEnd->GetName()) > 0;
	}
};

/// Ascending sorting function
struct SAscendingSort {
	bool operator()(const TagEntryPtr &rStart, const TagEntryPtr &rEnd) {
		return rEnd->GetName().Cmp(rStart->GetName()) > 0;
	}
};

struct tagParseResult {
	TagTreePtr tree;
	std::vector<DbRecordPtr> *comments;
	wxString fileName;
};

//---------------------------------------------------------------------------
// Tag cache entry
//---------------------------------------------------------------------------
TagCacheEntry::TagCacheEntry(const wxString &query, const std::vector<TagEntryPtr> &tags)
		: m_query(query)
		, m_tags(tags)
{
}

TagCacheEntry::~TagCacheEntry()
{
}

static int CtagsMgrTimerId = wxNewId();

//------------------------------------------------------------------------------
// CTAGS Manager
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(TagsManager, wxEvtHandler)
	EVT_TIMER(CtagsMgrTimerId, TagsManager::OnTimer)
END_EVENT_TABLE()

TagsManager::TagsManager() : wxEvtHandler()
		, m_ctagsPath(wxT("ctags-le"))
#if defined (__WXMSW__) || defined (__WXGTK__)
		, m_ctags(NULL)
#endif
		, m_canDeleteCtags(true)
		, m_timer(NULL)
{
	m_pDb = new TagsDatabase();
	m_pExternalDb = new TagsDatabase();

#if defined (__WXMSW__) || defined (__WXGTK__)
	m_ctagsCmd = wxT("  --excmd=pattern --sort=no --fields=aKmSsnit --c-kinds=+p --C++-kinds=+p --filter=yes  --filter-terminator=\"<<EOF>>\" ");
	m_timer = new wxTimer(this, CtagsMgrTimerId);
	m_timer->Start(100);
#else
	m_ctagsCmd = wxT(" -f- --excmd=pattern --sort=no --fields=aKmSsnit --c-kinds=+p --C++-kinds=+p ");
#endif

}

TagsManager::~TagsManager()
{
	delete m_pDb;
	delete m_pExternalDb;
	if (m_timer) {
		delete m_timer;
	}

#if defined (__WXMSW__) || defined (__WXGTK__)
	wxCriticalSectionLocker locker(m_cs);
	if (m_canDeleteCtags) {
		if (m_ctags)	m_ctags->Disconnect(m_ctags->GetUid(), wxEVT_END_PROCESS, wxProcessEventHandler(TagsManager::OnCtagsEnd), NULL, this);
		// terminate ctags process
		if (m_ctags) m_ctags->Terminate();

		std::list<clProcess*>::iterator it = m_gargabeCollector.begin();
		for (; it != m_gargabeCollector.end(); it++) {
			delete (*it);
		}

		if (m_gargabeCollector.empty() == false) {
		}
		m_gargabeCollector.clear();
	}
#endif
}

void TagsManager::OnTimer(wxTimerEvent &event)
{
	//clean the garbage collector
	wxUnusedVar(event);
	{
		wxCriticalSectionLocker locker(m_cs);
		if (m_canDeleteCtags) {

			std::list<clProcess*>::iterator it = m_gargabeCollector.begin();
			for (; it != m_gargabeCollector.end(); it++) {
				delete (*it);
			}

			if (m_gargabeCollector.empty() == false) {
			}
			m_gargabeCollector.clear();
		}
	}
}

void TagsManager::OpenDatabase(const wxFileName& fileName)
{
	m_pDb->OpenDatabase(fileName);
	if (m_pDb->GetVersion() != m_pDb->GetSchemaVersion()) {
		m_pDb->RecreateDatabase();
	}
}

// Currently not in use, maybe in the future??
TagTreePtr TagsManager::ParseTagsFile(const wxFileName& fp)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);


	tagFileInfo info;
	tagEntry entry;
	wxString tagFileName = fp.GetFullPath();
	const wxCharBuffer fileName = _C(tagFileName);

	tagFile *const file = tagsOpen(fileName.data(), &info);
	if ( !file ) {
		return TagTreePtr( NULL );
	}

	// Load the records and build a language tree
	TagEntry root;
	root.SetName(wxT("<ROOT>"));

	TagTreePtr tree( new TagTree(wxT("<ROOT>"), root) );
	while (tagsNext (file, &entry) == TagSuccess) {
		TagEntry tag( entry );
		tree->AddEntry(tag);
	}
	tagsClose(file);
	return tree;
}

void TagsManager::TagFromLine(const wxString& line, TagEntry& tag)
{
	wxString pattern, kind;
	wxString strLine = line;
	long lineNumber = wxNOT_FOUND;
	std::map<wxString, wxString> extFields;

	//get the token name
	wxString name = strLine.BeforeFirst(wxT('\t'));
	strLine	= strLine.AfterFirst(wxT('\t'));

	//get the file name
	wxString fileName = strLine.BeforeFirst(wxT('\t'));
	strLine	= strLine.AfterFirst(wxT('\t'));

	//here we can get two options:
	//pattern followed by ;"
	//or
	//line number followed by ;"
	int end = strLine.Find(wxT(";\""));
	if (end == wxNOT_FOUND) {
		//invalid pattern found
		return;
	}

	if (strLine.StartsWith(wxT("/^"))) {
		//regular expression pattern found
		pattern = strLine.Mid(0, end);
		strLine	= strLine.Right(strLine.Length() - (end + 2));
	} else {
		//line number pattern found, this is usually the case when
		//dealing with macros in C++
		pattern = strLine.Mid(0, end);
		strLine	= strLine.Right(strLine.Length() - (end + 2));

		pattern = pattern.Trim();
		pattern = pattern.Trim(false);
		pattern.ToLong(&lineNumber);
	}

	//next is the kind of the token
	if (strLine.StartsWith(wxT("\t"))) {
		strLine	= strLine.AfterFirst(wxT('\t'));
	}

	kind = strLine.BeforeFirst(wxT('\t'));
	strLine	= strLine.AfterFirst(wxT('\t'));

	if (strLine.IsEmpty() == false) {
		wxStringTokenizer tkz(strLine, wxT('\t'));
		while (tkz.HasMoreTokens()) {
			wxString token = tkz.NextToken();
			wxString key = token.BeforeFirst(wxT(':'));
			wxString val = token.AfterFirst(wxT(':'));
			key = key.Trim();
			key = key.Trim(false);

			val = val.Trim();
			val = val.Trim(false);
			if (key == wxT("line") && !val.IsEmpty()) {
				val.ToLong(&lineNumber);
			} else {
				if (key == wxT("enum")) {
					//enums are specials, they are not really a scope
					//so they should appear when I type:
					//enumName::
					//they should be member of their parent (which can be <global>, or class)
					val = val.BeforeLast(wxT(':'));
					val = val.BeforeLast(wxT(':'));
				}
				extFields[key] = val;
			}
		}
	}

	kind = kind.Trim();
	name = name.Trim();
	fileName = fileName.Trim();
	pattern = pattern.Trim();
	tag.Create(fileName, name, lineNumber, pattern, kind, extFields);
}

TagTreePtr TagsManager::ParseSourceFile(const wxFileName& fp, std::vector<DbRecordPtr> *comments)
{
	wxString tags;

#if defined (__WXMSW__) || defined (__WXGTK__)
	if ( !m_ctags ) {
		return TagTreePtr( NULL );
	}
	SourceToTags(fp, tags, m_ctags);
#else
	//Mac
	SourceToTags2(fp, tags);
#endif

	//	return ParseTagsFile(tags, project);
	TagTreePtr ttp = TagTreePtr( TreeFromTags(tags) );

	if ( comments && GetParseComments() ) {
		// parse comments
		LanguageST::Get()->ParseComments( fp, comments );

	}
	return ttp;
}

TagTreePtr TagsManager::ParseSourceFile2(const wxFileName& fp, const wxString &tags, std::vector<DbRecordPtr> *comments)
{
	//	return ParseTagsFile(tags, project);
	TagTreePtr ttp = TagTreePtr( TreeFromTags(tags) );

	if (comments && GetParseComments()) {
		// parse comments
		LanguageST::Get()->ParseComments(fp, comments);
	}
	return ttp;
}

//-----------------------------------------------------------
// Database operations
//-----------------------------------------------------------

void TagsManager::Store(TagTreePtr tree, const wxFileName& path)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	m_pDb->Store(tree, path);
}

TagTreePtr TagsManager::Load(const wxFileName& fileName)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	// Incase empty file path is provided, use the current file name
	TagTreePtr tree;
	try {
		wxSQLite3ResultSet rs = m_pDb->SelectTagsByFile(fileName.GetFullPath());

		// Load the records and build a language tree
		TagEntry root;
		std::vector<TagEntry> rejectedTags;
		root.SetName(wxT("<ROOT>"));
		tree.Reset( new TagTree(wxT("<ROOT>"), root) );
		while ( rs.NextRow() ) {
			TagEntry entry(rs);
			tree->AddEntry(entry);
		}
		rs.Finalize();
	} catch (wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
	return tree;
}

/**
 * \brief
 * \param path
 * \param fileName
 */
void TagsManager::Delete(const wxFileName& path, const wxString& fileName)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	m_pDb->DeleteByFileName(path, fileName);
}

//--------------------------------------------------------
// Process Handling of CTAGS
//--------------------------------------------------------

#if defined (__WXMSW__) || defined (__WXGTK__)
clProcess *TagsManager::StartCtagsProcess()
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	// Run ctags process
	wxString cmd;
	wxString ctagsCmd;
	ctagsCmd << m_options.ToString() << m_ctagsCmd;

	// build the command, we surround ctags name with double quatations
	cmd << wxT("\"") << m_ctagsPath.GetFullPath() << wxT("\"") << ctagsCmd;
	clProcess* process;

	process = new clProcess(wxNewId(), cmd);

	// Launch it!
	process->Start();
	m_processes[process->GetPid()] = process;

	if ( process->GetPid() <= 0 ) {
		m_ctags = NULL;
		return NULL;
	}

	// attach the termination event to the tags manager class
	process->Connect(process->GetUid(), wxEVT_END_PROCESS, wxProcessEventHandler(TagsManager::OnCtagsEnd), NULL, this);
	m_ctags = process;
	return process;
}

void TagsManager::RestartCtagsProcess()
{
	clProcess *oldProc(NULL);
	oldProc = m_ctags;

	if ( !oldProc ) {
		return ;
	}

	// no need to call StartCtagsProcess(), since it will be called automatically
	// by the termination handler OnCtagsEnd()
	oldProc->Terminate();
}
#endif

void TagsManager::SetCtagsPath(const wxString& path)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	m_ctagsPath = wxFileName(path, wxT("ctags-le"));
}

#if defined (__WXMSW__) || defined (__WXGTK__)
void TagsManager::OnCtagsEnd(wxProcessEvent& event)
{
	//-----------------------------------------------------------
	// This event handler is a must if you wish to delete
	// the process and prevent memory leaks
	// In addition, I implemented here some kind of a watchdog
	// mechanism: if ctags process terminated abnormally, it will
	// be restarted automatically by this function (unless the
	// termination of it was from OnClose() function, then we
	// choose to ignore the restart)
	//-----------------------------------------------------------

	// Which ctags process died?
	std::map<int, clProcess*>::iterator iter = m_processes.find(event.GetPid());
	if ( iter != m_processes.end()) {
		clProcess *proc = iter->second;
		proc->Disconnect(proc->GetUid(), wxEVT_END_PROCESS, wxProcessEventHandler(TagsManager::OnCtagsEnd), NULL, this);
		// start new process
		StartCtagsProcess();

		{
			wxCriticalSectionLocker locker(m_cs);
			// delete the one that just terminated
			if (m_canDeleteCtags) {
				delete proc;
				//also delete all old ctags that might be in the garbage collector
				std::list<clProcess*>::iterator it = m_gargabeCollector.begin();
				for (; it != m_gargabeCollector.end(); it++) {
					delete (*it);
				}
				m_gargabeCollector.clear();
			} else
				m_gargabeCollector.push_back(proc);
		}

		// remove it from the map
		m_processes.erase(iter);
	}
}
#endif

//---------------------------------------------------------------------
// Parsing
//---------------------------------------------------------------------
void TagsManager::SourceToTags(const wxFileName& source, wxString& tags, clProcess *ctags)
{
	wxASSERT_MSG(wxThread::IsMain(), wxT("SourceToTags can be called only from the main thread!"));

#if defined (__WXMSW__) || defined (__WXGTK__)
	if (ctags == NULL) {
		ctags = m_ctags;
	}

	wxOutputStream *out = ctags->GetOutputStream();
	if ( out ) {
		wxString cmd(source.GetFullPath());
		cmd += wxT("\n");

		const wxCharBuffer pWriteData = _C(cmd);
		out->Write(pWriteData.data(), cmd.Length());
	} else {
		return;
	}

	static int maxPeeks = 1000;
	int count = 0;

	tags.Empty();
	while (true) {
		if (ctags->IsInputAvailable()) {
			wxTextInputStream in(*ctags->GetInputStream());
			tags << in.GetChar();
			if (tags.EndsWith(wxT("<<EOF>>")))
				break;
			count =  0;
		} else {
			count++;
			wxMilliSleep(1);
			if (count >= maxPeeks) {
				RestartCtagsProcess();
				tags.Empty();
				break;
			}
		}
	}
#else
	//On mac use the second version of tags processing
	wxUnusedVar(ctags);
	SourceToTags2(source, tags);
	return;
#endif
}

void TagsManager::SourceToTags2(const wxFileName &fileName, wxString &tags)
{
	//second version of soruce->tags
	//this function can be called only from the main thread
	wxString cmd;

	// Get ctags flags from the map
	wxString ctagsCmd;
	ctagsCmd << m_options.ToString() << m_ctagsCmd;

	// build the command, we surround ctags name with double quatations
	cmd << wxT("\"") << m_ctagsPath.GetFullPath() << wxT("\"") << ctagsCmd << wxT(" \"") << fileName.GetFullPath() << wxT("\"");

	wxArrayString output;
	ProcUtils::ExecuteCommand(cmd, output, wxEXEC_SYNC);

	tags.Clear();
	for (size_t i=0; i<output.GetCount(); i++) {
		tags << output.Item(i) << wxT("\n");
	}

	//wxPrintf(wxT("%s\n"), cmd.GetData());
	wxPrintf(wxT("SourceToTags2:\n%s\n"), tags.GetData());
}

TagTreePtr TagsManager::TreeFromTags(const wxString& tags)
{
	// Load the records and build a language tree
	TagEntry root;
	root.SetName(wxT("<ROOT>"));

	TagTreePtr tree( new TagTree(wxT("<ROOT>"), root) );

	wxStringTokenizer tkz(tags, wxT("\n"));
	while (tkz.HasMoreTokens()) {
		TagEntry tag;
		wxString line = tkz.NextToken();

		line = line.Trim();
		line = line.Trim(false);
		if (line.IsEmpty())
			continue;

		// Construct the tag from the line
		TagFromLine(line, tag);

		// Add the tag to the tree, locals are not added to the
		// tree
		if ( tag.GetKind() != wxT("local") )
			tree->AddEntry(tag);
	}
	return tree;
}

bool TagsManager::IsValidCtagsFile(const wxFileName &filename) const
{
	bool is_ok(false);
	// Put a request on the parsing thread to update the GUI tree & the database
	wxString filespec = GetCtagsOptions().GetFileSpec();

	// do we support files without an extension?
	if (GetCtagsOptions().GetFlags() & CC_PARSE_EXT_LESS_FILES && filename.GetExt().IsEmpty())
		return true;

	//if the file spec matches the current file, notify ctags
	wxStringTokenizer tkz(filespec, wxT(";"));
	while (tkz.HasMoreTokens()) {
		wxString spec = tkz.NextToken();
		if (wxMatchWild(spec, filename.GetFullName())) {
			is_ok = true;
			break;
		}
	} // while(tkz.HasMoreTokens())
	return is_ok;
}

//-----------------------------------------------------------------------------
// >>>>>>>>>>>>>>>>>>>>> Code Completion API START
//-----------------------------------------------------------------------------

void TagsManager::TagsByScopeAndName(const wxString& scope, const wxString &name, std::vector<TagEntryPtr> &tags, SearchFlags flags)
{
	wxString sql;
	std::vector<wxString> derivationList;
	//add this scope as well to the derivation list
	derivationList.push_back(scope);
	GetDerivationList(scope, derivationList);

	//make enough room for max of 500 elements in the vector
	tags.reserve(500);
	wxString tmpName(name);
	tmpName.Replace(wxT("_"), wxT("^_"));

	for (size_t i=0; i<derivationList.size(); i++) {
		sql.Empty();
		if (flags & PartialMatch) {
			sql << wxT("select * from tags where scope='") << derivationList.at(i) << wxT("' and name like '") << tmpName << wxT("%%' ESCAPE '^' ");
		} else {
			sql << wxT("select * from tags where scope='") << derivationList.at(i) << wxT("' and name ='") << name << wxT("' ");
		}
		DoExecuteQueury(sql, tags);
	}

	// and finally sort the results
	std::sort(tags.begin(), tags.end(), SAscendingSort());
}

void TagsManager::TagsByScope(const wxString& scope, std::vector<TagEntryPtr> &tags)
{
	wxString sql;
	std::vector<wxString> derivationList;
	//add this scope as well to the derivation list
	derivationList.push_back(scope);
	GetDerivationList(scope, derivationList);

	//make enough room for max of 500 elements in the vector
	tags.reserve(500);

	for (size_t i=0; i<derivationList.size(); i++) {
		sql.Empty();
		wxString tmpScope(derivationList.at(i));
		sql << wxT("select * from tags where scope='") << tmpScope << wxT("'  ");
		DoExecuteQueury(sql, tags);
	}
	// and finally sort the results

	std::sort(tags.begin(), tags.end(), SAscendingSort());

}

bool TagsManager::WordCompletionCandidates(const wxFileName &fileName, int lineno, const wxString& expr, const wxString& text, const wxString &word, std::vector<TagEntryPtr> &candidates)
{
	candidates.clear();
	wxString path, tmp;
	wxString typeName, typeScope;

	//remove the word from the expression
	wxString expression(expr);

	// Trim whitespace from right and left
	static wxString trimString(wxT("!<>=(){};\r\n\t\v "));

	expression.erase(0, expression.find_first_not_of(trimString));
	expression.erase(expression.find_last_not_of(trimString)+1);
	tmp = expression;
	expression.EndsWith(word, &tmp);
	expression = tmp;

	wxString funcSig;
	std::vector<wxString> additionlScopes; //from 'using namespace XXX;' statements

	wxString scope;
	wxString scopeName = LanguageST::Get()->GetScopeName(text, &additionlScopes);
	TagEntryPtr funcTag = FunctionFromFileLine(fileName, lineno);
	if (funcTag) {
		funcSig = funcTag->GetSignature();
	}

	if (expression.IsEmpty()) {
		//collect all the tags from the current scope, and
		//from the global scope
		scope = LanguageST::Get()->GetScope(text);
		std::vector<TagEntryPtr> tmpCandidates;
		GetGlobalTags(word, tmpCandidates);
		GetLocalTags(word, scope, tmpCandidates);
		GetLocalTags(word, funcSig, tmpCandidates);
		TagsByScopeAndName(scopeName, word, tmpCandidates);
		for (size_t i=0; i<additionlScopes.size(); i++) {
			TagsByScopeAndName(additionlScopes.at(i), word, tmpCandidates);
		}
		RemoveDuplicates(tmpCandidates, candidates);
	} else {
		wxString typeName, typeScope;
		bool res = ProcessExpression(fileName, lineno, expression, text, typeName, typeScope);
		if (!res) {
			return false;
		}

		//get all symbols realted to this scope
		scope = wxT("");
		if (typeScope == wxT("<global>"))
			scope << typeName;
		else
			scope << typeScope << wxT("::") << typeName;

		std::vector<TagEntryPtr> tmpCandidates;
		TagsByScope(scope, tmpCandidates);
		RemoveDuplicates(tmpCandidates, candidates);
	}
	return true;
}

bool TagsManager::AutoCompleteCandidates(const wxFileName &fileName, int lineno, const wxString& expr, const wxString& text, std::vector<TagEntryPtr>& candidates)
{
	candidates.clear();
	wxString path;
	wxString typeName, typeScope;

	wxString expression(expr);
	static wxString trimLeftString(wxT("{};\r\n\t\v "));
	static wxString trimRightString(wxT("({};\r\n\t\v "));
	expression.erase(0, expression.find_first_not_of(trimLeftString));
	expression.erase(expression.find_last_not_of(trimRightString)+1);

	bool res = ProcessExpression(fileName, lineno, expression, text, typeName, typeScope);
	if (!res) {
		return false;
	}

	//load all tags from the database that matches typeName & typeScope
	wxString scope;
	if (typeScope == wxT("<global>"))
		scope << typeName;
	else
		scope << typeScope << wxT("::") << typeName;

	//this function will retrieve the ineherited tags as well
	TagsByScope(scope, candidates);
	return candidates.empty() == false;
}

void TagsManager::RemoveDuplicates(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target)
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

void TagsManager::RemoveDuplicatesTips(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target)
{
	for (size_t i=0; i<src.size(); i++) {
		if (i == 0) {
			target.push_back(src.at(0));
		} else {
			if (src.at(i)->GetSignature() != target.at(target.size()-1)->GetSignature()) {
				target.push_back(src.at(i));
			}
		}
	}
}

void TagsManager::GetGlobalTags(const wxString &name, std::vector<TagEntryPtr> &tags, SearchFlags flags)
{
	wxString sql, tmpName;

	//make enough room for max of 500 elements in the vector
	tags.reserve(500);
	tmpName = name;
	if (flags == PartialMatch) {
		tmpName.Replace(wxT("_"), wxT("^_"));
		sql << wxT("select * from tags where parent='<global>' and name like '") << tmpName << wxT("%%' ESCAPE '^'  ");
	} else {
		sql << wxT("select * from tags where parent='<global>' and name ='") << tmpName << wxT("'  ");
	}
	DoExecuteQueury(sql, tags);
	std::sort(tags.begin(), tags.end(), SAscendingSort());
}

void TagsManager::GetLocalTags(const wxString &name, const wxString &scope, std::vector<TagEntryPtr> &tags, SearchFlags flags)
{
	//collect tags from the current scope text
	LanguageST::Get()->GetLocalVariables(scope, tags, name, flags);
}

void TagsManager::GetHoverTip(const wxFileName &fileName, int lineno, const wxString & expr, const wxString &word, const wxString & text, std::vector<wxString> & tips)
{
	wxString path;
	wxString typeName, typeScope, tmp;
	std::vector<TagEntryPtr> tmpCandidates, candidates;

	//remove the word from the expression
	wxString expression(expr);

	// Trim whitespace from right and left
	static wxString trimLeftString(wxT("{};\r\n\t\v "));
	static wxString trimRightString(wxT("({};\r\n\t\v "));
	expression.erase(0, expression.find_first_not_of(trimLeftString));
	expression.erase(expression.find_last_not_of(trimRightString)+1);

	tmp = expression;
	expression.EndsWith(word, &tmp);
	expression = tmp;

	wxString scope = LanguageST::Get()->GetScope(text);
	wxString scopeName = LanguageST::Get()->GetScopeName(scope, NULL);
	if (expression.IsEmpty()) {
		//collect all the tags from the current scope, and
		//from the global scope

		GetGlobalTags(word, tmpCandidates, ExactMatch);
		GetLocalTags(word, scope, tmpCandidates, ExactMatch);
		TagsByScopeAndName(scopeName, word, tmpCandidates);
		RemoveDuplicates(tmpCandidates, candidates);

		// we now have a list of tags that matches our word
		TipsFromTags(candidates, word, tips);
	} else {
		wxString typeName, typeScope;
		bool res = ProcessExpression(fileName, lineno, expression, text, typeName, typeScope);
		if (!res) {
			return;
		}

		//get all symbols realted to this scope
		scope = wxT("");
		if (typeScope == wxT("<global>"))
			scope << typeName;
		else
			scope << typeScope << wxT("::") << typeName;

		std::vector<TagEntryPtr> tmpCandidates;
		TagsByScopeAndName(scope, word, tmpCandidates);
		RemoveDuplicatesTips(tmpCandidates, candidates);

		// we now have a list of tags that matches our word
		TipsFromTags(candidates, word, tips);
	}
}

void TagsManager::FindImplDecl(const wxFileName &fileName, int lineno, const wxString & expr, const wxString &word, const wxString & text, std::vector<TagEntryPtr> &tags, bool imp)
{
	wxString path;
	wxString typeName, typeScope, tmp;
	std::vector<TagEntryPtr> tmpCandidates, candidates;

	//remove the word from the expression
	wxString expression(expr);

	// Trim whitespace from right and left
	static wxString trimString(wxT("(){};\r\n\t\v "));

	expression.erase(0, expression.find_first_not_of(trimString));
	expression.erase(expression.find_last_not_of(trimString)+1);
	tmp = expression;
	expression.EndsWith(word, &tmp);
	expression = tmp;

	wxString scope(text);// = LanguageST::Get()->GetScope(text);
	wxString scopeName = LanguageST::Get()->GetScopeName(scope, NULL);
	if (expression.IsEmpty()) {

		//collect all the tags from the current scope, and
		//from the global scope
		GetGlobalTags(word, tmpCandidates, ExactMatch);
		TagsByScopeAndName(scopeName, word, tmpCandidates, ExactMatch);

		if (!imp) {
			//collect only implementation
			FilterImplementation(tmpCandidates, tags);
		} else {
			FilterDeclarations(tmpCandidates, tags);
		}

	} else {

		wxString typeName, typeScope;
		bool res = ProcessExpression(fileName, lineno, expression, text, typeName, typeScope);
		if (!res) {
			return;
		}

		//get all symbols realted to this scope
		scope = wxT("");
		if (typeScope == wxT("<global>"))
			scope << typeName;
		else
			scope << typeScope << wxT("::") << typeName;

		std::vector<TagEntryPtr> tmpCandidates;
		TagsByScopeAndName(scope, word, tmpCandidates, ExactMatch);

		if (!imp) {
			//collect only implementation
			FilterImplementation(tmpCandidates, tags);
		} else {
			FilterDeclarations(tmpCandidates, tags);
		}
	}
}

void TagsManager::FilterImplementation(const std::vector<TagEntryPtr> &src, std::vector<TagEntryPtr> &tags)
{
	//remove all implementations and leave only declarations
	std::map<wxString, TagEntryPtr> tmpMap;
	for (size_t i=0; i<src.size(); i++) {
		TagEntryPtr tag = src.at(i);
		if (tag->GetKind() != wxT("function")) {
			wxString key;
			key << tag->GetFile() << tag->GetLine();
			tmpMap[key] = tag;
		}
	}

	std::map<wxString, TagEntryPtr>::iterator iter = tmpMap.begin();
	for (; iter != tmpMap.end(); iter++) {
		tags.push_back(iter->second);
	}
}

void TagsManager::FilterDeclarations(const std::vector<TagEntryPtr> &src, std::vector<TagEntryPtr> &tags)
{
	std::map<wxString, TagEntryPtr> tmpMap;
	for (size_t i=0; i<src.size(); i++) {
		TagEntryPtr tag = src.at(i);
		if (tag->GetKind() != wxT("prototype")) {
			wxString key;
			key << tag->GetFile() << tag->GetLine();
			tmpMap[key] = tag;
		}
	}
	std::map<wxString, TagEntryPtr>::iterator iter = tmpMap.begin();
	for (; iter != tmpMap.end(); iter++) {
		tags.push_back(iter->second);
	}
}

clCallTipPtr TagsManager::GetFunctionTip(const wxFileName &fileName, int lineno, const wxString &expr, const wxString &text, const wxString &word)
{
	std::vector<TagEntryPtr> candidates;
	wxString path;
	wxString typeName, typeScope, tmp;
	std::vector<wxString> tips;

	// Trim whitespace from right and left
	wxString expression(expr);
	static wxString trimLeftString(wxT("{};\r\n\t\v "));
	static wxString trimRightString(wxT("({};\r\n\t\v "));
	expression.erase(0, expression.find_first_not_of(trimLeftString));
	expression.erase(expression.find_last_not_of(trimRightString)+1);

	//remove the last token from the expression
	expression.EndsWith(word, &tmp);
	expression = tmp;
	if (word.IsEmpty()) {
		return NULL;
	}

	CppScanner scanner;
	scanner.SetText(_C(word));
	if (scanner.yylex() != IDENTIFIER) {
		return NULL;
	}

	if (expression.IsEmpty()) {
		std::vector<wxString> additionlScopes;
		//we are probably examining a global function, or a scope function
		wxString scopeName = LanguageST::Get()->GetScopeName(text, &additionlScopes);
		GetGlobalTags(word, candidates, ExactMatch);
		TagsByScopeAndName(scopeName, word, candidates);
		for (size_t i=0; i<additionlScopes.size(); i++) {
			TagsByScopeAndName(additionlScopes.at(i), word, candidates);
		}
		GetFunctionTipFromTags(candidates, word, tips);
	} else {
		bool res = ProcessExpression(fileName, lineno, expression, text, typeName, typeScope);
		if (!res) {
			return false;
		}

		//load all tags from the database that matches typeName & typeScope
		wxString scope;
		if (typeScope == wxT("<global>"))
			scope << typeName;
		else
			scope << typeScope << wxT("::") << typeName;

		//this function will retrieve the ineherited tags as well
		std::vector<TagEntryPtr> tmpCandidates;
		TagsByScope(scope, tmpCandidates);
		GetFunctionTipFromTags(tmpCandidates, word, tips);
	}

	// display call tip with function prototype
	clCallTipPtr ct( new clCallTip(tips) );
	return ct;
}

//-----------------------------------------------------------------------------
// <<<<<<<<<<<<<<<<<<< Code Completion API END
//-----------------------------------------------------------------------------
void TagsManager::OpenType(std::vector<TagEntryPtr> &tags)
{
	wxString sql;
	sql << wxT("select * from tags where kind in ('class', 'namespace', 'struct', 'union', 'enum', 'typedef') order by name DESC");
	DoExecuteQueury(sql, tags);
}

void TagsManager::FindSymbol(const wxString& name, std::vector<TagEntryPtr> &tags)
{
	wxString query;
	query << wxT("select * from tags where name='") << name << wxT("'  ");
	DoExecuteQueury(query, tags);
}

void TagsManager::DeleteFilesTags(const std::vector<wxFileName> &projectFiles)
{
	wxString query;
	wxString filelist;
	query << wxT("delete from tags where file in (");
	for (size_t i=0; i<projectFiles.size(); i++) {
		filelist << wxT("'") << projectFiles.at(i).GetFullPath() << wxT("'") << wxT(",");
	}
	filelist = filelist.BeforeLast(wxT(','));
	query << filelist << wxT(")");

	m_pDb->Begin();
	m_pDb->ExecuteUpdate(query);
	m_pDb->Commit();
}

void TagsManager::BuildExternalDatabase(ExtDbData &data)
{
	// set cursor to busy
	wxBusyCursor busy;
	wxArrayString all_files;
	wxArrayString files;
	bool extlessFiles = m_options.GetFlags() & CC_PARSE_EXT_LESS_FILES ? true : false;

	wxDir::GetAllFiles(data.rootPath, &all_files);
	wxStringTokenizer tok(m_options.GetFileSpec(), wxT(";"));
	std::map<wxString, bool> specMap;
	while ( tok.HasMoreTokens() ) {
		std::pair<wxString, bool> val;
		val.first = tok.GetNextToken().AfterLast(wxT('*'));
		val.first = val.first.AfterLast(wxT('.')).MakeLower();
		val.second = true;
		specMap.insert( val );
	}

	//filter non interesting files
	for (size_t i=0; i<all_files.GetCount(); i++) {
		wxFileName fn(all_files.Item(i));

		if (data.includeDirs.Index(fn.GetPath()) == wxNOT_FOUND) {
			continue;
		}

		if ( specMap.empty() ) {
			files.Add(all_files.Item(i));
		} else if (fn.GetExt().IsEmpty() & extlessFiles) {
			files.Add(all_files.Item(i));
		} else if (specMap.find(fn.GetExt().MakeLower()) != specMap.end()) {
			files.Add(all_files.Item(i));
		}
	}

	TagsDatabase db;

	db.OpenDatabase(data.dbName);
	wxFileName dbPath(data.rootPath);
	wxString path = dbPath.GetFullPath();
	DoBuildDatabase(files, db, &path);
}

void TagsManager::RetagFiles(const std::vector<wxFileName> &files)
{
	DeleteFilesTags(files);
	wxArrayString strFiles;
	for (size_t i=0; i<files.size(); i++) {
		strFiles.Add(files.at(i).GetFullPath());
	}
	DoBuildDatabase(strFiles, *m_pDb);
}

void TagsManager::DoBuildDatabase(const wxArrayString &files, TagsDatabase &db, const wxString *rootPath)
{
	wxString tags;
	wxProgressDialog* prgDlg = NULL;

	if (files.IsEmpty())
		return;

	// Create a progress dialog
	prgDlg = new wxProgressDialog (wxT("Building tags database ..."), wxT("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"), (int)files.GetCount()*2, NULL, wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_AUTO_HIDE);
	prgDlg->GetSizer()->Fit(prgDlg);
	prgDlg->Layout();
	prgDlg->Centre();

	prgDlg->Update(0, wxT("Parsing..."));
	int maxVal = (int)files.GetCount();
	int i = 0;

	std::list<tagParseResult> trees;
	for (i=0; i<maxVal; i++) {
		wxString fileTags;
		wxFileName curFile(files.Item(i));

		// update the progress bar
		wxString msg;
		msg << wxT("Parsing file: ") << curFile.GetFullName();
		prgDlg->Update(i, msg);

		tags.Clear();
		tagParseResult result;
		result.fileName = curFile.GetFullName();
		if (GetParseComments()) {
			result.comments = new std::vector<DbRecordPtr>();
			result.tree = ParseSourceFile(curFile, result.comments);
		} else {
			result.tree = ParseSourceFile(curFile);
		}
		trees.push_back(result);
	}

	unsigned int cur = 0;
	for (std::list<tagParseResult>::iterator iter = trees.begin(); iter != trees.end(); iter++) {
		wxString msg;
		msg << wxT("Saving symbols from file: ") << (*iter).fileName;
		prgDlg->Update(maxVal + cur, msg);

		db.Store((*iter).tree, wxFileName());
		if (GetParseComments()) {
			// drop all old entries from this file
			try {
				db.Store(*(*iter).comments, wxFileName());
			} catch ( wxSQLite3Exception & e) {
				wxUnusedVar(e);
			}

			//free the vector
			delete (*iter).comments;
		}
		cur++;
	}

	//update the variable table
	if (rootPath) {
		DbRecordPtr record(new VariableEntry(*rootPath, *rootPath));
		db.Begin();
		if (db.Insert(record) == TagExist) {
			db.Update(record);
		}
		db.Commit();
	}
	prgDlg->Destroy();
}

void TagsManager::OpenExternalDatabase(const wxFileName &dbName)
{
	// check that the database exist
	if (!wxFile::Exists(dbName.GetFullPath()))
		return;

	wxString message;

	// load it to memory
	if (GetCtagsOptions().GetFlags() & CC_LOAD_EXT_DB_TO_MEMORY) {
		message << wxT("Attaching symbols database '") << dbName.GetFullName() << wxT("' to memory ...");
		wxBusyInfo wait(message);
		wxWindowDisabler disableAll;
		wxBusyCursor cursor;

		{
			//check version, we do it a inner scope, so that the db cerated on the stack
			//will be destroyed and close the underlying database
			TagsDatabase db;
			db.OpenDatabase(dbName);
			if (db.GetSchemaVersion() != db.GetVersion()) {
				db.RecreateDatabase();
			}
		}

		m_pExternalDb->LoadToMemory(dbName);
	} else {
		m_pExternalDb->OpenDatabase(dbName);
		if (m_pExternalDb->GetVersion() != m_pExternalDb->GetSchemaVersion()) {
			m_pExternalDb->RecreateDatabase();
		}
	}
}

wxString TagsManager::GetComment(const wxString &file, const int line)
{
	wxString sql;
	// the comments is searched one line above the requested line
	sql << wxT("SELECT * FROM COMMENTS WHERE file='")
	<< file << wxT("' AND line=") << line - 1;

	try {
		wxSQLite3ResultSet rs = m_pDb->Query( sql );
		if ( rs.NextRow() ) {
			Comment comment( rs );
			return comment.GetComment();
		}

		// try the external database
		if ( m_pExternalDb->IsOpen() ) {
			wxSQLite3ResultSet rs = m_pExternalDb->Query( sql );
			if ( rs.NextRow() ) {
				Comment comment( rs );
				return comment.GetComment();
			}
		}
	} catch ( wxSQLite3Exception &e ) {
		wxUnusedVar(e);
	}

	return wxEmptyString;
}

void TagsManager::StoreComments(const std::vector<DbRecordPtr> &comments, const wxFileName& path)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	m_pDb->Store( comments, path );
}

void TagsManager::FindByNameAndScope(const wxString &name, const wxString &scope, std::vector<TagEntryPtr> &tags)
{
	DoFindByNameAndScope(name, scope, tags);

	// Sort the results base on their name
	std::sort(tags.begin(), tags.end(), SDescendingSort());
}

void TagsManager::FindByPath(const wxString &path, std::vector<TagEntryPtr> &tags)
{
	wxString sql;
	sql << wxT("select * from tags where path='") << path << wxT("'  ");
	DoExecuteQueury(sql, tags);
}

void TagsManager::DoFindByNameAndScope(const wxString &name, const wxString &scope, std::vector<TagEntryPtr> &tags)
{
	wxString sql;
	if (scope == wxT("<global>")) {
		sql << wxT("select * from tags where name='") << name << wxT("' and parent='<global>'  ");
		DoExecuteQueury(sql, tags);
	} else {
		std::vector<wxString> derivationList;
		derivationList.push_back(scope);
		GetDerivationList(scope, derivationList);

		for (size_t i=0; i<derivationList.size(); i++) {
			sql.Empty();
			wxString path_;
			path_ << derivationList.at(i) << wxT("::") << name ;

			sql << wxT("select * from tags where path='") << path_ << wxT("'  ");
			DoExecuteQueury(sql, tags);
		}
	}
}

bool TagsManager::IsTypeAndScopeExists(const wxString &typeName, wxString &scope)
{
	wxString cacheKey;
	cacheKey << typeName << wxT("@") << scope;

	if (!m_pExternalDb->IsOpen()) {
		m_typeScopeCache.clear();
	}

	//we search the cache first, note that the cache
	//is used only for the external database
	std::map<wxString, bool>::iterator iter = m_typeScopeCache.find(cacheKey);
	if (iter != m_typeScopeCache.end()) {
		return iter->second;
	}

	wxString sql;

	long value = 0;
	sql << wxT("select count(*) from tags where name='") << typeName << wxT("' and scope='") << scope << wxT("'");

	for (size_t i=0; i<2; i++) {
		
		if (i == 1) {
			// Second try, change the SQL query to test against the global scope
			sql.Clear();
			sql << wxT("select count(*) from tags where name='") << typeName << wxT("' and scope='<global>'");
			value = 0;
		}
		
		wxSQLite3ResultSet rs = m_pDb->Query(sql);
		try {
			if (rs.NextRow()) {
				rs.GetAsString(0).ToLong(&value);
				if (value > 0) {
					if(i == 1) { scope = wxT("<global>"); }
					return true;
				}
			}

			if ( m_pExternalDb->IsOpen() ) {
				wxSQLite3ResultSet ex_rs;
				ex_rs = m_pExternalDb->Query(sql);

				// add results from external database to the workspace database
				if ( ex_rs.NextRow() ) {
					ex_rs.GetAsString(0).ToLong(&value);
					if (value > 0) {
						if(i == 1) { 
							scope = wxT("<global>"); 
							return true;
						}
						m_typeScopeCache[cacheKey] = true;
						return true;
					} else {
						if( i == 1 ){
							m_typeScopeCache[cacheKey] = false;
						}
					}
				}
				ex_rs.Finalize();
			}
			rs.Finalize();
		} catch ( wxSQLite3Exception& e) {
			wxUnusedVar(e);
			return false;
		}

		// no match, try to find the typeName in the global scope
	}
	return false;
}

bool TagsManager::QueryExtDbCache(const wxString &sql, std::vector<TagEntryPtr> &tags)
{
	std::map<wxString, TagCacheEntryPtr>::iterator iter = m_cache.find(sql);
	if (iter != m_cache.end()) {
		tags.insert(tags.end(), iter->second->GetTags().begin(), iter->second->GetTags().end());
		return true;
	}
	return false;
}

void TagsManager::AddToExtDbCache(const wxString &query, const std::vector<TagEntryPtr> &tags)
{
	TagCacheEntryPtr entry(new TagCacheEntry(query, tags));
	m_cache[entry->GetQueryKey()] = entry;
}

void TagsManager::DoExecuteQueury(const wxString &sql, std::vector<TagEntryPtr> &tags)
{
	try {
		//try the external database first
		if ( m_pExternalDb->IsOpen() ) {
			//check the cache first
			if (!QueryExtDbCache(sql, tags)) {
				//nothing found in the cache
				wxSQLite3ResultSet ex_rs;
				ex_rs = m_pExternalDb->Query(sql);

				// add results from external database to the workspace database
				std::vector<TagEntryPtr> tmpTags;
				while ( ex_rs.NextRow() ) {
					// Construct a TagEntry from the rescord set
					TagEntryPtr tag(new TagEntry(ex_rs));
					//conver the path to be real path
					ConvertPath(tag);
					tmpTags.push_back(tag);
				}
				//add results to cache
				AddToExtDbCache(sql, tmpTags);
				tags.insert(tags.end(), tmpTags.begin(), tmpTags.end());
				ex_rs.Finalize();
			}
		}
		//now try the local tags database
		wxSQLite3ResultSet rs = m_pDb->Query(sql);
		while ( rs.NextRow() ) {
			// Construct a TagEntry from the rescord set
			TagEntryPtr tag(new TagEntry(rs));
			tags.push_back(tag);
		}
		rs.Finalize();
	} catch ( wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
}

void TagsManager::ConvertPath(TagEntryPtr& tag)
{
	//get list of variables from database if not loaded
	if (m_pExternalDb->IsOpen() && m_vars.empty()) {
		m_pExternalDb->GetVariables(m_vars);
	}

	wxString filename = tag->GetFile();
	for (size_t i=0; i<m_vars.size(); i++) {
		wxString fixedpath;
		if (m_vars.at(i)->GetValue().IsEmpty() == false) {
			if (filename.StartsWith(m_vars.at(i)->GetName(), &fixedpath)) {
				fixedpath = fixedpath.Prepend(m_vars.at(i)->GetValue() + wxT("/"));
				tag->SetFile(fixedpath);
				break;
			}
		}
	}
}

bool TagsManager::GetDerivationList(const wxString &path, std::vector<wxString> &derivationList)
{
	wxString sql;
	sql << wxT("select * from tags where path='") << path << wxT("' and kind in ('struct', 'class', 'interface') ");

	std::vector<TagEntryPtr> tags;
	TagEntryPtr tag;
	DoExecuteQueury(sql, tags);
	if (tags.size() == 1) {
		tag = tags.at(0);
	} else {
		return false;
	}

	if (tag && tag->IsOk()) {
		wxString ineheritsList = tag->GetInherits();
		wxStringTokenizer tok(ineheritsList, wxT(','));
		while (tok.HasMoreTokens()) {
			wxString inherits = tok.GetNextToken();
			if (tag->GetScopeName() != wxT("<global>")) {
				inherits = tag->GetScopeName() + wxT("::") + inherits;
			}
			derivationList.push_back(inherits);
			GetDerivationList(inherits, derivationList);
		}
	}
	return true;
}

void TagsManager::TipsFromTags(const std::vector<TagEntryPtr> &tags, const wxString &word, std::vector<wxString> &tips)
{
	for (size_t i=0; i<tags.size(); i++) {
		if (tags.at(i)->GetName() != word)
			continue;

		wxString tip = tags.at(i)->GetPattern();
		wxString comment;

		//handle comments
		if (GetCtagsOptions().GetFlags() & CC_DISP_COMMENTS) {
			int      lineno   = tags.at(i)->GetLine();
			wxString filename = tags.at(i)->GetFile();
			if (lineno != wxNOT_FOUND && filename.IsEmpty() == false) {
				comment = GetComment(filename, lineno);
				if (comment.IsEmpty() == false) {
					comment << wxT("\n");
				}
			}
		}

		//remove the pattern perfix and suffix
		tip = tip.Trim().Trim(false);
		tip = tip.AfterFirst(wxT('^'));
		if (tip.Find(wxT('$')) != wxNOT_FOUND) {
			tip = tip.BeforeLast(wxT('$'));
		} else {
			if (tip.EndsWith(wxT("/"))) {
				tip = tip.BeforeLast(wxT('/'));
			}
		}

		//since the tip is built from the pattern, which is actually a regex expression
		//some characters might be escaped (e.g. '/' will appear as '\/')
		tip.Replace(wxT("\\/"), wxT("/"));

		// Trim whitespace from right and left
		static wxString trimString(wxT("{};\r\n\t\v "));

		tip.erase(0, tip.find_first_not_of(trimString));
		tip.erase(tip.find_last_not_of(trimString)+1);
		tip.Replace(wxT("\t"), wxT(" "));

		tip.Prepend(comment);
		tips.push_back(tip);
	}
}

void TagsManager::GetFunctionTipFromTags(const std::vector<TagEntryPtr> &tags, const wxString &word, std::vector<wxString> &tips)
{
	std::map<wxString, wxString> tipsMap;

	for (size_t i=0; i<tags.size(); i++) {
		if (tags.at(i)->GetName() != word)
			continue;

		if (tags.at(i)->GetKind() != wxT("function") && tags.at(i)->GetKind() != wxT("prototype"))
			continue;

		wxString tip;
		tip << tags.at(i)->GetSignature();

		//collect each signature only once, we do this by using
		//map
		tipsMap[tags.at(i)->GetSignature()] = tip;
	}

	for (std::map<wxString, wxString>::iterator iter = tipsMap.begin(); iter != tipsMap.end(); iter++) {
		tips.push_back(iter->second);
	}
}

void TagsManager::CloseDatabase()
{
	if (m_pDb) {
		delete m_pDb;
		m_pDb = new TagsDatabase();
	}
}

void TagsManager::CloseExternalDatabase()
{
	//close the database by simply deleting it and creating new
	//empty one
	if (m_pExternalDb) {
		delete m_pExternalDb;
		m_pExternalDb = new TagsDatabase();
		//clear variables cache
		m_vars.clear();
		//clear the cache
		m_cache.clear();
	}
}

wxString TagsManager::GenerateDoxygenComment(const wxString &file, const int line)
{
	if (m_pDb->IsOpen()) {
		wxString sql;
		sql << wxT("select * from tags where file='") << file << wxT("' and line=") << line + 1 <<  wxT(" ");
		std::vector<TagEntryPtr> tags;
		DoExecuteQueury(sql, tags);
		if (tags.empty() || tags.size() > 1)
			return wxEmptyString;

		//create a doxygen comment from the tag
		return DoCreateDoxygenComment(tags.at(0));
	}
	return wxEmptyString;
}

wxString TagsManager::DoCreateDoxygenComment(TagEntryPtr tag)
{
	CppCommentCreator commentCreator(tag);
	return commentCreator.CreateComment();
}

bool TagsManager::GetParseComments()
{
	//wxCriticalSectionLocker lock(m_cs);
	return m_parseComments;
}

void TagsManager::SetCtagsOptions(const TagsOptionsData &options)
{
	m_options = options;
#if defined (__WXMSW__) || defined (__WXGTK__)
	RestartCtagsProcess();
#endif

	wxCriticalSectionLocker locker(m_cs);
	m_parseComments = m_options.GetFlags() & CC_PARSE_COMMENTS ? true : false;
}

void TagsManager::GenerateSettersGetters(const wxString &scope, const SettersGettersData &data, const std::vector<TagEntryPtr> &tags, wxString &impl, wxString *decl)
{
	wxUnusedVar(scope);
	wxUnusedVar(data);
	wxUnusedVar(tags);
	wxUnusedVar(impl);
	wxUnusedVar(decl);
}

void TagsManager::TagsByScope(const wxString &scopeName, const wxString &kind, std::vector<TagEntryPtr> &tags, bool includeInherits)
{
	wxString sql;
	std::vector<wxString> derivationList;
	//add this scope as well to the derivation list
	derivationList.push_back(scopeName);

	if (includeInherits) {
		GetDerivationList(scopeName, derivationList);
	}

	//make enough room for max of 500 elements in the vector
	tags.reserve(500);

	for (size_t i=0; i<derivationList.size(); i++) {
		sql.Empty();
		wxString tmpScope(derivationList.at(i));
		sql << wxT("select * from tags where scope='") << tmpScope << wxT("' and kind='") << kind << wxT("' ");;
		DoExecuteQueury(sql, tags);
	}
	// and finally sort the results
	std::sort(tags.begin(), tags.end(), SAscendingSort());
}

wxString TagsManager::GetScopeName(const wxString &scope)
{
	Language *lang = LanguageST::Get();
	return lang->GetScopeName(scope, NULL);
}

bool TagsManager::ProcessExpression(const wxFileName &filename, int lineno, const wxString &expr, const wxString &scopeText, wxString &typeName, wxString &typeScope)
{
	return LanguageST::Get()->ProcessExpression(expr, scopeText, filename, lineno, typeName, typeScope);
}

bool TagsManager::GetMemberType(const wxString &scope, const wxString &name, wxString &type, wxString &typeScope)
{
	wxString expression(scope);
	expression << wxT("::") << name << wxT(".");
	return LanguageST::Get()->ProcessExpression(expression, wxEmptyString, wxFileName(), wxNOT_FOUND, type, typeScope);
}

int TagsManager::UpdatePathVariable(const wxString &name, const wxString &value)
{
	DbRecordPtr record(new VariableEntry(name, value));
	return m_pExternalDb->Update(record);
}

int TagsManager::InsertPathVariable(const wxString &name, const wxString &value)
{
	DbRecordPtr record(new VariableEntry(name, value));
	return m_pExternalDb->Insert(record);
}

int TagsManager::GetPathVariable(const wxString &name, wxString &path)
{
	VariableEntryPtr var = m_pExternalDb->FindVariableByName(name);
	if (var) {
		path = var->GetValue();
		return TagOk;
	}
	return TagError;
}

void TagsManager::ReloadExtDbPaths()
{
	m_vars.clear();
}

void TagsManager::GetFiles(const wxString &partialName, std::vector<wxFileName> &files)
{
	if (m_pDb) {
		m_pDb->GetFiles(partialName, files);
	}
	if (m_pExternalDb) {
		m_pExternalDb->GetFiles(partialName, files);
	}
}


TagEntryPtr TagsManager::FunctionFromFileLine(const wxFileName &fileName, int lineno)
{
	if (!m_pDb) {
		return NULL;
	}

	wxString sql;
	sql << wxT("select * from tags where file = '")
	<< fileName.GetFullPath()
	<< wxT("' and line <= ")
	<< lineno
	<< wxT(" and kind='function' order by line DESC");

	//we take the first entry
	try {
		wxSQLite3ResultSet rs = m_pDb->Query(sql);
		if ( rs.NextRow() ) {
			// Construct a TagEntry from the rescord set
			TagEntryPtr tag(new TagEntry(rs));
			rs.Finalize();
			return tag;
		}
		rs.Finalize();
	} catch ( wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
	return NULL;
}

void TagsManager::GetScopesFromFile(const wxFileName &fileName, std::vector< TagEntryPtr > &tags)
{
	if (!m_pDb) {
		return;
	}

	wxString sql;
	sql << wxT("select * from tags where file = '")
	<< fileName.GetFullPath()
	<< wxT("' and kind in('class', 'struct', 'union')");

	//we take the first entry
	try {
		wxSQLite3ResultSet rs = m_pDb->Query(sql);
		while ( rs.NextRow() ) {
			// Construct a TagEntry from the rescord set
			TagEntryPtr tag(new TagEntry(rs));
			tags.push_back(tag);
		}
		rs.Finalize();
	} catch ( wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
}

void TagsManager::TagsFromFileAndScope(const wxFileName& fileName, const wxString &scopeName, std::vector< TagEntryPtr > &tags)
{
	if (!m_pDb) {
		return;
	}

	wxString sql;
	sql << wxT("select * from tags where file = '")
	<< fileName.GetFullPath() << wxT("' ")
	<< wxT(" and scope='") << scopeName << wxT("' ")
	<< wxT(" and kind in('prototype', 'function', 'enum')");

	//we take the first entry
	try {
		wxSQLite3ResultSet rs = m_pDb->Query(sql);
		while ( rs.NextRow() ) {
			// Construct a TagEntry from the rescord set
			TagEntryPtr tag(new TagEntry(rs));
			tags.push_back(tag);
		}
		rs.Finalize();
	} catch ( wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
}

bool TagsManager::GetFunctionDetails(const wxFileName &fileName, int lineno, TagEntryPtr &tag, clFunction &func)
{
	tag = FunctionFromFileLine(fileName, lineno);
	if (tag) {
		LanguageST::Get()->FunctionFromPattern( tag->GetPattern(), func );
		return true;
	}
	return false;
}

TagEntryPtr TagsManager::FirstFunctionOfFile(const wxFileName &fileName)
{
	if (!m_pDb) {
		return NULL;
	}

	wxString sql;
	sql << wxT("select * from tags where file = '")
	<< fileName.GetFullPath()
	<< wxT("' ")
	<< wxT(" and kind='function' order by line ASC");

	//we take the first entry
	try {
		wxSQLite3ResultSet rs = m_pDb->Query(sql);
		if ( rs.NextRow() ) {
			// Construct a TagEntry from the rescord set
			TagEntryPtr tag(new TagEntry(rs));
			rs.Finalize();
			return tag;
		}
		rs.Finalize();
	} catch ( wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
	return NULL;
}
