#include <string.h>
#include "cl_indexer_reply.h"
#include "cl_indexer_macros.h"

clIndexerReply::clIndexerReply()
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

