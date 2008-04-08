#include "notebook_app.h"
#include "wx/image.h"
#include "wx/sysopt.h"
#include "wx/xrc/xmlres.h"
#include "notebook_frame.h"

IMPLEMENT_APP(NotebookApp)

NotebookApp::NotebookApp()
{
}

NotebookApp::~NotebookApp()
{
}

bool NotebookApp::OnInit()
{
	// Init resources and add the PNG handler
	wxSystemOptions::SetOption(_T("msw.remap"), 0);
	wxXmlResource::Get()->InitAllHandlers();
	wxImage::AddHandler( new wxPNGHandler );
	wxImage::AddHandler( new wxCURHandler );
	wxImage::AddHandler( new wxICOHandler );
	wxImage::AddHandler( new wxXPMHandler );
	wxImage::AddHandler( new wxGIFHandler );

	wxFrame* frame = new NotebookFrame(NULL,
                                   wxID_ANY,
                                   wxT("Notebook"),
                                   wxDefaultPosition,
								   wxSize(800, 600));
	


	frame->Show(TRUE);
	SetTopWindow(frame);
	return TRUE;
}

int NotebookApp::OnExit()
{
	return 0;
}

