#ifndef CXXVARIABLESCANNER_H
#define CXXVARIABLESCANNER_H

#include "CxxLexerAPI.h"
#include "CxxVariable.h"
#include "codelite_exports.h"
#include "macros.h"

#include <unordered_set>
#include <vector>
#include <wx/string.h>

class WXDLLIMPEXP_CL CxxVariableScanner
{
public:
    CxxVariableScanner(const wxString& buffer,
                       eCxxStandard standard,
                       const wxStringTable_t& macros,
                       bool isFuncSignature);
    ~CxxVariableScanner() = default;

    /**
     * @brief parse the buffer and return the variables
     */
    CxxVariable::Vec_t GetVariables(bool sort = true);

    /**
     * @brief parse the buffer and return a unique set of variables
     */
    CxxVariable::Map_t GetVariablesMap();

    /**
     * @brief parse the buffer and return list of variables
     */
    CxxVariable::Vec_t ParseFunctionArguments();

private:
    bool GetNextToken(CxxLexerToken& token);
    void UngetToken(const CxxLexerToken& token);
    bool IsEof() const { return m_eof; }
    bool TypeHasIdentifier(const CxxVariable::LexerToken::Vec_t& type);
    bool HasNativeTypeInList(const CxxVariable::LexerToken::Vec_t& type) const;

    wxString& Buffer();
    wxString& PushBuffer();
    wxString& PopBuffer();

    bool OnForLoop(Scanner_t scanner, wxString& variable_definition);
    bool OnCatch(Scanner_t scanner);
    bool OnWhile(Scanner_t scanner);
    bool OnDeclType(Scanner_t scanner);
    bool SkipToClosingParenthesis(Scanner_t scanner);

    /**
     * @brief read the variable type
     */
    bool ReadType(CxxVariable::LexerToken::Vec_t& vartype, bool& isAuto);
    /**
     * @brief read the variable name. Return true if there are more variables
     * for the current type
     */
    bool ReadName(wxString& varname, wxString& pointerOrRef, int& line_number, wxString& varInitialization);

    /**
     * @brief consume variable initialization
     */
    void ConsumeInitialization(wxString& consumed);

    int ReadUntil(const std::unordered_set<int>& delims, CxxLexerToken& token, wxString& consumed);

    CxxVariable::Vec_t DoGetVariables(const wxString& buffer, bool sort);
    CxxVariable::Vec_t DoParseFunctionArguments(const wxString& buffer);

    void DoOptimizeBuffer();

    /**
     * @brief move the scanner until we find the closing parenthesis `)`
     * @param scanner
     * @return true if found, false, when reached EOF
     */
    bool skip_parenthesis_block(Scanner_t scanner);

private:
    Scanner_t m_scanner = nullptr;
    wxString m_buffer;
    bool m_eof = false;
    int m_parenthesisDepth = 0;
    eCxxStandard m_standard = eCxxStandard::kCxx11;
    wxStringTable_t m_macros;
    std::vector<wxString> m_buffers;
    bool m_isFuncSignature = false;
    wxString m_optimized_buffer;
    bool m_buffer_optimized = false;
};

#endif // CXXVARIABLESCANNER_H
