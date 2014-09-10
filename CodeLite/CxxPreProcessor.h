#ifndef CXXPREPROCESSOR_H
#define CXXPREPROCESSOR_H

#include "CxxLexerAPI.h"
#include <wx/filename.h>
#include "CxxPreProcessorScanner.h"
#include <set>
#include "codelite_exports.h"

class WXDLLIMPEXP_CL CxxPreProcessor
{
    CxxPreProcessorToken::Map_t m_tokens;
    wxArrayString m_includePaths;
    std::set<wxString> m_noSuchFiles;
    std::map<wxString, wxString> m_fileMapping;
    size_t m_options;
    
public:
    CxxPreProcessor();
    virtual ~CxxPreProcessor();
    
    /**
     * @brief return a command that generates a single file with all defines in it
     */
    wxString GetGxxCommand(const wxString& gxx, const wxString &filename) const;
    CxxPreProcessorToken::Map_t& GetTokens()
    {
        return m_tokens;
    }
    
    const CxxPreProcessorToken::Map_t& GetTokens() const
    {
        return m_tokens;
    }
    void SetTokens(const CxxPreProcessorToken::Map_t& tokens) { m_tokens = tokens; }
    
    /**
     * @brief add search path to the PreProcessor
     */
    void AddIncludePath(const wxString &path);
    
    /**
     * @brief add definition in the form of A[=B] (=B is optional)
     */
    void AddDefinition(const wxString &def);
    /**
     * @brief resolve an include statement and return a scanner for it
     * @param currentFile the current file being processed. ( we use it's path to resolve relative include
     * statements)
     * @param includeStatement the include statement found by the scanner
     */
    CxxPreProcessorScanner* CreateScanner(const wxFileName& currentFile, const wxString& includeStatement);
    /**
     * @brief the main entry function
     * @param filename
     */
    void Parse(const wxFileName& filename, size_t options);
    
    /**
     * @brief return the definitions collected as an array
     * @return 
     */
    wxArrayString GetDefinitions() const;
};

#endif // CXXPREPROCESSOR_H
