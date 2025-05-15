//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : lineparser.cpp
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

#include "lineparser.h"

#include <wx/listimpl.cpp>

WX_DEFINE_LIST(LineParserList);

LineParser* LineParser::Clone() const
{
	LineParser *newline = new LineParser();
	
	newline->index = this->index;
	newline->time = this->time;
	newline->self = this->self;
	newline->children = this->children;
	newline->called0 = this->called0;
	newline->called1 = this->called1;
	newline->name = this->name;
	newline->nameid = this->nameid;
	newline->parents = this->parents; 
	newline->pline = this->pline;
	newline->child = this->child;
	newline->cycle = this->cycle; 
	newline->recursive = this->recursive;
	newline->cycleid = this->cycleid;
	
	return newline;
}
