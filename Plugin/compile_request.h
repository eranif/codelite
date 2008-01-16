#ifndef COMPILE_REQUEST_H
#define COMPILE_REQUEST_H

#include "compiler_action.h"

#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK

class WXDLLIMPEXP_LE_SDK CompileRequest : public CompilerAction {
	wxString m_project;
	bool m_projectOnly;
	wxString m_fileName;
	
public:
	/**
	 * Construct a compilation request. The compiler thread will build the selected project and all
	 * its dependencies as appeard in the build order dialog
	 * \param projectName the selected project to build
	 * \param configurationName the workspace selected configuration
	 */
	CompileRequest(wxEvtHandler *owner, const wxString &projectName, bool projectOnly = false, const wxString &fileName = wxEmptyString);

	///dtor
	virtual ~CompileRequest();

	//process the request
	virtual void Process();

	//setters/getters
	const wxString &GetProjectName() const { return m_project; }
};

#endif // COMPILE_REQUEST_H
