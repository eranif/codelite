#ifndef NODEJSLOCATOR_H
#define NODEJSLOCATOR_H

#include "codelite_exports.h"
#include <wx/filename.h>
#include <wx/string.h>
#include <wx/arrstr.h>

class WXDLLIMPEXP_SDK NodeJSLocator
{
    wxString m_nodejs;
    wxString m_npm;

protected:
    bool TryPaths(const wxArrayString& paths, const wxString& fullname, wxFileName& fullpath);

public:
    NodeJSLocator();
    ~NodeJSLocator();

    /**
     * @brief attempt to auto locate nodejs installation
     */
    void Locate();

    const wxString& GetNodejs() const { return m_nodejs; }
    const wxString& GetNpm() const { return m_npm; }
};

#endif // NODEJSLOCATOR_H
