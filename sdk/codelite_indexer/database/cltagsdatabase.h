#ifndef __cltagsdatabase__
#define __cltagsdatabase__

#include "tagentry.h"
#include <string>

/**
 * @class clTagsDatabase small class which provide an interface to
 * sqlite database
 * @author eran
 * @date 01/14/09
 * @file cltagsdatabase.h
 */
class clTagsDatabase {
	void *m_db;

private:
	void do_close();
	bool do_exec(const char *sql);
public:
	clTagsDatabase(const std::string &db_file);
	~clTagsDatabase();

	bool is_ok();
	bool store(const TagEntry& tags);
	bool delete_by_filename(const std::string &file_name);
	bool begin_transaction();
	bool commit_transaction();
	void close();
};
#endif // __cltagsdatabase__
