#include "tokendb.h"
#include <wx/wxsqlite3.h>

TokenDb::TokenDb()
		: m_db(new wxSQLite3Database())
{
}

TokenDb::~TokenDb()
{
	delete m_db;
	m_db = NULL;
}

void TokenDb::BeginTransaction()
{
	try {
		m_db->Begin();
	} catch (wxSQLite3Exception &e) {
		wxUnusedVar(e);
	}
}

void TokenDb::Commit()
{
	try {
		m_db->Commit();
	} catch (wxSQLite3Exception &e) {
		wxUnusedVar(e);
	}
}

void TokenDb::Open(const wxString& file_name)
{
	wxString sql;
	try {
		m_db->Open(file_name);
		
		// create the database schema
		sql = wxT("PRAGMA synchronous = OFF;");
		m_db->ExecuteUpdate(sql);

		sql = wxT("PRAGMA temp_store = MEMORY;");
		m_db->ExecuteUpdate(sql);

		sql = wxT("PRAGMA default_cache_size = 20000;");
		m_db->ExecuteUpdate(sql);
	
		sql = wxT("create  table if not exists TOKENS (ID INTEGER PRIMARY KEY AUTOINCREMENT, name string, file string, offset integer)");
		m_db->ExecuteUpdate(sql);
		
		// Create search indexes
		sql = wxT("CREATE INDEX IF NOT EXISTS TOKEN_NAME on TOKENS(name);");
		m_db->ExecuteUpdate(sql);
		
		sql = wxT("CREATE INDEX IF NOT EXISTS TOKEN_FILE on TOKENS(file);");
		m_db->ExecuteUpdate(sql);
		
	} catch (wxSQLite3Exception &e) {
		wxUnusedVar(e);
	}
}

void TokenDb::Store(const CppToken& token)
{
	try {
		wxString insertSql(wxT("INSERT INTO TOKENS VALUES (NULL, ?, ?, ?)"));
		wxSQLite3Statement insertStmt = m_db->PrepareStatement( insertSql );

		insertStmt.Bind(1, token.getName());
		insertStmt.Bind(2, token.getFilename());
		insertStmt.Bind(3, (int)token.getOffset());
		
		insertStmt.ExecuteUpdate();
		insertStmt.Reset();

	} catch (wxSQLite3Exception &e) {
		wxUnusedVar(e);
	}
}

void TokenDb::Fetch(const wxString& name, CppTokenList& l)
{
	wxString sql(wxT("select * from TOKENS where name='"));
	sql << name << wxT("'");
	
	try {
		wxSQLite3ResultSet rs = m_db->ExecuteQuery(sql);
		while ( rs.NextRow() ) {
			
			// construct CppToken from the result set
			CppToken token;
			token.setId(rs.GetInt(0));
			token.setName(rs.GetString(1));
			token.setFilename(rs.GetString(2));
			token.setOffset((size_t)rs.GetInt(3));
			
			l.push_back(token);
		}
		rs.Finalize();
	} catch ( wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
}

void TokenDb::DeleteByFile(const wxString& file)
{
	wxString sql(wxT("delete from TOKENS where file='"));
	sql << file << wxT("'");
	
	try {
		wxSQLite3ResultSet rs = m_db->ExecuteQuery(sql);
	} catch ( wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
	
}

void TokenDb::Rollback()
{
	try {
		m_db->Rollback();
	} catch (wxSQLite3Exception &e) {
		wxUnusedVar(e);
	}
}

void TokenDb::RecreateSchema()
{
	wxString sql;
	try {
		
		// create the database schema
		sql = wxT("PRAGMA synchronous = OFF;");
		m_db->ExecuteUpdate(sql);

		sql = wxT("PRAGMA temp_store = MEMORY;");
		m_db->ExecuteUpdate(sql);

		sql = wxT("PRAGMA default_cache_size = 20000;");
		m_db->ExecuteUpdate(sql);
		
		sql = wxT("drop table if exists TOKENS");
		m_db->ExecuteUpdate(sql);
		
		sql = wxT("create  table if not exists TOKENS (ID INTEGER PRIMARY KEY AUTOINCREMENT, name string, file string, offset integer)");
		m_db->ExecuteUpdate(sql);
		
		// Create search indexes
		sql = wxT("CREATE INDEX IF NOT EXISTS TOKEN_NAME on TOKENS(name);");
		m_db->ExecuteUpdate(sql);
		
		sql = wxT("CREATE INDEX IF NOT EXISTS TOKEN_FILE on TOKENS(file);");
		m_db->ExecuteUpdate(sql);
		
	} catch (wxSQLite3Exception &e) {
		wxUnusedVar(e);
	}
}
