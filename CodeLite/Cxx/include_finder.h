//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : include_finder.h
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

#ifndef INCLUDE_FINDER_H
#define INCLUDE_FINDER_H

#include <vector>
#include <string>
#include "codelite_exports.h"

class WXDLLIMPEXP_CL IncludeStatement {
public:
	std::string file;
	int         line;
	std::string includedFrom;
	std::string pattern;
};

extern WXDLLIMPEXP_CL int IncludeFinder( const char* filePath, std::vector<IncludeStatement> &includes );

#endif
