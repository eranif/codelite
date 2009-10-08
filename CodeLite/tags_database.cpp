//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : tags_database.cpp
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
#include <wx/longlong.h>
#include "precompiled_header.h"
#include "tags_database.h"
#include "tagscache.h"


//-------------------------------------------------
// Tags database class implementation
//-------------------------------------------------
TagsDatabase::TagsDatabase(bool useCache)
		: ITagsStorage(useCache)
		, m_cache     (NULL    )
{
	m_db    = new wxSQLite3Database();
	m_cache = new TagsCache();
	m_cache->SetMaxCacheSize(1000);
}

TagsDatabase::~TagsDatabase()
{
	if (m_db) {
		m_db->Close();
		delete m_db;
		m_db = NULL;
	}

	// clear and delete the cache
	ClearCache();
	delete m_cache;
	m_cache = NULL;
}

void TagsDatabase::OpenDatabase(const wxFileName& fileName)
{
	if (m_fileName == fileName)
		return;

	// Did we get a file name to use?
	if (!fileName.IsOk() && !m_fileName.IsOk())
		return;

	// We did not get any file name to use BUT we
	// do have an open database, so we will use it
	if (!fileName.IsOk())
		return;

	if (!m_fileName.IsOk()) {
		// First time we open the db
		m_db->Open(fileName.GetFullPath());
		CreateSchema();
		m_fileName = fileName;
		m_cache->SetName(m_fileName.GetFullName());
	} else {
		// We have both fileName & m_fileName and they
		// are different, Close previous db
		m_db->Close();
		m_db->Open(fileName.GetFullPath());
		CreateSchema();
		m_fileName = fileName;
		ClearCache();
		m_cache->SetName(m_fileName.GetFullName());
	}
}

void TagsDatabase::CreateSchema()
{
	wxString sql;

	// improve performace by using pragma command:
	// (this needs to be done before the creation of the
	// tables and indices)
	try {
		sql = wxT("PRAGMA synchronous = OFF;");
		m_db->ExecuteUpdate(sql);

		sql = wxT("PRAGMA temp_store = MEMORY;");
		m_db->ExecuteUpdate(sql);

		sql = wxT("PRAGMA default_cache_size = 2000;");
		m_db->ExecuteUpdate(sql);

		sql = wxT("create  table if not exists tags (ID INTEGER PRIMARY KEY AUTOINCREMENT, name string, file string, line integer, kind string, access string, signature string, pattern string, parent string, inherits string, path string, typeref string, scope string);");
		m_db->ExecuteUpdate(sql);

		sql = wxT("create  table if not exists FILES (ID INTEGER PRIMARY KEY AUTOINCREMENT, file string, last_retagged integer);");
		m_db->ExecuteUpdate(sql);

		// create unuque index on Files' file column
		sql = wxT("CREATE UNIQUE INDEX IF NOT EXISTS FILES_NAME on FILES(file)");
		m_db->ExecuteUpdate(sql);

		sql = wxT("create  table if not exists comments (comment string, file string, line number);");
		m_db->ExecuteUpdate(sql);

		sql = wxT("create table if not exists variables (name string primary key, value string)");
		m_db->ExecuteUpdate(sql);

		sql = wxT("CREATE UNIQUE INDEX IF NOT EXISTS COMMENTS_UNIQ on comments(file, line)");
		m_db->ExecuteUpdate(sql);

		// Create unique index on tags table
		sql = wxT("CREATE UNIQUE INDEX IF NOT EXISTS TAGS_UNIQ on tags(kind, path, signature);");
		m_db->ExecuteUpdate(sql);

		sql = wxT("CREATE INDEX IF NOT EXISTS KIND_IDX on tags(kind);");
		m_db->ExecuteUpdate(sql);

		sql = wxT("CREATE INDEX IF NOT EXISTS FILE_IDX on tags(file);");
		m_db->ExecuteUpdate(sql);

		// Create search indexes
		sql = wxT("CREATE INDEX IF NOT EXISTS TAGS_NAME on tags(name);");
		m_db->ExecuteUpdate(sql);

		sql = wxT("CREATE INDEX IF NOT EXISTS TAGS_SCOPE on tags(scope);");
		m_db->ExecuteUpdate(sql);

		sql = wxT("CREATE INDEX IF NOT EXISTS TAGS_PATH on tags(path);");
		m_db->ExecuteUpdate(sql);

		sql = wxT("CREATE INDEX IF NOT EXISTS TAGS_PARENT on tags(parent);");
		m_db->ExecuteUpdate(sql);

		sql = wxT("CREATE INDEX IF NOT EXISTS TAGS_KIND on tags(kind);");
		m_db->ExecuteUpdate(sql);

		sql = wxT("CREATE INDEX IF NOT EXISTS COMMENTS_FILE on COMMENTS(file);");
		m_db->ExecuteUpdate(sql);

		sql = wxT("CREATE INDEX IF NOT EXISTS COMMENTS_LINE on COMMENTS(line);");
		m_db->ExecuteUpdate(sql);

		sql = wxT("create table if not exists tags_version (version string primary key);");
		m_db->ExecuteUpdate(sql);

		sql = wxT("create unique index if not exists tags_version_uniq on tags_version(version);");
		m_db->ExecuteUpdate(sql);

		sql = wxString(wxT("insert into tags_version values ('"));
		sql << GetVersion() << wxT("');");
		m_db->ExecuteUpdate(sql);
	} catch (wxSQLite3Exception &e) {
		wxUnusedVar(e);
	}
}

void TagsDatabase::RecreateDatabase()
{
	try {
		m_db->ExecuteUpdate(wxT("DROP TABLE IF EXISTS TAGS"));
		m_db->ExecuteUpdate(wxT("DROP TABLE IF EXISTS COMMENTS"));
		m_db->ExecuteUpdate(wxT("DROP TABLE IF EXISTS TAGS_VERSION"));
		m_db->ExecuteUpdate(wxT("DROP TABLE IF EXISTS VARIABLES"));
		m_db->ExecuteUpdate(wxT("DROP TABLE IF EXISTS FILES"));

		// Recreate the schema
		CreateSchema();

	} catch (wxSQLite3Exception &e) {
		wxUnusedVar(e);
	}
}

wxString TagsDatabase::GetSchemaVersion() const
{
	// return the current schema version
	try {
		wxString sql;
		wxString version;
		sql = wxT("SELECT * FROM TAGS_VERSION");
		wxSQLite3ResultSet rs = m_db->ExecuteQuery( sql );

		if ( rs.NextRow() )
			version = rs.GetString(0);
		return version;
	} catch (wxSQLite3Exception &e ) {
		wxUnusedVar(e);
	}
	return wxEmptyString;
}

void TagsDatabase::Store(const std::vector<DbRecordPtr> &records, const wxFileName& path, bool autoCommit)
{
	if (!path.IsOk() && !m_fileName.IsOk()) {
		// An attempt is made to save the tree into db but no database
		// is provided and none is currently opened to use
		return;
	}

	if (records.empty())
		return;

	OpenDatabase(path);
	try {
		// Create the statements before the execution
		wxSQLite3Statement insertStmt = m_db->PrepareStatement(records[0]->GetInsertOneStatement());

		std::vector<DbRecordPtr> updateList;

		// AddChild entries to database
		// we break the bug transaction into samller ones of 1000 operations
		// each
		const size_t bulk = 1000;
		if ( autoCommit )
			m_db->Begin();

		size_t i=0;
		for (; i<records.size(); i++) {
			if (records[i]->Store(insertStmt, this) == TagExist) {
				// Update the record
				updateList.push_back(records[i]);
			} else {
				// insert succeeded
				if (i % bulk == 0 && autoCommit) {
					m_db->Commit();
					m_db->Begin();
				}
			}
		}
		insertStmt.Finalize();

		if ( autoCommit )
			m_db->Commit();

		// Do we need to update?
		if (!updateList.empty()) {
			wxSQLite3Statement updateStmt = m_db->PrepareStatement( updateList[0]->GetUpdateOneStatement() );
			if ( autoCommit )
				m_db->Begin();
			for (size_t i=0; i<updateList.size(); i++) {
				updateList[i]->Update(updateStmt);
				if ( i % bulk == 0 && autoCommit ) {
					m_db->Commit();
					m_db->Begin();
				}
			}
			updateStmt.Finalize();
			if ( autoCommit )
				m_db->Commit();
		}
	} catch (wxSQLite3Exception& e) {
		wxUnusedVar(e);
		try {
			if ( autoCommit )
				m_db->Rollback();
		} catch (wxSQLite3Exception& e1) {
			wxUnusedVar(e1);
		}
	}
}

void TagsDatabase::Store(TagTreePtr tree, const wxFileName& path, bool autoCommit)
{
	if (!path.IsOk() && !m_fileName.IsOk()) {
		// An attempt is made to save the tree into db but no database
		// is provided and none is currently opened to use
		return;
	}

	if ( !tree )
		return;

	OpenDatabase(path);
	TreeWalker<wxString, TagEntry> walker( tree->GetRoot() );

	try {
		// Create the statements before the execution
		TagEntry dummy;
		wxSQLite3Statement insertStmt = m_db->PrepareStatement( dummy.GetInsertOneStatement() );

		std::vector<TagEntry> updateList;

		// AddChild entries to database
		if ( autoCommit )
			m_db->Begin();

		for (; !walker.End(); walker++) {
			// Skip root node
			if (walker.GetNode() == tree->GetRoot())
				continue;

			// does not matter if we insert or update, the cache must be cleared for any related tags
			if ( m_useCache ) {
				m_cache->DeleteEntriesByRelation( walker.GetNode()->GetData() );
			}

			//walker.GetNode()->GetData().SetParentId(walker.GetNode()->GetParent()->GetData().GetId());
			if (walker.GetNode()->GetData().Store(insertStmt, this) == TagExist) {
				// Update the record
				updateList.push_back(walker.GetNode()->GetData());
			}
		}
		insertStmt.Finalize();

		if ( autoCommit )
			m_db->Commit();

		// Do we need to update?
		if (!updateList.empty()) {
			wxSQLite3Statement updateStmt = m_db->PrepareStatement( updateList[0].GetUpdateOneStatement() );
			if ( autoCommit )
				m_db->Begin();
			for (size_t i=0; i<updateList.size(); i++)
				updateList[i].Update(updateStmt);
			updateStmt.Finalize();
			if ( autoCommit )
				m_db->Commit();
		}
	} catch (wxSQLite3Exception& e) {
		wxUnusedVar(e);
		try {
			if ( autoCommit )
				m_db->Rollback();
		} catch (wxSQLite3Exception& WXUNUSED(e1)) {
			wxUnusedVar(e);
		}
	}
}

wxSQLite3ResultSet TagsDatabase::SelectTagsByFile(const wxString& file, const wxFileName& path)
{
	// Incase empty file path is provided, use the current file name
	wxFileName databaseFileName(path);
	path.IsOk() == false ? databaseFileName = m_fileName : databaseFileName = path;
	OpenDatabase(databaseFileName);

	wxString query;
	query << wxT("select * from tags where file='") << file << wxT("' ");
	return m_db->ExecuteQuery(query);
}


void TagsDatabase::DeleteByFileName(const wxFileName& path, const wxString& fileName, bool autoCommit)
{
	// make sure database is open
	OpenDatabase(path);

	try {
		if ( autoCommit )
			m_db->Begin();

		m_db->ExecuteUpdate(wxString::Format(wxT("Delete from tags where File='%s'"), fileName.GetData()));

		if ( autoCommit )
			m_db->Commit();
	} catch (wxSQLite3Exception& e) {
		wxUnusedVar(e);
		if ( autoCommit )
			m_db->Rollback();
	}
}

wxSQLite3ResultSet TagsDatabase::Query(const wxString& sql, const wxFileName& path)
{
	// make sure database is open
	OpenDatabase(path);

	try {
		return m_db->ExecuteQuery(sql);
	} catch (wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
	return wxSQLite3ResultSet();
}

void TagsDatabase::ExecuteUpdate(const wxString& sql)
{
	try {
		m_db->ExecuteUpdate(sql);
	} catch (wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
}

const bool TagsDatabase::IsOpen() const
{
	return m_db->IsOpen();
}

TagEntryPtr TagsDatabase::FindTagById(int id) const
{
	wxString sql;
	sql << wxT("select * from tags where id=") << id;
	try {
		wxSQLite3ResultSet q = m_db->ExecuteQuery(sql);
		if (q.NextRow()) {
			return TagEntryPtr(new TagEntry(q));
		}
	} catch (wxSQLite3Exception &e) {
		wxUnusedVar(e);
	}
	return NULL;
}

VariableEntryPtr TagsDatabase::FindVariableByName(const wxString &name) const
{
	wxString sql;
	sql << wxT("select * from variables where name='") << name << wxT("'");
	try {
		wxSQLite3ResultSet q = m_db->ExecuteQuery(sql);
		if (q.NextRow()) {
			return VariableEntryPtr(new VariableEntry(q));
		}
	} catch (wxSQLite3Exception &e) {
		wxUnusedVar(e);
	}
	return NULL;
}

int TagsDatabase::Insert(DbRecordPtr record)
{
	try {
		// Create the statements before the execution
		wxSQLite3Statement insertStmt = m_db->PrepareStatement(record->GetInsertOneStatement());
		return record->Store(insertStmt, this);
	} catch (wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
	return TagError;
}

int TagsDatabase::Delete(DbRecordPtr record)
{
	try {
		// Create the statements before the execution
		wxSQLite3Statement delStmnt = m_db->PrepareStatement(record->GetDeleteOneStatement());
		return record->Delete(delStmnt);
	} catch (wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
	return TagError;
}

int TagsDatabase::Update(DbRecordPtr record)
{
	try {
		// Create the statements before the execution
		wxSQLite3Statement updStmnt = m_db->PrepareStatement(record->GetUpdateOneStatement());
		return record->Update(updStmnt);
	} catch (wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
	return TagError;
}

void TagsDatabase::GetVariables(std::vector<VariableEntryPtr> &vars)
{
	try {
		wxSQLite3ResultSet res = m_db->ExecuteQuery(wxT("select * from variables"));
		while (res.NextRow()) {
			vars.push_back(new VariableEntry(res));
		}
	} catch (wxSQLite3Exception &e) {
		wxUnusedVar(e);
	}
}

void TagsDatabase::GetFiles(const wxString &partialName, std::vector<FileEntryPtr> &files)
{
	try {
		bool match_path = (!partialName.IsEmpty() &&
		                   partialName.Last() == wxFileName::GetPathSeparator());

		wxString query;
		wxString tmpName(partialName);
		tmpName.Replace(wxT("_"), wxT("^_"));
		query << wxT("select * from files where file like '%%") << tmpName << wxT("%%' ESCAPE '^' ")
		<< wxT("order by file");

		wxSQLite3ResultSet res = m_db->ExecuteQuery(query);
		while (res.NextRow()) {

			FileEntryPtr fe(new FileEntry());
			fe->SetId(res.GetInt(0));
			fe->SetFile(res.GetString(1));
			fe->SetLastRetaggedTimestamp(res.GetInt(2));

			wxFileName fileName(fe->GetFile());
			wxString match = match_path ? fileName.GetFullPath() : fileName.GetFullName();
			if (match.StartsWith(partialName)) {
				files.push_back(fe);
			}
		}
	} catch (wxSQLite3Exception &e) {
		wxUnusedVar(e);
	}
}

long TagsDatabase::LastRowId() const
{
	wxLongLong id = m_db->GetLastRowId();
	return id.ToLong();
}

void TagsDatabase::DeleteByFilePrefix(const wxFileName& dbpath, const wxString& filePrefix)
{
	// make sure database is open
	OpenDatabase(dbpath);

	try {
		wxString sql;
		wxString name(filePrefix);
		name.Replace(wxT("_"), wxT("^_"));

		// clear the cache
		ClearCache();

		sql << wxT("delete from tags where file like '") << name << wxT("%%' ESCAPE '^' ");
		m_db->ExecuteUpdate(sql);

	} catch (wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
}

void TagsDatabase::GetFiles(std::vector<FileEntryPtr>& files)
{
	try {
		wxString query(wxT("select * from files order by file"));
		wxSQLite3ResultSet res = m_db->ExecuteQuery(query);

		while (res.NextRow()) {

			FileEntryPtr fe(new FileEntry());
			fe->SetId(res.GetInt(0));
			fe->SetFile(res.GetString(1));
			fe->SetLastRetaggedTimestamp(res.GetInt(2));

			files.push_back( fe );
		}

	} catch (wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
}

void TagsDatabase::DeleteFromFiles(const wxArrayString& files)
{
	if (files.IsEmpty()) {
		return;
	}

	wxString query;
	query << wxT("delete from FILES where file in (");

	for (size_t i=0; i<files.GetCount(); i++) {
		query << wxT("'") << files.Item(i) << wxT("',");
	}

	// remove last ','
	query.RemoveLast();
	query << wxT(")");

	try {
		m_db->ExecuteQuery(query);
	} catch (wxSQLite3Exception &e) {
		wxUnusedVar(e);
	}
}

void TagsDatabase::DeleteFromFilesByPrefix(const wxFileName& dbpath, const wxString& filePrefix)
{
	// make sure database is open
	OpenDatabase(dbpath);

	try {
		wxString sql;
		wxString name(filePrefix);
		name.Replace(wxT("_"), wxT("^_"));

		sql << wxT("delete from FILES where file like '") << name << wxT("%%' ESCAPE '^' ");
		m_db->ExecuteUpdate(sql);

	} catch (wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
}

bool TagsDatabase::GetCacheTags(const wxString& sql, std::vector<TagEntryPtr>& tags)
{
	// is it enabled?
	if ( !m_useCache ) {
		return false;
	}

	TagCacheEntryPtr entry = m_cache->FindByQuery( sql );
	if ( entry ) {
		tags.insert(tags.end(), entry->GetTags().begin(), entry->GetTags().end());
		return true;
	}
	return false;
}

void TagsDatabase::CacheTags(const wxString& sql, const std::vector<TagEntryPtr>& tags)
{
	if ( m_useCache ) {
		m_cache->AddEntry( new TagCacheEntry(sql, tags) );
	}
}

void TagsDatabase::ClearCache()
{
	if ( m_useCache ) {
		m_cache->Clear();
	}
}


int TagsDatabase::GetCacheHitRate()
{
	if ( !m_useCache ) {
		return 0;
	}

	return m_cache->GetHitRate();
}


void TagsDatabase::DeleteCachedEntriesByRelation(const std::vector<std::pair<wxString, TagEntry> >& tags)
{
	if ( m_useCache ) {
		m_cache->DeleteEntriesByRelation(tags);
	}
}


void TagsDatabase::SetMaxCacheSize(int size)
{
	if (m_useCache) {
		if (size<500) size = 500;
		m_cache->SetMaxCacheSize((size_t)size);
	}
}


int TagsDatabase::GetCacheItemsCount()
{
	return m_cache->GetItemCount();
}

void TagsDatabase::DoFixPath(TagEntryPtr& tag)
{
	wxUnusedVar(tag);
}

void TagsDatabase::DoFetchTags(const wxString& sql, std::vector<TagEntryPtr>& tags)
{
	// try the cache first
	if ( GetCacheTags(sql, tags) )
		return;

	try {
		wxSQLite3ResultSet ex_rs;
		ex_rs = Query(sql);

		// add results from external database to the workspace database
		std::vector<TagEntryPtr> tmpTags;
		while ( ex_rs.NextRow() ) {
			// Construct a TagEntry from the rescord set
			TagEntryPtr tag(new TagEntry(ex_rs));
			//conver the path to be real path
			DoFixPath( tag );
			tmpTags.push_back(tag);
		}

		// Add results to cache
		// EI: to avoid errors, dont add to cache empty results
		// since they might be added later
		if ( tmpTags.empty() == false ) {
			CacheTags(sql, tmpTags);
		}

		// Append the matched results to the tags we collected so far
		tags.insert(tags.end(), tmpTags.begin(), tmpTags.end());
		ex_rs.Finalize();

	} catch (wxSQLite3Exception &e) {
		wxLogMessage(e.GetMessage());
	}
}

void TagsDatabase::GetTagsByScopeAndName(const wxString& scope, const wxString& name, bool partialNameAllowed, std::vector<TagEntryPtr> &tags)
{
	wxString sql;
	wxString tmpName(name);
	tmpName.Replace(wxT("_"), wxT("^_"));

	sql << wxT("select * from tags where ");

	// did we get scope?
	if ( scope.IsEmpty() == false ) {
		sql << wxT("scope='") << scope << wxT("' and ");
	}

	// add the name condition
	if (partialNameAllowed) {
		sql << wxT(" name like '") << tmpName << wxT("%%' ESCAPE '^' ");
	} else {
		sql << wxT(" name ='") << name << wxT("' ");
	}

	// get get the tags
	DoFetchTags(sql, tags);
}

void TagsDatabase::GetTagsByScope(const wxString& scope, std::vector<TagEntryPtr>& tags)
{
	wxString sql;
	// Build the SQL statement
	sql << wxT("select * from tags where scope='") << scope << wxT("'");

	DoFetchTags(sql, tags);
}

void TagsDatabase::GetTagsByKind(const wxArrayString& kinds, const wxString &orderingColumn, int order, std::vector<TagEntryPtr>& tags)
{
	wxString sql;
	sql << wxT("select * from tags where kind in (");
	for (size_t i=0; i<kinds.GetCount(); i++) {
		sql << wxT("'") << kinds.Item(i) << wxT("',");
	}
	sql.RemoveLast();
	sql << wxT(") ");

	if ( orderingColumn.IsEmpty() == false ) {
		sql << wxT("order by ") << orderingColumn;
		switch (order) {
		case ITagsStorage::OrderAsc:
			sql << wxT(" ASC");
			break;
		case ITagsStorage::OrderDesc:
			sql << wxT(" DESC");
			break;
		case ITagsStorage::OrderNone:
		default:
			break;
		}
	}

	DoFetchTags(sql, tags);
}


void TagsDatabase::GetTagsByPath(const wxString& path, std::vector<TagEntryPtr>& tags)
{
	wxString sql;
	sql << wxT("select * from tags where path='") << path << wxT("' ");
	DoFetchTags(sql, tags);
}

void TagsDatabase::GetTagsByNameAndParent(const wxString& name, const wxString& parent, std::vector<TagEntryPtr>& tags)
{
	wxString sql;
	sql << wxT("select * from tags where name='") << name << wxT("' and parent='") << parent << wxT("'");

	DoFetchTags(sql, tags);
}

void TagsDatabase::GetTagsByKindAndPath(const wxArrayString& kinds, const wxString& path, std::vector<TagEntryPtr>& tags)
{
	if ( kinds.empty() ) {
		return;
	}

	wxString sql;
	sql << wxT("select * from tags where path='") << path << wxT("' and kind in (");
	for (size_t i=0; i<kinds.GetCount(); i++) {
		sql << wxT("'") << kinds.Item(i) << wxT("',");
	}
	sql.RemoveLast();
	sql << wxT(")");

	DoFetchTags(sql, tags);
}


void TagsDatabase::GetTagsByFileAndLine(const wxString& file, int line, std::vector<TagEntryPtr>& tags)
{
	wxString sql;
	sql << wxT("select * from tags where file='") << file << wxT("' and line=") << line <<  wxT(" ");
	DoFetchTags(sql, tags);
}

void TagsDatabase::GetTagsByScopeAndKind(const wxString& scope, const wxArrayString& kinds, std::vector<TagEntryPtr>& tags)
{
	if ( kinds.empty() ) {
		return;
	}

	wxString sql;
	sql << wxT("select * from tags where scope='") << scope << wxT("' and kind in (");
	for (size_t i=0; i<kinds.GetCount(); i++) {
		sql << wxT("'") << kinds.Item(i) << wxT("',");
	}
	sql.RemoveLast();
	sql << wxT(")");

	DoFetchTags(sql, tags);
}

void TagsDatabase::GetTagsByKindAndFile(const wxArrayString& kind, const wxString &fileName, const wxString& orderingColumn, int order, std::vector<TagEntryPtr>& tags)
{
	if ( kind.empty() ) {
		return;
	}

	wxString sql;
	sql << wxT("select * from tags where file='") << fileName << wxT("' and kind in (");

	for (size_t i=0; i<kind.GetCount(); i++) {
		sql << wxT("'") << kind.Item(i) << wxT("',");
	}
	sql.RemoveLast();
	sql << wxT(")");

	if ( orderingColumn.IsEmpty() == false ) {
		sql << wxT("order by ") << orderingColumn;
		switch (order) {
		case ITagsStorage::OrderAsc:
			sql << wxT(" ASC");
			break;
		case ITagsStorage::OrderDesc:
			sql << wxT(" DESC");
			break;
		case ITagsStorage::OrderNone:
		default:
			break;
		}
	}
	DoFetchTags(sql, tags);
}
