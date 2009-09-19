#include "network/cppcheck_net_macros.h"
#include <string.h>
#include "network/cppchecker_request.h"

CPPCheckerRequest::CPPCheckerRequest()
{
}

CPPCheckerRequest::~CPPCheckerRequest()
{
}

void CPPCheckerRequest::fromBinary(char* data)
{
	NET_UNPACK_INT       (m_cmd,  data);
	NET_UNPACK_STD_STRING(m_file, data);
}

char* CPPCheckerRequest::toBinary(size_t &buffer_size)
{
	buffer_size = 0;
	buffer_size += sizeof(m_cmd);   // command type
	buffer_size += sizeof(size_t);  // length of the database file name
	buffer_size += m_file.length(); // file name length

	// allocate char array for the data
	char *data = new char[buffer_size];
	char *ptr = data;

	NET_PACK_INT       (data, m_cmd );
	NET_PACK_STD_STRING(data, m_file);
	return ptr;
}
