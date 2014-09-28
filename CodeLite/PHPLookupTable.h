#ifndef PHPLOOKUPTABLE_H
#define PHPLOOKUPTABLE_H

#include "codelite_exports.h"
#include <wx/string.h>
#include "PHPEntityBase.h"
#include "wx/wxsqlite3.h"
#include "PHPSourceFile.h"

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
    void SplitFullname(const wxString& fullname, wxString& name, wxString& scope);

public:
    PHPLookupTable();
    virtual ~PHPLookupTable();

    /**
     * @brief open the lookup table database
     */
    void Open(const wxString& workspacePath);

    /**
     * @brief find a symbol by fullname (scope+name)
     */
    virtual PHPEntityBase::Ptr_t FindSymbol(const wxString& fullname);
    
    /**
     * @brief save source file into the database
     */
    void UpdateSourceFile(PHPSourceFile &source);
};

#endif // PHPLOOKUPTABLE_H
