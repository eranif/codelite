#ifndef MACROMANAGER_H
#define MACROMANAGER_H

#include "imacromanager.h" // Base class: IMacroManager
#include <wx/string.h>

class IManager;
class MacroManager : public IMacroManager {

	static MacroManager* ms_instance;

public:
	static MacroManager* Instance();
	static void Release();

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
	 */
	wxString Expand(const wxString &expression, IManager *manager, const wxString &project, const wxString &confToBuild = wxEmptyString);
};

#endif // MACROMANAGER_H
