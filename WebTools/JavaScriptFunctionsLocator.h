#ifndef JAVASCRIPTFUNCTIONSLOCATOR_H
#define JAVASCRIPTFUNCTIONSLOCATOR_H

#include <wx/string.h>
#include "macros.h"
#include "JSLexerAPI.h"

class JavaScriptFunctionsLocator
{
    enum eState { kNormal, kScopeOperator };
    wxString m_lastIdentifier;
    wxStringSet_t m_functions;
    wxStringSet_t m_properties;
    wxStringSet_t m_keywords;
    eState m_state;
    JSScanner_t m_scanner;

protected:
    void OnToken(JSLexerToken& token);

public:
    JavaScriptFunctionsLocator(const wxFileName& filename, const wxString& content = "");
    virtual ~JavaScriptFunctionsLocator();

    wxString GetFunctionsString() const;
    wxString GetPropertiesString() const;

    void Parse();
};

#endif // JAVASCRIPTFUNCTIONSLOCATOR_H
