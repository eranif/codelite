//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : parse_thread.cpp
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
#include <wx/tokenzr.h>
#include "fc_fileopener.h"

#include "parse_thread.h"
#include "ctags_manager.h"
#include "istorage.h"
#include <wx/stopwatch.h>
#include <wx/xrc/xmlres.h>

DEFINE_EVENT_TYPE(wxEVT_COMMAND_SYMBOL_TREE_UPDATE_ITEM)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SYMBOL_TREE_DELETE_ITEM)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SYMBOL_TREE_ADD_ITEM)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SYMBOL_TREE_DELETE_PROJECT)

#if 0
#    ifdef __WXMSW__
#        define DEBUG_MESSAGE(x) wxLogMessage(x)
#    else
#        define DEBUG_MESSAGE(x) wxPrintf(x); wxPrintf(wxT("\n"));
#    endif
#else
#        define DEBUG_MESSAGE(x)
#endif

#define TEST_DESTROY() {\
		if( TestDestroy() ) {\
			DEBUG_MESSAGE( wxString::Format(wxT("ParseThread::ProcessIncludes -> received 'TestDestroy()'") ) );\
			return;\
		}\
}

const wxEventType wxEVT_PARSE_THREAD_UPDATED_FILE_SYMBOLS = XRCID("parse_thread_updated_symbols");
const wxEventType wxEVT_PARSE_THREAD_MESSAGE              = XRCID("parse_thread_update_status_bar");
extern int crawlerScan(const char *path);

ParseThread::ParseThread()
		: WorkerThread()
		, m_pDb              ( NULL )
{
}

ParseThread::~ParseThread()
{
	if ( m_pDb ) {
		delete m_pDb;
	}
}

void ParseThread::ProcessRequest(ThreadRequest * request)
{
	// request is delete by the parent WorkerThread after this method is completed
	ParseRequest *req    = (ParseRequest*)request;

	switch (req->getType()) {
//	case ParseRequest::PR_PARSEINCLUDES:
//		ProcessIncludes( req );
//		break;
	default:
	case ParseRequest::PR_FILESAVED:
		ProcessSimple( req );
		break;
	}
}

void ParseThread::ParseIncludeFiles(const wxString& filename)
{
	wxArrayString arrFiles;
	fcFileOpener::Instance()->ClearResults();
	GetFileListToParse(filename, arrFiles);
	int initalCount = arrFiles.GetCount();

	TEST_DESTROY();

	DEBUG_MESSAGE( wxString::Format(wxT("Files that need parse %d"), arrFiles.GetCount()) ) ;
	TagsManagerST::Get()->FilterNonNeededFilesForRetaging(arrFiles, m_pDb);
	DEBUG_MESSAGE( wxString::Format(wxT("Actual files that need parse %d"), arrFiles.GetCount()) );

	ParseAndStoreFiles(arrFiles, initalCount);
}

TagTreePtr ParseThread::DoTreeFromTags(const wxString& tags, int &count)
{
	return TagsManagerST::Get()->TreeFromTags(tags, count);
}

void ParseThread::DoStoreTags(const wxString& tags, const wxString &filename, int &count)
{
	TagTreePtr ttp = DoTreeFromTags(tags, count);
	m_pDb->Begin();
	m_pDb->DeleteByFileName( m_pDb->GetDatabaseFileName(), filename, false);
	m_pDb->Store(ttp, wxFileName(), false);
	m_pDb->Commit();
}

void ParseThread::SendEvent(int evtType, const wxString &fileName, std::vector<std::pair<wxString, TagEntry> >  &items)
{
	SymbolTreeEvent event(items, evtType);
	event.SetFileName(fileName.c_str());
	wxPostEvent(m_notifiedWindow, event);
}

void ParseThread::SetCrawlerEnabeld(bool b)
{
	wxCriticalSectionLocker locker ( m_cs );
	m_crawlerEnabled = b;
}

void ParseThread::SetSearchPaths(const wxArrayString& paths)
{
	wxCriticalSectionLocker locker( m_cs );
	m_searchPaths.Clear();
	for (size_t i=0; i<paths.GetCount(); i++) {
		m_searchPaths.Add( paths.Item(i).c_str() );
	}
}

bool ParseThread::IsCrawlerEnabled()
{
	wxCriticalSectionLocker locker( m_cs );
	return m_crawlerEnabled;
}

void ParseThread::GetSearchPaths(wxArrayString& paths)
{
	wxCriticalSectionLocker locker( m_cs );
	for (size_t i=0; i<m_searchPaths.GetCount(); i++) {
		paths.Add( m_searchPaths.Item(i).c_str() );
	}
}

void ParseThread::ProcessIncludes(ParseRequest* req)
{
	DEBUG_MESSAGE( wxString::Format(wxT("ProcessIncludes -> restarting codelite_indexer")) ) ;
	TagsManagerST::Get()->RestartCtagsProcess();

	DEBUG_MESSAGE( wxString::Format(wxT("ProcessIncludes -> started")) ) ;
	wxString      dbfile = req->getDbfile();
	if ( !m_pDb ) {
		m_pDb = new TagsStorageSQLite();
	}

	m_pDb->OpenDatabase( dbfile );
	std::vector<FileEntryPtr> files;

	m_pDb->GetFiles(files);

	// Remove from this list all files which starts with one of the crawler search paths
	wxArrayString searchPaths, filteredFileList;
	GetSearchPaths( searchPaths );

	for(size_t i=0; i<files.size(); i++) {
		wxString name = files.at(i)->GetFile();
		wxFileName fn = name;
		fn.MakeAbsolute();

		bool isFromSearchPath(false);
		for(size_t j=0; j<searchPaths.GetCount(); j++) {
			wxFileName p ( searchPaths.Item(j) );
			if( fn.GetFullPath().StartsWith(p.GetPath()) ) {
				isFromSearchPath = true;
				break;
			}
		}

		// this file is not part of the search paths
		if( !isFromSearchPath ) {
			filteredFileList.Add( fn.GetFullPath() );
		}
	}

	DEBUG_MESSAGE( wxString::Format(wxT("ParseThread::ProcessIncludes -> Workspace files %d"), filteredFileList.GetCount()) );

	wxArrayString arrFiles;

	// Clear the results once
	fcFileOpener::Instance()->ClearResults();
	fcFileOpener::Instance()->ClearSearchPath();

	for(size_t i=0; i<searchPaths.GetCount(); i++) {
		fcFileOpener::Instance()->AddSearchPath(searchPaths.Item(i).mb_str(wxConvUTF8).data());
	}
	wxStopWatch sw;
	sw.Start();
	for(size_t i=0; i<filteredFileList.GetCount(); i++) {
		crawlerScan(filteredFileList.Item(i).mb_str(wxConvUTF8).data());
		TEST_DESTROY();
	}

	// collect the results
	std::set<std::string> fileSet = fcFileOpener::Instance()->GetResults();
	std::set<std::string>::iterator iter = fileSet.begin();
	for (; iter != fileSet.end(); iter++ ) {
		wxFileName fn(wxString((*iter).c_str(), wxConvUTF8));
		fn.MakeAbsolute();
		if ( arrFiles.Index(fn.GetFullPath()) == wxNOT_FOUND ) {
			arrFiles.Add( fn.GetFullPath() );
		}
	}

	// Remove any file which was recently updated
	int initalCount = arrFiles.GetCount();
	DEBUG_MESSAGE( wxString::Format(wxT("ParseThread::ProcessIncludes -> Files that need parse %d, time elapsed to get list %d ms"), arrFiles.GetCount(), sw.Time()/1000) ) ;
	TagsManagerST::Get()->FilterNonNeededFilesForRetaging(arrFiles, m_pDb);
	DEBUG_MESSAGE( wxString::Format(wxT("ParseThread::ProcessIncludes -> Actual files that need parse %d"), arrFiles.GetCount()) );

	ParseAndStoreFiles( arrFiles, initalCount );

	delete m_pDb;
	m_pDb = NULL;
}

void ParseThread::ProcessSimple(ParseRequest* req)
{
	wxString      dbfile = req->getDbfile();
	wxString      file   = req->getFile();

	// convert the file to tags
	TagsManager *tagmgr = TagsManagerST::Get();

	if ( !m_pDb ) {
		m_pDb = new TagsStorageSQLite();
	}

	//convert the file content into tags
	wxString tags;
	wxString file_name(req->getFile());
	tagmgr->SourceToTags(file_name, tags);

	req->setTags(tags);

	//----------------------------------------------
	// Build a tree from project/file/project
	// from the value which are set in the database
	//----------------------------------------------
	TagTreePtr oldTree;

	std::vector<TagEntryPtr> tagsByFile;

	m_pDb->SelectTagsByFile(file, tagsByFile, wxFileName(dbfile));

	// Load the records and build a language tree
	TagEntry root;
	root.SetName(wxT("<ROOT>"));
	oldTree.Reset( new TagTree(wxT("<ROOT>"), root) );
	for(size_t i=0; i<tagsByFile.size(); i++) {
		oldTree->AddEntry( *(tagsByFile.at(i).Get()) );
	}

	// Build second tree from the updated file
	TagsManager *mgr = TagsManagerST::Get();
	TagTreePtr newTree = mgr->ParseSourceFile2(wxFileName(file), req->getTags());

	//-------------------------------------------------------------------
	// Now what is left to be done here, is to update the GUI tree
	// The GUI tree needs to be updated item by item, to avoid total tree
	// Collapsing
	//-------------------------------------------------------------------


	// Compare old tree vs new tree
	std::vector<std::pair<wxString, TagEntry> >  deletedItems;
	std::vector<std::pair<wxString, TagEntry> >  newItems;
	std::vector<std::pair<wxString, TagEntry> >  goodNewItems;
	std::vector<std::pair<wxString, TagEntry> >  modifiedItems;

	oldTree->Compare(newTree.Get(), deletedItems, modifiedItems, newItems);

	// Delete old entries
	size_t i=0;
	m_pDb->OpenDatabase( dbfile );
	m_pDb->Begin();

	// remove all the 'deleted' items from the database
	for (i=0; i<deletedItems.size(); i++) {
		m_pDb->DeleteTagEntry(deletedItems[i].second.GetKind(), deletedItems[i].second.GetSignature(), deletedItems[i].second.GetPath());
	}

	// insert all new items to database
	for (i=0; i<newItems.size(); i++) {
		if (m_pDb->InsertTagEntry(newItems[i].second) == TagOk) {
			goodNewItems.push_back(newItems[i]);
		}
	}

	// Update modified items
	for (i=0; i<modifiedItems.size(); i++) {
		m_pDb->UpdateTagEntry( modifiedItems[i].second );
	}

	///////////////////////////////////////////
	// update the file retag timestamp
	///////////////////////////////////////////
	m_pDb->InsertFileEntry(file, (int)time(NULL));

	m_pDb->Commit();

	// Parse the saved file to get a list of files to include
	ParseIncludeFiles( file );

	// If there is no event handler set to handle this comaprison
	// results, then nothing more to be done
	if (m_notifiedWindow ) {

		// send "end" event
		wxCommandEvent e(wxEVT_PARSE_THREAD_UPDATED_FILE_SYMBOLS);
		wxPostEvent(m_notifiedWindow, e);

		// Send an event for each operation type
		if ( !deletedItems.empty() )
			SendEvent(wxEVT_COMMAND_SYMBOL_TREE_DELETE_ITEM, req->getFile(), deletedItems);

		if ( !newItems.empty() )
			SendEvent(wxEVT_COMMAND_SYMBOL_TREE_ADD_ITEM, req->getFile(), goodNewItems);

		if ( !modifiedItems.empty() ) {
			std::vector<std::pair<wxString, TagEntry> >  realModifiedItems;
			for (size_t i=0; i<modifiedItems.size(); i++) {
				std::pair<wxString, TagEntry> p = modifiedItems.at(i);
				if (!p.second.GetDifferOnByLineNumber()) {
					realModifiedItems.push_back(p);
				}
			}
			if (realModifiedItems.empty() == false) {
				SendEvent(wxEVT_COMMAND_SYMBOL_TREE_UPDATE_ITEM, req->getFile(), realModifiedItems);
			}
		}
	}

	// close the database
	delete m_pDb;
	m_pDb = NULL;
}

void ParseThread::GetFileListToParse(const wxString& filename, wxArrayString& arrFiles)
{
	if ( !this->IsCrawlerEnabled() ) {
		return;
	}

	wxArrayString includePaths;
	GetSearchPaths( includePaths );

	fcFileOpener::Instance()->ClearSearchPath();
	for(size_t i=0; i<includePaths.GetCount(); i++) {
		fcFileOpener::Instance()->AddSearchPath( includePaths.Item(i).mb_str(wxConvUTF8).data() );
	}

	// Invoke the crawler
	const wxCharBuffer cfile = filename.mb_str(wxConvUTF8);
	crawlerScan( cfile.data() );

	std::set<std::string> fileSet = fcFileOpener::Instance()->GetResults();
	std::set<std::string>::iterator iter = fileSet.begin();
	for (; iter != fileSet.end(); iter++ ) {
		wxFileName fn(wxString((*iter).c_str(), wxConvUTF8));
		fn.MakeAbsolute();
		if ( arrFiles.Index(fn.GetFullPath()) == wxNOT_FOUND ) {
			arrFiles.Add(fn.GetFullPath());
		}
	}

}

void ParseThread::ParseAndStoreFiles(const wxArrayString& arrFiles, int initalCount)
{
	// Loop over the files and parse them
	int totalSymbols (0);
	DEBUG_MESSAGE(wxString::Format(wxT("Parsing and saving files to database....")));
	for (size_t i=0; i<arrFiles.GetCount(); i++) {

		// give a shutdown request a chance
		TEST_DESTROY();

		wxString tags;  // output
		TagsManagerST::Get()->SourceToTags(arrFiles.Item(i), tags);

		if ( tags.IsEmpty() == false ) {
			DoStoreTags(tags, arrFiles.Item(i), totalSymbols);
		}
	}

	DEBUG_MESSAGE(wxT("Done"));

	// Update the retagging timestamp
	TagsManagerST::Get()->UpdateFilesRetagTimestamp(arrFiles, m_pDb);

	if ( m_notifiedWindow && !arrFiles.IsEmpty() ) {
		wxCommandEvent e(wxEVT_PARSE_THREAD_MESSAGE);
		wxString message;
		message << wxT("INFO: Found ") << initalCount << wxT(" system include files. ");
		message << arrFiles.GetCount() << wxT(" needed to be parsed. Stored ") << totalSymbols << wxT(" new tags to the database");

		e.SetClientData(new wxString(message.c_str()));
		m_notifiedWindow->AddPendingEvent( e );
	}
}

//--------------------------------------------------------------------------------------
// Parse Request Class
//--------------------------------------------------------------------------------------
void ParseRequest::setDbFile(const wxString& dbfile)
{
	_dbfile = dbfile.c_str();
}

void ParseRequest::setTags(const wxString& tags)
{
	_tags = tags.c_str();
}

ParseRequest::ParseRequest(const ParseRequest& rhs)
{
	if (this == &rhs) {
		return;
	}
	*this = rhs;
}

ParseRequest &ParseRequest::operator =(const ParseRequest& rhs)
{
	setFile  (rhs._file.c_str()  );
	setDbFile(rhs._dbfile.c_str());
	setTags  (rhs._tags          );
	setType  (rhs._type          );
	return *this;
}

void ParseRequest::setFile(const wxString& file)
{
	_file = file.c_str();
}

ParseRequest::~ParseRequest()
{
}
