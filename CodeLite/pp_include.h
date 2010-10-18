#ifndef PP_INCLUDE_H
#define PP_INCLUDE_H

#include <vector>
#include <wx/string.h>

#ifndef WXDLLIMPEXP_CL

#ifdef WXMAKINGDLL_CL
#    define WXDLLIMPEXP_CL __declspec(dllexport)
#elif defined(WXUSINGDLL_CL)
#    define WXDLLIMPEXP_CL __declspec(dllimport)
#else // not making nor using DLL
#    define WXDLLIMPEXP_CL
#endif

#endif

/**
 * @brief the main interface to the PP API.
 * @param filePath
 * @return 
 */
extern WXDLLIMPEXP_CL int PPScan      ( const wxString &filePath, bool forCC );
/**
 * @brief scan input string
 */
extern WXDLLIMPEXP_CL int PPScanString( const wxString &inputString );

#endif
