//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : cl_indexer_request.h
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

#ifndef __clindexercommand__
#define __clindexercommand__

#include <string>
#include <vector>

class clIndexerRequest
{
	std::vector<std::string> m_files;
	std::string m_ctagOptions;
	size_t m_cmd;
	std::string m_databaseFileName;
public:
	enum {
		CLI_PARSE,
		CLI_PARSE_AND_SAVE
	};

public:
	clIndexerRequest();
	~clIndexerRequest();

	void fromBinary(char *data);
	char *toBinary(size_t &buffer_size);

	void setCmd(const size_t& cmd) {
		this->m_cmd = cmd;
	}
	void setFiles(const std::vector<std::string>& files) {
		this->m_files = files;
	}
	const size_t& getCmd() const {
		return m_cmd;
	}
	const std::vector<std::string>& getFiles() const {
		return m_files;
	}
	void setCtagOptions(const std::string& ctagOptions) {
		this->m_ctagOptions = ctagOptions;
	}
	const std::string& getCtagOptions() const {
		return m_ctagOptions;
	}
	void setDatabaseFileName(const std::string& databaseFileName) {
		this->m_databaseFileName = databaseFileName;
	}
	const std::string& getDatabaseFileName() const {
		return m_databaseFileName;
	}
};
#endif // __clindexercommand__
