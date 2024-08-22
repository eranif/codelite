#ifndef EXPRESSION_RESULT_H
#define EXPRESSION_RESULT_H

#include <cstdio>
#include <string>

#ifndef WXDLLIMPEXP_CL

#ifdef WXMAKINGDLL_CL
#    define WXDLLIMPEXP_CL __declspec(dllexport)
#elif defined(WXUSINGDLL_CL)
#    define WXDLLIMPEXP_CL __declspec(dllimport)
#else // not making nor using DLL
#    define WXDLLIMPEXP_CL
#endif

#endif


class WXDLLIMPEXP_CL ExpressionResult
{
public:

	bool        m_isFunc;
	std::string m_name;
	bool        m_isThis;
	bool        m_isaType;
	bool        m_isPtr;
	std::string m_scope;
	bool        m_isTemplate;
	std::string m_templateInitList;
	bool        m_isGlobalScope;

public:
	ExpressionResult();
	virtual ~ExpressionResult();
	void Reset();
	void Print();
	std::string ToString() const;
};
#endif //EXPRESSION_RESULT_H
