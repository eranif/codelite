#ifndef PHPINDEX_H
#define PHPINDEX_H

#include "codelite_exports.h"
#include <wx/string.h>
#include <wx/arrstr.h>

/**
 * @class PHPIndex this class represents a collection of PHPSourceFiles
 */
class WXDLLIMPEXP_CL PHPIndex
{
public:
    PHPIndex();
    virtual ~PHPIndex();
    
    /**
     * @brief index list of files and store them into the database
     */
    void Index(const wxArrayString& files, const wxString &dbpath);
};

#endif // PHPINDEX_H
