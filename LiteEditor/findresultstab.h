#ifndef __findresultstab__
#define __findresultstab__

#include "outputtabwindow.h"

class FindResultsTab : public OutputTabWindow 
{

private:
	FindResultsTab(const FindResultsTab& rhs);
	FindResultsTab& operator=(const FindResultsTab& rhs);

public:
	FindResultsTab(wxWindow *parent, wxWindowID id, const wxString &name);
	virtual ~FindResultsTab();

	virtual void OnMouseDClick(wxScintillaEvent &event);
};
#endif // __findresultstab__
