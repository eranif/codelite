#ifndef CLCONSOLEBASH_H
#define CLCONSOLEBASH_H

#include "clConsoleBase.h"
#include "codelite_exports.h"
#include <wx/filename.h>

class WXDLLIMPEXP_CL clConsoleBash : public clConsoleBase
{
protected:
    /**
     * @brief return execution script and return its path
     */
    wxFileName PrepareExecScript() const;

public:
    clConsoleBash();
    virtual ~clConsoleBash();
};

#endif // CLCONSOLEBASH_H
