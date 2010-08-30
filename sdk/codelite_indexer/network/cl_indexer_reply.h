//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : cl_indexer_reply.h
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

#ifndef __clindexerreply__
#define __clindexerreply__
#include <string>

class clIndexerReply
{
	size_t m_completionCode;
	std::string m_fileName;
	std::string m_tags;

public:
	clIndexerReply();
	~clIndexerReply();

	void fromBinary(char *data);
	char *toBinary(size_t &buffer_size);

	void setCompletionCode(const size_t& completionCode) {
		this->m_completionCode = completionCode;
	}
	void setFileName(const std::string& fileName) {
		this->m_fileName = fileName;
	}
	void setTags(const std::string& tags) {
		this->m_tags = tags;
	}
	const size_t& getCompletionCode() const {
		return m_completionCode;
	}
	const std::string& getFileName() const {
		return m_fileName;
	}
	const std::string& getTags() const {
		return m_tags;
	}
};
#endif // __clindexerreply__
