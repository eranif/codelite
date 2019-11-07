#ifndef CLPYTHONLOCATOR_HPP
#define CLPYTHONLOCATOR_HPP

#include "codelite_exports.h"
#include <wx/string.h>

class WXDLLIMPEXP_SDK clPythonLocator
{
    wxString m_python;
    wxString m_pip;

protected:
    bool MSWLocate();

public:
    clPythonLocator();
    virtual ~clPythonLocator();

    /**
     * @brief locate python
     */
    bool Locate();

    const wxString& GetPython() const { return m_python; }
    const wxString& GetPip() const { return m_pip; }
};

#endif // CLPYTHONLOCATOR_HPP
