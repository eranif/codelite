//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : gprofparser.h
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
 * Name:      gprofparser.h
 * Purpose:   Header to create stream parser from gprof. 
 * Author:    Vaclav Sprucek
 * Created:   2012-03-04
 * Copyright: Vaclav Sprucek 
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include <wx/wx.h>
#include <wx/string.h> 
#include <wx/stream.h>
#include <wx/txtstrm.h>
#include <wx/hashmap.h>

#include "lineparser.h"

WX_DECLARE_HASH_MAP(int, int, wxIntegerHash, wxIntegerEqual, OccurrenceMap);

/**
 * @class GprofParser
 * @brief Class define structure for parser to read stream of data from gprof tool.
 */
class GprofParser
{
private:
	wxString readlinetext;
	wxString readlinetexttemp;
	bool lineheader;
	bool primaryline;
	int nameLen;
	char *nameandid;
	bool isdot;
	bool iscycle;
	bool islom;
	bool isplus;	
	bool isspontaneous;	
	
	OccurrenceMap calls;
	wxArrayInt sortedCalls;
	
public:
	/**
	 * @brief Defautl constructor.
	 */
	GprofParser();
	/**
	 * @brief Defautl destructor.
	 */
	~GprofParser();
	/**
	 * @brief  List lines type LineParserList.
	 */
	LineParserList lines;
	/**
	 * @brief Function is reading the input stream from gprof application and scan the rows to save to collection of objects lines. 
	 * @param m_pInputStream pointer of type wxInputStream. 
	 */
	void GprofParserStream(wxInputStream *m_pInputStream);
	/**
	 * @brief Suggest call diagram's node threshold so no more than 100 items should be displayed at once.
	 */
	int GetSuggestedNodeThreshold();
};
