/*********************************************************************
 * Name:      	main.cpp
 * Purpose:   	Implements simple wxWidgets application with GUI.
 * Author:    
 * Created:   
 * Copyright: 
 * License:   	wxWidgets license (www.wxwidgets.org)
 * 
 * Notes:		
 *********************************************************************/
 
#include <wx/wx.h>

// application class
class wxMiniApp : public wxApp
{
    public:
		// function called at the application initialization
        virtual bool OnInit();

		// event handler for button click
        void OnClick(wxCommandEvent& event) { GetTopWindow()->Close(); }
};

IMPLEMENT_APP(wxMiniApp);

bool wxMiniApp::OnInit()
{
	// create a new frame and set it as the top most application window
    SetTopWindow( new wxFrame( NULL, -1, wxT(""), wxDefaultPosition, wxSize( 100, 50) ) );

	// create new button and assign it to the main frame
    new wxButton( GetTopWindow(), wxID_EXIT, wxT("Click!") );

	// connect button click event with event handler
    Connect(wxID_EXIT, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(wxMiniApp::OnClick) );

	// show main frame
    GetTopWindow()->Show();

	// enter the application's main loop
    return true;
}
