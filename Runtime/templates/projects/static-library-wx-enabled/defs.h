#ifndef _WXDEFS_H
#define _WXDEFS_H

#ifdef USING_SOURCE_MYLIB
	#define WXDLLIMPEXP_MYLIB
    #define WXDLLIMPEXP_DATA_MYLIB(type) type
#elif defined( LIB_USINGDLL )
    #define WXDLLIMPEXP_MYLIB
    #define WXDLLIMPEXP_DATA_MYLIB(type)
#elif defined( WXMAKINGDLL_MYLIB )
    #define WXDLLIMPEXP_MYLIB WXEXPORT
    #define WXDLLIMPEXP_DATA_MYLIB(type) WXEXPORT type
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_MYLIB WXIMPORT
    #define WXDLLIMPEXP_DATA_MYLIB(type) WXIMPORT type
#else // not making nor using DLL
    #define WXDLLIMPEXP_MYLIB
    #define WXDLLIMPEXP_DATA_MYLIB(type) type
#endif

#endif//_WXDEFS_H
