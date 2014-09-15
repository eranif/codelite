#ifndef CXXUSINGNAMESPACECOLLECTOR_H
#define CXXUSINGNAMESPACECOLLECTOR_H

#include "codelite_exports.h"
#include "CxxScannerBase.h"

class WXDLLIMPEXP_CL CxxUsingNamespaceCollector : public CxxScannerBase
{
    wxArrayString m_usingNamespaces;

private:
    void ParseUsingNamespace();

public:
    virtual void OnToken(CxxLexerToken& token);

    void SetUsingNamespaces(const wxArrayString& usingNamespaces) { this->m_usingNamespaces = usingNamespaces; }
    const wxArrayString& GetUsingNamespaces() const { return m_usingNamespaces; }
    CxxUsingNamespaceCollector(CxxPreProcessor* preProcessor, const wxFileName& filename);
    virtual ~CxxUsingNamespaceCollector();
};

#endif // CXXUSINGNAMESPACECOLLECTOR_H
