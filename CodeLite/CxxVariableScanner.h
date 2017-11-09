#ifndef CXXVARIABLESCANNER_H
#define CXXVARIABLESCANNER_H

#include "codelite_exports.h"
#include "CxxLexerAPI.h"
#include "CxxVariable.h"
#include <unordered_set>
#include "macros.h"

class WXDLLIMPEXP_CL CxxVariableScanner
{
    enum eState { kNormal, kInParen, kInForLoop, kInCatch, kPreProcessor, kInDecltype, kInWhile };

protected:
    Scanner_t m_scanner;
    wxString m_buffer;
    bool m_eof;
    int m_parenthesisDepth;
    std::unordered_set<int> m_nativeTypes;
    eCxxStandard m_standard;
    wxStringTable_t m_macros;

protected:
    bool GetNextToken(CxxLexerToken& token);
    void UngetToken(const CxxLexerToken& token);
    bool IsEof() const { return m_eof; }
    void OptimizeBuffer(wxString& strippedBuffer, wxString& parenthesisBuffer);
    bool TypeHasIdentifier(const CxxVariable::LexerToken::Vec_t& type);
    bool HasNativeTypeInList(const CxxVariable::LexerToken::Vec_t& type) const;

protected:
    /**
     * @brief read the variable type
     */
    bool ReadType(CxxVariable::LexerToken::Vec_t& vartype, bool& isAuto);
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

    CxxVariable::Vec_t DoGetVariables(const wxString& buffer, bool sort);
    CxxVariable::Vec_t DoParseFunctionArguments(const wxString& buffer);

public:
    CxxVariableScanner(const wxString& buffer, eCxxStandard standard, const wxStringTable_t& macros);
    virtual ~CxxVariableScanner();

    /**
     * @brief parse the buffer and return list of variables
     * @return
     */
    CxxVariable::Vec_t GetVariables(bool sort = true);

    /**
     * @brief parse the buffer and return list of variables
     * @return
     */
    CxxVariable::Vec_t ParseFunctionArguments();

    /**
     * @brief parse the buffer and return a unique set of variables
     */
    CxxVariable::Map_t GetVariablesMap();
};

#endif // CXXVARIABLESCANNER_H
