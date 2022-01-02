#include <wx/string.h>
const wxString big_file = R"(
#include "CxxScannerTokens.h"
#include "CxxVariableScanner.h"
#include "file_logger.h"

#include <algorithm>
#include <unordered_set>

CxxVariableScanner::CxxVariableScanner(const wxString& buffer, eCxxStandard standard, const wxStringTable_t& macros,
                                       bool isFuncSignature)
    : m_scanner(NULL)
    , m_buffer(buffer)
    , m_eof(false)
    , m_parenthesisDepth(0)
    , m_standard(standard)
    , m_macros(macros)
    , m_isFuncSignature(isFuncSignature)
{
    if(!m_buffer.IsEmpty() && m_buffer[0] == '(') {
        m_buffer.Remove(0, 1);
    }
    m_nativeTypes.insert(T_AUTO);
    m_nativeTypes.insert(T_BOOL);
    m_nativeTypes.insert(T_CHAR);
    m_nativeTypes.insert(T_CHAR16_T);
    m_nativeTypes.insert(T_CHAR32_T);
    m_nativeTypes.insert(T_DOUBLE);
    m_nativeTypes.insert(T_FLOAT);
    m_nativeTypes.insert(T_INT);
    m_nativeTypes.insert(T_LONG);
    m_nativeTypes.insert(T_SHORT);
    m_nativeTypes.insert(T_SIGNED);
    m_nativeTypes.insert(T_UNSIGNED);
    m_nativeTypes.insert(T_VOID);
    m_nativeTypes.insert(T_WCHAR_T);
}

CxxVariableScanner::~CxxVariableScanner() {}

CxxVariable::Vec_t CxxVariableScanner::GetVariables(bool sort)
{
    wxString strippedBuffer;
    OptimizeBuffer(m_buffer, strippedBuffer);
    CxxVariable::Vec_t vars = DoGetVariables(strippedBuffer, sort);
    if(sort) {
        std::sort(vars.begin(), vars.end(),
                  [&](CxxVariable::Ptr_t a, CxxVariable::Ptr_t b) { return a->GetName() < b->GetName(); });
    }
    return vars;
}

bool CxxVariableScanner::ReadType(CxxVariable::LexerToken::Vec_t& vartype, bool& isAuto)
{
    isAuto = false;
    int depth = 0;
    CxxLexerToken token;
    while(GetNextToken(token)) {
        if(depth == 0) {
            if(vartype.empty()) {
                // a type can start the following tokens
                switch(token.GetType()) {
                case T_AUTO:
                    isAuto = true;
                // fall
                case T_CLASS:
                case T_STRUCT:
                case T_IDENTIFIER:
                case T_DOUBLE_COLONS:
                case T_BOOL:
                case T_CHAR:
                case T_CHAR16_T:
                case T_CHAR32_T:
                case T_CONST:
                case T_CONSTEXPR:
                case T_DOUBLE:
                case T_FLOAT:
                case T_INT:
                case T_LONG:
                case T_MUTABLE:
                case T_REGISTER:
                case T_SHORT:
                case T_SIGNED:
                case T_STATIC:
                case T_UNSIGNED:
                case T_VOLATILE:
                case T_VOID:
                case T_USING:
                case T_WCHAR_T: {
                    vartype.push_back(CxxVariable::LexerToken(token, depth));
                    break;
                }
                default:
                    // Not a type definition
                    return false;
                }
            } else {
                const CxxVariable::LexerToken& lastToken = vartype.back();
                switch(token.GetType()) {
                case T_IDENTIFIER: {
                    if(TypeHasIdentifier(vartype) && (vartype.back().type != T_DOUBLE_COLONS)) {
                        // We already found the identifier for this type, its probably part of the name
                        UngetToken(token);
                        return true;
                    } else if(HasNativeTypeInList(vartype) && (vartype.back().type != T_DOUBLE_COLONS)) {
                        UngetToken(token);
                        return true;
                    }
                    // Found an identifier
                    // We consider this part of the type only if the previous token was "::" or
                    // if it was T_CONST
                    switch(lastToken.type) {
                    case T_CLASS:
                    case T_STRUCT:
                    case T_DOUBLE_COLONS:
                    case T_CONST:
                    case T_CONSTEXPR:
                    case T_REGISTER:
                    case T_MUTABLE:
                    case T_VOLATILE:
                    case T_STATIC:
                        vartype.push_back(CxxVariable::LexerToken(token, depth));
                        break;
                    default:
                        UngetToken(token);
                        return true;
                    }
                    break;
                case T_AUTO:
                    isAuto = true;
                // fall
                case T_DOUBLE_COLONS:
                case T_BOOL:
                case T_CHAR:
                case T_CHAR16_T:
                case T_CHAR32_T:
                case T_CONST:
                case T_CONSTEXPR:
                case T_DOUBLE:
                case T_FLOAT:
                case T_INT:
                case T_LONG:
                case T_SHORT:
                case T_SIGNED:
                case T_UNSIGNED:
                case T_VOID:
                case T_WCHAR_T: {
                    vartype.push_back(CxxVariable::LexerToken(token, depth));
                    break;
                }
                }
                case '<':
                case '[':
                    vartype.push_back(CxxVariable::LexerToken(token, depth));
                    depth++;
                    break;
                case '*':
                case '&':
                case '@':       // AngelScript
                case T_AND_AND: // C++11 reference rvalue
                    // Part of the name
                    UngetToken(token);
                    return true;
                default:
                    return false;
                }
            }
        } else {
            // Depth > 0
            vartype.push_back(CxxVariable::LexerToken(token, depth));
            if(token.GetType() == '>' || token.GetType() == ']') {
                --depth;
            } else if(token.GetType() == '<' || token.GetType() == '[') {
                ++depth;
            }
        }
    }
    return false;
}

thread_local std::unordered_set<int> s_validLocalTerminators;
bool CxxVariableScanner::ReadName(wxString& varname, wxString& pointerOrRef, wxString& varInitialization)
{
    CxxLexerToken token;
    while(GetNextToken(token)) {
        if(token.GetType() == '@') {
            // AngelScript. @ is similar to * in C/C++
            // @see https://github.com/eranif/codelite/issues/1839
            if(!GetNextToken(token) || token.GetType() != T_IDENTIFIER) {
                varname.Clear();
                return false;
            }
            varname = token.GetWXString();
            varInitialization.Clear();
            pointerOrRef = "@";
            return true;
        } else if(token.GetType() == T_IDENTIFIER) {
            varname = token.GetWXString();

            // When parsing function signature, we don't have multiple arguments
            // tied to the same TYPE
            if(m_isFuncSignature) {
                return false;
            }

            // Peek at the next token
            // We can expect "=", "," "(", ";" or
            // Examples:
            // TYPE name = 1;
            // TYPE name, secondVariable;
            // TYPE name(10);
            // TYPE name;
            if(s_validLocalTerminators.empty()) {
                s_validLocalTerminators.insert((int)',');
                s_validLocalTerminators.insert((int)'=');
                s_validLocalTerminators.insert((int)';');
                s_validLocalTerminators.insert((int)')');
                s_validLocalTerminators.insert((int)'(');
                s_validLocalTerminators.insert((int)'{'); // C++11 initialization, e.g: vector<int> v {1,2,3};
                s_validLocalTerminators.insert((int)'['); // Array
            }

            // Now that we got the name, check if have more variables to expect
            if(!GetNextToken(token)) {
                // We reached EOF, but we do got the variable name
                // se we return false ("don't continue") but we dont clear the name
                return false;
            }

            // Always return the token
            UngetToken(token);

            if(s_validLocalTerminators.count(token.GetType()) == 0) {
                varname.Clear();
                return false;
            }

            ConsumeInitialization(varInitialization);

            // Now that we got the name, check if have more variables to expect
            if(!GetNextToken(token)) {
                return false;
            }

            if((token.GetType() == '{') && !varInitialization.IsEmpty()) {
                // Don't collect functions and consider them as variables
                UngetToken(token);
                varname.clear();
                return false;
            }

            if(!varInitialization.empty()) {
                varInitialization.RemoveLast();
            }

            // If we found comma, return true. Unless we are parsing a function signature
            if(!m_isFuncSignature && (token.GetType() == ',')) {
                return true;
            } else {
                UngetToken(token);
                return false;
            }
        } else if(token.GetType() == '*' || token.GetType() == '&' || token.GetType() == T_AND_AND) {
            pointerOrRef << token.GetWXString();
        } else {
            return false;
        }
    }
    return false;
}

void CxxVariableScanner::ConsumeInitialization(wxString& consumed)
{
    CxxLexerToken token;
    wxString dummy;
    if(!GetNextToken(token))
        return;
    int type = wxNOT_FOUND;
    int tokType = token.GetType();
    if(tokType == '(') {
        // Read the initialization
        std::unordered_set<int> delims;
        delims.insert(')');
        if(ReadUntil(delims, token, consumed) == wxNOT_FOUND) {
            return;
        }
        consumed.Prepend("(");
        // Now read until the delimiter
        delims.clear();
        delims.insert(';');
        delims.insert(',');
        delims.insert('{');
        type = ReadUntil(delims, token, dummy);

    } else if(tokType == '[') {
        // Array
        std::unordered_set<int> delims;
        delims.insert(']');
        if(ReadUntil(delims, token, consumed) == wxNOT_FOUND) {
            return;
        }
        consumed.Prepend("[");
        // Now read until the delimiter
        delims.clear();
        delims.insert(';');
        delims.insert(',');
        type = ReadUntil(delims, token, dummy);

    } else if(tokType == '{') {
        // Read the initialization
        std::unordered_set<int> delims;
        delims.insert('}');
        if(ReadUntil(delims, token, consumed) == wxNOT_FOUND) {
            return;
        }
        consumed.Prepend("{");
        // Now read until the delimiter
        delims.clear();
        delims.insert(';');
        delims.insert(',');
        type = ReadUntil(delims, token, dummy);

    } else if(tokType == '=') {
        std::unordered_set<int> delims;
        delims.insert(';');
        delims.insert(',');
        type = ReadUntil(delims, token, consumed);
    } else {
        UngetToken(token);
        consumed.clear();
        std::unordered_set<int> delims;
        delims.insert(';');
        delims.insert(',');
        delims.insert('{');
        type = ReadUntil(delims, token, dummy);
    }

    if(type == ',' || type == (int)'{' || type == ';') {
        UngetToken(token);
    }
}

int CxxVariableScanner::ReadUntil(const std::unordered_set<int>& delims, CxxLexerToken& token, wxString& consumed)
{
    // loop until we find the open brace
    CxxVariable::LexerToken::Vec_t v;
    int depth = 0;
    while(GetNextToken(token)) {
        v.push_back(CxxVariable::LexerToken(token, depth));
        if(depth == 0) {
            if(delims.count(token.GetType())) {
                consumed = CxxVariable::PackType(v, m_standard);
                return token.GetType();
            } else {
                switch(token.GetType()) {
                case '<':
                case '{':
                case '[':
                case '(':
                    depth++;
                    break;
                default:
                    // ignore it
                    break;
                }
            }
        } else {
            switch(token.GetType()) {
            case '>':
            case '}':
            case ']':
            case ')':
                depth--;
                break;
            default:
                // ignore it
                break;
            }
        }
    }
    return wxNOT_FOUND;
}

bool CxxVariableScanner::GetNextToken(CxxLexerToken& token)
{
    bool res = false;

    while(true) {
        res = ::LexerNext(m_scanner, token);
        if(!res)
            break;

        // Ignore any T_IDENTIFIER which is declared as macro
        if((token.GetType() == T_IDENTIFIER) && m_macros.count(token.GetWXString())) {
            continue;
        }
        break;
    }

    m_eof = !res;
    switch(token.GetType()) {
    case '(':
        ++m_parenthesisDepth;
        break;
    case ')':
        --m_parenthesisDepth;
        break;
    default:
        break;
    }
    return res;
}

void CxxVariableScanner::OptimizeBuffer(const wxString& buffer, wxString& stripped_buffer)
{
    stripped_buffer.Clear();
    Scanner_t sc = ::LexerNew(buffer);
    if(!sc) {
        clWARNING() << "CxxVariableScanner::OptimizeBuffer(): failed to create Scanner_t" << clEndl;
        return; // Failed to allocate scanner
    }
)";

const wxString sample_cxx_file = R"(
class MyClass {
    std::string m_name;

public:
    std::string GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
};

int main(int argc, char** argv)
{
    std::string name = "hello";
    MyClass cls, cls2;
    cls.SetName(name);
    return 0;
}
)";

const wxString sample_text_language_h = R"(class TemplateHelper
{
    std::vector<wxArrayString> templateInstantiationVector;
};
)";

const wxString sample_text = R"(
#ifdef __WXMSW__
#endif
m_string.)";

wxString file_content =
    R"(
using namespace std;
void foo() {
    vector<string> V;
    V.empty();
    // other content goes here
    // bla bla
)";

const wxString tokenizer_sample_file_0 = R"(#include <unistd.h>
/** comment **/
int main(int argc, char** argv)
{
    /**
        multi line comment

    **/
    std::string name = "hello\nworld";
    MyClass cls, 1cls2;
    cls.SetName(name);
    return 0;
}
//last line comment
//)";

const wxString tokenizer_sample_file_1 = R"(#include <unistd.h>
/** comment **/
int main(int argc, char** argv)
{
    /**
        multi line comment

    **/
    std::string name = "hello\nworld";
    MyClass cls, 1cls2;
    cls.SetName(name);
    return 0;
}
)";

const wxString tokenizer_sample_file_2 = R"(#include "TextView.h"
#include "wxTerminalColourHandler.h"
#include "wxTerminalCtrl.h"

#ifdef __WXMSW__
#include "wx/msw/wrapwin.h" // includes windows.h
#endif
#include <fileutils.h>
#include <wx/tokenzr.h>
#include <wx/wupdlock.h>

wxTerminalColourHandler::wxTerminalColourHandler()
{
    // we use the Ubuntu colour scheme
    // Text colours
    m_colours.insert({ 30, wxColour(1, 1, 1) });
)";

// --------------------------- CxxCodeCompletion text ----------------------------------------

const wxString cc_text_simple = R"(wxString str;)";
const wxString cc_text_lsp_event = R"(LSPEvent& event;)";
const wxString cc_text_auto_simple = R"(auto str = wxString::AfterFirst();)";
const wxString cc_text_auto_chained = R"(
auto arr = wxStringTokenize();
auto str = arr.Item(0);
)";
const wxString cc_text_function_args_simple = R"(
void Foo::Bar(const wxString& str, shared_ptr<JSON>&& json) {
)";
const wxString cc_lamda_text = R"(
void Foo::Bar(const wxString& str, shared_ptr<JSON>&& json) {
        auto callback = [&](const wxString& variable, const std::vector<std::string>& V) {
            CxxPreProcessor pp;
)";
