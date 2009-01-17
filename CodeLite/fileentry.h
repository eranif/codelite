#ifndef __fileentry__
#define __fileentry__

#include <wx/string.h>
#include "db_record.h"

class FileEntry : public DbRecord
{
	long      m_id;
	wxString  m_file;
	int       m_lastRetaggedTimestamp;

public:
	FileEntry();
	~FileEntry();

public:
	virtual int      Delete                (wxSQLite3Statement& deletePreparedStmnt);
	virtual wxString GetDeleteOneStatement ();
	virtual wxString GetInsertOneStatement ();
	virtual wxString GetUpdateOneStatement ();
	virtual int      Store                 (wxSQLite3Statement& insertPreparedStmnt, TagsDatabase *db);
	virtual int      Update                (wxSQLite3Statement& updatePreparedStmnt);
	virtual int      Fetch                 (TagsDatabase *db);

	void SetFile(const wxString& file) {
		this->m_file = file;
	}
	void SetLastRetaggedTimestamp(const int& lastRetaggedTimestamp) {
		this->m_lastRetaggedTimestamp = lastRetaggedTimestamp;
	}
	const wxString& GetFile() const {
		return m_file;
	}
	const int& GetLastRetaggedTimestamp() const {
		return m_lastRetaggedTimestamp;
	}
	void SetId(const long& id) {
		this->m_id = id;
	}
	const long& GetId() const {
		return m_id;
	}
};
typedef SmartPtr<FileEntry> FileEntryPtr;
#endif // __fileentry__
