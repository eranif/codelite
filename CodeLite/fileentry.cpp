#include "fileentry.h"
#include "tags_database.h"

FileEntry::FileEntry()
		: m_id(wxNOT_FOUND)
		, m_file(wxEmptyString)
		, m_lastRetaggedTimestamp((int)time(NULL))
{
}

FileEntry::~FileEntry()
{
}

wxString FileEntry::GetDeleteOneStatement()
{
	return wxString(wxT("DELETE FROM FILES WHERE FILE=?"));
}

wxString FileEntry::GetInsertOneStatement()
{
	return wxString(wxT("INSERT INTO FILES VALUES(NULL, ?, ?)"));
}

wxString FileEntry::GetUpdateOneStatement()
{
	return wxString(wxT("UPDATE FILES SET last_retagged=? WHERE file=?"));
}

int FileEntry::Store(wxSQLite3Statement& insertPreparedStmnt, TagsDatabase* db)
{
	try {

		insertPreparedStmnt.Bind(1,  GetFile());
		insertPreparedStmnt.Bind(2,  GetLastRetaggedTimestamp());
		insertPreparedStmnt.ExecuteUpdate();
		insertPreparedStmnt.Reset();

	} catch (wxSQLite3Exception& exc) {
		if (exc.ErrorCodeAsString(exc.GetErrorCode()) == wxT("SQLITE_CONSTRAINT"))
			return TagExist;
		return TagError;
	}
	return TagOk;
}

int FileEntry::Update(wxSQLite3Statement& updatePreparedStmnt)
{
	try {

		updatePreparedStmnt.Bind(1,  GetLastRetaggedTimestamp());
		updatePreparedStmnt.Bind(2,  GetFile());
		updatePreparedStmnt.ExecuteUpdate();
		updatePreparedStmnt.Reset();

	} catch (wxSQLite3Exception& exc) {
		if (exc.ErrorCodeAsString(exc.GetErrorCode()) == wxT("SQLITE_CONSTRAINT"))
			return TagExist;
		return TagError;
	}
	return TagOk;
}

int FileEntry::Delete(wxSQLite3Statement& deletePreparedStmnt)
{
	try {

		deletePreparedStmnt.Bind(1,  GetFile());
		deletePreparedStmnt.ExecuteUpdate();
		deletePreparedStmnt.Reset();

	} catch (wxSQLite3Exception& exc) {
		if (exc.ErrorCodeAsString(exc.GetErrorCode()) == wxT("SQLITE_CONSTRAINT"))
			return TagExist;
		return TagError;
	}
	return TagOk;
}

int FileEntry::Fetch(TagsDatabase* db)
{
	try {
		wxSQLite3ResultSet rs = db->Query(wxString::Format(wxT("SELECT * from files where file='%s'"), GetFile().c_str()));
		if (rs.NextRow()) {
			m_id                    = rs.GetInt(0);
			m_file                  = rs.GetString(1);
			m_lastRetaggedTimestamp = rs.GetInt(2);
			return TagOk;
		}
	} catch (wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}
	return TagError;
}
