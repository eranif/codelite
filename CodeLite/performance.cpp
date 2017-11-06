//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : performance.cpp
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

#define __PERFORMANCE
#include "performance.h"

#ifdef __WXMSW__

#include <assert.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <windows.h>

#if 0
static FILE* dbgFile(NULL);
static std::string output = "";
static std::vector<std::pair<DWORD, DWORD> > tickCounts;
#endif
void PERF_OUTPUT(const char* path) 
{ 
#if 0
    output = path; 
#endif
}

void PERF_START(const char* func_name)
{
#if 0
    if(!dbgFile) {
        dbgFile = fopen(output.c_str(), "w+");
        assert(dbgFile != 0);
    }
    fprintf(dbgFile, "%*c<block name=\"%s\">\n", 4 * tickCounts.size(), ' ', func_name);
    fflush(dbgFile);
    tickCounts.push_back(std::make_pair(DWORD(GetTickCount()), DWORD(0)));
#endif
}

void PERF_END()
{
#if 0
    assert(dbgFile != 0);
    if(tickCounts.empty()) { return; }
    DWORD tickCount = GetTickCount();
    DWORD elapsed = tickCount - tickCounts.back().first;
    DWORD unaccounted = elapsed - tickCounts.back().second;

    fprintf(dbgFile, "%*c<elapsed time=\"%ld\"", 4 * tickCounts.size(), ' ', elapsed);
    if(0 < unaccounted && unaccounted < elapsed) { fprintf(dbgFile, " unaccounted=\"%ld\"", unaccounted); }
    fprintf(dbgFile, "/>\n");

    tickCounts.pop_back();
    if(!tickCounts.empty()) { tickCounts.back().second += elapsed; }

    fprintf(dbgFile, "%*c</block>\n", 4 * tickCounts.size(), ' ');
    fflush(dbgFile);
#endif
}

#endif
