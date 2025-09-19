#include "expression_result.h"

#define BOOL_TO_STR(b) b ? "true" : "false"

ExpressionResult::ExpressionResult()
{
	Reset();
}

void ExpressionResult::Print()
{
	printf("%s\n", ToString().c_str());
}

std::string ExpressionResult::ToString() const
{
	char tmp[256];
	sprintf(tmp, "{m_name:%s, m_isFunc:%s, m_isTemplate:%s, m_isThis:%s, m_isaType:%s, m_isPtr:%s, m_scope:%s, m_templateInitList:%s}", 
				m_name.c_str(), 
				BOOL_TO_STR(m_isFunc), 
				BOOL_TO_STR(m_isTemplate),
				BOOL_TO_STR(m_isThis),
				BOOL_TO_STR(m_isaType),
				BOOL_TO_STR(m_isPtr),
				m_scope.c_str(),
				m_templateInitList.c_str());
	return tmp;
}

void ExpressionResult::Reset()
{
	m_isFunc = false;
	m_name = "";
	m_isThis = false;
	m_isaType = false;
	m_isPtr = false;
	m_scope = "";
	m_isTemplate = false;
	m_isGlobalScope = false;
	m_templateInitList = "";
}
