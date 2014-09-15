#ifndef CXXPREPROCESSORSCANNERBASE_H
#define CXXPREPROCESSORSCANNERBASE_H

#include "codelite_exports.h"
#include "CxxLexerAPI.h"
#include <wx/filename.h>
#include "CxxScannerBase.h"

class WXDLLIMPEXP_CL CxxPreProcessor;
class WXDLLIMPEXP_CL CIncludeStatementCollector : public CxxScannerBase
{
public:
    CIncludeStatementCollector(CxxPreProcessor* pp, const wxFileName& filename);
    virtual ~CIncludeStatementCollector();
    void OnToken(CxxLexerToken &token);
};

#endif // CXXPREPROCESSORSCANNERBASE_H
