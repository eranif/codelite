//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cl_standard_paths.h
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

#ifndef CLSTANDARDPATHS_H
#define CLSTANDARDPATHS_H

#include <wx/string.h>
#include "codelite_exports.h"
#include "cl_defs.h"

class WXDLLIMPEXP_CL clStandardPaths
{
private:
    clStandardPaths();
    virtual ~clStandardPaths();
    
    wxString m_path;
    
public:
    static clStandardPaths& Get();
    
    /**
     * @brief remove the temp directory and its content
     */
    void RemoveTempDir();
    
    /**
     * @brief return the user local data folder. Usually this is ~/.codelite or %APPDATA%\CodeLite 
     * However, under Linux, this function will return ~/.codelite-dbg to avoid clobbering the release config
     * settings
     */
    wxString GetUserDataDir() const;
    /**
     * @brief Sets an alternative local data folder, to be returned by GetUserDataDir()
     */
    void SetUserDataDir(const wxString& path);
    
    /**
     * @brief return the temp folder for this OS
     */
    wxString GetTempDir() const;
    
    /**
     * @brief return the path to the plugins folder
     */
    wxString GetPluginsDirectory() const;
    
    /**
     * @brief Return the location of the applications global, i.e. not user-specific, data files.
     * Example return values:
        Unix: prefix/share/appinfo
        Windows: the directory where the executable file is located
        Mac: appinfo.app/Contents/SharedSupport bundle subdirectory
     */
    wxString GetDataDir() const;

    // Under MSW, OSX and GTK the bin folder is the same directory as the
    // main executable (codelite{.exe})
    // Typically this function returns:
    // MSW: /path/to/codelite.exe
    // GTK: /usr/bin
    // OSX: /path/to/codelite.app/Contents/MacOS
    wxString GetBinFolder() const;

    /**
     * @brief return the full path for an executable. This function
     * usually returns: GetBinFolder() + "/" + toolname + ".exe"
     * @note the .exe and "/" are platform dependant
     */
    wxString GetBinaryFullPath(const wxString &toolname) const;
    
    /**
     * @brief get CodeLite executale path
     */
    wxString GetExecutablePath() const;
    
    /**
     * @brief return the user lexers directory
     */
    wxString GetUserLexersDir() const;

    /**
     * @brief return the default (installation) lexers directory
     */
    wxString GetLexersDir() const;
    
    /**
     * @brief return the project templates dircectory
     */
    wxString GetProjectTemplatesDir() const;
    
    /**
     * @brief return the user templates folder
     * @return 
     */
    wxString GetUserProjectTemplatesDir() const;
    
    /**
     * @brief return the documents directory.
     * Windows: C:\Users\name\Documents
     * Unix: /home/name/Documents
     * OSX: /Users/name/Documents
     */
    wxString GetDocumentsDir() const;
    
    /**
     * @brief return the installation directory.
     */
    wxString GetInstallDir() const;
    
    void IgnoreAppSubDir(const wxString & subdirPattern);
};

#endif // CLSTANDARDPATHS_H
