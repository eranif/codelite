//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : ctags_manager.cpp
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
#include <wx/frame.h>
#include <wx/app.h>
#include <wx/sizer.h>
#include <wx/log.h>
#include "parse_thread.h"
#include "ctags_manager.h"
#include "named_pipe_client.h"
#include <set>
#include "cl_indexer_request.h"
#include "clindexerprotocol.h"
#include "cl_indexer_reply.h"
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
#include "procutils.h"
#include <sstream>
#include <wx/string.h>
#include <wx/xrc/xmlres.h>
#include <wx/msgdlg.h>
#include "code_completion_api.h"

//#define __PERFORMANCE
#include "performance.h"

#ifdef __WXMSW__
#define PIPE_NAME "\\\\.\\pipe\\codelite_indexer_%s"
#else
#define PIPE_NAME "/tmp/codelite_indexer.%s.sock"
#endif

#define MAX_SEARCH_LIMIT 5000

const wxEventType wxEVT_UPDATE_FILETREE_EVENT = XRCID("update_file_tree_event");
const wxEventType wxEVT_TAGS_DB_UPGRADE       = XRCID("tags_db_upgraded");

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
	std::vector<CommentPtr> *comments;
	wxString fileName;
};

static int CtagsMgrTimerId = wxNewId();

//------------------------------------------------------------------------------
// Progress dialog
//------------------------------------------------------------------------------
class MyProgress : public wxProgressDialog
{
public:
	MyProgress(const wxString &title, size_t count) :
		wxProgressDialog(title, wxT(""), (int) count, NULL, wxPD_APP_MODAL|wxPD_AUTO_HIDE|wxPD_CAN_ABORT|wxPD_SMOOTH )
	{
		SetSize(500, -1);
		Centre();
	}

	virtual ~MyProgress()
	{}
};

//------------------------------------------------------------------------------
// CTAGS Manager
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(TagsManager, wxEvtHandler)
	EVT_TIMER(CtagsMgrTimerId, TagsManager::OnTimer)
	EVT_COMMAND(wxID_ANY, wxEVT_UPDATE_FILETREE_EVENT, TagsManager::OnUpdateFileTreeEvent)
END_EVENT_TABLE()

TagsManager::TagsManager()
		: wxEvtHandler()
		, m_codeliteIndexerPath(wxT("codelite_indexer"))
		, m_codeliteIndexerProcess(NULL)
		, m_canDeleteCtags(true)
		, m_timer(NULL)
		, m_lang(NULL)
		, m_evtHandler(NULL)
{
	// Create databases
	m_workspaceDatabase = new TagsStorageSQLite( );
	m_workspaceDatabase->SetSingleSearchLimit( MAX_SEARCH_LIMIT );
	m_ctagsCmd = wxT("  --excmd=pattern --sort=no --fields=aKmSsnit --c-kinds=+p --C++-kinds=+p ");
	m_timer = new wxTimer(this, CtagsMgrTimerId);
	m_timer->Start(100);

	// CPP keywords that are usually followed by open brace '('
	m_CppIgnoreKeyWords.insert(wxT("while"));
	m_CppIgnoreKeyWords.insert(wxT("if"));
	m_CppIgnoreKeyWords.insert(wxT("for"));
	m_CppIgnoreKeyWords.insert(wxT("switch"));
}

TagsManager::~TagsManager()
{
	delete m_workspaceDatabase;
	if (m_timer) {
		delete m_timer;
	}

	wxCriticalSectionLocker locker(m_cs);
	if (m_canDeleteCtags) {
		if (m_codeliteIndexerProcess) m_codeliteIndexerProcess->Disconnect(m_codeliteIndexerProcess->GetUid(), wxEVT_END_PROCESS, wxProcessEventHandler(TagsManager::OnCtagsEnd), NULL, this);

		// terminate ctags process
		if (m_codeliteIndexerProcess) m_codeliteIndexerProcess->Terminate();

		std::list<clProcess*>::iterator it = m_gargabeCollector.begin();
		for (; it != m_gargabeCollector.end(); it++) {
			delete (*it);
		}

		if (m_gargabeCollector.empty() == false) {
		}
		m_gargabeCollector.clear();
	}
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
	UpdateFileTree(m_workspaceDatabase, false);
	m_workspaceDatabase->OpenDatabase(fileName);

	if (m_workspaceDatabase->GetVersion() != m_workspaceDatabase->GetSchemaVersion()) {
		m_workspaceDatabase->RecreateDatabase();

		// Send event to the main frame notifying it about database recreation
		if( m_evtHandler ) {
			wxCommandEvent event(wxEVT_TAGS_DB_UPGRADE);
			m_evtHandler->AddPendingEvent( event );
		}
	}

	UpdateFileTree(m_workspaceDatabase, true);
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

TagTreePtr TagsManager::ParseSourceFile(const wxFileName& fp, std::vector<CommentPtr> *comments)
{
	wxString tags;

	if ( !m_codeliteIndexerProcess ) {
		return TagTreePtr( NULL );
	}
	SourceToTags(fp, tags);

	// return ParseTagsFile(tags, project);
	int dummy;
	TagTreePtr ttp = TagTreePtr( TreeFromTags(tags, dummy) );

	if ( comments && GetParseComments() ) {
		// parse comments
		GetLanguage()->ParseComments( fp, comments );

	}
	return ttp;
}

TagTreePtr TagsManager::ParseSourceFile2(const wxFileName& fp, const wxString &tags, std::vector<CommentPtr> *comments)
{
	//	return ParseTagsFile(tags, project);
	int count(0);
	TagTreePtr ttp = TagTreePtr( TreeFromTags(tags, count) );

	if (comments && GetParseComments()) {
		// parse comments
		GetLanguage()->ParseComments(fp, comments);
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

	m_workspaceDatabase->Store(tree, path);
}

TagTreePtr TagsManager::Load(const wxFileName& fileName)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	TagTreePtr               tree;
	std::vector<TagEntryPtr> tagsByFile;
	m_workspaceDatabase->SelectTagsByFile(fileName.GetFullPath(), tagsByFile);

	// Load the records and build a language tree
	TagEntry root;
	root.SetName(wxT("<ROOT>"));
	tree.Reset( new TagTree(wxT("<ROOT>"), root) );
	for(size_t i=0; i<tagsByFile.size(); i++) {
		tree->AddEntry( *(tagsByFile.at(i)) );
	}
	return tree;
}

void TagsManager::Delete(const wxFileName& path, const wxString& fileName)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	m_workspaceDatabase->DeleteByFileName(path, fileName);
	UpdateFileTree(std::vector<wxFileName>(1, fileName), false);
}

//--------------------------------------------------------
// Process Handling of CTAGS
//--------------------------------------------------------

clProcess *TagsManager::StartCtagsProcess()
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	// Run ctags process
	wxString cmd;
	wxString ctagsCmd;

	// build the command, we surround ctags name with double quatations
	wxString uid;
	uid << wxGetProcessId();

	if(m_codeliteIndexerPath.FileExists() == false) {
		wxLogMessage(wxT("ERROR: Could not locate indexer: %s"), m_codeliteIndexerPath.GetFullPath().c_str());
		m_codeliteIndexerProcess = NULL;
		return NULL;
	}

	// concatenate the PID to identifies this channel to this instance of codelite
	cmd << wxT("\"") << m_codeliteIndexerPath.GetFullPath() << wxT("\" ") << uid << wxT(" --pid");
	clProcess* process;

	process = new clProcess(wxNewId(), cmd);

	// Launch it!
	process->Start();
	m_processes[process->GetPid()] = process;

	if ( process->GetPid() <= 0 ) {
		m_codeliteIndexerProcess = NULL;
		return NULL;
	}

	// attach the termination event to the tags manager class
	process->Connect(process->GetUid(), wxEVT_END_PROCESS, wxProcessEventHandler(TagsManager::OnCtagsEnd), NULL, this);
	m_codeliteIndexerProcess = process;
	return process;
}

void TagsManager::RestartCtagsProcess()
{
	clProcess *oldProc(NULL);
	oldProc = m_codeliteIndexerProcess;

	if ( !oldProc ) {
		return ;
	}

	// no need to call StartCtagsProcess(), since it will be called automatically
	// by the termination handler OnCtagsEnd()
	oldProc->Terminate();
}

void TagsManager::SetCodeLiteIndexerPath(const wxString& path)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	m_codeliteIndexerPath = wxFileName(path, wxT("codelite_indexer"));
#ifdef __WXMSW__
	m_codeliteIndexerPath.SetExt(wxT("exe"));
#endif
}

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

//---------------------------------------------------------------------
// Parsing
//---------------------------------------------------------------------
void TagsManager::SourceToTags(const wxFileName& source, wxString& tags)
{
	std::stringstream s;
	s << wxGetProcessId();

	char channel_name[1024];
	memset(channel_name, 0, sizeof(channel_name));
	sprintf(channel_name, PIPE_NAME, s.str().c_str());

	clNamedPipeClient client(channel_name);

	// Build a request for the indexer
	clIndexerRequest req;
	// set the command
	req.setCmd(clIndexerRequest::CLI_PARSE);

	// prepare list of files to be parsed
	std::vector<std::string> files;
	files.push_back(source.GetFullPath().mb_str(wxConvUTF8).data());
	req.setFiles(files);

	// set ctags options to be used
	wxString ctagsCmd;
	ctagsCmd << wxT(" ") << m_tagsOptions.ToString() << wxT(" --excmd=pattern --sort=no --fields=aKmSsnit --c-kinds=+p --C++-kinds=+p ");
	req.setCtagOptions(ctagsCmd.mb_str(wxConvUTF8).data());

	// connect to the indexer
	if (!client.connect()) {
		wxPrintf(wxT("Failed to connect to indexer ID %d!\n"), wxGetProcessId());
		return;
	}

	
	// send the request
	if ( !clIndexerProtocol::SendRequest(&client, req) ) {
		wxPrintf(wxT("Failed to send request to indexer ID [%d]\n"), wxGetProcessId());
		return;
	}
	// read the reply
	clIndexerReply reply;
	try {
		if (!clIndexerProtocol::ReadReply(&client, reply)) {
			//wxLogMessage(wxT("codelite_indexer is not responding - restarting it. Input file was:%s"), source.GetFullPath().c_str());
			RestartCtagsProcess();
			return;
		}
	} catch (std::bad_alloc &ex) {
		tags.Clear();
#ifdef __WXMSW__
		wxLogMessage(wxString::Format(wxT("ERROR: failed to read reply from codelite_indexer for file %s: cought std::bad_alloc exception"), source.GetFullPath().c_str()));
#else
		wxPrintf(wxT("ERROR: failed to read reply: cought std::bad_alloc exception\n"));
#endif
		return;
	}

	// convert the data into wxString
	tags = wxString(reply.getTags().c_str(), wxConvUTF8);
	if(tags.empty()) {
		tags = wxString::From8BitData(reply.getTags().c_str());
	}
}

TagTreePtr TagsManager::TreeFromTags(const wxString& tags, int &count)
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
		tag.FromLine(line);

		// Add the tag to the tree, locals are not added to the
		// tree
		count++;
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
		spec.MakeLower();
		wxString lowerName = filename.GetFullName();
		lowerName.MakeLower();
		if (wxMatchWild(spec, lowerName)) {
			is_ok = true;
			break;
		}
	} // while(tkz.HasMoreTokens())
	return is_ok;
}

//-----------------------------------------------------------------------------
// >>>>>>>>>>>>>>>>>>>>> Code Completion API START
//-----------------------------------------------------------------------------

void TagsManager::TagsByScopeAndName(const wxString& scope, const wxString &name, std::vector<TagEntryPtr> &tags, size_t flags)
{
	std::vector<wxString> derivationList;
	// add this scope as well to the derivation list

	wxString _scopeName = DoReplaceMacros( scope );
	derivationList.push_back(_scopeName);

	GetDerivationList(_scopeName, derivationList);

	// make enough room for max of 500 elements in the vector
	tags.reserve(500);
	wxArrayString scopes;

	for (size_t i=0; i<derivationList.size(); i++) {
		// try the worksapce database for match
		scopes.Add(derivationList.at(i));
	}

	m_workspaceDatabase->GetTagsByScopeAndName(scopes, name, flags & PartialMatch, tags);

	// and finally sort the results
	std::sort(tags.begin(), tags.end(), SAscendingSort());
}

void TagsManager::TagsByScope(const wxString& scope, std::vector<TagEntryPtr> &tags)
{
	std::vector<wxString> derivationList;
	//add this scope as well to the derivation list
	wxString _scopeName = DoReplaceMacros( scope );
	derivationList.push_back(_scopeName);

	GetDerivationList(_scopeName, derivationList);

	//make enough room for max of 500 elements in the vector
	tags.reserve(500);

	for (size_t i=0; i<derivationList.size(); i++) {
		wxString tmpScope(derivationList.at(i));
		tmpScope = DoReplaceMacros(tmpScope);

		// try the external database for match
		m_workspaceDatabase->GetTagsByScope(derivationList.at(i), tags);
	}

	// and finally sort the results
	std::sort(tags.begin(), tags.end(), SAscendingSort());

}

bool TagsManager::WordCompletionCandidates(const wxFileName &fileName, int lineno, const wxString& expr, const wxString& text, const wxString &word, std::vector<TagEntryPtr> &candidates)
{
	PERF_START("WordCompletionCandidates");

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
	wxString scopeName = GetLanguage()->GetScopeName(text, &additionlScopes);

	if( GetCtagsOptions().GetFlags() &  CC_DEEP_SCAN_USING_NAMESPACE_RESOLVING ) {
		// Do a deep scan for 'using namespace'
		GetLanguage()->SetAdditionalScopes(additionlScopes, fileName.GetFullPath());
		additionlScopes = GetLanguage()->GetAdditionalScopes();
	}

	TagEntryPtr funcTag = FunctionFromFileLine(fileName, lineno);
	if (funcTag) {
		funcSig = funcTag->GetSignature();
	}

	wxString oper;
	wxString tmpExp(expression);
	tmpExp.Trim().Trim(false);

	if ( tmpExp.IsEmpty() ) {
		// Collect all the tags from the current scope, and
		// from the global scope
		scope = GetLanguage()->OptimizeScope(text);
		std::vector<TagEntryPtr> tmpCandidates;
		GetGlobalTags     (word, tmpCandidates);
		GetLocalTags      (word, scope,   tmpCandidates, PartialMatch | IgnoreCaseSensitive);
		GetLocalTags      (word, funcSig, tmpCandidates, PartialMatch | IgnoreCaseSensitive);
		TagsByScopeAndName(scopeName, word, tmpCandidates);

		for (size_t i=0; i<additionlScopes.size(); i++) {
			TagsByScopeAndName(additionlScopes.at(i), word, tmpCandidates);
		}

		DoFilterDuplicatesByTagID  (tmpCandidates, candidates);
		DoFilterDuplicatesBySignature(candidates, candidates);

	} else if( tmpExp == wxT("::") ) {
		// Global scope only
		// e.g.: ::My <CTRL>+<SPACE>
		// Collect all tags from the global scope which starts with 'My' (i.e. 'word')
		std::vector<TagEntryPtr> tmpCandidates;
		GetGlobalTags     (word, tmpCandidates);
		DoFilterDuplicatesByTagID  (tmpCandidates, candidates);
		DoFilterDuplicatesBySignature(candidates, candidates);

	} else {
		wxString typeName, typeScope, dummy;
		bool res = ProcessExpression(fileName, lineno, expression, text, typeName, typeScope, oper, dummy);
		if (!res) {
			PERF_END();
			return false;
		}

		//get all symbols realted to this scope
		scope = wxT("");
		if (typeScope == wxT("<global>"))
			scope << typeName;
		else
			scope << typeScope << wxT("::") << typeName;

		std::vector<TagEntryPtr> tmpCandidates, tmpCandidates1;
		TagsByScope(scope, tmpCandidates);

		wxString partialName(word);
		partialName.MakeLower();

		if(partialName.IsEmpty() == false)
		{
			for(size_t i=0; i<tmpCandidates.size(); i++) {
				wxString nm = tmpCandidates[i]->GetName();
				nm.MakeLower();
				if(nm.StartsWith(partialName)) {
					tmpCandidates1.push_back( tmpCandidates.at(i) );
				}
			}
			DoFilterDuplicatesByTagID(tmpCandidates1, candidates);
			DoFilterDuplicatesBySignature(candidates, candidates);
		}
		else
		{
			DoFilterDuplicatesByTagID(tmpCandidates, candidates);
			DoFilterDuplicatesBySignature(candidates, candidates);
		}
	}

	PERF_END();
	return true;
}

bool TagsManager::AutoCompleteCandidates(const wxFileName &fileName, int lineno, const wxString& expr, const wxString& text, std::vector<TagEntryPtr>& candidates)
{
	PERF_START("AutoCompleteCandidates");

	candidates.clear();
	wxString path;
	wxString typeName, typeScope;

	wxString expression(expr);
	static wxString trimLeftString(wxT("{};\r\n\t\v "));
	static wxString trimRightString(wxT("({};\r\n\t\v "));
	expression.erase(0, expression.find_first_not_of(trimLeftString));
	expression.erase(expression.find_last_not_of(trimRightString)+1);
	wxString oper;
	wxString scopeTeamplateInitList;
	bool     isGlobalScopeOperator(false);

	if( expression == wxT("::") ) {
		// global scope
		isGlobalScopeOperator = true;

	} else {

		PERF_BLOCK("ProcessExpression") {
			bool res = ProcessExpression(fileName, lineno, expression, text, typeName, typeScope, oper, scopeTeamplateInitList);
			if (!res) {
				PERF_END();
				wxLogMessage(wxString::Format(wxT("Failed to resolve %s"), expression.c_str()));
				return false;
			}
		}
	}
	// Load all tags from the database that matches typeName & typeScope
	wxString scope;
	if (typeScope == wxT("<global>"))
		scope << typeName;
	else
		scope << typeScope << wxT("::") << typeName;

	//this function will retrieve the ineherited tags as well
	//incase the last operator used was '::', retrieve all kinds of tags. Otherwise (-> , . operators were used)
	//retrieve only the members/prototypes/functions/enums
	wxArrayString filter;

	if ( isGlobalScopeOperator ) {
		// Fetch all tags from the global scope
		GetDatabase()->GetGlobalFunctions(candidates);

		if(candidates.empty() == false)
			std::sort(candidates.begin(), candidates.end(), SAscendingSort());

	} else if (oper == wxT("::")) {

		filter.Add(wxT("function"));
		filter.Add(wxT("member"));
		filter.Add(wxT("prototype"));
		filter.Add(wxT("typedef"));
		filter.Add(wxT("enum"));
		filter.Add(wxT("enumerator"));
		filter.Add(wxT("union"));
		filter.Add(wxT("class"));
		filter.Add(wxT("struct"));
		filter.Add(wxT("namespace"));

		PERF_BLOCK("TagsByScope") {
			TagsByScope(scope, filter, candidates, true);
		}

	} else {

		filter.Add(wxT("function"));
		filter.Add(wxT("member"));
		filter.Add(wxT("prototype"));
		PERF_BLOCK("TagsByScope") {
			TagsByScope(scope, filter, candidates, true);
		}
	}

	PERF_END();
	return candidates.empty() == false;
}

void TagsManager::DoFilterDuplicatesBySignature(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target)
{
	// filter out all entries with the same signature (we do keep declaration overa an implenetation
	// since usually the declaration contains more useful information)
	std::map<wxString, TagEntryPtr> others, impls;

	for (size_t i=0; i<src.size(); i++) {
		const TagEntryPtr& t = src.at(i);
		if(t->IsMethod()) {
			wxString strippedSignature = NormalizeFunctionSig(t->GetSignature(), 0);
			strippedSignature.Prepend( t->GetName() );

			if(t->IsPrototype()) {
				// keep declaration in the output map
				others[strippedSignature] = t;
			} else {
				// keep the signature in a different map
				impls[strippedSignature] = t;
			}
		} else {
			// keep all other entries
			others[t->GetName()] = t;
		}

	}

	// unified the two multimaps
	std::map<wxString, TagEntryPtr>::iterator iter = impls.begin();
	for(; iter != impls.end(); iter++) {
		if(others.find(iter->first) == others.end()) {
			others[iter->first] = iter->second;
		}
	}

	target.clear();
	// convert the map into vector
	iter = others.begin();
	for(; iter != others.end(); iter++) {
		target.push_back(iter->second);
	}
}

void TagsManager::DoFilterDuplicatesByTagID(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target)
{
	std::map<int, TagEntryPtr> mapTags;

	for (size_t i=0; i<src.size(); i++) {
		const TagEntryPtr& t = src.at(i);
		int tagId = t->GetId();
		if(mapTags.find(tagId) == mapTags.end()) {
			mapTags[tagId] = t;
		} else {
			tagId = -1;
		}
	}

	std::map<int, TagEntryPtr>::iterator iter = mapTags.begin();
	for(; iter != mapTags.end(); iter++) {
		target.push_back( iter->second );
	}
}

void TagsManager::RemoveDuplicatesTips(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target)
{
	std::map<wxString, TagEntryPtr> unique_tags;

	for (size_t i=0; i<src.size(); i++) {

		wxString raw_sig ( src.at(i)->GetSignature().Trim().Trim(false) );
		wxString sig;
		if (raw_sig.empty() == false) {
			sig = NormalizeFunctionSig(raw_sig, 0);
		}

		// the signature that we want to keep is one with name & default values, so try and get the maximum out of the
		// function signature
		bool hasDefaultValues = (raw_sig.Find(wxT("=")) != wxNOT_FOUND);

		wxString name = src.at(i)->GetName();
		wxString key = name + sig;

		std::map<wxString, TagEntryPtr>::iterator iter = unique_tags.find(key);
		if (iter == unique_tags.end()) {
			// does not exist
			unique_tags[key] = src.at(i);
		} else {
			// an entry with this key already exist
			if (hasDefaultValues) {
				// this entry has a default values, it means that we probably prefer this signature over the other
				TagEntryPtr t = iter->second;
				t->SetSignature(raw_sig);
				unique_tags[key] = t;
			}
		}
	}

	// conver the map back to std::vector
	std::map<wxString, TagEntryPtr>::iterator iter = unique_tags.begin();
	target.clear();

	for (; iter != unique_tags.end(); iter++) {
		target.push_back(iter->second);
	}
}

void TagsManager::GetGlobalTags(const wxString &name, std::vector<TagEntryPtr> &tags, size_t flags)
{
	// Make enough room for max of 500 elements in the vector
	tags.reserve(500);
	m_workspaceDatabase->GetTagsByScopeAndName(wxT("<global>"), name, flags & PartialMatch, tags);
	std::sort(tags.begin(), tags.end(), SAscendingSort());
}

void TagsManager::GetLocalTags(const wxString &name, const wxString &scope, std::vector<TagEntryPtr> &tags, size_t flags)
{
	//collect tags from the current scope text
	GetLanguage()->GetLocalVariables(scope, tags, name, flags);
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

	wxString scope = GetLanguage()->OptimizeScope(text);
	wxString scopeName = GetLanguage()->GetScopeName(scope, NULL);
	if (expression.IsEmpty()) {
		//collect all the tags from the current scope, and
		//from the global scope

		GetGlobalTags(word, tmpCandidates, ExactMatch);
		GetLocalTags(word, scope, tmpCandidates, ExactMatch);
		TagsByScopeAndName(scopeName, word, tmpCandidates);
		RemoveDuplicatesTips(tmpCandidates, candidates);

		// we now have a list of tags that matches our word
		TipsFromTags(candidates, word, tips);
	} else {
		wxString typeName, typeScope;
		wxString oper, dummy;
		bool res = ProcessExpression(fileName, lineno, expression, text, typeName, typeScope, oper, dummy);
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

void TagsManager::FindImplDecl(const wxFileName &fileName, int lineno, const wxString & expr, const wxString &word, const wxString & text, std::vector<TagEntryPtr> &tags, bool imp, bool workspaceOnly)
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

	wxString scope(text);// = GetLanguage()->GetScope(text);
	wxString scopeName = GetLanguage()->GetScopeName(scope, NULL);
	if (expression.IsEmpty()) {

		TagsByScopeAndName(scopeName, word, tmpCandidates, ExactMatch);
		if (tmpCandidates.empty()) {
			// no match in the given scope, try to collect from global scope as well
			GetGlobalTags(word, tmpCandidates, ExactMatch);
		}

		if (!imp) {
			//collect only implementation
			FilterImplementation(tmpCandidates, tags);
		} else {
			FilterDeclarations(tmpCandidates, tags);
		}

	} else {

		wxString typeName, typeScope;
		wxString oper, dummy;
		bool res = ProcessExpression(fileName, lineno, expression, text, typeName, typeScope, oper, dummy);
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
	wxString path;
	wxString typeName, typeScope, tmp;
	std::vector<TagEntryPtr> tips;

	// Skip any C++ keywords
	if ( m_CppIgnoreKeyWords.find(word) != m_CppIgnoreKeyWords.end() ) {
		return NULL;
	}

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

	expression.Trim().Trim(false);
	if (expression.IsEmpty()) {
		DoGetFunctionTipForEmptyExpression(word, text, tips);

		if(tips.empty()) {
			// no luck yet
			// we now try this:
			// Perhaps our "function" is actually a constuctor, e.g.:
			// ClassName cls(
			wxString alteredText ( text );
			alteredText.Append(wxT(";"));
			std::vector<TagEntryPtr> tmpCandidates;
			GetLocalTags(word, text, tmpCandidates, ExactMatch);
			if( tmpCandidates.size() == 1) {
				TagEntryPtr t = tmpCandidates.at(0);
				DoGetFunctionTipForEmptyExpression(t->GetScope(), text, tips);
			}
		}
	} else if( expression == wxT("::") ) {
		// Test the global scope
		DoGetFunctionTipForEmptyExpression(word, text, tips, true);

	} else {
		wxString oper, dummy;
		bool res = ProcessExpression(fileName, lineno, expression, text, typeName, typeScope, oper, dummy);
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
	wxArrayString kinds;
	kinds.Add(wxT("class"));
	kinds.Add(wxT("namespace"));
	kinds.Add(wxT("struct"));
	kinds.Add(wxT("union"));
	kinds.Add(wxT("enum"));
	kinds.Add(wxT("typedef"));

	m_workspaceDatabase->GetTagsByKind(kinds, wxT("name"), ITagsStorage::OrderDesc, tags);
}

void TagsManager::FindSymbol(const wxString& name, std::vector<TagEntryPtr> &tags)
{
	m_workspaceDatabase->GetTagsByScopeAndName(wxEmptyString, name, false, tags);
}

void TagsManager::DeleteFilesTags(const wxArrayString &files)
{
	std::vector<wxFileName> files_;
	for (size_t i=0; i<files.GetCount(); i++) {
		files_.push_back(files.Item(i));
	}
	DeleteFilesTags(files_);
}

void TagsManager::DeleteFilesTags(const std::vector<wxFileName> &projectFiles)
{
	if (projectFiles.empty()) {
		return;
	}

	wxArrayString file_array;

	m_workspaceDatabase->Begin();

	for (size_t i=0; i<projectFiles.size(); i++) {
		m_workspaceDatabase->DeleteByFileName(wxFileName(), projectFiles.at(i).GetFullPath(), false);
		file_array.Add(projectFiles.at(i).GetFullPath());
	}
	m_workspaceDatabase->DeleteFromFiles(file_array);
	m_workspaceDatabase->Commit();
	UpdateFileTree(projectFiles, false);
}

void TagsManager::RetagFiles(const std::vector<wxFileName> &files, bool quickRetag)
{
	wxArrayString strFiles;
	// step 1: remove all non-tags files
	for (size_t i=0; i<files.size(); i++) {
		if (!IsValidCtagsFile(files.at(i).GetFullPath())) {
			continue;
		}

		strFiles.Add(files.at(i).GetFullPath());
	}

	if(strFiles.IsEmpty())
		return;

	// step 2: remove all files which do not need retag
	if ( quickRetag )
		DoFilterNonNeededFilesForRetaging(strFiles, m_workspaceDatabase);

	if (strFiles.IsEmpty()) {
		wxFrame *frame = dynamic_cast<wxFrame*>( wxTheApp->GetTopWindow() );
		if (frame) {
			frame->SetStatusText(wxT("All files are up-to-date"), 0);
		}
		return;
	}

	// step 4: Remove tags belonging to these files
	DeleteFilesTags(strFiles);

	// step 5: build the database

#if USE_PARSER_TREAD_FOR_RETAGGING_WORKSPACE

	// Expermintal code: perform the 'retag workspace' using the parser thread
	// Put a request to the parsing thread
	ParseRequest *req = new ParseRequest();
	req->setDbFile( GetDatabase()->GetDatabaseFileName().GetFullPath().c_str() );
	req->setType  ( ParseRequest::PR_PARSE_AND_STORE );

	req->_workspaceFiles.clear();
	req->_workspaceFiles.reserve( strFiles.size() );
	for(size_t i=0; i<strFiles.GetCount(); i++) {
		req->_workspaceFiles.push_back( strFiles[i].mb_str(wxConvUTF8).data() );
	}
	ParseThreadST::Get()->Add ( req );

#else
	// step 5: build the database
	DoBuildDatabase(strFiles, *m_workspaceDatabase);
	UpdateFileTree(m_workspaceDatabase, true);

#endif
}

bool TagsManager::DoBuildDatabase(const wxArrayString &files, ITagsStorage &db, const wxString *rootPath)
{
	wxString tags;

	int maxVal = (int)files.GetCount();
	if (files.IsEmpty()) {
		return false;
	}

	// Create a progress dialog
	MyProgress prgDlg(wxT("Building tags database ..."), files.GetCount());
	prgDlg.Update(0, wxT("Parsing..."));

	// We commit every 10 files
	db.Begin();
	for (int i=0; i<maxVal; i++) {
		wxString fileTags;
		wxFileName curFile(files.Item(i));

		// if the cached file is being re-tagged, clear the cache
		if (IsFileCached(curFile.GetFullPath())) {
			ClearCachedFile(curFile.GetFullPath());
		}

		// update the progress bar
		if (!prgDlg.Update(i, wxString::Format(wxT("Parsing : %s"), curFile.GetFullName().c_str()))) {
			prgDlg.Destroy();
			return false;
		}

		tags.Clear();
		tagParseResult parsing_result;

		parsing_result.fileName = curFile.GetFullName();
		parsing_result.tree = ParseSourceFile(curFile);

		db.Store(parsing_result.tree, wxFileName(), false);
		if(db.InsertFileEntry(curFile.GetFullPath(), (int)time(NULL)) == TagExist) {
			db.UpdateFileEntry(curFile.GetFullPath(), (int)time(NULL));
		}

		if ( i % 50 == 0 ) {
			// update the progress bar
			if (!prgDlg.Update(i, wxT("Committing..."))) {
				prgDlg.Destroy();
				return false;
			}

			// Commit what we got so far
			db.Commit();

			// Start a new transaction
			db.Begin();
		}
	}

	// Commit whats left
	db.Commit();
	return true;
}

void TagsManager::FindByNameAndScope(const wxString &name, const wxString &scope, std::vector<TagEntryPtr> &tags)
{
	wxString _name  = DoReplaceMacros(name);
	wxString _scope = DoReplaceMacros(scope);
	DoFindByNameAndScope(_name, _scope, tags);

	// Sort the results base on their name
	std::sort(tags.begin(), tags.end(), SAscendingSort());
}

void TagsManager::FindByPath(const wxString &path, std::vector<TagEntryPtr> &tags)
{
	m_workspaceDatabase->GetTagsByPath(path, tags);
}

void TagsManager::DoFindByNameAndScope(const wxString &name, const wxString &scope, std::vector<TagEntryPtr> &tags)
{
	wxString sql;
	if (scope == wxT("<global>")) {
		// try the workspace database for match
		m_workspaceDatabase->GetTagsByNameAndParent(name, wxT("<global>"), tags);
	} else {
		std::vector<wxString> derivationList;
		derivationList.push_back(scope);
		GetDerivationList(scope, derivationList);
		wxArrayString paths;
		for (size_t i=0; i<derivationList.size(); i++) {
			wxString path_;
			path_ << derivationList.at(i) << wxT("::") << name ;
			paths.Add(path_);
		}

		// try the workspace database for match
		m_workspaceDatabase->GetTagsByPath(paths, tags);
	}
}

bool TagsManager::IsTypeAndScopeContainer(wxString& typeName, wxString& scope)
{
	wxString cacheKey;
	cacheKey << typeName << wxT("@") << scope;

	//we search the cache first, note that the cache
	//is used only for the external database
	std::map<wxString, bool>::iterator iter = m_typeScopeContainerCache.find(cacheKey);
	if (iter != m_typeScopeContainerCache.end()) {
		return iter->second;
	}

	// replace macros:
	// replace the provided typeName and scope with user defined macros as appeared in the PreprocessorMap
	wxString _typeName = DoReplaceMacros(typeName);
	wxString _scope    = DoReplaceMacros(scope);

	bool res = m_workspaceDatabase->IsTypeAndScopeContainer(_typeName, _scope);
	if(res) {
		typeName = _typeName;
		scope    = _scope;
	}
	return res;
}

bool TagsManager::IsTypeAndScopeExists(wxString &typeName, wxString &scope)
{
	wxString cacheKey;
	cacheKey << typeName << wxT("@") << scope;

	//we search the cache first, note that the cache
	//is used only for the external database
	std::map<wxString, bool>::iterator iter = m_typeScopeCache.find(cacheKey);
	if (iter != m_typeScopeCache.end()) {
		return iter->second;
	}

	// First try the fast query to save some time
	if(m_workspaceDatabase->IsTypeAndScopeExistLimitOne(typeName, scope)) {
		//wxLogMessage(wxT("Type Exist! Skipping!"));
		return true;
	}
	// replace macros:
	// replace the provided typeName and scope with user defined macros as appeared in the PreprocessorMap
	typeName = DoReplaceMacros(typeName);
	scope    = DoReplaceMacros(scope);

	return m_workspaceDatabase->IsTypeAndScopeExist(typeName, scope);
}

bool TagsManager::GetDerivationList(const wxString &path, std::vector<wxString> &derivationList)
{
	std::vector<TagEntryPtr> tags;
	TagEntryPtr tag;

	wxArrayString kind;
	kind.Add(wxT("class"));
	kind.Add(wxT("struct"));

	m_workspaceDatabase->GetTagsByKindAndPath(kind, path, tags);

	if (tags.size() == 1) {
		tag = tags.at(0);
	} else {
		return false;
	}

	if (tag && tag->IsOk()) {
		wxArrayString ineheritsList = tag->GetInheritsAsArrayNoTemplates();
		for(size_t i=0; i<ineheritsList.GetCount(); i++) {
			wxString inherits = ineheritsList.Item(i);
			wxString tagName  = tag->GetName();
			wxString tmpInhr  = inherits;

			tagName.MakeLower();
			tmpInhr.MakeLower();

			// Make sure that inherits != the current name or we will end up in an infinite loop
			if(tmpInhr != tagName) {
				wxString possibleScope(wxT("<global>"));
				// Correc the type/scope
				IsTypeAndScopeExists(inherits, possibleScope);

				if (possibleScope != wxT("<global>")) {
					inherits = possibleScope + wxT("::") + inherits;
				}

				derivationList.push_back(inherits);
				GetDerivationList(inherits, derivationList);
			}
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
		while (tip.Replace(wxT("  "), wxT(" "))) {}

		// create a proper tooltip from the stripped pattern
		TagEntryPtr t= tags.at(i);
		if (t->IsMethod()) {

			// add return value
			tip.Clear();

			wxString ret_value = GetFunctionReturnValueFromPattern(t);
			if(ret_value.IsEmpty() == false) {
				tip << ret_value << wxT(" ");

			} else {
				wxString retValue = t->GetReturnValue();
				if(retValue.IsEmpty() == false) {
					tip << retValue << wxT(" ");
				}

			}
			// add the scope
			if (!t->IsScopeGlobal()) {
				tip << t->GetScope() << wxT("::");
			}

			// name
			tip << t->GetName();

			// method signature
			tip << NormalizeFunctionSig(t->GetSignature(), Normalize_Func_Name | Normalize_Func_Default_value);
		}

		// remove any extra spaces from the tip
		while (tip.Replace(wxT("  "), wxT(" "))) {}

		// prepend any comment if exists
		tips.push_back(tip);
	}
}

void TagsManager::GetFunctionTipFromTags(const std::vector<TagEntryPtr> &tags, const wxString &word, std::vector<TagEntryPtr> &tips)
{
	std::map<wxString, TagEntryPtr> tipsMap;
	std::vector<TagEntryPtr> ctor_tags;

	for (size_t i=0; i<tags.size(); i++) {
		if (tags.at(i)->GetName() != word)
			continue;

		TagEntryPtr t = tags.at(i);
		wxString pat = t->GetPattern();

		if ( t->IsMethod() ) {
			wxString tip;
			tip << wxT("function:") << t->GetSignature();

			// collect each signature only once, we do this by using
			// map
			tipsMap[tip] = t;

		} else if (t->IsClass()) {

			// this tag is a class declaration that matches the word
			// user is probably is typing something like
			// Class *a = new Class(
			// or even Class a = Class(
			// the steps to take from here:
			// - lookup in the tables for tags that matches path of: WordScope::Word::Word and of type prototype/function
			wxString scope;
			if ( t->GetScope().IsEmpty() == false && t->GetScope() != wxT("<global>") ) {
				scope << t->GetScope() << wxT("::");
			}

			scope << t->GetName();
			ctor_tags.clear();
			TagsByScopeAndName(scope, t->GetName(), ctor_tags, ExactMatch);

			for (size_t i=0; i<ctor_tags.size(); i++) {
				TagEntryPtr ctor_tag = ctor_tags.at(i);
				if ( ctor_tag->IsMethod() ) {
					wxString tip;
					tip << wxT("function:") << ctor_tag->GetSignature();
					tipsMap[ctor_tag->GetSignature()] = ctor_tag;
				}
			}

		} else if (t->IsMacro()) {

			wxString tip;
			wxString macroName = t->GetName();
			wxString pattern = t->GetPattern();

			int where = pattern.Find(macroName);
			if (where != wxNOT_FOUND) {
				//remove the #define <name> from the pattern
				pattern = pattern.Mid(where + macroName.Length());
				pattern = pattern.Trim().Trim(false);
				if (pattern.StartsWith(wxT("("))) {
					//this macro has the form of a function
					pattern = pattern.BeforeFirst(wxT(')'));
					pattern.Append(wxT(')'));

					tip << wxT("macro:") << pattern;

					//collect each signature only once, we do this by using
					//map
					tipsMap[tip] = t;
				}
			}
		}
	}

	for (std::map<wxString, TagEntryPtr>::iterator iter = tipsMap.begin(); iter != tipsMap.end(); iter++) {
		tips.push_back(iter->second);
	}
}

void TagsManager::CloseDatabase()
{
	if (m_workspaceDatabase) {
		UpdateFileTree(m_workspaceDatabase, false);
		delete m_workspaceDatabase;
		m_workspaceDatabase = new TagsStorageSQLite( );
		m_workspaceDatabase->SetSingleSearchLimit( MAX_SEARCH_LIMIT );
	}
}

DoxygenComment TagsManager::GenerateDoxygenComment(const wxString &file, const int line, wxChar keyPrefix)
{
	if (m_workspaceDatabase->IsOpen()) {
		std::vector<TagEntryPtr> tags;

		m_workspaceDatabase->GetTagsByFileAndLine(file, line + 1, tags );

		if (tags.empty() || tags.size() > 1)
			return DoxygenComment();

		//create a doxygen comment from the tag
		return DoCreateDoxygenComment(tags.at(0), keyPrefix);
	}
	return DoxygenComment();
}

DoxygenComment TagsManager::DoCreateDoxygenComment(TagEntryPtr tag, wxChar keyPrefix)
{
	CppCommentCreator commentCreator(tag, keyPrefix);
	DoxygenComment dc;
	dc.comment = commentCreator.CreateComment();
	dc.name = tag->GetName();
	return dc;
}

bool TagsManager::GetParseComments()
{
	//wxCriticalSectionLocker lock(m_cs);
	return m_parseComments;
}

void TagsManager::SetCtagsOptions(const TagsOptionsData &options)
{
	m_tagsOptions = options;
	RestartCtagsProcess();

	wxCriticalSectionLocker locker(m_cs);
	m_parseComments = m_tagsOptions.GetFlags() & CC_PARSE_COMMENTS ? true : false;
}

void TagsManager::GenerateSettersGetters(const wxString &scope, const SettersGettersData &data, const std::vector<TagEntryPtr> &tags, wxString &impl, wxString *decl)
{
	wxUnusedVar(scope);
	wxUnusedVar(data);
	wxUnusedVar(tags);
	wxUnusedVar(impl);
	wxUnusedVar(decl);
}

void TagsManager::TagsByScope(const wxString &scopeName, const wxString &kind, std::vector<TagEntryPtr> &tags, bool includeInherits, bool onlyWorkspace)
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
	wxArrayString kinds, scopes;
	kinds.Add(kind);

	for (size_t i=0; i<derivationList.size(); i++) {
		scopes.Add(derivationList.at(i));
	}

	m_workspaceDatabase->GetTagsByScopesAndKind(scopes, kinds, tags);
	// and finally sort the results
	std::sort(tags.begin(), tags.end(), SAscendingSort());
}

wxString TagsManager::GetScopeName(const wxString &scope)
{
	Language *lang = GetLanguage();
	return lang->GetScopeName(scope, NULL);
}

bool TagsManager::ProcessExpression(const wxFileName &filename, int lineno, const wxString &expr, const wxString &scopeText, wxString &typeName, wxString &typeScope, wxString &oper, wxString &scopeTempalteInitiList)
{
	bool res = GetLanguage()->ProcessExpression(expr, scopeText, filename, lineno, typeName, typeScope, oper, scopeTempalteInitiList);
//	if (res && IsTypeAndScopeExists(typeName, typeScope) == false && scopeTempalteInitiList.empty() == false) {
//		// try to resolve it again
//		res = GetLanguage()->ResolveTemplate(typeName, typeScope, typeScope, scopeTempalteInitiList);
//	}
	return res;
}

bool TagsManager::GetMemberType(const wxString &scope, const wxString &name, wxString &type, wxString &typeScope)
{
	wxString expression(scope);
	expression << wxT("::") << name << wxT(".");
	wxString dummy;
	return GetLanguage()->ProcessExpression(expression, wxEmptyString, wxFileName(), wxNOT_FOUND, type, typeScope, dummy, dummy);
}

void TagsManager::GetFiles(const wxString &partialName, std::vector<FileEntryPtr> &files)
{
	if (m_workspaceDatabase) {
		m_workspaceDatabase->GetFiles(partialName, files);
	}
}

void TagsManager::GetFiles(const wxString &partialName, std::vector<wxFileName> &files)
{
	std::vector<FileEntryPtr> f;
	GetFiles(partialName, f);

	for (size_t i=0; i<f.size(); i++) {
		files.push_back( wxFileName(f.at(i)->GetFile()) );
	}
}

TagEntryPtr TagsManager::FunctionFromFileLine(const wxFileName &fileName, int lineno, bool nextFunction /*false*/)
{
	if (!m_workspaceDatabase) {
		return NULL;
	}

	if (!IsFileCached(fileName.GetFullPath())) {
		CacheFile(fileName.GetFullPath());
	}

	TagEntryPtr foo = NULL;
	for (size_t i=0; i<m_cachedFileFunctionsTags.size(); i++) {
		TagEntryPtr t = m_cachedFileFunctionsTags.at(i);

		if (nextFunction && t->GetLine() > lineno) {
			// keep the last non matched method
			foo = t;
		} else if (t->GetLine() <= lineno) {
			if (nextFunction ) {
				return foo;
			} else {
				return t;
			}
		}
	}
	return NULL;
}

void TagsManager::GetScopesFromFile(const wxFileName &fileName, std::vector< wxString > &scopes)
{
	if (!m_workspaceDatabase) {
		return;
	}

	m_workspaceDatabase->GetScopesFromFileAsc(fileName, scopes);
}

void TagsManager::TagsFromFileAndScope(const wxFileName& fileName, const wxString &scopeName, std::vector< TagEntryPtr > &tags)
{
	if (!m_workspaceDatabase) {
		return;
	}

	wxArrayString kind;
	kind.Add(wxT("function"));
	kind.Add(wxT("prototype"));
	kind.Add(wxT("enum"));

	m_workspaceDatabase->GetTagsByFileScopeAndKind(fileName, scopeName, kind, tags);
	std::sort(tags.begin(), tags.end(), SAscendingSort());
}

bool TagsManager::GetFunctionDetails(const wxFileName &fileName, int lineno, TagEntryPtr &tag, clFunction &func)
{
	tag = FunctionFromFileLine(fileName, lineno);
	if (tag) {
		GetLanguage()->FunctionFromPattern( tag, func );
		return true;
	}
	return false;
}

TagEntryPtr TagsManager::FirstFunctionOfFile(const wxFileName &fileName)
{
	if (!m_workspaceDatabase) {
		return NULL;
	}

	std::vector<TagEntryPtr> tags;
	wxArrayString            kind;
	kind.Add(wxT("function"));
	m_workspaceDatabase->GetTagsByKindAndFile(kind, fileName.GetFullPath(), wxT("line"), ITagsStorage::OrderAsc, tags);

	if ( tags.empty() ) return NULL;
	return tags.at(0);
}

TagEntryPtr TagsManager::FirstScopeOfFile(const wxFileName &fileName)
{
	if (!m_workspaceDatabase) {
		return NULL;
	}
	std::vector<TagEntryPtr> tags;
	wxArrayString            kind;
	kind.Add(wxT("struct"));
	kind.Add(wxT("class"));
	kind.Add(wxT("namespace"));
	m_workspaceDatabase->GetTagsByKindAndFile(kind, fileName.GetFullPath(), wxT("line"), ITagsStorage::OrderAsc, tags);

	if ( tags.empty() ) return NULL;
	return tags.at(0);
}

wxString TagsManager::FormatFunction(TagEntryPtr tag, size_t flags, const wxString &scope)
{
	clFunction foo;
	if (!GetLanguage()->FunctionFromPattern(tag, foo)) {
		return wxEmptyString;
	}

	wxString body;
	// add virtual keyword to declarations only && if the flags is set
	if (foo.m_isVirtual && (flags & FunctionFormat_WithVirtual) && !(flags & FunctionFormat_Impl)) {
		body << wxT("virtual ");
	}

	wxString ret_value = GetFunctionReturnValueFromPattern(tag);
	if(ret_value.IsEmpty() == false) {
		body << ret_value << wxT(" ");

	} else {
		wxString retValue = tag->GetReturnValue();
		if(retValue.IsEmpty() == false) {
			body << retValue << wxT(" ");
		}

	}

	if (flags & FunctionFormat_Impl) {
		if (scope.IsEmpty()) {
			if (tag->GetScope() != wxT("<global>")) {
				body << tag->GetScope() << wxT("::");
			}
		} else {
			body << scope << wxT("::");
		}
	}

	// Build the flags required by the NormalizeFunctionSig() method
	size_t tmpFlags(0);
	if ( flags & FunctionFormat_Impl ) {
		tmpFlags |= Normalize_Func_Name | Normalize_Func_Reverse_Macro;
	} else {
		tmpFlags |= Normalize_Func_Name | Normalize_Func_Reverse_Macro | Normalize_Func_Default_value;
	}

	if(flags & FunctionFormat_Arg_Per_Line)
		tmpFlags |= Normalize_Func_Arg_Per_Line;

	if(flags & FunctionFormat_Arg_Per_Line)
		body << wxT("\n");

	body << tag->GetName();
	body << NormalizeFunctionSig( tag->GetSignature(), tmpFlags);

	if ( foo.m_isConst ) {
		body << wxT(" const");
	}

	if (flags & FunctionFormat_Impl) {
		body << wxT("\n{\n}\n");
	} else {
		body << wxT(";\n");
	}

	// convert \t to spaces
	body.Replace(wxT("\t"), wxT(" "));

	// remove any extra spaces from the tip
	while (body.Replace(wxT("  "), wxT(" "))) {}
	return body;
}

bool TagsManager::IsPureVirtual(TagEntryPtr tag)
{
	clFunction foo;
	if (!GetLanguage()->FunctionFromPattern(tag, foo)) {
		return false;
	}
	return foo.m_isPureVirtual;
}

bool TagsManager::IsVirtual(TagEntryPtr tag)
{
	clFunction foo;
	if (!GetLanguage()->FunctionFromPattern(tag, foo)) {
		return false;
	}
	return foo.m_isVirtual;
}
void TagsManager::SetLanguage(Language *lang)
{
	m_lang = lang;
}

Language* TagsManager::GetLanguage()
{
	if ( !m_lang ) {
		//for backward compatibility allows access to the tags manager using
		//the singleton call
		return LanguageST::Get();
	} else {
		return m_lang;
	}
}

bool TagsManager::ProcessExpression(const wxString &expression, wxString &type, wxString &typeScope)
{
	wxString oper, dummy;
	return ProcessExpression(wxFileName(), wxNOT_FOUND, expression, wxEmptyString, type, typeScope, oper, dummy);
}

void TagsManager::GetClasses(std::vector< TagEntryPtr > &tags, bool onlyWorkspace)
{
	wxArrayString kind;
	kind.Add(wxT("class"));
	kind.Add(wxT("struct"));
	kind.Add(wxT("union"));

	m_workspaceDatabase->GetTagsByKind(kind, wxT("name"), ITagsStorage::OrderAsc, tags);
}

void TagsManager::StripComments(const wxString &text, wxString &stippedText)
{
	CppScanner scanner;
	scanner.SetText( _C(text) );

	bool changedLine = false;
	bool prepLine = false;
	int curline = 0;

	while (true) {
		int type = scanner.yylex();
		if (type == 0) {
			break;
		}

		// eat up all tokens until next line
		if ( prepLine && scanner.lineno() == curline) {
			continue;
		}

		prepLine = false;

		// Get the current line number, it will help us detect preprocessor lines
		changedLine = (scanner.lineno() > curline);
		if (changedLine) {
			stippedText << wxT("\n");
		}

		curline = scanner.lineno();
		if (type == '#') {
			if (changedLine) {
				// We are at the start of a new line
				// consume everything until new line is found or end of text
				prepLine = true;
				continue;
			}
		}
		stippedText << _U( scanner.YYText() ) << wxT(" ");
	}
}

void TagsManager::GetFunctions(std::vector< TagEntryPtr > &tags, const wxString &fileName , bool onlyWorkspace )
{
	wxArrayString kind;
	kind.Add(wxT("function"));
	kind.Add(wxT("prototype"));
	m_workspaceDatabase->GetTagsByKindAndFile(kind, fileName, wxT("name"), ITagsStorage::OrderAsc, tags);
}

void TagsManager::GetAllTagsNames(wxArrayString &tagsList)
{
	size_t kind = GetCtagsOptions().GetCcColourFlags();
	if (kind == CC_COLOUR_ALL)
	{
		m_workspaceDatabase->GetAllTagsNames(tagsList);
		return;
	}

	wxArrayString kindArr;

	if ( kind & CC_COLOUR_CLASS) {
		kindArr.Add(wxT("class"));
	}
	if ( kind & CC_COLOUR_ENUM) {
		kindArr.Add(wxT("enum"));
	}
	if ( kind & CC_COLOUR_FUNCTION) {
		kindArr.Add(wxT("function"));
	}
	if ( kind & CC_COLOUR_MACRO) {
		kindArr.Add(wxT("macro"));
	}
	if ( kind & CC_COLOUR_NAMESPACE) {
		kindArr.Add(wxT("namespace"));
	}
	if ( kind & CC_COLOUR_PROTOTYPE) {
		kindArr.Add(wxT("prototype"));
	}
	if ( kind & CC_COLOUR_STRUCT) {
		kindArr.Add(wxT("struct"));
	}
	if ( kind & CC_COLOUR_TYPEDEF) {
		kindArr.Add(wxT("typedef"));
	}
	if ( kind & CC_COLOUR_UNION) {
		kindArr.Add(wxT("union"));
	}
	if ( kind & CC_COLOUR_ENUMERATOR) {
		kindArr.Add(wxT("enumerator"));
	}
	if ( kind & CC_COLOUR_VARIABLE) {
		kindArr.Add(wxT("variable"));
	}
	if ( kind & CC_COLOUR_MEMBER) {
		kindArr.Add(wxT("member"));
	}

	if ( kindArr.IsEmpty() ) {
		return;
	}

	m_workspaceDatabase->GetTagsNames(kindArr, tagsList);
}

void TagsManager::TagsByScope(const wxString &scopeName, const wxArrayString &kind, std::vector<TagEntryPtr> &tags, bool include_anon)
{
	wxUnusedVar(include_anon);
	std::vector<wxString> derivationList;

	//add this scope as well to the derivation list
	wxString _scopeName = DoReplaceMacros( scopeName );
	derivationList.push_back(_scopeName);
	GetDerivationList(_scopeName, derivationList);

	//make enough room for max of 500 elements in the vector
	tags.reserve(500);
	wxArrayString scopes;
	for (size_t i=0; i<derivationList.size(); i++) {
		wxString tmpScope(derivationList.at(i));
		tmpScope = DoReplaceMacros(tmpScope);
		scopes.Add(tmpScope);
	}

	m_workspaceDatabase->GetTagsByScopesAndKind(scopes, kind, tags);

	// and finally sort the results
	std::sort(tags.begin(), tags.end(), SAscendingSort());
}

wxString TagsManager::NormalizeFunctionSig(const wxString &sig, size_t flags, std::vector<std::pair<int, int> > *paramLen)
{
	std::map<std::string, std::string> ignoreTokens = GetCtagsOptions().GetTokensMap();
	std::map<std::string, std::string> reverseTokens;

	if(flags & Normalize_Func_Reverse_Macro)
		reverseTokens = GetCtagsOptions().GetTokensReversedMap();

	VariableList li;
	const wxCharBuffer patbuf = _C(sig);

	get_variables(patbuf.data(), li, ignoreTokens, true);

	//construct a function signature from the results
	wxString str_output;
	str_output << wxT("(");

	if (paramLen) {
		paramLen->clear();
	}
	if(flags & Normalize_Func_Arg_Per_Line && li.size()) {
		str_output << wxT("\n    ");
	}

	VariableList::iterator iter = li.begin();
	for ( ; iter != li.end() ; iter++) {
		Variable v = *iter;
		int start_offset = str_output.length();

		//add const qualifier
		if (v.m_isConst) {
			str_output << wxT("const ");
		}

		//add scope
		if (v.m_typeScope.empty() == false) {
			str_output << _U(v.m_typeScope.c_str()) << wxT("::");
		}

		if (v.m_type.empty() == false) {
			if(flags & Normalize_Func_Reverse_Macro) {
				// replace the type if it exists in the map
				std::map<std::string, std::string>::iterator miter = reverseTokens.find(v.m_type);
				if(miter != reverseTokens.end()) {
					v.m_type = miter->second;
				}
			}
			str_output << _U(v.m_type.c_str());
		}

		if (v.m_templateDecl.empty() == false) {
			str_output << _U(v.m_templateDecl.c_str());
		}

		if (v.m_starAmp.empty() == false) {
			str_output << _U(v.m_starAmp.c_str());
		}

		if (v.m_rightSideConst.empty() == false) {
			str_output << wxT(" ") << _U(v.m_rightSideConst.c_str());
		}

		if (v.m_name.empty() == false && (flags & Normalize_Func_Name)) {
			str_output << wxT(" ") << _U(v.m_name.c_str());
		} else if ( v.m_isEllipsis ) {
			str_output << wxT(" ...");
		}

		if (v.m_arrayBrackets.empty() == false) {
			str_output << wxT(" ") << _U(v.m_arrayBrackets.c_str());
		}

		if (v.m_defaultValue.empty() == false && (flags & Normalize_Func_Default_value)) {
			str_output << wxT(" = ") << _U(v.m_defaultValue.c_str());
		}

		// keep the length of this argument
		if (paramLen) {
			paramLen->push_back(std::pair<int, int>(start_offset, str_output.length() - start_offset));
		}
		str_output << wxT(", ");
		if(flags & Normalize_Func_Arg_Per_Line && li.size()) {
			str_output << wxT("\n    ");
		}
	}

	if (li.empty() == false) {
		str_output = str_output.BeforeLast(wxT(','));
	}

	str_output << wxT(")");
	return str_output;
}

void TagsManager::GetUnImplementedFunctions(const wxString& scopeName, std::map<wxString, TagEntryPtr>& protos)
{
	//get list of all prototype functions from the database
	std::vector< TagEntryPtr > vproto;
	std::vector< TagEntryPtr > vimpl;

	//currently we want to add implementation only for workspace classes
	TagsByScope(scopeName, wxT("prototype"), vproto, false, true);
	TagsByScope(scopeName, wxT("function"), vimpl, false, true);

	//filter out functions which already has implementation
	for ( size_t i=0; i < vproto.size() ; i++ ) {
		TagEntryPtr tag = vproto.at(i);
		wxString key = tag->GetName();

		//override the scope to be our scope...
		tag->SetScope( scopeName );

		key << NormalizeFunctionSig( tag->GetSignature(), Normalize_Func_Reverse_Macro );
		protos[key] = tag;
	}

	std::map<std::string, std::string> ignoreTokens = GetCtagsOptions().GetTokensMap();

	// remove functions with implementation
	for ( size_t i=0; i < vimpl.size() ; i++ ) {
		TagEntryPtr tag = vimpl.at(i);
		wxString key = tag->GetName();
		key << NormalizeFunctionSig( tag->GetSignature(), Normalize_Func_Reverse_Macro );
		std::map<wxString, TagEntryPtr>::iterator iter = protos.find(key);

		if ( iter != protos.end() ) {
			protos.erase( iter );
		}
	}

	std::map<wxString, TagEntryPtr> tmpMap( protos );
	std::map<wxString, TagEntryPtr>::iterator it = tmpMap.begin();
	protos.clear();

	// collect only non-pure virtual methods
	for (; it != tmpMap.end() ; it++ ) {
		TagEntryPtr tag = it->second;
		clFunction f;
		if ( GetLanguage()->FunctionFromPattern(tag, f) ) {
			if ( !f.m_isPureVirtual ) {
				// incude this function
				protos[it->first] = it->second;
			}
		} else {
			// parsing failed
			protos[it->first] = it->second;
		}
	}
}

void TagsManager::CacheFile(const wxString& fileName)
{
	if (!m_workspaceDatabase) {
		return;
	}

	m_cachedFile = fileName;
	m_cachedFileFunctionsTags.clear();

	wxArrayString kinds;
	kinds.Add(wxT("function"));
	kinds.Add(wxT("prototype"));
	// disable the cache
	m_workspaceDatabase->SetUseCache(false);
	m_workspaceDatabase->GetTagsByKindAndFile(kinds, fileName, wxT("line"), ITagsStorage::OrderDesc, m_cachedFileFunctionsTags);
	// re-enable it
	m_workspaceDatabase->SetUseCache(true);
}

void TagsManager::ClearCachedFile(const wxString &fileName)
{
	if (fileName == m_cachedFile) {
		m_cachedFile.Clear();
		m_cachedFileFunctionsTags.clear();
	}
}

bool TagsManager::IsFileCached(const wxString& fileName) const
{
	return fileName == m_cachedFile;
}

wxString TagsManager::GetCTagsCmd()
{
	wxString cmd;
	wxString ctagsCmd;
	ctagsCmd << m_tagsOptions.ToString() << m_ctagsCmd;

	// build the command, we surround ctags name with double quatations
	cmd << wxT("\"") << m_codeliteIndexerPath.GetFullPath() << wxT("\"") << ctagsCmd;

	return cmd;
}

wxString TagsManager::DoReplaceMacros(wxString name)
{
	// replace macros:
	// replace the provided typeName and scope with user defined macros as appeared in the PreprocessorMap
	wxString _name(name);

	std::map<wxString, wxString> iTokens = GetCtagsOptions().GetTokensWxMap();
	std::map<wxString, wxString>::iterator it = iTokens.end();

	it = iTokens.find(name);
	if (it != iTokens.end()) {
		if (it->second.empty() == false) {
			_name = it->second;
		}
	}
	return _name;
}

// wrapper function to update the file tree given a list of files.
void TagsManager::UpdateFileTree(const std::vector<wxFileName> &files, bool bold)
{
	if (GetCtagsOptions().GetFlags() & CC_MARK_TAGS_FILES_IN_BOLD) {
		wxCommandEvent e(wxEVT_UPDATE_FILETREE_EVENT);
		e.SetClientData((void*)&files);
		e.SetInt((int)bold);
		ProcessEvent(e);
	}
}

void TagsManager::UpdateFileTree(ITagsStorage *td, bool bold)
{
	if (GetCtagsOptions().GetFlags() & CC_MARK_TAGS_FILES_IN_BOLD) {
		std::vector<FileEntryPtr> files;
		std::vector<wxFileName> file_names;

		td->GetFiles(wxEmptyString, files);
		for (size_t i=0; i<files.size(); i++) {
			file_names.push_back(wxFileName(files.at(i)->GetFile()));
		}
		UpdateFileTree(file_names, bold);
	}
}

void TagsManager::OnUpdateFileTreeEvent(wxCommandEvent& e)
{
	wxUnusedVar(e);
}

void TagsManager::NotifyFileTree(bool bold)
{
	size_t origFlags = GetCtagsOptions().GetFlags();

	// we temporarly set the flag CC_MARK_TAGS_FILES_IN_BOLD
	m_tagsOptions.SetFlags(origFlags | CC_MARK_TAGS_FILES_IN_BOLD);

	if (m_workspaceDatabase && m_workspaceDatabase->IsOpen()) {
		UpdateFileTree(m_workspaceDatabase, bold);
	}

	// restore original flags
	m_tagsOptions.SetFlags(origFlags);
}

void TagsManager::DeleteTagsByFilePrefix(const wxString& dbfileName, const wxString& filePrefix)
{
	ITagsStorage *db = new TagsStorageSQLite();
	db->OpenDatabase(wxFileName(dbfileName));
	db->Begin();

	// delete the tags
	db->DeleteByFilePrefix     (db->GetDatabaseFileName(), filePrefix);

	// deelete the FILES entries
	db->DeleteFromFilesByPrefix(db->GetDatabaseFileName(), filePrefix);
	db->Commit();

	delete db;
}

void TagsManager::UpdateFilesRetagTimestamp(const wxArrayString& files, ITagsStorage* db)
{
	db->Begin();
	for (size_t i=0; i<files.GetCount(); i++) {
		db->InsertFileEntry(files.Item(i), (int)time(NULL));
	}
	db->Commit();
}

void TagsManager::FilterNonNeededFilesForRetaging(wxArrayString& strFiles, ITagsStorage* db)
{
	std::vector<FileEntryPtr> files_entries;
	db->GetFiles(files_entries);
	std::set<wxString> files_set;

	for (size_t i=0; i<strFiles.GetCount(); i++) {
		files_set.insert(strFiles.Item(i));
	}

	for (size_t i=0; i<files_entries.size(); i++) {
		FileEntryPtr fe = files_entries.at(i);

		// does the file exist in both lists?
		std::set<wxString>::iterator iter = files_set.find(fe->GetFile());
		if ( iter != files_set.end() ) {
			// get the actual modifiaction time of the file from the disk
			struct stat buff;
			int modified(0);

			const wxCharBuffer cname = _C((*iter));
			if (stat(cname.data(), &buff) == 0) {
				modified = (int)buff.st_mtime;
			}

			// if the timestamp from the database < then the actual timestamp, re-tag the file
			if (fe->GetLastRetaggedTimestamp() >= modified) {
				files_set.erase(iter);
			}
		}
	}

	// copy back the files to the array
	std::set<wxString>::iterator iter = files_set.begin();
	strFiles.Clear();
	for (; iter != files_set.end(); iter++ ) {
		strFiles.Add( *iter );
	}
}

void TagsManager::DoFilterNonNeededFilesForRetaging(wxArrayString& strFiles, ITagsStorage* db)
{
	FilterNonNeededFilesForRetaging(strFiles, db);
}

wxString TagsManager::GetFunctionReturnValueFromPattern(TagEntryPtr tag)
{
	// evaluate the return value of the tag
	clFunction foo;
	wxString return_value;
	if (GetLanguage()->FunctionFromPattern(tag, foo)) {
		if (foo.m_retrunValusConst.empty() == false) {
			return_value << _U(foo.m_retrunValusConst.c_str()) << wxT(" ");
		}

		if (foo.m_returnValue.m_typeScope.empty() == false) {
			return_value << _U(foo.m_returnValue.m_typeScope.c_str()) << wxT("::");
		}

		if (foo.m_returnValue.m_type.empty() == false) {
			return_value << _U(foo.m_returnValue.m_type.c_str());
			if (foo.m_returnValue.m_templateDecl.empty() == false) {
				return_value << wxT("<") << _U(foo.m_returnValue.m_templateDecl.c_str()) << wxT(">");
			}
			return_value << _U(foo.m_returnValue.m_starAmp.c_str());
			return_value << wxT(" ");
		}
	}
	return return_value;
}

void TagsManager::GetTagsByKind(std::vector<TagEntryPtr>& tags, const wxArrayString& kind, const wxString& partName)
{
	wxUnusedVar(partName);
	m_workspaceDatabase->GetTagsByKind(kind, wxEmptyString, ITagsStorage::OrderNone, tags);
}

void TagsManager::GetTagsByKindLimit(std::vector<TagEntryPtr>& tags, const wxArrayString& kind, int limit, const wxString& partName)
{
	m_workspaceDatabase->GetTagsByKindLimit(kind, wxEmptyString, ITagsStorage::OrderNone, limit, partName, tags);
}

void TagsManager::DoGetFunctionTipForEmptyExpression(const wxString& word, const wxString& text, std::vector<TagEntryPtr>& tips, bool globalScopeOnly/* = false*/)
{
	std::vector<TagEntryPtr> candidates;
	std::vector<wxString>    additionlScopes;

	//we are probably examining a global function, or a scope function
	GetGlobalTags(word, candidates, ExactMatch);

	if( !globalScopeOnly ) {
		wxString scopeName = GetLanguage()->GetScopeName(text, &additionlScopes);
		TagsByScopeAndName(scopeName, word, candidates);
		for (size_t i=0; i<additionlScopes.size(); i++) {
			TagsByScopeAndName(additionlScopes.at(i), word, candidates);
		}

	}
	GetFunctionTipFromTags(candidates, word, tips);
}

void TagsManager::GetUnOverridedParentVirtualFunctions(const wxString& scopeName, bool onlyPureVirtual, std::vector<TagEntryPtr> &protos)
{
	std::vector<TagEntryPtr> tags;
	std::map<wxString, TagEntryPtr> parentSignature2tag;
	std::map<wxString, TagEntryPtr> classSignature2tag;

	GetDatabase()->GetTagsByPath(scopeName, tags);
	if(tags.size() != 1) {
		return;
	}

	TagEntryPtr classTag = tags.at(0);
	if(classTag->GetKind() != wxT("class") && classTag->GetKind() != wxT("struct"))
		return;


	// Step 1:
	// ========
	// Compoze a list of all virtual functions from the direct parent(s)
	// class (there could be a multiple inheritance...)
	wxArrayString parents = classTag->GetInheritsAsArrayNoTemplates();
	wxArrayString kind;

	tags.clear();
	kind.Add(wxT("prototype"));
	kind.Add(wxT("function" ));
	for(wxArrayString::size_type i=0; i<parents.GetCount(); i++) {
		GetDatabase()->GetTagsByScopeAndKind(parents.Item(i), kind, tags);
	}

	for(wxArrayString::size_type i=0; i<tags.size(); i++) {
		TagEntryPtr t   = tags.at(i);

		// Skip c-tors/d-tors
		if(t->IsDestructor() || t->IsConstructor())
			continue;

		if( onlyPureVirtual ) {

			// Collect only pure virtual methods
			if( IsPureVirtual(t) ) {
				TagEntryPtr t   = tags.at(i);
				wxString    sig = NormalizeFunctionSig(t->GetSignature(), Normalize_Func_Reverse_Macro);
				sig.Prepend(t->GetName());
				parentSignature2tag[sig] = tags.at(i);
			}

		} else {

			// Collect both virtual and pure virtual
			if( IsVirtual(tags.at(i)) || IsPureVirtual(tags.at(i)) ) {
				wxString    sig = NormalizeFunctionSig(t->GetSignature(), Normalize_Func_Reverse_Macro);
				sig.Prepend(t->GetName());
				parentSignature2tag[sig] = tags.at(i);
			}
		}
	}

	// Step 2:
	// ========
	// Collect a list of function prototypes from the class
	tags.clear();
	GetDatabase()->GetTagsByScopeAndKind(scopeName, kind, tags);
	for(size_t i=0; i<tags.size(); i++){
		TagEntryPtr t   = tags.at(i);
		wxString    sig = NormalizeFunctionSig(t->GetSignature(), Normalize_Func_Reverse_Macro);
		sig.Prepend(t->GetName());
		classSignature2tag[sig] = t;
	}

	// Step 3:
	// =======
	// remove any entry from the parent tags which exists in the child tags
	std::map<wxString, TagEntryPtr>::iterator iter = classSignature2tag.begin();
	for(; iter != classSignature2tag.end(); iter++) {
		if(parentSignature2tag.find(iter->first) != parentSignature2tag.end()) {
			// the current signature exists both in the child and the parent,
			// remove it
			parentSignature2tag.erase(iter->first);
		}
	}

	// Step 4:
	// =======
	// parentSignature2tag now contains map of signature/tags of virtual functions which exists
	// in the parent but could not be found in the child
	iter = parentSignature2tag.begin();
	for(; iter != parentSignature2tag.end(); iter++) {
		protos.push_back(iter->second);
	}
}

void TagsManager::ClearTagsCache()
{
	m_workspaceDatabase->ClearCache();
}

void TagsManager::SetProjectPaths(const wxArrayString& paths)
{
	m_projectPaths.Clear();
	m_projectPaths = paths;
}

void TagsManager::GetDereferenceOperator(const wxString& scope, std::vector<TagEntryPtr>& tags)
{
	std::vector<wxString> derivationList;

	//add this scope as well to the derivation list
	wxString _scopeName = DoReplaceMacros( scope );
	derivationList.push_back(_scopeName);
	GetDerivationList(_scopeName, derivationList);

	//make enough room for max of 500 elements in the vector
	for (size_t i=0; i<derivationList.size(); i++) {
		wxString tmpScope(derivationList.at(i));
		tmpScope = DoReplaceMacros(tmpScope);

		GetDatabase()->GetDereferenceOperator(tmpScope, tags);
		if(tags.size()) {

			// No need to further check
			break;

		}
	}
}

void TagsManager::GetSubscriptOperator(const wxString& scope, std::vector<TagEntryPtr>& tags)
{
	std::vector<wxString> derivationList;

	//add this scope as well to the derivation list
	wxString _scopeName = DoReplaceMacros( scope );
	derivationList.push_back(_scopeName);
	GetDerivationList(_scopeName, derivationList);

	//make enough room for max of 500 elements in the vector
	for (size_t i=0; i<derivationList.size(); i++) {
		wxString tmpScope(derivationList.at(i));
		tmpScope = DoReplaceMacros(tmpScope);

		GetDatabase()->GetSubscriptOperator(scope, tags);
		if(tags.size()) {

			// No need to further check
			break;

		}
	}
}

void TagsManager::ClearAllCaches()
{
	m_cachedFile.Clear();
	m_cachedFileFunctionsTags.clear();
	m_workspaceDatabase->ClearCache();
}
