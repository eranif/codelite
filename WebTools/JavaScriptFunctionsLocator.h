#ifndef JAVASCRIPTFUNCTIONSLOCATOR_H
#define JAVASCRIPTFUNCTIONSLOCATOR_H

#include "CxxScannerBase.h"
#include <wx/string.h>
#include "macros.h"

class JavaScriptFunctionsLocator : public CxxScannerBase
{
    wxString m_lastIdentifier;
    wxStringSet_t m_functions;
    wxStringSet_t m_keywords;

protected:
    void OnToken(CxxLexerToken& token);

public:
    JavaScriptFunctionsLocator(CxxPreProcessor* preProcessor, const wxFileName& filename);
    virtual ~JavaScriptFunctionsLocator();
    
    wxString GetString() const;
};

#endif // JAVASCRIPTFUNCTIONSLOCATOR_H
