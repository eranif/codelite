#ifndef PERSPECTIVEMANAGER_H
#define PERSPECTIVEMANAGER_H

#include "precompiled_header.h"
#include <wx/string.h>
#include <wx/arrstr.h>

extern wxString DEBUG_LAYOUT ;
extern wxString NORMAL_LAYOUT;

class PerspectiveManager
{

public:
	PerspectiveManager();
	virtual ~PerspectiveManager();

	void LoadPerspective(const wxString &name);
	void SavePerspective(const wxString &name);
	wxArrayString GetAllPerspectives();
	void DeleteAllPerspectives();
};

#endif // PERSPECTIVEMANAGER_H
