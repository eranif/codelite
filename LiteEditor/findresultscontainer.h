#ifndef __findresultscontainer__
#define __findresultscontainer__

#include "wx/panel.h"
class FindResultsTab;
class wxFlatNotebook;

class FindResultsContainer : public wxPanel {
	wxFlatNotebook *m_book;
	
protected:
	void Initialize();
	
public:
	FindResultsContainer(wxWindow *win, wxWindowID id);
	~FindResultsContainer();
	
	FindResultsTab *GetActiveTab();
	void SetActiveTab(int selection);
	
	wxFlatNotebook *GetNotebook(){return m_book;}
};
#endif // __findresultscontainer__
