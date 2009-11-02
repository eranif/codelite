#include "activetipdemo_frame.h"
#include "ActiveTip/active_tip.h"
#include <wx/toolbar.h>
#include <wx/statusbr.h>
#include <wx/sizer.h>
#include <wx/menu.h> //wxMenuBar

ATFrame CreateNewFrame()
{
	static int count(0);

	// Construct a frame
	ATFrame content;
	for(int i=0; i<5; i++) {
		ATLine line(wxString::Format(wxT("Key_%d"), ++count), wxT(""), count % 3 == 0 ? true : false, NULL);
		line.SetValue( wxString::Format(wxT("Line Value %d"), count ) );
		content.m_lines.push_back( line );
	}
	return content;
}

BEGIN_EVENT_TABLE(ActivetipdemoFrame, wxFrame)
	EVT_CLOSE(ActivetipdemoFrame::OnClose)
	EVT_MENU(10903,     ActivetipdemoFrame::OnShowTip)
	EVT_MENU(wxID_EXIT, ActivetipdemoFrame::OnQuit)
END_EVENT_TABLE()

ActivetipdemoFrame::ActivetipdemoFrame(wxWindow* parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style)
		: wxFrame(parent, id, title, pos, size, style)
{
	SetBackgroundColour( wxT("WHITE") );
	Initialize();
}

ActivetipdemoFrame::~ActivetipdemoFrame()
{

}

void ActivetipdemoFrame::Initialize()
{
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	SetSizer(sz);

	//Create a status bar
	wxStatusBar* statusBar = new wxStatusBar(this, wxID_ANY);
	statusBar->SetFieldsCount(1);
	SetStatusBar(statusBar);
	GetStatusBar()->SetStatusText(wxT("Ready"));

	//Create a menu bar
	CreateMenuBar();
	sz->Layout();
}

void ActivetipdemoFrame::OnClose(wxCloseEvent &e)
{
	e.Skip();
}

void ActivetipdemoFrame::OnQuit(wxCommandEvent &e)
{
	wxUnusedVar(e);
	Close();
}

void ActivetipdemoFrame::CreateMenuBar()
{
	wxMenuBar *mb = new wxMenuBar();
	//File Menu
	wxMenu *menu = new wxMenu();
	menu->Append(10903, wxT("Show tip"), wxT("Show tip"), false);
	menu->AppendSeparator();
	menu->Append(wxID_EXIT);

	mb->Append(menu, wxT("&File"));

	SetMenuBar(mb);
}

void ActivetipdemoFrame::OnShowTip(wxCommandEvent& e)
{
	ATFrame frame = CreateNewFrame();
	frame.m_title = wxT("First Frame");
	ActiveTipWinodw tip(this, frame);
	tip.ShowModal();
}
