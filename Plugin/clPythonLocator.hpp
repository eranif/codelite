#ifndef CLPYTHONLOCATOR_HPP
#define CLPYTHONLOCATOR_HPP

#include "codelite_exports.h"
#include <wx/string.h>

class WXDLLIMPEXP_SDK clPythonLocator
{
    wxString m_python;

protected:
    bool MSWLocate();

public:
    clPythonLocator();
    virtual ~clPythonLocator();

    /**
     * @brief locate python
     */
    bool Locate();
};

#endif // CLPYTHONLOCATOR_HPP
