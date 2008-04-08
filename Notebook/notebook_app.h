#ifndef __NotebookApp__
#define __NotebookApp__

#include "wx/app.h"

class NotebookApp : public wxApp {
	

public:
	NotebookApp();
	~NotebookApp();
	
protected:
	virtual bool OnInit();
	virtual int OnExit();
};
#endif // NotebookApp

