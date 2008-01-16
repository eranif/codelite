#ifndef BROWSE_HISTORY_H
#define BROWSE_HISTORY_H

#include "wx/string.h"

#ifndef WXDLLIMPEXP_LE
	#ifdef WXMAKINGDLL
	#    define WXDLLIMPEXP_LE WXEXPORT
	#elif defined(WXUSINGDLL)
	#    define WXDLLIMPEXP_LE WXIMPORT
	#else 
	#    define WXDLLIMPEXP_LE
	#endif // WXDLLIMPEXP_LE
#endif

class BrowseRecord
{
public:
	wxString filename;
	wxString project;
	int lineno;
	int position;
public:
	BrowseRecord() : filename(wxEmptyString), project(wxEmptyString), lineno(wxNOT_FOUND), position(wxNOT_FOUND)
	{}

	~BrowseRecord()
	{}
};

#endif //BROWSE_HISTORY_H
