#include "cltagsdatabase.h"
#include "sqlite3.h"

clTagsDatabase::~clTagsDatabase()
{
}

clTagsDatabase::clTagsDatabase(const std::string& db_file)
		: m_db(0)
{
	int rc = sqlite3_open(db_file.c_str(), (sqlite3**)&m_db);
	if (rc != SQLITE_OK) {
		do_close();
	}
}

bool clTagsDatabase::begin_transaction()
{
	return do_exec("begin transaction");
}

bool clTagsDatabase::commit_transaction()
{
	return do_exec("commit transaction");
}

bool clTagsDatabase::store(const TagEntry& tags)
{
	return false;
}

bool clTagsDatabase::is_ok()
{
	return m_db != 0;
}

void clTagsDatabase::close()
{
	do_close();
}

bool clTagsDatabase::delete_by_filename(const std::string& file_name)
{
	std::string sql;
	sql += "delete from tags where file='";
	sql += file_name;
	sql += "'";
	return do_exec(sql.c_str());
}

void clTagsDatabase::do_close()
{
	if (m_db) {
		sqlite3_close((sqlite3*) m_db);
		m_db = 0;
	}
}

bool clTagsDatabase::do_exec(const char* sql)
{
	if ( is_ok() ) {
		return false;
	}

	char* localError = 0;
	int rc = sqlite3_exec((sqlite3*)m_db, sql, NULL, NULL, &localError);

	if (rc == SQLITE_OK) {
		return true;
	}
	return false;
}
