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
    
};

#endif // CLSTANDARDPATHS_H
