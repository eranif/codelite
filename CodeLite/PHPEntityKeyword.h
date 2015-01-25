#ifndef PHPENTITYKEYWORD_H
#define PHPENTITYKEYWORD_H

#include "PHPEntityBase.h"
#include "PHPEntityBase.h" // Base class: PHPEntityBase
#include <wx/string.h>

class WXDLLIMPEXP_CL PHPEntityKeyword : public PHPEntityBase
{
public:
    PHPEntityKeyword();
    virtual ~PHPEntityKeyword();

public:
    virtual wxString FormatPhpDoc() const;
    virtual void FromResultSet(wxSQLite3ResultSet& res);
    virtual wxString GetDisplayName() const;
    virtual bool Is(eEntityType type) const;
    virtual void PrintStdout(int indent) const;
    virtual void Store(wxSQLite3Database& db);
    virtual wxString Type() const;
};

#endif // PHPENTITYKEYWORD_H
