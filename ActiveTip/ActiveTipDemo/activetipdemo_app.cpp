#include "activetipdemo_app.h"
#include "wx/image.h"
#include "wx/sysopt.h"
#include "wx/xrc/xmlres.h"
#include "activetipdemo_frame.h"

IMPLEMENT_APP(ActivetipdemoApp)

ActivetipdemoApp::ActivetipdemoApp()
{
}

ActivetipdemoApp::~ActivetipdemoApp()
{
}

bool ActivetipdemoApp::OnInit()
{
	// Init resources and add the PNG handler
	wxSystemOptions::SetOption(_T("msw.remap"), 0);
	wxXmlResource::Get()->InitAllHandlers();
	wxImage::AddHandler( new wxPNGHandler );
	wxImage::AddHandler( new wxCURHandler );
	wxImage::AddHandler( new wxICOHandler );
	wxImage::AddHandler( new wxXPMHandler );
	wxImage::AddHandler( new wxGIFHandler );

	wxFrame* frame = new ActivetipdemoFrame(NULL,
                                   wxID_ANY,
                                   wxT("ActiveTipDemo"),
                                   wxDefaultPosition,
								   wxSize(800, 600));
	


	frame->Show(TRUE);
	SetTopWindow(frame);
	return TRUE;
}

int ActivetipdemoApp::OnExit()
{
	return 0;
}

