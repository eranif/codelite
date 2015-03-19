#ifndef JAVASCRIPTFUNCTIONSLOCATOR_H
#define JAVASCRIPTFUNCTIONSLOCATOR_H

#include "CxxScannerBase.h"
#include <wx/string.h>
#include "macros.h"

class JavaScriptFunctionsLocator : public CxxScannerBase
{
    enum eState { kNormal, kScopeOperator };
    wxString m_lastIdentifier;
    wxStringSet_t m_functions;
    wxStringSet_t m_properties;
    wxStringSet_t m_keywords;
    eState m_state;
    
protected:
    void OnToken(CxxLexerToken& token);

public:
    JavaScriptFunctionsLocator(CxxPreProcessor* preProcessor, const wxFileName& filename);
    virtual ~JavaScriptFunctionsLocator();

    wxString GetFunctionsString() const;
    wxString GetPropertiesString() const;
};

#endif // JAVASCRIPTFUNCTIONSLOCATOR_H
