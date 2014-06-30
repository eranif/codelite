//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : cl_indexer_reply.cpp
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
#include "cl_indexer_reply.h"
#include "cl_indexer_macros.h"

clIndexerReply::clIndexerReply()
: m_completionCode(0)
{
}

clIndexerReply::~clIndexerReply()
{
}

void clIndexerReply::fromBinary(char* data)
{
	////////////////////////////////////////////////////////
	// integer      | completion code
	// integer      | file name len
	// string       | file name string
	// integer      | tags length
	// string       | tags string
	////////////////////////////////////////////////////////
	UNPACK_INT(m_completionCode, data);
	UNPACK_STD_STRING(m_fileName, data);
	UNPACK_STD_STRING(m_tags, data);
}

char* clIndexerReply::toBinary(size_t& buffer_size)
{
	buffer_size = 0;
	buffer_size += sizeof(m_completionCode);
	buffer_size += sizeof(size_t);          // length of the file name
	buffer_size += m_fileName.length();
	buffer_size += sizeof(size_t);
	buffer_size += m_tags.length();

	char *data = new char[buffer_size];
	char *ptr = data;
	PACK_INT(data, m_completionCode);
	PACK_STD_STRING(data, m_fileName);
	PACK_STD_STRING(data, m_tags);
	return ptr;
}

