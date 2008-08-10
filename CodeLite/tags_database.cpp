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

#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif 

//-------------------------------------------------
// Tags database class implementation
//-------------------------------------------------
TagsDatabase::TagsDatabase(bool extDb)
: m_extDb(extDb)
{
	m_db = new wxSQLite3Database();
}

TagsDatabase::~TagsDatabase()
{
	if(m_db)
	{
		m_db->Close();
		delete m_db;
		m_db = NULL;
	}
}

void TagsDatabase::OpenDatabase(const wxFileName& fileName)
{
	if(m_fileName == fileName)
		return;

	// Did we get a file name to use?
	if(!fileName.IsOk() && !m_fileName.IsOk())
		return;

	// We did not get any file name to use BUT we
	// do have an open database, so we will use it
	if(!fileName.IsOk())
		return;

	if(!m_fileName.IsOk())
	{
		// First time we open the db
		m_db->Open(fileName.GetFullPath());
		CreateSchema();
		m_fileName = fileName;
	}
	else
	{
		// We have both fileName & m_fileName and they 
		// are different, Close previous db
		m_db->Close();
		m_db->Open(fileName.GetFullPath());
		CreateSchema();
		m_fileName = fileName;
	}
}

void TagsDatabase::CreateSchema()
{
	wxString sql;

	// improve performace by using pragma command:
	// (this needs to be done before the creation of the
	// tables and indices)
	try
	{
		sql = wxT("PRAGMA temp_store = MEMORY;");
		m_db->ExecuteUpdate(sql);

		sql = wxT("PRAGMA default_cache_size = 20000;");
		m_db->ExecuteUpdate(sql);
	
		sql = wxT("create  table if not exists tags (ID INTEGER PRIMARY KEY AUTOINCREMENT, name string, file string, line integer, kind string, access string, signature string, pattern string, parent string, inherits string, path string, typeref string, scope string);");
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
	}
	catch(wxSQLite3Exception &e)
	{	
		wxUnusedVar(e);
	}
}

void TagsDatabase::RecreateDatabase()
{
	try
	{
		wxString sql;
		m_db->ExecuteUpdate(wxT("DROP TABLE IF EXISTS TAGS"));
		m_db->ExecuteUpdate(wxT("DROP TABLE IF EXISTS COMMENTS"));
		m_db->ExecuteUpdate(wxT("DROP TABLE IF EXISTS TAGS_VERSION"));
		m_db->ExecuteUpdate(wxT("DROP TABLE IF EXISTS VARIABLES"));

		//recreate the schema
		CreateSchema();
	}
	catch(wxSQLite3Exception &e)
	{
		wxUnusedVar(e);
	}
}


wxString TagsDatabase::GetSchemaVersion() const 
{
	// return the current schema version
	try 
	{
		wxString sql;
		wxString version;
		sql = wxT("SELECT * FROM TAGS_VERSION");
		wxSQLite3ResultSet rs = m_db->ExecuteQuery( sql );

		if( rs.NextRow() )
			version = rs.GetString(0);
		return version;
	}
	catch(wxSQLite3Exception &e )
	{
		wxUnusedVar(e);
	}
	return wxEmptyString;
}

void TagsDatabase::Store(const std::vector<DbRecordPtr> &records, const wxFileName& path, bool autoCommit)
{
	if(!path.IsOk() && !m_fileName.IsOk())
	{
		// An attempt is made to save the tree into db but no database
		// is provided and none is currently opened to use
		return;
	}

	if(records.empty())
		return;

	OpenDatabase(path);
	try
	{
		// Create the statements before the execution
		wxSQLite3Statement insertStmt = m_db->PrepareStatement(records[0]->GetInsertOneStatement());
		
		std::vector<DbRecordPtr> updateList;

		// AddChild entries to database
		// we break the bug transaction into samller ones of 1000 operations
		// each
		const size_t bulk = 1000;
		if( autoCommit )
			m_db->Begin();

		size_t i=0;
		for(; i<records.size(); i++)
		{
			if(records[i]->Store(insertStmt, this) == TagExist)
			{
				// Update the record
				updateList.push_back(records[i]);
			}
			else
			{
				// insert succeeded
				if(i % bulk == 0 && autoCommit)
				{
					m_db->Commit();
					m_db->Begin();
				}
			}
		}
		insertStmt.Finalize();

		if( autoCommit )
			m_db->Commit();

		// Do we need to update?
		if(!updateList.empty())
		{
			wxSQLite3Statement updateStmt = m_db->PrepareStatement( updateList[0]->GetUpdateOneStatement() );
			if( autoCommit )
				m_db->Begin();
			for(size_t i=0; i<updateList.size(); i++)
			{
				updateList[i]->Update(updateStmt);
				if( i % bulk == 0 && autoCommit )
				{
					m_db->Commit();
					m_db->Begin();
				}
			}
			updateStmt.Finalize();
			if( autoCommit )
				m_db->Commit();
		}
	}
	catch (wxSQLite3Exception& e)
	{
		wxUnusedVar(e);
		try{
			if( autoCommit )
				m_db->Rollback();
		}catch(wxSQLite3Exception& e1){
			wxUnusedVar(e1);
		}
	}	
}
    
void TagsDatabase::Store(TagTreePtr tree, const wxFileName& path, bool autoCommit)
{
	if(!path.IsOk() && !m_fileName.IsOk())
	{
		// An attempt is made to save the tree into db but no database
		// is provided and none is currently opened to use
		return;
	}
	
	if( !tree )
		return;

	OpenDatabase(path);
	TreeWalker<wxString, TagEntry> walker( tree->GetRoot() );

	try
	{
		// Create the statements before the execution
		TagEntry dummy;
		wxSQLite3Statement insertStmt = m_db->PrepareStatement( dummy.GetInsertOneStatement() );
		
		std::vector<TagEntry> updateList;

		// AddChild entries to database
		if( autoCommit )
			m_db->Begin();

		for(; !walker.End(); walker++)
		{
			// Skip root node
			if(walker.GetNode() == tree->GetRoot())
				continue;

			//walker.GetNode()->GetData().SetParentId(walker.GetNode()->GetParent()->GetData().GetId());
			if(walker.GetNode()->GetData().Store(insertStmt, this) == TagExist)
			{
				// Update the record
				updateList.push_back(walker.GetNode()->GetData());
			}
		}
		insertStmt.Finalize();

		if( autoCommit )
			m_db->Commit();

		// Do we need to update?
		if(!updateList.empty())
		{
			wxSQLite3Statement updateStmt = m_db->PrepareStatement( updateList[0].GetUpdateOneStatement() );
			if( autoCommit )
				m_db->Begin();
			for(size_t i=0; i<updateList.size(); i++)
				updateList[i].Update(updateStmt);
			updateStmt.Finalize();
			if( autoCommit )
				m_db->Commit();
		}
	}
	catch (wxSQLite3Exception& e)
	{
		wxUnusedVar(e);
		try{
			if( autoCommit )
				m_db->Rollback();
		}catch(wxSQLite3Exception& WXUNUSED(e1)){
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
	return m_db->ExecuteQuery(query.GetData());
}


void TagsDatabase::DeleteByFileName(const wxFileName& path, const wxString& fileName, bool autoCommit)
{
	// make sure database is open
	OpenDatabase(path);

	try	
	{
		if( autoCommit )
			m_db->Begin();
		m_db->ExecuteUpdate(wxString::Format(wxT("Delete from tags where File='%s'"), fileName.GetData()));

		if( autoCommit )
			m_db->Commit();
	}
	catch (wxSQLite3Exception& e)
	{
		wxUnusedVar(e);
		if( autoCommit )
			m_db->Rollback();
	}
}

wxSQLite3ResultSet TagsDatabase::Query(const wxString& sql, const wxFileName& path)
{
	// make sure database is open
	OpenDatabase(path);

	try	
	{
		return m_db->ExecuteQuery(sql);
	}
	catch (wxSQLite3Exception& e)
	{
		wxUnusedVar(e);
	}
	return wxSQLite3ResultSet();
}

void TagsDatabase::ExecuteUpdate(const wxString& sql)
{
	try	
	{
		m_db->ExecuteUpdate(sql);
	}
	catch (wxSQLite3Exception& e)
	{
		wxUnusedVar(e);
	}
}

const bool TagsDatabase::IsOpen() const
{
	return m_db->IsOpen();
}

void TagsDatabase::LoadToMemory(const wxFileName& fn)
{

	// this function is useful if our database is static and unlikely to be changed
	// A good candidate for it, is the external database (a database for static tags like, stl, wxwidgets symbols etc)
	
	// to load a database into memory, we do the following:
	// - attach to the file database and copy its schema to our memory
	// - copy the whole database content to our memory
	// - dettach the file database
	if(m_db->IsOpen())
	{
		// close any opened database and reopen it as in-memory
		m_db->Close();
	}

	try
	{
		m_db->Open(wxT(":memory:"));
		// copy database schema, we do it by opening a second database
		wxString sql;
		wxSQLite3Database *budb = new wxSQLite3Database();
		budb->Open(fn.GetFullPath());
	
		budb->Begin();
		wxSQLite3ResultSet rs = budb->ExecuteQuery( wxT("SELECT sql FROM sqlite_master WHERE sql NOT NULL") );
		while( rs.NextRow() )
		{
			sql = rs.GetString(0);
			if(sql.Find(wxT("sqlite_sequence")) == wxNOT_FOUND){
				m_db->ExecuteUpdate( sql );
			}
		}
		budb->Commit();
		budb->Close();
		delete budb;

		// insert all data from file database into memory db
		sql.Empty();
		sql << wxT("ATTACH DATABASE '") << fn.GetFullPath() << wxT("' as backup");
		m_db->ExecuteUpdate(sql);

		//copy tags table
		m_db->Begin();
		sql = wxT("insert into tags select id, name, file, line, kind, access, signature, pattern, parent, inherits, path, typeref, scope FROM backup.tags");
		m_db->ExecuteUpdate(sql);
		m_db->Commit();
		
		//copy comments table
		m_db->Begin();
		sql = wxT("insert into comments select comment, file, line FROM backup.comments");
		m_db->ExecuteUpdate(sql);
		m_db->Commit();

		//copy comments table
		m_db->Begin();
		sql = wxT("insert into variables select name, value FROM backup.variables");
		m_db->ExecuteUpdate(sql);
		m_db->Commit();		
	}
	catch(wxSQLite3Exception& e)
	{
		wxUnusedVar(e);
	}
}

TagEntryPtr TagsDatabase::FindTagById(int id) const
{
	wxString sql;
	sql << wxT("select * from tags where id=") << id;
	try
	{
		wxSQLite3ResultSet q = m_db->ExecuteQuery(sql);
		if(q.NextRow()){
			return TagEntryPtr(new TagEntry(q));
		}
	}
	catch(wxSQLite3Exception &e)
	{
		wxUnusedVar(e);
	}
	return NULL;
}

VariableEntryPtr TagsDatabase::FindVariableByName(const wxString &name) const
{
	wxString sql;
	sql << wxT("select * from variables where name='") << name << wxT("'");
	try
	{
		wxSQLite3ResultSet q = m_db->ExecuteQuery(sql);
		if(q.NextRow()){
			return VariableEntryPtr(new VariableEntry(q));
		}
	}
	catch(wxSQLite3Exception &e)
	{
		wxUnusedVar(e);
	}
	return NULL;
}

int TagsDatabase::Insert(DbRecordPtr record)
{
	try
	{
		// Create the statements before the execution
		wxSQLite3Statement insertStmt = m_db->PrepareStatement(record->GetInsertOneStatement());
		return record->Store(insertStmt, this);
	}
	catch (wxSQLite3Exception& e)
	{
		wxUnusedVar(e);
	}	
	return TagError;
}

int TagsDatabase::Delete(DbRecordPtr record)
{
	try
	{
		// Create the statements before the execution
		wxSQLite3Statement delStmnt = m_db->PrepareStatement(record->GetDeleteOneStatement());
		return record->Delete(delStmnt);
	}
	catch (wxSQLite3Exception& e)
	{
		wxUnusedVar(e);
	}	
	return TagError;
}

int TagsDatabase::Update(DbRecordPtr record)
{
	try
	{
		// Create the statements before the execution
		wxSQLite3Statement updStmnt = m_db->PrepareStatement(record->GetUpdateOneStatement());
		return record->Update(updStmnt);
	}
	catch (wxSQLite3Exception& e)
	{
		wxUnusedVar(e);
	}	
	return TagError;
}

void TagsDatabase::GetVariables(std::vector<VariableEntryPtr> &vars)
{
	try
	{
		wxSQLite3ResultSet res = m_db->ExecuteQuery(wxT("select * from variables"));
		while(res.NextRow()){
			vars.push_back(new VariableEntry(res));
		}
	}
	catch(wxSQLite3Exception &e)
	{
		wxUnusedVar(e);
	}
}

void TagsDatabase::GetFiles(const wxString &partialName, std::vector<wxFileName> &files)
{
	try
	{
		wxString query;
		wxString tmpName(partialName);
		tmpName.Replace(wxT("_"), wxT("^_"));
		query << wxT("select distinct file from tags where file like '%%") << tmpName << wxT("%%' ESCAPE '^' ")
			  << wxT("order by file");

		wxSQLite3ResultSet res = m_db->ExecuteQuery(query);
		while(res.NextRow()){
			wxFileName fileName(res.GetString(0));
			if(fileName.GetFullName().StartsWith(partialName)){
				files.push_back(fileName);
			}
		}
	}
	catch(wxSQLite3Exception &e)
	{
		wxUnusedVar(e);
	}
}

long TagsDatabase::LastRowId() const
{
	wxLongLong id = m_db->GetLastRowId();
	return id.ToLong();
}
