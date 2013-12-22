#ifndef CLSTANDARDPATHS_H
#define CLSTANDARDPATHS_H

#include <wx/string.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clStandardPaths
{
private:
    clStandardPaths();
    virtual ~clStandardPaths();
    
public:
    static clStandardPaths& Get();
    /**
     * @brief return the user local data folder. Usually this is ~/.codelite or %APPDATA%\CodeLite 
     * However, under Linux, this function will return ~/.codelite-dbg to avoid cloberring with the release
     * settings
     */
    wxString GetUserDataDir() const;
    
};

#endif // CLSTANDARDPATHS_H
