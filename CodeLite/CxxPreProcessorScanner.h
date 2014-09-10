#ifndef CXXPREPROCESSORSCANNER_H
#define CXXPREPROCESSORSCANNER_H

#include "CxxLexerAPI.h"
#include <wx/string.h>
#include <list>
#include <wx/sharedptr.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_CL CxxPreProcessor;
class WXDLLIMPEXP_CL CxxPreProcessorScanner
{
    Scanner_t m_scanner;
    wxFileName m_filename;
    size_t m_options;
    
public:
    typedef wxSharedPtr<CxxPreProcessorScanner> Ptr_t;
    
private:
    /**
     * @brief run the scanner until we reach the closing #endif
     * directive
     * Note that this function consumes the 'endif' directive
     */
    bool ConsumeBlock();
    /**
     * @brief run the scanner until it reaches the next pre processor branch
     * or until it finds the closing #endif directive
     * Note that this function also consumes the directive 'endif'
     * but will _not_ consume the elif/else directives
     */
    bool ConsumeCurrentBranch();
    /**
     * @brief read the next token that matches 'type' 
     * If we reached the end of the 'PreProcessor' state and there is no match
     * throw an exception
     */
    void ReadUntilMatch(int type, CxxLexerToken& token) throw (CxxLexerException);
    
    void GetRestOfPPLine(wxString &rest, bool collectNumberOnly = false);
    bool CheckIfDefined(const CxxPreProcessorToken::Map_t& table);
    bool CheckIf(const CxxPreProcessorToken::Map_t& table);
    bool IsTokenExists(const CxxPreProcessorToken::Map_t& table, const CxxLexerToken& token);
    
public:
    CxxPreProcessorScanner(const wxFileName &file, size_t options);
    
    /**
     * @brief return true if we got a valid scanner
     */
    bool IsNull() const {
        return m_scanner == NULL;;
    }
    
    virtual ~CxxPreProcessorScanner();
    
    /**
     * @brief the main parsing function
     * @param ppTable
     */
    void Parse(CxxPreProcessor* pp)  throw (CxxLexerException);
};

#endif // CXXPREPROCESSORSCANNER_H
