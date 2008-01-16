#ifndef FILEUTILS_H
#define FILEUTILS_H
#include "wx/filename.h"

#ifdef WXMAKINGDLL_CODELITE
#    define WXDLLIMPEXP_CL WXEXPORT
#elif defined(WXUSINGDLL_CODELITE)
#    define WXDLLIMPEXP_CL WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_CL
#endif

class WXDLLIMPEXP_CL FileUtils {
public:
	static bool ReadFileUTF8(const wxFileName &fn, wxString &data);
};
#endif //FILEUTILS_H
