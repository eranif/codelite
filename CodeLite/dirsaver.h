#ifndef DIR_SAVER_H
#define DIR_SAVER_H

#include "wx/string.h"
#include "wx/filefn.h"

#ifdef WXMAKINGDLL_CODELITE
#    define WXDLLIMPEXP_CL WXEXPORT
#elif defined(WXUSINGDLL_CODELITE)
#    define WXDLLIMPEXP_CL WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_CL
#endif // WXMAKINGDLL_CODELITE

// Utility class that helps keeping the current directory
class WXDLLIMPEXP_CL DirSaver 
{
	wxString m_curDir;
public:
	DirSaver(){
		m_curDir = wxGetCwd();
	}

	virtual ~DirSaver(){
		wxSetWorkingDirectory(m_curDir);
	}
};
#endif // DIR_SAVER_H
