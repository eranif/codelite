//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : function.cpp
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
	m_isVirtual = false;
	m_isPureVirtual = false;
	m_isConst = false;
}

void clFunction::Print()
{
	fprintf(
	    stdout, "{m_name=%s, m_isConst=%s, m_lineno=%d, m_scope=%s, m_signature=%s, m_isVirtual=%s, m_isPureVirtual=%s, m_retrunValusConst=%s, m_throws=%s\nm_returnValue=",
	    m_name.c_str(),
	    m_isConst ? "yes" : "no",
	    m_lineno,
	    m_scope.c_str(),
	    m_signature.c_str(),
	    m_isVirtual ? "yes" : "no",
	    m_isPureVirtual ? "yes" : "no",
	    m_retrunValusConst.c_str(),
		m_throws.c_str()
	);

	m_returnValue.Print();
	fprintf(stdout, "}\n");
	fflush(stdout);
}
