#define __PERFORMANCE 
#include "performance.h"

#ifdef __WXMSW__

#include <vector>
#include <string>
#include <stdio.h>
#include <windows.h>
#include <assert.h>

static FILE * dbgFile(NULL);
static std::string output = "";
static std::vector<std::pair<DWORD, DWORD> > tickCounts;

void PERF_OUTPUT(const char *path) {
	output = path;
}

void PERF_START(const char* func_name) {
	if (!dbgFile) {
        dbgFile = fopen(output.c_str(), "w+");
        assert(dbgFile != 0);
    }
	fprintf(dbgFile, "%*c<block name=\"%s\">\n", 4*tickCounts.size(), ' ', func_name);
    fflush(dbgFile);	
	tickCounts.push_back(std::make_pair(DWORD(GetTickCount()), DWORD(0)));    
}

void PERF_END() {
	assert(dbgFile != 0);
    if (tickCounts.empty())
        return;

	DWORD tickCount   = GetTickCount();
    DWORD elapsed     = tickCount - tickCounts.back().first;
    DWORD unaccounted = elapsed   - tickCounts.back().second;
    
    fprintf(dbgFile, "%*c<elapsed time=\"%ld\"", 4*tickCounts.size(), ' ', elapsed);
    if (0 < unaccounted && unaccounted < elapsed) {
        fprintf(dbgFile, " unaccounted=\"%ld\"", unaccounted);
    }
    fprintf(dbgFile, "/>\n");

    tickCounts.pop_back();
    if (!tickCounts.empty()) {
        tickCounts.back().second += elapsed;
    }
    
    fprintf(dbgFile, "%*c</block>\n", 4*tickCounts.size(), ' ');
	fflush(dbgFile);	
}

#endif
