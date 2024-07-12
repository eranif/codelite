#ifndef COMPILEFLAGSTXT_H
#define COMPILEFLAGSTXT_H

#include "codelite_exports.h"

#include <wx/arrstr.h>
#include <wx/filename.h>

class WXDLLIMPEXP_SDK CompileFlagsTxt
{
    wxFileName m_filename;
    wxArrayString m_includes;
    wxArrayString m_macros;
    wxArrayString m_others;

public:
    CompileFlagsTxt(const wxFileName& filename);
    virtual ~CompileFlagsTxt();
    void SetFilename(const wxFileName& filename) { this->m_filename = filename; }
    void SetIncludes(const wxArrayString& includes) { this->m_includes = includes; }
    void SetMacros(const wxArrayString& macros) { this->m_macros = macros; }
    void SetOthers(const wxArrayString& others) { this->m_others = others; }
    const wxFileName& GetFilename() const { return m_filename; }
    const wxArrayString& GetIncludes() const { return m_includes; }
    const wxArrayString& GetMacros() const { return m_macros; }
    const wxArrayString& GetOthers() const { return m_others; }
};

#endif // COMPILEFLAGSTXT_H
