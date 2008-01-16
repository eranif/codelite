#include "function.h"

clFunction::clFunction()
{
	Reset();
}

clFunction::~clFunction()
{
}

void clFunction::Reset()
{
	m_name = "";
	m_scope = "";
	m_returnValue.Reset();
	m_name = "";
	m_signature = "";
	m_lineno = 0;
	m_retrunValusConst = "";
}

void clFunction::Print()
{
	fprintf(	stdout, "{m_name=%s, m_lineno=%d, m_scope=%s, m_signature=%s, m_retrunValusConst=%s\nm_returnValue=", 
				m_name.c_str(), m_lineno, m_scope.c_str(), m_signature.c_str(), m_retrunValusConst.c_str());
	m_returnValue.Print();
	fprintf(	stdout, "}\n");
	fflush(stdout);
}

