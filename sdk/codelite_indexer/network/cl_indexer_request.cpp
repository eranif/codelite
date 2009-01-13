#include "cl_indexer_request.h"

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

clIndexerRequest::clIndexerRequest()
{
}

clIndexerRequest::~clIndexerRequest()
{
}

void clIndexerRequest::fromBinary(char* data)
{
	////////////////////////////////////////////////////////
	// integer 		| command type
	// integer 		| options string length
	// string		| option string
	// integer		| number of files to parse
	// integer		| first file len
	// string		| first file string
	// ...
	// integer		| N file len
	// string		| N file string
	////////////////////////////////////////////////////////

	memcpy((void*)&m_cmd, data, sizeof(m_cmd));
	data += sizeof(m_cmd);

	size_t len(0);
	memcpy((void*)&len, data, sizeof(len));

	if (len < 0) {
		return;
	}
	data += sizeof(len);

	// read the ctags options string
	char *ctagsOptions = new char[len+1];
	memcpy(ctagsOptions, data, len);
	ctagsOptions[len] = 0;

	data += len;
	m_ctagOptions = ctagsOptions;
	delete ctagsOptions;

	// read the number of files
	size_t numFiles(0);
	memcpy((void*)&numFiles, data, sizeof(numFiles));
	data += sizeof(numFiles);

	m_files.clear();
	for (size_t i=0; i<numFiles; i++) {
		// read string
		memcpy((void*)&len, data, sizeof(len));
		data += sizeof(len);

		char *fileName = new char[len+1];
		memcpy((void*)fileName, data, len);
		data += len;

		fileName[len] = 0;
		m_files.push_back(std::string(fileName));
		delete fileName;
	}
}

char* clIndexerRequest::toBinary(size_t &buffer_size)
{
	buffer_size = 0;
	buffer_size += sizeof(m_cmd);			// command type
	buffer_size += sizeof(size_t);			// length ctags options tring
	buffer_size += m_ctagOptions.length(); 	// ctags options actual string
	buffer_size += sizeof(size_t); 			// number of files
	for(size_t i=0; i<m_files.size(); i++){
		buffer_size += sizeof(size_t);			// file name len
		buffer_size += m_files.at(i).length();	// file name string
	}

	// allocate char array for the data
	char *data = new char[buffer_size];
	char *ptr = data;
	PACK_INT(data, m_cmd);
	PACK_STD_STRING(data, m_ctagOptions);

	size_t size = m_files.size();
	PACK_INT(data, size);
	for(size_t i=0; i<m_files.size(); i++){
		PACK_STD_STRING(data, m_files.at(i));
	}
	return ptr;
}
