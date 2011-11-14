#ifndef CODELITE_EXPORTS
#define CODELITE_EXPORTS

#ifdef __WXMSW__

#include <wx/string.h>
#ifdef WXDLLIMPEXP_CL
#undef WXDLLIMPEXP_CL
#endif

//////////////////////////////////////
// codelite 
//////////////////////////////////////

#ifdef WXMAKINGDLL_CL
#    define WXDLLIMPEXP_CL __declspec(dllexport)
#elif defined(WXUSINGDLL_CL)
#    define WXDLLIMPEXP_CL __declspec(dllimport)
#else // not making nor using DLL
#    define WXDLLIMPEXP_CL
#endif

//////////////////////////////////////
// SDK
//////////////////////////////////////
#ifdef WXDLLIMPEXP_SDK
#undef WXDLLIMPEXP_SDK
#endif

#ifdef WXMAKINGDLL_SDK
#    define WXDLLIMPEXP_SDK __declspec(dllexport)
#elif defined(WXUSINGDLL_SDK)
#    define WXDLLIMPEXP_SDK __declspec(dllimport)
#else // not making nor using DLL
#    define WXDLLIMPEXP_SDK
#endif

#else // !MSW
#    define WXDLLIMPEXP_CL
#    define WXDLLIMPEXP_SDK
#endif

#endif // CODELITE_EXPORTS


