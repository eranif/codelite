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
#include "fc_fileopener.h"

#include "parse_thread.h"
#include "ctags_manager.h"
#include "tags_database.h"
#include <wx/stopwatch.h>

DEFINE_EVENT_TYPE(wxEVT_COMMAND_SYMBOL_TREE_UPDATE_ITEM)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SYMBOL_TREE_DELETE_ITEM)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SYMBOL_TREE_ADD_ITEM)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SYMBOL_TREE_DELETE_PROJECT)

#if 1
#    ifdef __WXMSW__
#        define DEBUG_MESSAGE(x) wxLogMessage(x)
#    else
#        define DEBUG_MESSAGE(x) wxPrintf(x)
#    endif
#else
#        define DEBUG_MESSAGE(x)
#endif

const wxEventType wxEVT_PARSE_THREAD_UPDATED_FILE_SYMBOLS = wxNewId();
extern int crawlerScan(const char *path);

ParseThread::ParseThread()
		: WorkerThread()
		, m_pDb              ( new TagsDatabase() )
		, m_externalSymbolsDb( new TagsDatabase() )
{
}

ParseThread::~ParseThread()
{
	if ( m_externalSymbolsDb ) {
		delete m_externalSymbolsDb;
	}
}

void ParseThread::ProcessRequest(ThreadRequest * request)
{
	// request is delete by the parent WorkerThread after this method is completed
	ParseRequest *req    = (ParseRequest*)request;
	wxString      dbfile = req->getDbfile();
	wxString      file   = req->getFile();
	wxString   extDbFile = req->getExtDbFile();

	// req->tags should contain the tags
	// convert the file to tags
	TagsManager *tagmgr = TagsManagerST::Get();

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
	try {
		// Open the external database
		DEBUG_MESSAGE( wxString::Format(wxT("Opening external database %s"), extDbFile.c_str()) ) ;
		m_externalSymbolsDb->OpenDatabase(extDbFile);

		wxSQLite3ResultSet rs = m_pDb->SelectTagsByFile(file, wxFileName(dbfile));

		// Load the records and build a language tree
		TagEntry root;
		root.SetName(wxT("<ROOT>"));
		oldTree.Reset( new TagTree(wxT("<ROOT>"), root) );
		while ( rs.NextRow() ) {
			TagEntry entry(rs);
			oldTree->AddEntry(entry);
		}
		rs.Finalize();
	} catch (wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}

	// Build second tree from the updated file
	TagTreePtr newTree;
	std::vector<CommentPtr> comments;

	TagsManager *mgr = TagsManagerST::Get();
	if (mgr->GetParseComments() ) {
		newTree = mgr->ParseSourceFile2(wxFileName(file), req->getTags(), &comments);
	} else {
		newTree = mgr->ParseSourceFile2(wxFileName(file), req->getTags());
	}

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


	m_pDb->Begin();

	// Prepare sql statements
	TagEntry dummy;
	try {
		wxSQLite3Statement insertStmt = m_pDb->PrepareStatement( dummy.GetInsertOneStatement() );
		wxSQLite3Statement updateStmt = m_pDb->PrepareStatement( dummy.GetUpdateOneStatement() );
		wxSQLite3Statement deleteStmt = m_pDb->PrepareStatement( dummy.GetDeleteOneStatement() );

		for (i=0; i<deletedItems.size(); i++)
			deletedItems[i].second.Delete(deleteStmt);

		for (i=0; i<newItems.size(); i++) {
			if (newItems[i].second.Store(insertStmt, m_pDb.get()) == TagOk) {
				goodNewItems.push_back(newItems[i]);
			}
		}

		for (i=0; i<modifiedItems.size(); i++)
			modifiedItems[i].second.Update(updateStmt);

		///////////////////////////////////////////
		// update the file retag timestamp
		///////////////////////////////////////////

		std::vector<DbRecordPtr> entries;

		FileEntry *fe = new FileEntry();
		fe->SetFile(file);
		fe->SetLastRetaggedTimestamp((int)time(NULL));

		DbRecordPtr fep(fe);
		entries.push_back(fep);
		m_pDb->Store(entries, wxFileName(), false);

	} catch (wxSQLite3Exception &e) {
		wxUnusedVar(e);
	}

	m_pDb->Commit();

	// Parse the saved file to get a list of files to include
	//ParseIncludeFiles( file );

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
}

void ParseThread::ParseIncludeFiles(const wxString& filename)
{
	// TODO:: insert here the user defined search-path
#if defined(__WXGTK__)||defined(__WXMAC__)
	fcFileOpener::Instance()->AddSearchPath("/usr/include");
	fcFileOpener::Instance()->AddSearchPath("/usr/include/c++/4.2.4/");
	fcFileOpener::Instance()->AddSearchPath("/usr/include/wx-2.8/");
#else
	fcFileOpener::Instance()->AddSearchPath("C:/MinGW-3.4.5/include/c++/3.4.5");
	fcFileOpener::Instance()->AddSearchPath("C:/wxWidgets-2.8.10/include");
#endif

	// Invoke the crawler
	const wxCharBuffer cfile = filename.mb_str(wxConvUTF8);
	crawlerScan( cfile.data() );

	// Loop over the crawler results and remove any files
	// which were recently updated
	std::set<std::string> fileSet = fcFileOpener::Instance()->GetResults();
	std::set<std::string>::iterator iter = fileSet.begin();
	wxArrayString                    arrFiles;
	for (; iter != fileSet.end(); iter++ ) {
		arrFiles.Add(wxString((*iter).c_str(), wxConvUTF8));
	}

	DEBUG_MESSAGE( wxString::Format(wxT("Files that need parse %d"), arrFiles.GetCount()) ) ;
	TagsManagerST::Get()->FilterNonNeededFilesForRetaging(arrFiles, m_externalSymbolsDb);
	DEBUG_MESSAGE( wxString::Format(wxT("Actual files that need parse %d"), arrFiles.GetCount()) );

	// Loop over the files and parse them
	DEBUG_MESSAGE(wxString::Format(wxT("Saving files to database....")));
	for (size_t i=0; i<arrFiles.GetCount(); i++) {
		wxString tags;  // output
		TagsManagerST::Get()->SourceToTags(arrFiles.Item(i), tags);

		if ( tags.IsEmpty() == false ) {
			DoStoreTags(tags, arrFiles.Item(i));
		}
	}
	DEBUG_MESSAGE(wxT("Done"));

	// Update the retagging timestamp
	try {
		TagsManagerST::Get()->UpdateFilesRetagTimestamp(arrFiles, m_externalSymbolsDb);
	} catch (wxSQLite3Exception & e) {
		DEBUG_MESSAGE(wxString::Format(wxT("Got sqlite3 exception: %s"), e.GetMessage().c_str()));
	}
}

TagTreePtr ParseThread::DoTreeFromTags(const wxString& tags)
{
	return TagsManagerST::Get()->TreeFromTags(tags);
}

void ParseThread::DoStoreTags(const wxString& tags, const wxString &filename)
{
	TagTreePtr ttp = DoTreeFromTags(tags);
	try {
		m_externalSymbolsDb->Begin();
		m_externalSymbolsDb->DeleteByFileName( m_externalSymbolsDb->GetDatabaseFileName(), filename, false);
		m_externalSymbolsDb->Store(ttp, wxFileName(), false);
		m_externalSymbolsDb->Commit();

	} catch ( wxSQLite3Exception &e ) {
		DEBUG_MESSAGE(wxString::Format(wxT("Got sqlite3 exception: %s"), e.GetMessage().c_str()));
	}
}

void ParseThread::SendEvent(int evtType, const wxString &fileName, std::vector<std::pair<wxString, TagEntry> >  &items)
{
	SymbolTreeEvent event(items, evtType);
	event.SetFileName(fileName.c_str());
	wxPostEvent(m_notifiedWindow, event);
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
	setFile(rhs._file);
	setDbFile(rhs._dbfile);
	setTags(rhs._tags);
	return *this;
}

void ParseRequest::setFile(const wxString& file)
{
	_file = file.c_str();
}

ParseRequest::~ParseRequest()
{
}

void ParseRequest::setExtDbFile(const wxString& extDbFile)
{
	_extDbFile = extDbFile.c_str();
}
