#ifndef CXXSCANNERBASE_H
#define CXXSCANNERBASE_H

#include <wx/filename.h>
#include "CxxLexerAPI.h"

class CxxPreProcessor;
class WXDLLIMPEXP_CL CxxScannerBase
{
protected:
    Scanner_t m_scanner;
    wxFileName m_filename;
    size_t m_options;
    CxxPreProcessor* m_preProcessor;

public:
    CxxScannerBase(CxxPreProcessor* preProcessor, const wxFileName& filename);
    virtual ~CxxScannerBase();
    
    /**
     * @brief main loop
     */
    void Parse();
    
    /**
     * @brief
     * @param token
     */
    virtual void OnToken(CxxLexerToken& token) { wxUnusedVar(token); }
};

#endif // CXXSCANNERBASE_H
