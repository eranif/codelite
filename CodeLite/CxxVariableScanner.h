#ifndef CXXVARIABLESCANNER_H
#define CXXVARIABLESCANNER_H

#include "CxxLexerAPI.h"
#include "CxxVariable.h"
#include "codelite_exports.h"
#include "macros.h"
#include "wxStringHash.h"
#include <stack>

class WXDLLIMPEXP_CL CxxVariableScanner
{

protected:
    Scanner_t m_scanner;
    wxString m_buffer;
    bool m_eof;
    int m_parenthesisDepth;
    std::unordered_set<int> m_nativeTypes;
    eCxxStandard m_standard;
    wxStringTable_t m_macros;
    std::vector<wxString> m_buffers;
    bool m_isFuncSignature;

protected:
    bool GetNextToken(CxxLexerToken& token);
    void UngetToken(const CxxLexerToken& token);
    bool IsEof() const { return m_eof; }
    bool TypeHasIdentifier(const CxxVariable::LexerToken::Vec_t& type);
    bool HasNativeTypeInList(const CxxVariable::LexerToken::Vec_t& type) const;

    wxString& Buffer();
    wxString& PushBuffer();
    wxString& PopBuffer();

    bool OnForLoop(Scanner_t scanner);
    bool OnCatch(Scanner_t scanner);
    bool OnWhile(Scanner_t scanner);
    bool OnDeclType(Scanner_t scanner);
    bool OnLambda(Scanner_t scanner);

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

    int ReadUntil(const std::unordered_set<int>& delims, CxxLexerToken& token, wxString& consumed);

    CxxVariable::Vec_t DoGetVariables(const wxString& buffer, bool sort);
    CxxVariable::Vec_t DoParseFunctionArguments(const wxString& buffer);

public:
    CxxVariableScanner(const wxString& buffer, eCxxStandard standard, const wxStringTable_t& macros,
                       bool isFuncSignature);
    virtual ~CxxVariableScanner();

    /**
     * @brief strip buffer from unreachable code blocks (assuming the caret is at the last position of the bufer)
     */
    void OptimizeBuffer(const wxString& buffer, wxString& strippedBuffer);

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
