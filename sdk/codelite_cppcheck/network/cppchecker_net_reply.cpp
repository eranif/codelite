#include "network/cppcheck_net_macros.h"
#include <string.h>
#include "network/cppchecker_net_reply.h"

CPPCheckerReply::CPPCheckerReply()
{
}

CPPCheckerReply::~CPPCheckerReply()
{
}

void CPPCheckerReply::fromBinary(char* data)
{
	NET_UNPACK_INT(m_completionCode, data);
	NET_UNPACK_STD_STRING(m_fileName, data);
	NET_UNPACK_STD_STRING(m_report, data);
}

char* CPPCheckerReply::toBinary(size_t& buffer_size)
{
	buffer_size = 0;
	buffer_size += sizeof(m_completionCode);
	buffer_size += sizeof(size_t);          // length of the file name
	buffer_size += m_fileName.length();
	buffer_size += sizeof(size_t);
	buffer_size += m_report.length();

	char *data = new char[buffer_size];
	char *ptr = data;

	NET_PACK_INT       (data, m_completionCode);
	NET_PACK_STD_STRING(data, m_fileName);
	NET_PACK_STD_STRING(data, m_report);
	return ptr;
}

