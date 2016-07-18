#ifndef CXXVARIABLESCANNER_H
#define CXXVARIABLESCANNER_H

#include "codelite_exports.h"
#include "CxxLexerAPI.h"
#include "CxxVariable.h"

class WXDLLIMPEXP_CL CxxVariableScanner
{
    Scanner_t m_scanner;

protected:
    /**
     * @brief read the variable type
     */
    bool ReadType(CxxLexerToken::List_t& vartype);
    /**
     * @brief read the variable name. Return true if there are more variables
     * for the current type
     */
    bool ReadName(wxString& varname, wxString& pointerOrRef);

    /**
     * @brief consume variable initialization
     */
    void ConsumeInitialization();
    
    int ReadUntil(int delim1, int delim2, CxxLexerToken& token);
    
public:
    CxxVariableScanner(const wxString& buffer);
    virtual ~CxxVariableScanner();

    CxxVariable::List_t GetVariables();
};

#endif // CXXVARIABLESCANNER_H
