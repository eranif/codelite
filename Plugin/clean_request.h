#ifndef CLEAN_REQUEST_H
#define CLEAN_REQUEST_H
#include "compiler_action.h"

#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK

/**
 * \class CleanRequest
 * \brief 
 * \author Eran
 * \date 07/22/07
 */
class WXDLLIMPEXP_LE_SDK CleanRequest : public CompilerAction {
	wxString m_project;
	bool m_projectOnly;

public:
	/**
	 * Construct a compilation clean request. The compiler thread will clean the selected project and all
	 * its dependencies as appeard in the build order dialog
	 * \param projectName the selected project to build
	 * \param configurationName the workspace selected configuration
	 */
	CleanRequest(wxEvtHandler *owner, const wxString &projectName, bool projectOnly);

	///dtor
	virtual ~CleanRequest();

	//process the request
	virtual void Process();

	//setters/getters
	const wxString &GetProjectName() const { return m_project; }
};

#endif // CLEAN_REQUEST_H
