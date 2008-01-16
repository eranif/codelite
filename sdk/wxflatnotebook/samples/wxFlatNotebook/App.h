#pragma once 
#include <wx/wx.h>

class App :
	public wxApp
{
public:
	App(void);
	~App(void);
	virtual bool OnInit(void);
	virtual int OnExit(void);
};
DECLARE_APP(App)
