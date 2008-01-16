#include "variable.h"

Variable::Variable()
{
	Reset();
}

Variable::~Variable()
{
}

Variable::Variable(const Variable &src)
{
	*this = src;
}

Variable & Variable::operator =(const Variable &src)
{
	m_type = src.m_type;
	m_templateDecl = src.m_templateDecl;
	m_name = src.m_name;
	m_isTemplate = src.m_isTemplate;
	m_isPtr = src.m_isPtr;
	m_typeScope = src.m_typeScope;
	m_pattern = src.m_pattern;
	m_starAmp = src.m_starAmp;
	m_lineno = src.m_lineno;
	return *this;
}

void Variable::Reset()
{
	m_type = "";
	m_templateDecl = "";
	m_name = "";
	m_isTemplate = false;
	m_isPtr = false;
	m_typeScope = "";
	m_pattern = "";
	m_starAmp = "";
	m_lineno = 0;
}

void Variable::Print()
{
	fprintf(	stdout, "{m_name=%s, m_lineno=%d, m_starAmp=%s, m_type=%s, m_typeScope=%s, m_templateDecl=%s, m_isPtr=%s, m_isTemplate=%s }\n", 
				m_name.c_str(), m_lineno, m_starAmp.c_str(), m_type.c_str(), m_typeScope.c_str(), m_templateDecl.c_str(),
				m_isPtr ? "true" : "false", 
				m_isTemplate ? "true" : "false");
	fprintf( stdout, "Pattern: %s\n", m_pattern.c_str());
	fflush(stdout);
}
