#ifndef IMACROMANAGER_H
#define IMACROMANAGER_H

#include <wx/string.h>

class IManager;
class IMacroManager
{
public:
	IMacroManager() {}
	virtual ~IMacroManager() {}
	
	/**
	 * \brief expand 'expression' by replacing all macros with their values
	 * \param expression
	 * \param manager
	 * \return 
	 */
	virtual wxString Expand(const wxString &expression, IManager *manager, const wxString &project, const wxString &confToBuild = wxEmptyString) = 0;
};

#endif // IMACROMANAGER_H
