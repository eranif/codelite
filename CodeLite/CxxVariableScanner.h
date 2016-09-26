#ifndef CXXVARIABLESCANNER_H
#define CXXVARIABLESCANNER_H

#include "codelite_exports.h"
#include "CxxLexerAPI.h"
#include "CxxVariable.h"
#include <set>
#include "macros.h"

class WXDLLIMPEXP_CL CxxVariableScanner
{
    Scanner_t m_scanner;
    wxString m_buffer;
    bool m_eof;
    int m_parenthesisDepth;
    std::set<int> m_nativeTypes;
    
    enum eState { kNormal, kInParen, kInForLoop, kInCatch, kPreProcessor };

protected:
    bool GetNextToken(CxxLexerToken& token);
    bool IsEof() const { return m_eof; }
    void OptimizeBuffer(wxString& strippedBuffer, wxString& parenthesisBuffer);
    bool TypeHasIdentifier(const CxxVariable::LexerToken::List_t& type);
    bool HasTypeInList(const CxxVariable::LexerToken::List_t& type) const;
    
protected:
    /**
     * @brief read the variable type
     */
    bool ReadType(CxxVariable::LexerToken::List_t& vartype);
    /**
     * @brief read the variable name. Return true if there are more variables
     * for the current type
     */
    bool ReadName(wxString& varname, wxString& pointerOrRef, wxString& varInitialization);

    /**
     * @brief consume variable initialization
     */
    void ConsumeInitialization(wxString& consumed);

    int ReadUntil(const std::set<int>& delims, CxxLexerToken& token, wxString& consumed);

    CxxVariable::List_t DoGetVariables(const wxString& buffer);

public:
    CxxVariableScanner(const wxString& buffer);
    virtual ~CxxVariableScanner();
    
    /**
     * @brief parse the buffer and return list of variables
     * @return 
     */
    CxxVariable::List_t GetVariables();
    
    /**
     * @brief parse the buffer and return a unique set of variables
     */
    CxxVariable::Map_t GetVariablesMap();
};

#endif // CXXVARIABLESCANNER_H
