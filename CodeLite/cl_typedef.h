//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : cl_typedef.h
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

#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <string>
#include "variable.h"

class clTypedef
{
public:
	std::string m_name;
	Variable    m_realType;
	
public:
	clTypedef()  {}
	~clTypedef() {}
	
	void print() {
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
