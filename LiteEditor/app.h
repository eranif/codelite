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
	void CopySettings(const wxString &installPath);
	bool CheckRevision(const wxString &fileName);
	
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
