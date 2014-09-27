#ifndef PHPENTITYNAMESPACE_H
#define PHPENTITYNAMESPACE_H

#include "codelite_exports.h"
#include "PHPEntityBase.h" // Base class: PHPEntityBase
#include <wx/string.h>

class WXDLLIMPEXP_CL PHPEntityNamespace : public PHPEntityBase
{
    wxString m_path;

public:
    virtual void PrintStdout(int indent) const;
    PHPEntityNamespace();
    virtual ~PHPEntityNamespace();

    void SetPath(const wxString& path) { this->m_path = path; }
    const wxString& GetPath() const { return m_path; }

    virtual wxString ID() const { return m_path; }
};

#endif // PHPENTITYNAMESPACE_H
