#include "MainApp.h"
#include <stdio.h>

MainApp::MainApp()
{
    std::map<wxString, wxString> m;

    //for(auto it : m) {
    //    size_t len = it.first.Len();
	//	wxUnusedVar(len);
    //}
}

MainApp::~MainApp()
{
}

bool MainApp::OnInit()
{
    //wxFrame *frame = new wxFrame(NULL, wxID_ANY, "My Frame");
    //SetTopWindow( frame );
    //frame->Show();
    for(int i=0; i<1000; ++i) {
        printf("Hello world\n");
        wxThread::Sleep(1000);
    }
    return true;
}
