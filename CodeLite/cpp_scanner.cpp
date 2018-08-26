//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cpp_scanner.cpp
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
#include "precompiled_header.h"
#include "cpp_scanner.h"

#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif

CppScanner::CppScanner()
: m_curr(0)
{
	m_data = NULL;
	m_pcurr = NULL;
	m_keepComments = 0;
	m_returnWhite = 0;
	m_comment = wxEmptyString;
}

CppScanner::~CppScanner(void)
{
	free(m_data);
}

int CppScanner::LexerInput(char *buf, int max_size)
{
	if( !m_data )
		return 0;

	memset(buf, 0, max_size);
	char *pendData = m_data + strlen(m_data);
	int n = (max_size < (pendData - m_pcurr)) ? max_size : (pendData - m_pcurr);
	if(n > 0)
	{
		memcpy(buf, m_pcurr, n);
		m_pcurr += n;
	}
	return n;
}

void CppScanner::SetText(const char* data)
{
	// release previous buffer
	Reset();

	m_data = strdup(data);
	m_pcurr = m_data;
}

void CppScanner::Reset()
{
	if(m_data)
	{
		free(m_data);
		m_data = NULL;
		m_pcurr = NULL;
		m_curr = 0;
	}

	// Notify lex to restart its buffer
	yy_flush_buffer(yy_current_buffer);
	m_comment = wxEmptyString;
	yylineno = 1;
}

void CppScanner::Restart()
{
	char* p = strdup(m_data);
	SetText( p );
	free(p);
}
