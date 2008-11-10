#ifndef __PERFORMANCE_H__
#define __PERFORMANCE_H__

// Usage: in your file that you want to profile, first do this:
//     #define __PERFORMANCE
//     #include "performance.h"
// (Just comment out the #define __PERFORMANCE when you're done.)
//
// Then use any of these forms in functions that you want to profile:
//
//     PERF_FUNCTION();  -- put this at the very top of any function to profile the whole function.
//
//     PERF_BLOCK("Your Comment Here") {    -- put this around parts of a function you want to profile 
//         [your code here]
//     }
//
//    PERF_START("Your Comment Here");  -- use this instead when the braces of PERF_BLOCK won't work for you because of scoping issues
//    [your code here]
//    PERF_END();
//
// Output is sent to file "codelite.perf" in the startup directory.  The file is opened automatically for you.  It is
// overwritten on each run.
//
// Output format is XML, so you can use xml tools, or view the file in a folding editor.  Looks like this:
//     <block name="Function Signature|Your Comment">
//        <block ...>
//           ...
//       </block>
//       ...
//       <elapsed .../>
//    </block>
//
// The <elapsed> element shows the time spent for the entire block it is in, and includes an optional "unaccounted"
// attribute for any ticks not counted by inner blocks (so you know how much time you're missing from profiled
//  subfunctions).

#ifdef __PERFORMANCE
    #ifdef __WXMSW__ 
        //Uncomment this to globally disable all profiling
        //#undef __PERFORMANCE
    #else
        // Don't change this
        #undef __PERFORMANCE
    #endif
#endif

#ifdef __PERFORMANCE

    extern void PERF_START(const char* func_name);
    extern void PERF_END();
	extern void PERF_OUTPUT(const char* path);
	
    struct PERF_CLASS {
        PERF_CLASS(const char *name) : count(0) { PERF_START(name); }
        ~PERF_CLASS()                           { PERF_END();       }
            
        int count;
    };

    #define PERF_FUNCTION()   PERF_CLASS PERF_OBJ(__PRETTY_FUNCTION__)
    #define PERF_REPEAT(nm,n) for (PERF_CLASS PERF_OBJ(nm); PERF_OBJ.count < (n); PERF_OBJ.count++)
    #define PERF_BLOCK(nm)    PERF_REPEAT(nm,1)

#else 

	#define PERF_START(func_name)
	#define PERF_END()
	#define PERF_OUTPUT(path)
	#define PERF_FUNCTION()
    #define PERF_REPEAT(nm,n)
    #define PERF_BLOCK(nm)
    
#endif

#endif // __PERFORMANCE_H__
