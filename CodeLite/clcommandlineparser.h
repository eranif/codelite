#ifndef CLCOMMANDLINEPARSER_H
#define CLCOMMANDLINEPARSER_H

#include <wx/string.h>
#include <wx/arrstr.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clCommandLineParser
{
    wxString      m_commandline;
    size_t        m_flags;
    wxArrayString m_tokens;
public:
    enum {
        // ignore both \r and \n 
        kIgnoreNewLines = 0x00000001, 
    };
protected:
    void DoParse();
public:
    clCommandLineParser(const wxString &str, size_t flags = kIgnoreNewLines);
    virtual ~clCommandLineParser();
    
    const wxArrayString& ToArray() const {
        return m_tokens;
    }
};

#endif // CLCOMMANDLINEPARSER_H
