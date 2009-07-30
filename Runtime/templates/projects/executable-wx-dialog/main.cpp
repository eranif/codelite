#include "main.h"

// initialize the application
IMPLEMENT_APP(MainApp);

////////////////////////////////////////////////////////////////////////////////
// application class implementation 
////////////////////////////////////////////////////////////////////////////////

bool MainApp::OnInit()
{
	SetTopWindow( new MainDialog( NULL ) );
	GetTopWindow()->Show();
	
	// true = enter the main loop
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// main application frame implementation 
////////////////////////////////////////////////////////////////////////////////

MainDialog::MainDialog(wxWindow *parent) : MainDialogBase( parent )
{
}

MainDialog::~MainDialog()
{
}

void MainDialog::OnCloseDialog(wxCloseEvent& event)
{
	Destroy();
}

void MainDialog::OnOKClick(wxCommandEvent& event)
{
	Destroy();
}

void MainDialog::OnCancelClick(wxCommandEvent& event)
{
	Destroy();
}

