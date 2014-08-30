#ifndef PHPUSERWORKSPACE_H
#define PHPUSERWORKSPACE_H

#include "XDebugBreakpoint.h"
#include <wx/filename.h>

class PHPUserWorkspace
{
    XDebugBreakpoint::List_t m_breakpoints;
    wxString                 m_workspacePath;
    
protected:
    wxFileName GetFileName() const;

public:
    PHPUserWorkspace(const wxString &workspacePath);
    virtual ~PHPUserWorkspace();

    PHPUserWorkspace& Load();
    PHPUserWorkspace& Save();
    PHPUserWorkspace& SetBreakpoints(const XDebugBreakpoint::List_t& breakpoints) {
        this->m_breakpoints = breakpoints;
        return *this;
    }
    const XDebugBreakpoint::List_t& GetBreakpoints() const {
        return m_breakpoints;
    }
};

#endif // PHPUSERWORKSPACE_H
