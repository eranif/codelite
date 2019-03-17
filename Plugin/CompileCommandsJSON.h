#ifndef COMPILECOMMANDSJSON_H
#define COMPILECOMMANDSJSON_H

#include "codelite_exports.h"
#include <wx/filename.h>
#include <wx/arrstr.h>

class WXDLLIMPEXP_SDK CompileCommandsJSON
{
    wxFileName m_filename;
    wxArrayString m_macros;
    wxArrayString m_includes;
    wxArrayString m_others;

public:
    CompileCommandsJSON(const wxString& filename);
    virtual ~CompileCommandsJSON();
    
    void SetFilename(const wxFileName& filename) { this->m_filename = filename; }
    void SetIncludes(const wxArrayString& includes) { this->m_includes = includes; }
    void SetMacros(const wxArrayString& macros) { this->m_macros = macros; }
    void SetOthers(const wxArrayString& others) { this->m_others = others; }
    const wxFileName& GetFilename() const { return m_filename; }
    const wxArrayString& GetIncludes() const { return m_includes; }
    const wxArrayString& GetMacros() const { return m_macros; }
    const wxArrayString& GetOthers() const { return m_others; }
};

#endif // COMPILECOMMANDSJSON_H
