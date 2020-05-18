#ifndef CLDIRCHANGER_HPP
#define CLDIRCHANGER_HPP

#include "codelite_events.h"
#include <wx/string.h>

class WXDLLIMPEXP_CL clDirChanger
{
protected:
    wxString m_oldDirectory;

public:
    clDirChanger(const wxString& newDirectory)
    {
        m_oldDirectory = ::wxGetCwd();
        if(!newDirectory.IsEmpty()) {
            ::wxSetWorkingDirectory(newDirectory);
        }
    }
    virtual ~clDirChanger() { ::wxSetWorkingDirectory(m_oldDirectory); }
};

#endif // CLDIRCHANGER_HPP
