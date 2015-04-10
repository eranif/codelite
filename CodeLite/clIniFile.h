#ifndef CLINIFILE_H
#define CLINIFILE_H

#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/string.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clIniFile : public wxFileConfig
{
public:
    /**
     * @brief create a INI file under iniFullPath.GetFullPath()
     */
    clIniFile(const wxFileName& iniFullPath);
    
    /**
     * @brief create a INI file under ~/.codelite/config/fullname
     * note that the fullname should include the file suffix
     */
    clIniFile(const wxString& fullname);
    
    virtual ~clIniFile();
};

#endif // CLINIFILE_H
