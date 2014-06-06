#ifndef CLSTANDARDPATHS_H
#define CLSTANDARDPATHS_H

#include <wx/string.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clStandardPaths
{
private:
    clStandardPaths();
    virtual ~clStandardPaths();
    
    wxString m_path;
    
public:
    static clStandardPaths& Get();
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
};

#endif // CLSTANDARDPATHS_H
