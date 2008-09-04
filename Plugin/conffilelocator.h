#ifndef __conffilelocator__
#define __conffilelocator__
#include <wx/string.h>

class ConfFileLocator {

	static ConfFileLocator* ms_instance;
	wxString m_installPath;
	wxString m_startupPath;
	
private:
	ConfFileLocator(const ConfFileLocator& rhs);
	ConfFileLocator& operator=(const ConfFileLocator& rhs);

public:
	static ConfFileLocator* Instance();
	static void Release();
	
	void Initialize(const wxString &installpath, const wxString &startuppath);
	wxString Locate(const wxString &baseName);
	wxString GetLocalCopy(const wxString &baseName);
	wxString GetDefaultCopy(const wxString &baseName);
	
private:
	ConfFileLocator();
	virtual ~ConfFileLocator();
	
	
	

};
#endif // __conffilelocator__
