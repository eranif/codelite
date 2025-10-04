//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : fileentry.h
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

#ifndef __fileentry__
#define __fileentry__

#include <memory>
#include <wx/string.h>

class FileEntry
{
	long      m_id;
	wxString  m_file;
	int       m_lastRetaggedTimestamp;

public:
	FileEntry();
	~FileEntry() = default;

public:
	void SetFile(const wxString& file) {
		this->m_file = file;
	}
	void SetLastRetaggedTimestamp(int lastRetaggedTimestamp) { this->m_lastRetaggedTimestamp = lastRetaggedTimestamp; }
	const wxString& GetFile() const {
		return m_file;
	}
	int GetLastRetaggedTimestamp() const { return m_lastRetaggedTimestamp; }
	void SetId(long id) { this->m_id = id; }
	long GetId() const { return m_id; }
};
using FileEntryPtr = std::unique_ptr<FileEntry>;

#endif // __fileentry__
