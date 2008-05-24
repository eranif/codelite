#ifndef __stringsearcher__
#define __stringsearcher__

#include <wx/string.h>

// Possible search data options:
enum {
	wxSD_MATCHCASE         = 1,
	wxSD_MATCHWHOLEWORD    = 2,
	wxSD_REGULAREXPRESSION = 4,
	wxSD_SEARCH_BACKWARD   = 8
};

class StringFindReplacer {

protected:
	static wxString GetString(const wxString& input, int from, bool search_up);
	static bool DoRESearch(const wxString &input, int startOffset, const wxString &find_what, size_t flags, int &pos, int &matchLen);
	static bool DoSimpleSearch(const wxString &input, int startOffset, const wxString &find_what, size_t flags, int &pos, int &matchLen);
	
public:
	static bool Search(const wxString &input, int startOffset, const wxString &find_what, size_t flags, int &pos, int &matchLen);
};
#endif // __stringsearcher__
