#ifndef CODELITE_EXPORTS
#define CODELITE_EXPORTS

#ifdef __WXMSW__

#include <wx/string.h>
#ifndef WXDLLIMPEXP_CL

//////////////////////////////////////
// codelite 
//////////////////////////////////////

#ifdef WXMAKINGDLL_CL
#    define WXDLLIMPEXP_CL WXEXPORT
#elif defined(WXUSINGDLL_CL)
#    define WXDLLIMPEXP_CL WXIMPORT
#else // not making nor using DLL
#    define WXDLLIMPEXP_CL
#endif

#endif // !WXDLLIMPEXP_CL

//////////////////////////////////////
// SDK
//////////////////////////////////////
#ifndef WXDLLIMPEXP_SDK

#ifdef WXMAKINGDLL_SDK
#    define WXDLLIMPEXP_SDK WXEXPORT
#elif defined(WXUSINGDLL_SDK)
#    define WXDLLIMPEXP_SDK WXIMPORT
#else // not making nor using DLL
#    define WXDLLIMPEXP_SDK
#endif

#endif // !WXDLLIMPEXP_SDK

#else // !MSW
#    define WXDLLIMPEXP_CL
#    define WXDLLIMPEXP_SDK
#endif

#endif // CODELITE_EXPORTS


