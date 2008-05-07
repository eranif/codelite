#include "cpptoken.h"
CppToken::CppToken()
{
	reset();
}

CppToken::~CppToken()
{
}

void CppToken::reset()
{
	name.clear();
	offset = std::string::npos;
}
void CppToken::append(const char ch)
{
	name += ch;
}

void CppToken::print()
{
	printf("%s | %ld\n", name.c_str(), offset);
}
