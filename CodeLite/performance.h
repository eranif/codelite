#ifndef __PERFORMANCE_H__
#define __PERFORMANCE_H__

#ifdef __WXMSW__
	// turn on performance
	#define __PERFORMANCE 0
#else 
	// DONT change this
	#define __PERFORMANCE 0
#endif

#if __PERFORMANCE

	#include <stdio.h>
		
	extern void PERF_INIT();
	extern void PERF_START(const char* func_name);
	extern void PERF_END(const char* func_name);
	
#else 

	#define PERF_INIT()
	#define PERF_END(func_name)
	#define PERF_START(func_name)

#endif

#endif // __PERFORMANCE_H__


