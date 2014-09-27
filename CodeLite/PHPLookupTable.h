#ifndef PHPLOOKUPTABLE_H
#define PHPLOOKUPTABLE_H

#include "codelite_exports.h"
#include <wx/string.h>
#include "PHPEntityBase.h"

class WXDLLIMPEXP_CL PHPLookupTable
{
public:
    PHPLookupTable();
    virtual ~PHPLookupTable();
    
    /**
     * @brief find a symbol by name and parent
     * Note that the parent must be in absolute path
     */
    virtual PHPEntityBase::Ptr_t FindSymbol(const wxString &name, const wxString &path);
};

#endif // PHPLOOKUPTABLE_H
