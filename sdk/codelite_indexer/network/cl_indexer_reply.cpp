#include <string.h>
#include "cl_indexer_reply.h"

#define PACK_INT(p, i) {\
	memcpy((void*)p, (void*)&i, sizeof(i));\
	p += sizeof(i);\
}

#define PACK_STD_STRING(p, s) {\
	size_t l = s.length();\
	memcpy((void*)p, (void*)&l, sizeof(l));\
	p += sizeof(l);\
	memcpy((void*)p, (void*)s.c_str(), l);\
	p += l;\
}

clIndexerReply::clIndexerReply()
{
}

clIndexerReply::~clIndexerReply()
{
}

void clIndexerReply::fromBinary(char* data)
{
	////////////////////////////////////////////////////////
	// integer 		| completion code
	// integer 		| file name len
	// string		| file name string
	// integer		| tags length
	// string		| tags string
	////////////////////////////////////////////////////////

	memcpy((void*)&m_completionCode, data, sizeof(m_completionCode));
	data += sizeof(m_completionCode);

	size_t len(0);
	memcpy((void*)&len, data, sizeof(len));
	data += sizeof(len);

	// read the file name
	char *file_name = new char[len+1];
	memcpy(file_name, data, len);
	file_name[len] = 0;

	data += len;
	m_fileName = file_name;
	delete file_name;

	// read the tags
	len = 0;
	memcpy((void*)&len, data, sizeof(len));
	data += sizeof(len);

	if(len > 0){
		char *tags = new char[len+1];
		memcpy(tags, data, len);
		tags[len] = 0;

		data += len;
		m_tags = tags;
		delete tags;
	}
}

char* clIndexerReply::toBinary(size_t& buffer_size)
{
	buffer_size = 0;
	buffer_size += sizeof(m_completionCode);
	buffer_size += sizeof(size_t);			// length of the file name
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

