#include "performance.h"

#if __PERFORMANCE

#ifndef __WXMSW__
	#error "Performance only supported on Windows !"
#endif

#include <windows.h>

FILE * dbgFile(NULL);
int depth(0);
DWORD lastTickCount;

void PERF_INIT() {
	if(!dbgFile) {
		dbgFile = fopen("codelite.perf", "w+");
	}
}

void PERF_START(const char* func_name) {
	 if(depth < 0) depth=0;
	
	for(int i=0; i<depth; i++) fprintf(dbgFile, "\t");
	
	DWORD tickCount = GetTickCount();
	fprintf(dbgFile, "-> %s\n",func_name);
	fflush(dbgFile);
	
	lastTickCount = tickCount;
	depth++;
}

void PERF_END(const char* func_name) {
	depth--;
	if(depth < 0) depth=0;
	for(int i=0; i<depth; i++) fprintf(dbgFile, "\t");
	
	DWORD tickCount = GetTickCount();
	fprintf(dbgFile, "<- %s - elapsed %ld\n", func_name, tickCount - lastTickCount);
	fflush(dbgFile);
	
	lastTickCount = tickCount;
}

#endif // __PERFORMANCE
