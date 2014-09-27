#ifndef PHPLOOKUPTABLE_H
#define PHPLOOKUPTABLE_H

#include "codelite_exports.h"
#include <wx/string.h>
#include "PHPEntityBase.h"
#include "wx/wxsqlite3.h"

/**
 * @class PHPLookupTable
 * @brief a database lookup class which allows a quick access
 * to all entities parsed in the workspace
 */
class WXDLLIMPEXP_CL PHPLookupTable
{
    wxSQLite3Database m_db;
private:
    void CreateSchema();
    
public:
    PHPLookupTable();
    virtual ~PHPLookupTable();
    
    /**
     * @brief open the lookup table database
     */
    void Open(const wxString& workspacePath);
    
    /**
     * @brief find a symbol by name and parent
     * Note that the parent must be in absolute path
     */
    virtual PHPEntityBase::Ptr_t FindSymbol(const wxString &name, const wxString &path);
};

#endif // PHPLOOKUPTABLE_H
