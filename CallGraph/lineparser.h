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
	float time ;
	float self;
	float childern;
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
};

WX_DECLARE_LIST( LineParser, LineParserList );

#endif