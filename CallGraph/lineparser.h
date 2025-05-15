//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : lineparser.h
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

/***************************************************************
 * Name:      lineparser.h
 * Purpose:   Header to store lines from gprof parser.
 * Author:    Vaclav Sprucek
 * Created:   2012-03-04
 * Copyright: Vaclav Sprucek
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _LINEPARSER_H__
#define _LINEPARSER_H__

#include <wx/string.h>
#include <wx/list.h>
/**
 * @class LineParser
 * @brief Class define structure for data structure.
 */
class LineParser
{	
public:
	int index;
	float time;
	float self;
	float children;
	int called0;
	int called1;
	wxString name;
	int nameid;
	bool parents; // input to primary line - not used in call graph
	bool pline; // primary line
	bool child; //  output from primary line
	bool cycle; 
	bool recursive;
	int  cycleid;
	
	LineParser* Clone() const;
};

WX_DECLARE_LIST( LineParser, LineParserList );

#endif