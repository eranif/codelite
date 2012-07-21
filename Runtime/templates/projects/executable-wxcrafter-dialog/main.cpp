#include <wx/app.h>
#include <wx/event.h>
#include "MainDialog.h"

// Define the MainApp
class MainApp : public wxApp
{
public:
	MainApp() {}
	virtual ~MainApp() {}
	
	virtual bool OnInit() {
		MainDialog mainDialog(NULL);
		mainDialog.ShowModal();
		return false;
	}
};

DECLARE_APP(MainApp)
IMPLEMENT_APP(MainApp)
