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
#include "precompiled_header.h"
#include <wx/longlong.h>
#include "tags_database.h"


//-------------------------------------------------
// Tags database class implementation
//-------------------------------------------------
TagsStorageSQLite::TagsStorageSQLite()
		: ITagsStorage()
{
	m_db    = new wxSQLite3Database();
}

TagsStorageSQLite::~TagsStorageSQLite()
{
	if (m_db) {
		m_db->Close();
		delete m_db;
		m_db = NULL;
	}
}

void TagsStorageSQLite::OpenDatabase(const wxFileName& fileName)
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

	try {
		if (!m_fileName.IsOk()) {
			// First time we open the db
			m_db->Open(fileName.GetFullPath());
			CreateSchema();
			m_fileName = fileName;

		} else {
			// We have both fileName & m_fileName and they
			// are different, Close previous db
			m_db->Close();
			m_db->Open(fileName.GetFullPath());
			CreateSchema();
			m_fileName = fileName;

		}

	} catch (wxSQLite3Exception &e) {
		wxUnusedVar(e);
	}
}

void TagsStorageSQLite::CreateSchema()
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

//		sql = wxT("PRAGMA default_cache_size = 2000;");
//		m_db->ExecuteUpdate(sql);

		sql = wxT("create  table if not exists tags (ID INTEGER PRIMARY KEY AUTOINCREMENT, name string, file string, line integer, kind string, access string, signature string, pattern string, parent string, inherits string, path string, typeref string, scope string);");
		m_db->ExecuteUpdate(sql);

		sql = wxT("create  table if not exists FILES (ID INTEGER PRIMARY KEY AUTOINCREMENT, file string, last_retagged integer);");
		m_db->ExecuteUpdate(sql);

		// create unuque index on Files' file column
		sql = wxT("CREATE UNIQUE INDEX IF NOT EXISTS FILES_NAME on FILES(file)");
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

		sql = wxT("create table if not exists tags_version (version string primary key);");
		m_db->ExecuteUpdate(sql);

		sql = wxT("create unique index if not exists tags_version_uniq on tags_version(version);");
		m_db->ExecuteUpdate(sql);

		sql = wxString(wxT("insert into tags_version values ('")) << GetVersion() << wxT("');");
		m_db->ExecuteUpdate(sql);

	} catch (wxSQLite3Exception &e) {
		wxUnusedVar(e);
	}
}

void TagsStorageSQLite::RecreateDatabase()
{
	try {
		// commit any open transactions
		Commit();

		// Close the database
		m_db->Close();
		wxString filename = m_fileName.GetFullPath();
		if(wxRemoveFile(m_fileName.GetFullPath()) == false ) {

			// faild to delete it, probably someone else got it opened as well
			m_fileName.Clear();
			m_db->Open( filename ); // re-open the database

			// and drop tables
			m_db->ExecuteUpdate(wxT("DROP TABLE IF EXISTS TAGS"));
			m_db->ExecuteUpdate(wxT("DROP TABLE IF EXISTS COMMENTS"));
			m_db->ExecuteUpdate(wxT("DROP TABLE IF EXISTS TAGS_VERSION"));
			m_db->ExecuteUpdate(wxT("DROP TABLE IF EXISTS VARIABLES"));
			m_db->ExecuteUpdate(wxT("DROP TABLE IF EXISTS FILES"));

			// drop indexes
			m_db->ExecuteUpdate(wxT("DROP INDEX IF EXISTS FILES_NAME"));
			m_db->ExecuteUpdate(wxT("DROP INDEX IF EXISTS TAGS_UNIQ"));
			m_db->ExecuteUpdate(wxT("DROP INDEX IF EXISTS KIND_IDX"));
			m_db->ExecuteUpdate(wxT("DROP INDEX IF EXISTS FILE_IDX"));
			m_db->ExecuteUpdate(wxT("DROP INDEX IF EXISTS TAGS_NAME"));
			m_db->ExecuteUpdate(wxT("DROP INDEX IF EXISTS TAGS_SCOPE"));
			m_db->ExecuteUpdate(wxT("DROP INDEX IF EXISTS TAGS_PATH"));
			m_db->ExecuteUpdate(wxT("DROP INDEX IF EXISTS TAGS_PARENT"));
			m_db->ExecuteUpdate(wxT("DROP INDEX IF EXISTS tags_version_uniq"));

			// Recreate the schema
			CreateSchema();
		} else {
			// We managed to delete the file
			// re-open it

			m_fileName.Clear();
			OpenDatabase(filename);
		}
	} catch (wxSQLite3Exception &e) {
		wxUnusedVar(e);
	}
}

wxString TagsStorageSQLite::GetSchemaVersion() const
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


void TagsStorageSQLite::Store(TagTreePtr tree, const wxFileName& path, bool autoCommit)
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
		std::vector<TagEntry> updateList;

		// AddChild entries to database
		if ( autoCommit )
			m_db->Begin();

		for (; !walker.End(); walker++) {
			// Skip root node
			if (walker.GetNode() == tree->GetRoot())
				continue;

			// does not matter if we insert or update, the cache must be cleared for any related tags

			if (InsertTagEntry(walker.GetNode()->GetData()) == TagExist) {
				// Update the record
				updateList.push_back(walker.GetNode()->GetData());
			}
		}

		if ( autoCommit )
			m_db->Commit();

		// Do we need to update?
		if (!updateList.empty()) {
			if ( autoCommit )
				m_db->Begin();

			for (size_t i=0; i<updateList.size(); i++)
				UpdateTagEntry(updateList.at(i));

			if ( autoCommit )
				m_db->Commit();
		}
	} catch (wxSQLite3Exception& e) {
		try {
			if ( autoCommit )
				m_db->Rollback();
		} catch (wxSQLite3Exception& WXUNUSED(e1)) {
			wxUnusedVar(e);
		}
	}
}

void TagsStorageSQLite::SelectTagsByFile(const wxString& file, std::vector<TagEntryPtr> &tags, const wxFileName& path)
{
	// Incase empty file path is provided, use the current file name
	wxFileName databaseFileName(path);
	path.IsOk() == false ? databaseFileName = m_fileName : databaseFileName = path;
	OpenDatabase(databaseFileName);

	wxString query;
	query << wxT("select * from tags where file='") << file << wxT("' ");

	DoFetchTags(query, tags);
}

void TagsStorageSQLite::DeleteByFileName(const wxFileName& path, const wxString& fileName, bool autoCommit)
{
	// make sure database is open


	try {
		OpenDatabase(path);

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

wxSQLite3ResultSet TagsStorageSQLite::Query(const wxString& sql, const wxFileName& path)
{
	// make sure database is open

	try {
		OpenDatabase(path);
		return m_db->ExecuteQuery(sql);
	} catch (wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
	return wxSQLite3ResultSet();
}

void TagsStorageSQLite::ExecuteUpdate(const wxString& sql)
{
	try {
		m_db->ExecuteUpdate(sql);
	} catch (wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
}

const bool TagsStorageSQLite::IsOpen() const
{
	return m_db->IsOpen();
}

void TagsStorageSQLite::GetFiles(const wxString &partialName, std::vector<FileEntryPtr> &files)
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

long TagsStorageSQLite::LastRowId() const
{
	wxLongLong id = m_db->GetLastRowId();
	return id.ToLong();
}

void TagsStorageSQLite::DeleteByFilePrefix(const wxFileName& dbpath, const wxString& filePrefix)
{
	// make sure database is open


	try {
		OpenDatabase(dbpath);
		wxString sql;
		wxString name(filePrefix);
		name.Replace(wxT("_"), wxT("^_"));

		sql << wxT("delete from tags where file like '") << name << wxT("%%' ESCAPE '^' ");
		m_db->ExecuteUpdate(sql);

	} catch (wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
}

void TagsStorageSQLite::GetFiles(std::vector<FileEntryPtr>& files)
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

void TagsStorageSQLite::DeleteFromFiles(const wxArrayString& files)
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

void TagsStorageSQLite::DeleteFromFilesByPrefix(const wxFileName& dbpath, const wxString& filePrefix)
{
	// make sure database is open


	try {
		OpenDatabase(dbpath);
		wxString sql;
		wxString name(filePrefix);
		name.Replace(wxT("_"), wxT("^_"));

		sql << wxT("delete from FILES where file like '") << name << wxT("%%' ESCAPE '^' ");
		m_db->ExecuteUpdate(sql);

	} catch (wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
}

TagEntry* TagsStorageSQLite::FromSQLite3ResultSet(wxSQLite3ResultSet& rs)
{
	TagEntry *entry = new TagEntry();
	entry->SetId       (rs.GetInt(0)   );
	entry->SetName     (rs.GetString(1));
	entry->SetFile     (rs.GetString(2));
	entry->SetLine     (rs.GetInt(3)   );
	entry->SetKind     (rs.GetString(4));
	entry->SetAccess   (rs.GetString(5));
	entry->SetSignature(rs.GetString(6));
	entry->SetPattern  (rs.GetString(7));
	entry->SetParent   (rs.GetString(8));
	entry->SetInherits (rs.GetString(9));
	entry->SetPath     (rs.GetString(10));
	entry->SetTyperef  (rs.GetString(11));
	entry->SetScope    (rs.GetString(12));
	return entry;
}

void TagsStorageSQLite::DoFetchTags(const wxString& sql, std::vector<TagEntryPtr>& tags)
{
	// try the cache first
	try {
		wxSQLite3ResultSet ex_rs;
		ex_rs = Query(sql);

		// add results from external database to the workspace database
		while ( ex_rs.NextRow() ) {
			// Construct a TagEntry from the rescord set
			TagEntryPtr tag(FromSQLite3ResultSet(ex_rs));
			//conver the path to be real path
			tags.push_back(tag);
		}
		ex_rs.Finalize();
	} catch (wxSQLite3Exception &e) {
		wxUnusedVar ( e );
	}
}

void TagsStorageSQLite::DoFetchTags(const wxString& sql, std::vector<TagEntryPtr>& tags, const wxArrayString& kinds)
{
	try {
		wxSQLite3ResultSet ex_rs;
		ex_rs = Query(sql);

		// add results from external database to the workspace database
		while ( ex_rs.NextRow() ) {
			// check if this kind is accepted
			if ( kinds.Index(ex_rs.GetString(4)) != wxNOT_FOUND ) {

				// Construct a TagEntry from the rescord set
				TagEntryPtr tag(FromSQLite3ResultSet(ex_rs));

				//conver the path to be real path
				tags.push_back(tag);
			}
		}
		ex_rs.Finalize();

	} catch (wxSQLite3Exception &e) {
		wxUnusedVar ( e );
	}
}

void TagsStorageSQLite::GetTagsByScopeAndName(const wxString& scope, const wxString& name, bool partialNameAllowed, std::vector<TagEntryPtr> &tags)
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
	sql << wxT(" LIMIT ") << this->GetSingleSearchLimit();

	// get get the tags
	DoFetchTags(sql, tags);
}

void TagsStorageSQLite::GetTagsByScope(const wxString& scope, std::vector<TagEntryPtr>& tags)
{
	wxString sql;

	// Build the SQL statement
	sql << wxT("select * from tags where scope='") << scope << wxT("' limit ") << GetSingleSearchLimit();

	DoFetchTags(sql, tags);
}

void TagsStorageSQLite::GetTagsByKind(const wxArrayString& kinds, const wxString &orderingColumn, int order, std::vector<TagEntryPtr>& tags)
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


void TagsStorageSQLite::GetTagsByPath(const wxArrayString& path, std::vector<TagEntryPtr>& tags)
{
	if (path.empty()) return;

	wxString sql;

	sql << wxT("select * from tags where path IN(");
	for (size_t i=0; i<path.GetCount(); i++) {
		sql << wxT("'") << path.Item(i) << wxT("',");
	}
	sql.RemoveLast();
	sql << wxT(")");
	DoFetchTags(sql, tags);
}

void TagsStorageSQLite::GetTagsByNameAndParent(const wxString& name, const wxString& parent, std::vector<TagEntryPtr>& tags)
{
	wxString sql;
	sql << wxT("select * from tags where name='") << name << wxT("'");

	std::vector<TagEntryPtr> tmpResults;
	DoFetchTags(sql, tmpResults);

	// Filter by parent
	for (size_t i=0; i<tmpResults.size(); i++) {
		if (tmpResults.at(i)->GetParent() == parent) {
			tags.push_back( tmpResults.at(i) );
		}
	}
}

void TagsStorageSQLite::GetTagsByKindAndPath(const wxArrayString& kinds, const wxString& path, std::vector<TagEntryPtr>& tags)
{
	if ( kinds.empty() ) {
		return;
	}

	wxString sql;
	sql << wxT("select * from tags where path='") << path << wxT("'");

	DoFetchTags(sql, tags, kinds);
}

void TagsStorageSQLite::GetTagsByFileAndLine(const wxString& file, int line, std::vector<TagEntryPtr>& tags)
{
	wxString sql;
	sql << wxT("select * from tags where file='") << file << wxT("' and line=") << line <<  wxT(" ");
	DoFetchTags(sql, tags);
}

void TagsStorageSQLite::GetTagsByScopeAndKind(const wxString& scope, const wxArrayString& kinds, std::vector<TagEntryPtr>& tags)
{
	if ( kinds.empty() ) {
		return;
	}

	wxString sql;
	sql << wxT("select * from tags where scope='") << scope << wxT("'");
	DoFetchTags(sql, tags, kinds);
}

void TagsStorageSQLite::GetTagsByKindAndFile(const wxArrayString& kind, const wxString &fileName, const wxString& orderingColumn, int order, std::vector<TagEntryPtr>& tags)
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

int TagsStorageSQLite::DeleteFileEntry(const wxString& filename)
{
	try {
		wxSQLite3Statement statement = m_db->PrepareStatement(wxT("DELETE FROM FILES WHERE FILE=?"));
		statement.Bind(1, filename);
		statement.ExecuteUpdate();

	} catch (wxSQLite3Exception& exc) {
		if (exc.ErrorCodeAsString(exc.GetErrorCode()) == wxT("SQLITE_CONSTRAINT"))
			return TagExist;
		return TagError;
	}
	return TagOk;
}


int TagsStorageSQLite::InsertFileEntry(const wxString& filename, int timestamp)
{
	try {
		wxSQLite3Statement statement = m_db->PrepareStatement(wxT("INSERT INTO FILES VALUES(NULL, ?, ?)"));
		statement.Bind(1, filename);
		statement.Bind(2, timestamp);
		statement.ExecuteUpdate();

	} catch (wxSQLite3Exception& exc) {
		if (exc.ErrorCodeAsString(exc.GetErrorCode()) == wxT("SQLITE_CONSTRAINT"))
			return TagExist;
		return TagError;
	}
	return TagOk;
}


int TagsStorageSQLite::UpdateFileEntry(const wxString& filename, int timestamp)
{
	try {
		wxSQLite3Statement statement = m_db->PrepareStatement(wxT("UPDATE FILES SET last_retagged=? WHERE file=?"));
		statement.Bind(1,  timestamp);
		statement.Bind(2,  filename);
		statement.ExecuteUpdate();

	} catch (wxSQLite3Exception& exc) {
		if (exc.ErrorCodeAsString(exc.GetErrorCode()) == wxT("SQLITE_CONSTRAINT"))
			return TagExist;
		return TagError;
	}
	return TagOk;
}

int TagsStorageSQLite::DeleteTagEntry(const wxString& kind, const wxString& signature, const wxString& path)
{
	// Delete this record from database.
	// Delete is done using the index
	try {
		wxSQLite3Statement statement = m_db->PrepareStatement(wxT("DELETE FROM TAGS WHERE Kind=? AND Signature=? AND Path=?"));
		statement.Bind(1, kind);        // Kind
		statement.Bind(2, signature);   // Signature
		statement.Bind(3, path);        // Path
		statement.ExecuteUpdate();
	} catch (wxSQLite3Exception& exc) {
		wxUnusedVar(exc);
		return TagError;
	}
	return TagOk;
}

int TagsStorageSQLite::InsertTagEntry(const TagEntry& tag)
{
	// If this node is a dummy, (IsOk() == false) we dont insert it to database
	if ( !tag.IsOk() )
		return TagOk;

	try {
		wxSQLite3Statement statement = m_db->PrepareStatement(wxT("INSERT INTO TAGS VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"));
		statement.Bind(1,  tag.GetName());
		statement.Bind(2,  tag.GetFile());
		statement.Bind(3,  tag.GetLine());
		statement.Bind(4,  tag.GetKind());
		statement.Bind(5,  tag.GetAccess());
		statement.Bind(6,  tag.GetSignature());
		statement.Bind(7,  tag.GetPattern());
		statement.Bind(8,  tag.GetParent());
		statement.Bind(9,  tag.GetInherits());
		statement.Bind(10, tag.GetPath());
		statement.Bind(11, tag.GetTyperef());
		statement.Bind(12, tag.GetScope());
		statement.ExecuteUpdate();
	} catch (wxSQLite3Exception& exc) {

		if (exc.ErrorCodeAsString(exc.GetErrorCode()) == wxT("SQLITE_CONSTRAINT"))
			return TagExist;

		return TagError;
	}
	return TagOk;

}

int TagsStorageSQLite::UpdateTagEntry(const TagEntry& tag)
{
	if ( !tag.IsOk() )
		return TagOk;

	try {
		wxSQLite3Statement statement = m_db->PrepareStatement(wxT("UPDATE TAGS SET Name=?, File=?, Line=?, Access=?, Pattern=?, Parent=?, Inherits=?, Typeref=?, Scope=? WHERE Kind=? AND Signature=? AND Path=?"));
		// update
		statement.Bind(1,  tag.GetName());
		statement.Bind(2,  tag.GetFile());
		statement.Bind(3,  tag.GetLine());
		statement.Bind(4,  tag.GetAccess());
		statement.Bind(5,  tag.GetPattern());
		statement.Bind(6,  tag.GetParent());
		statement.Bind(7,  tag.GetInherits());
		statement.Bind(8,  tag.GetTyperef());
		statement.Bind(9,  tag.GetScope());

		// where?
		statement.Bind(10, tag.GetKind());
		statement.Bind(11, tag.GetSignature());
		statement.Bind(12, tag.GetPath());

		statement.ExecuteUpdate();
	} catch (wxSQLite3Exception& exc) {
		if (exc.ErrorCodeAsString(exc.GetErrorCode()) == wxT("SQLITE_CONSTRAINT"))
			return TagExist;
		return TagError;
	}
	return TagOk;
}

bool TagsStorageSQLite::IsTypeAndScopeContainer(const wxString& typeName, wxString& scope)
{
	wxString sql;
	sql << wxT("select scope,kind from tags where name='") << typeName << wxT("'");

	bool found_global(false);

	try {
		wxSQLite3ResultSet rs = Query(sql);
		while (rs.NextRow()) {
			wxString scopeFounded (rs.GetString(0));
			wxString kindFounded  (rs.GetString(1));

			bool containerKind = kindFounded == wxT("struct") || kindFounded == wxT("class");
			if (scopeFounded == scope && containerKind) {
				//we got an exact match
				return true;
			} else if ( containerKind && scopeFounded == wxT("<global>") ) {
				found_global = true;
			}
		}

	} catch ( wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}

	// if we reached here, it means we did not find any exact match
	if ( found_global ) {
		scope = wxT("<global>");
		return true;
	}
	return false;
}


bool TagsStorageSQLite::IsTypeAndScopeExist(const wxString& typeName, wxString& scope)
{
	wxString sql;
	sql << wxT("select scope from tags where name='") << typeName << wxT("'");
	bool found_global(false);
	try {
		wxSQLite3ResultSet rs = Query(sql);
		while (rs.NextRow()) {
			wxString scopeFounded (rs.GetString(0));
			if ( scopeFounded == scope ) {
				// exact match
				return true;
			} else if ( scopeFounded == wxT("<global>") ) {
				found_global = true;
			}
		}

	} catch ( wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}

	// if we reached here, it means we did not find any exact match
	if ( found_global ) {
		scope = wxT("<global>");
		return true;
	}
	return false;
}

void TagsStorageSQLite::GetScopesFromFileAsc(const wxFileName& fileName, std::vector<wxString>& scopes)
{
	wxString sql;
	sql << wxT("select distinct scope from tags where file = '")
	<< fileName.GetFullPath() << wxT("' ")
	<< wxT(" and kind in('prototype', 'function', 'enum')")
	<< wxT(" order by scope ASC");

	//we take the first entry
	try {
		wxSQLite3ResultSet rs = Query(sql);
		while ( rs.NextRow() ) {
			scopes.push_back(rs.GetString(0));
		}
		rs.Finalize();
	} catch ( wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
}

void TagsStorageSQLite::GetTagsByFileScopeAndKind(const wxFileName& fileName, const wxString& scopeName, const wxArrayString& kind, std::vector<TagEntryPtr>& tags)
{
	wxString sql;
	sql << wxT("select * from tags where file = '")
	<< fileName.GetFullPath() << wxT("' ")
	<< wxT(" and scope='") << scopeName << wxT("' ");

	if ( kind.IsEmpty() == false ) {
		sql << wxT(" and kind in(");
		for (size_t i=0; i<kind.GetCount(); i++) {
			sql << wxT("'") << kind.Item(i) << wxT("',");
		}
		sql.RemoveLast();
		sql << wxT(")");
	}

	DoFetchTags(sql, tags);
}

void TagsStorageSQLite::GetAllTagsNames(wxArrayString& names)
{
	try {

		wxString query(wxT("SELECT distinct name FROM tags order by name ASC LIMIT "));
		query  << GetMaxWorkspaceTagToColour();

		wxSQLite3ResultSet res = Query(query);
		while (res.NextRow()) {
			// add unique strings only
			names.Add( res.GetString(0) );
		}

	} catch (wxSQLite3Exception &e) {
		wxUnusedVar(e);
	}
}

void TagsStorageSQLite::GetTagsNames(const wxArrayString& kind, wxArrayString& names)
{
	if ( kind.IsEmpty() ) return;
	try {

		wxString whereClause;
		whereClause << wxT(" kind IN (");
		for (size_t i=0; i<kind.GetCount(); i++) {
			whereClause << wxT("'") << kind.Item(i) << wxT("',");
		}

		whereClause = whereClause.BeforeLast(wxT(','));
		whereClause << wxT(") ");

		wxString query(wxT("SELECT distinct name FROM tags WHERE "));
		query << whereClause << wxT(" order by name ASC LIMIT ") << GetMaxWorkspaceTagToColour();

		wxSQLite3ResultSet res = Query(query);
		while (res.NextRow()) {
			// add unique strings only
			names.Add( res.GetString(0) );
		}

	} catch (wxSQLite3Exception &e) {
		wxUnusedVar(e);
	}
}

void TagsStorageSQLite::GetTagsByScopesAndKind(const wxArrayString& scopes, const wxArrayString& kinds, std::vector<TagEntryPtr>& tags)
{
	if ( kinds.empty() || scopes.empty() ) {
		return;
	}

	wxString sql;
	sql << wxT("select * from tags where scope in (");
	for (size_t i=0; i<scopes.GetCount(); i++) {
		sql << wxT("'") << scopes.Item(i) << wxT("',");
	}
	sql.RemoveLast();
	sql << wxT(") ");

	DoFetchTags(sql, tags, kinds);
}

void TagsStorageSQLite::GetTagsByPath(const wxString& path, std::vector<TagEntryPtr>& tags)
{
	if (path.empty()) return;

	wxString sql;
	sql << wxT("select * from tags where path ='") << path << wxT("'");
	DoFetchTags(sql, tags);
}

void TagsStorageSQLite::GetTagsByScopeAndName(const wxArrayString& scope, const wxString& name, bool partialNameAllowed, std::vector<TagEntryPtr>& tags)
{
	if (scope.empty()) return;

	wxString sql;
	wxString tmpName(name);
	tmpName.Replace(wxT("_"), wxT("^_"));

	sql << wxT("select * from tags where scope in(");

	for (size_t i=0; i<scope.GetCount(); i++) {
		sql <<wxT("'")<<scope.Item(i)<<wxT("',");
	}
	sql.RemoveLast();
	sql << wxT(") and ");
	// add the name condition
	if (partialNameAllowed) {
		sql << wxT(" name like '") << tmpName << wxT("%%' ESCAPE '^' ");
	} else {
		sql << wxT(" name ='") << name << wxT("' ");
	}

	// get get the tags
	DoFetchTags(sql, tags);
}
