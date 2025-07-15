#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <string>
#include "variable.h"

#ifndef WXDLLIMPEXP_CL

#ifdef WXMAKINGDLL_CL
#    define WXDLLIMPEXP_CL __declspec(dllexport)
#elif defined(WXUSINGDLL_CL)
#    define WXDLLIMPEXP_CL __declspec(dllimport)
#else // not making nor using DLL
#    define WXDLLIMPEXP_CL
#endif

#endif


class WXDLLIMPEXP_CL clTypedef
{
public:
	std::string m_name;
	Variable    m_realType;
	
public:
	clTypedef()  {}
	~clTypedef() {}
	
	void print() const {
		printf("Name: %s\n", m_name.c_str());
		m_realType.Print();
	}
	
	void clear() {
		m_realType.Reset();
		m_name.clear();
	}
};

typedef std::list<clTypedef> clTypedefList;
#endif // TYPEDEF_H
