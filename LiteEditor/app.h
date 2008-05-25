//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : app.h              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )                     
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/                     
//              \____/\___/ \__,_|\___\_____/_|\__\___|                     
//                                                                          
//                                                  F i l e                 
//                                                                          
//    This program is free software; you can redistribute it and/or modify  
//    it under the terms of the GNU General Public License as published by  
//    the Free Software Foundation; either version 2 of the License, or     
//    (at your option) any later version.                                   
//                                                                          
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
 #ifndef LITEEDITOR_APP_H
#define LITEEDITOR_APP_H

#include "frame.h"
class SplashScreen;

class App : public wxApp
{
private:
	SplashScreen* m_splash;
	Frame *m_pMainFrame;
	bool ReadControlFile(wxString &installPath, long &installRev);
	void CopySettings(const wxString &destDir, const wxString &installPath);
	bool CheckRevision(const wxString &fileName);

#ifdef __WXMSW__	
	HINSTANCE m_handler;
#endif

public:
	App(void);
	virtual ~App(void);

protected:
	virtual bool OnInit();
	virtual int OnExit();
	virtual void OnFatalException();

private:
	DECLARE_EVENT_TABLE()
	void OnIdle(wxIdleEvent &e);
	void OnHideSplash(wxCommandEvent &e);
};

#endif // LITEEDITOR_APP_H
