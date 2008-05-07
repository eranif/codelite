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
	offset = wxString::npos;
}
void CppToken::append(const char ch)
{
	name += ch;
}

void CppToken::print()
{
	wxPrintf(wxT("%s | %ld\n"), name.GetData(), offset);
}
