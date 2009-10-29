#ifndef __ActivetipdemoApp__
#define __ActivetipdemoApp__

#include "wx/app.h"

class ActivetipdemoApp : public wxApp {
	

public:
	ActivetipdemoApp();
	~ActivetipdemoApp();
	
protected:
	virtual bool OnInit();
	virtual int OnExit();
};
#endif // ActivetipdemoApp

