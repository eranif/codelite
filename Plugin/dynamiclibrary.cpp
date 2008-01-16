#include "dynamiclibrary.h"

#ifdef __WXMAC__
# include <dlfcn.h>
# include "precompiled_header.h"
#endif

clDynamicLibrary::clDynamicLibrary()
#ifdef __WXMAC__
		:
		m_dllhandle(NULL)
#endif
{
}

clDynamicLibrary::~clDynamicLibrary()
{
#ifdef __WXMAC__
	if(m_dllhandle){
		dlclose(m_dllhandle);
		m_dllhandle = NULL;
	}
#endif
}

bool clDynamicLibrary::Load(const wxString &name)
{
#if defined (__WXMSW__) || defined (__WXGTK__)
	return m_lib.Load(name);
#else
	// open the library
	m_dllhandle = dlopen(_C(name), RTLD_LAZY);
	if (!m_dllhandle) {
		wxString error = wxString(dlerror(), wxConvUTF8);
		return false;
	}
	return true;
#endif
}

void clDynamicLibrary::Detach()
{
#if defined (__WXMSW__) || defined (__WXGTK__)
	m_lib.Detach();
#else
	if (m_dllhandle) {
		dlclose(m_dllhandle);
		m_dllhandle = NULL;
	}
#endif
}

void *clDynamicLibrary::GetSymbol(const wxString &name, bool *success)
{
#if defined (__WXMSW__) || defined (__WXGTK__)
	bool rc;
	void *symb = m_lib.GetSymbol(name, &rc);
	*success = rc;
	return symb;
#else
	dlerror(); // reset errors

	// load the symbol
	void *symb = dlsym(m_dllhandle, _C(name));
	if(symb){
		*success = true;
	}else{
		*success = false;
	}
	return symb;
#endif
}
