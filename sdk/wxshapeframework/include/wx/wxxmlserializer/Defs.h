#ifndef _XSDEFS_H
#define _XSDEFS_H

#ifdef USING_SOURCE_XS
    #define WXDLLIMPEXP_XS
    #define WXDLLIMPEXP_DATA_XS(type) type
#elif defined( LIB_USINGDLL )
    #define WXDLLIMPEXP_XS
    #define WXDLLIMPEXP_DATA_XS(type)
#elif defined( WXMAKINGDLL_WXXS )
    #define WXDLLIMPEXP_XS WXEXPORT
    #define WXDLLIMPEXP_DATA_XS(type) WXEXPORT type
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_XS WXIMPORT
    #define WXDLLIMPEXP_DATA_XS(type) WXIMPORT type
#else // not making nor using DLL
    #define WXDLLIMPEXP_XS
    #define WXDLLIMPEXP_DATA_XS(type) type
#endif

#endif//_XSDEFS_H
