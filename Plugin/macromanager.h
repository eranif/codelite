#ifndef MACROMANAGER_H
#define MACROMANAGER_H

#include <wx/string.h>
#include "codelite_exports.h"

class IManager;
class WXDLLIMPEXP_SDK MacroManager
{
public:
	static MacroManager* Instance();

private:
	MacroManager();
	virtual ~MacroManager();

public:
	/*
	 * The following macro will be expanded into their real values:
	 * $(ProjectPath)
	 * $(WorkspacePath)
	 * $(ProjectName)
	 * $(IntermediateDirectory)
	 * $(ConfigurationName)
	 * $(OutDir)
	 * $(ProjectFiles)
	 * $(ProjectFiles)
	 * $(ProjectFilesAbs)
	 * $(CurrentFileName)
	 * $(CurrentFilePath)
	 * $(CurrentFileExt)
	 * $(CurrentFileFullPath)
	 * $(User)
	 * $(Date)
	 * $(CodeLitePath)
	 * $(CurrentSelection)
	 * $(ProjectOutputFile)
	 */
	wxString Expand(const wxString &expression, IManager *manager, const wxString &project, const wxString &confToBuild = wxEmptyString);
	
	
	/**
	 * @brief search for variableName and replace all its occurance with 'replaceWith'
	 * This function supports the following formats:
	 * $variableName
	 * ${variableName}
	 * $(variableName)
	 * %variableName%
	 */
	wxString Replace(const wxString &inString, const wxString &variableName, const wxString &replaceWith, bool bIgnoreCase = false);
	
	/**
	 * @brief search for variable in the inString.
	 * @param inString [input]
	 * @param name [output] name only
	 * @param fullname [output] includes the $ / %%
	 */
	bool FindVariable(const wxString &inString, wxString &name, wxString &fullname);
};

#endif // MACROMANAGER_H
