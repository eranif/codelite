#ifndef CXXPREPROCESSORSCANNERBASE_H
#define CXXPREPROCESSORSCANNERBASE_H

#include "codelite_exports.h"
#include "CxxLexerAPI.h"
#include <wx/filename.h>

class WXDLLIMPEXP_CL CxxPreProcessor;
class WXDLLIMPEXP_CL CxxPreProcessorScannerBase
{
protected:
    Scanner_t m_scanner;
    wxFileName m_filename;
    size_t m_options;

public:
    CxxPreProcessorScannerBase(const wxFileName& filename, size_t options);
    virtual ~CxxPreProcessorScannerBase();
    
    /**
     * @brief run the main lexing function
     */
    void Parse(CxxPreProcessor* pp);
};

#endif // CXXPREPROCESSORSCANNERBASE_H
