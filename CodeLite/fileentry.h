#ifndef __fileentry__
#define __fileentry__

#include <wx/string.h>
#include "smart_ptr.h"
class FileEntry
{
	long      m_id;
	wxString  m_file;
	int       m_lastRetaggedTimestamp;

public:
	FileEntry();
	~FileEntry();

public:
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
