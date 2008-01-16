#ifndef __dynamiclibrary__
#define __dynamiclibrary__

#include "wx/dynlib.h"
#include "wx/string.h"

class clDynamicLibrary {

#if defined (__WXMSW__) || defined (__WXGTK__)
	wxDynamicLibrary m_lib;
#else //Mac OSX
	void *m_dllhandle;
#endif

public:
	clDynamicLibrary();
	~clDynamicLibrary();
	
	bool Load(const wxString &name);
	void Detach();
	void* GetSymbol(const wxString &name, bool *success);

};
#endif // __dynamiclibrary__
