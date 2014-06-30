//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : expression_result.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "expression_result.h"

#define BOOL_TO_STR(b) b ? "true" : "false"

ExpressionResult::ExpressionResult()
{
	Reset();
}

ExpressionResult::~ExpressionResult()
{
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
