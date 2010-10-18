#ifndef CRAWLER_INCLUDE_H
#define CRAWLER_INCLUDE_H

#include <vector>
#include "fc_fileopener.h"

#ifndef WXDLLIMPEXP_CL

#ifdef WXMAKINGDLL_CL
#    define WXDLLIMPEXP_CL __declspec(dllexport)
#elif defined(WXUSINGDLL_CL)
#    define WXDLLIMPEXP_CL __declspec(dllimport)
#else // not making nor using DLL
#    define WXDLLIMPEXP_CL
#endif

#endif

extern WXDLLIMPEXP_CL int crawlerScan        ( const char* filePath );

#endif
