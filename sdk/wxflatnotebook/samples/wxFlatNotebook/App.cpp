#include "App.h"
#include <wx/image.h> 
#include <wx/xrc/xmlres.h>
#include "Frame.h"
#include <wx/sysopt.h>
 
IMPLEMENT_APP(App)

Frame *pMainFrame = (Frame *) NULL;
wxPrintData *g_printData = (wxPrintData*) NULL;
wxPageSetupData *g_pageSetupData = (wxPageSetupData*) NULL;

// Externs
extern void InitXmlResource();

App::App(void)
{
}

App::~App(void)
{
}

// Initialize our application
bool App::OnInit(void)
{ 
	// Init resources and add the PNG handler
	wxSystemOptions::SetOption(wxT("msw.remap"), 0); 
	wxXmlResource::Get()->InitAllHandlers(); 
 	wxImage::AddHandler( new wxPNGHandler ); 
	wxImage::AddHandler( new wxCURHandler ); 
	wxImage::AddHandler( new wxBMPHandler );
	wxImage::AddHandler( new wxXPMHandler );

	InitXmlResource();

	pMainFrame = new Frame(NULL, wxT("Frame"));
	 
	// initialize print data and setup
    g_printData = new wxPrintData;
    g_pageSetupData = new wxPageSetupDialogData;

	// show the main frame
	pMainFrame->Show(TRUE);

	// set the main frame to be the main window
	SetTopWindow(pMainFrame);
	return TRUE;	
}

int App::OnExit(void) 
{
    // delete global print data and setup
    if (g_printData) delete g_printData;
    if (g_pageSetupData) delete g_pageSetupData;
	return 0;
}
