#ifndef __cppwordscanner__
#define __cppwordscanner__

#include "cpptoken.h"

class CppWordScanner {
	std::string m_text;
public:
	CppWordScanner(const std::string &text);
	~CppWordScanner();
	
	void parse(CppTokenList &l);

};
#endif // __cppwordscanner__
