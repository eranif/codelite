#include <wx/app.h>
#include <wx/event.h>
#include "MainFrame.h"

// Define the MainApp
class MainApp : public wxApp
{
public:
	MainApp() {}
	virtual ~MainApp() {}
	
	virtual bool OnInit() {
		MainFrame *mainFrame = new MainFrame(NULL);
		SetTopWindow(mainFrame);
		return GetTopWindow()->Show();
	}
};

DECLARE_APP(MainApp)
IMPLEMENT_APP(MainApp)
