#ifndef PHPENTITYNAMESPACE_H
#define PHPENTITYNAMESPACE_H

#include "codelite_exports.h"
#include "PHPEntityBase.h" // Base class: PHPEntityBase
#include <wx/string.h>

class WXDLLIMPEXP_CL PHPEntityNamespace : public PHPEntityBase
{
protected:
    void DoEnsureNamespacePathExists(wxSQLite3Database& db, const wxString& path);

public:
    wxString GetParentNamespace() const;
    virtual wxString FormatPhpDoc() const;
    virtual wxString GetDisplayName() const;
    virtual bool Is(eEntityType type) const;
    virtual wxString Type() const;
    virtual void FromResultSet(wxSQLite3ResultSet& res);
    virtual void Store(wxSQLite3Database& db);
    virtual void PrintStdout(int indent) const;
    PHPEntityNamespace();
    virtual ~PHPEntityNamespace();
    
    /**
     * @brief build namespace from 2 strings
     */
    static wxString BuildNamespace(const wxString &part1, const wxString &part2);
};

#endif // PHPENTITYNAMESPACE_H
