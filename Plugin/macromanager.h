//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : macromanager.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef MACROMANAGER_H
#define MACROMANAGER_H

#include "codelite_exports.h"

#include <wx/string.h>

class IManager;
class WXDLLIMPEXP_SDK MacroManager
{
public:
    static MacroManager* Instance();

private:
    MacroManager();
    virtual ~MacroManager();

    wxString DoExpand(const wxString& expression, IManager* manager, const wxString& project, bool applyEnv,
                      const wxString& confToBuild = wxEmptyString);

public:
    /*
     * The following macro will be expanded into their real values:
     * $(ProjectPath)
     * $(WorkspaceName)
     * $(WorkspaceConfiguration)
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
     * $(OutputDirectory)
     * $(ProjectOutputFile)
     * $(Selection)
     */
    wxString Expand(const wxString& expression, IManager* manager, const wxString& project,
                    const wxString& confToBuild = wxEmptyString);

    /**
     * @brief replace all file related macros $(CurrentFile*)
     * taking into consideration remote/local paths
     */
    wxString ExpandFileMacros(const wxString& expression, const wxString& filepath);

    /**
     * @brief return true if macroname can be resolved as CodeLite internal macro
     */
    bool IsCodeLiteMacro(const wxString& macroname) const;

    /**
     * @brief same as above, but don't apply environment variables
     */
    wxString ExpandNoEnv(const wxString& expression, const wxString& project,
                         const wxString& confToBuild = wxEmptyString);

    /**
     * @brief search for variableName and replace all its occurance with 'replaceWith'
     * This function supports the following formats:
     * $variableName
     * ${variableName}
     * $(variableName)
     * %variableName%
     */
    wxString Replace(const wxString& inString, const wxString& variableName, const wxString& replaceWith,
                     bool bIgnoreCase = false);

    /**
     * @brief search for variable in the inString.
     * @param inString [input]
     * @param name [output] name only
     * @param fullname [output] includes the $ / %%
     */
    bool FindVariable(const wxString& inString, wxString& name, wxString& fullname);
};

#endif // MACROMANAGER_H
