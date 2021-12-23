#ifndef PHPOPTIONS_H
#define PHPOPTIONS_H

#include "cl_config.h" // Base class: clConfigItem
#include "codelite_exports.h"

#include <wx/arrstr.h>

class WXDLLIMPEXP_SDK PhpOptions : public clConfigItem
{
    wxString m_phpExe;
    wxArrayString m_includePaths;
    wxString m_errorReporting;

public:
    PhpOptions();
    virtual ~PhpOptions();

    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;

    PhpOptions& Load();
    PhpOptions& Save();

    PhpOptions& SetErrorReporting(const wxString& errorReporting)
    {
        this->m_errorReporting = errorReporting;
        return *this;
    }
    PhpOptions& SetIncludePaths(const wxArrayString& includePaths)
    {
        this->m_includePaths = includePaths;
        return *this;
    }
    PhpOptions& SetPhpExe(const wxString& phpExe)
    {
        this->m_phpExe = phpExe;
        return *this;
    }
    const wxString& GetErrorReporting() const { return m_errorReporting; }
    const wxArrayString& GetIncludePaths() const { return m_includePaths; }
    const wxString& GetPhpExe() const { return m_phpExe; }
};

#endif // PHPOPTIONS_H
