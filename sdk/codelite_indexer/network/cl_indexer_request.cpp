//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : cl_indexer_request.cpp
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

#include <string.h>
#include "cl_indexer_request.h"
#include "cl_indexer_macros.h"

clIndexerRequest::clIndexerRequest()
: m_cmd(CLI_PARSE)
{
}

clIndexerRequest::~clIndexerRequest()
{
}

void clIndexerRequest::fromBinary(char* data)
{
	UNPACK_INT(m_cmd, data);
	UNPACK_STD_STRING(m_ctagOptions, data);
	UNPACK_STD_STRING(m_databaseFileName, data);

	// read the number of files
	size_t numFiles(0);
	UNPACK_INT(numFiles, data);

	m_files.clear();
	for (size_t i=0; i<numFiles; i++) {
		std::string file_name;

		UNPACK_STD_STRING(file_name, data);
		m_files.push_back(file_name);
	}
}

char* clIndexerRequest::toBinary(size_t &buffer_size)
{
	buffer_size = 0;
	buffer_size += sizeof(m_cmd);               // command type
	buffer_size += sizeof(size_t);              // length ctags options tring
	buffer_size += m_ctagOptions.length();      // ctags options actual string
	buffer_size += sizeof(size_t);              // length of the database file name
	buffer_size += m_databaseFileName.length(); // database file name

	buffer_size += sizeof(size_t);              // number of files
	for(size_t i=0; i<m_files.size(); i++){
		buffer_size += sizeof(size_t);          // file name len
		buffer_size += m_files.at(i).length();  // file name string
	}

	// allocate char array for the data
	char *data = new char[buffer_size];
	char *ptr = data;

	PACK_INT(data, m_cmd);
	PACK_STD_STRING(data, m_ctagOptions);
	PACK_STD_STRING(data, m_databaseFileName);

	size_t size = m_files.size();
	PACK_INT(data, size);
	for(size_t i=0; i<m_files.size(); i++){
		PACK_STD_STRING(data, m_files.at(i));
	}
	return ptr;
}
