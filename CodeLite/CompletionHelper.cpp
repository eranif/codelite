#include "CompletionHelper.hpp"

#include "CxxScannerTokens.h"
#include "CxxTokenizer.h"
#include "file_logger.h"

#include <vector>
#include <wx/regex.h>

using namespace std;
namespace
{
#define PREPEND_STRING(tokn) expression.insert(expression.begin(), text)
#define MAX_TIP_LINE_SIZE 200

bool is_word_char(wxChar ch)
{
    return (ch >= 65 && ch <= 90)     // uppercase A-Z
           || (ch >= 97 && ch <= 122) // lowercase a-z
           || (ch >= 48 && ch <= 57)  // 0-9
           || (ch == '_');
}

wxString wrap_lines(const wxString& str)
{
    wxString wrappedString;

    int curLineBytes(0);
    wxString::const_iterator iter = str.begin();
    for(; iter != str.end(); iter++) {
        if(*iter == wxT('\t')) {
            wrappedString << wxT(" ");

        } else if(*iter == wxT('\n')) {
            wrappedString << wxT("\n");
            curLineBytes = 0;

        } else if(*iter == wxT('\r')) {
            // Skip it

        } else {
            wrappedString << *iter;
        }
        curLineBytes++;

        if(curLineBytes == MAX_TIP_LINE_SIZE) {

            // Wrap the lines
            if(wrappedString.IsEmpty() == false && wrappedString.Last() != wxT('\n')) {
                wrappedString << wxT("\n");
            }
            curLineBytes = 0;
        }
    }
    return wrappedString;
}

} // namespace

CompletionHelper::CompletionHelper() {}

CompletionHelper::~CompletionHelper() {}

wxString CompletionHelper::get_expression(const wxString& file_content, bool for_calltip, wxString* last_word) const
{
#define LAST_TOKEN_IS(token_type) (!types.empty() && (types[types.size() - 1] == token_type))
    // tokenize the text
    CxxTokenizer tokenizer;
    tokenizer.Reset(file_content);

    CxxLexerToken token;
    vector<pair<wxString, int>> tokens;
    tokens.reserve(10000);

    while(tokenizer.NextToken(token)) {
        tokens.push_back({ token.GetWXString(), token.GetType() });
    }

    int i = static_cast<int>(tokens.size() - 1);
    bool cont = true;
    int parentheses_depth = 0;
    if(for_calltip) {
        // read backwards until we find the first open parentheses
        for(; (i >= 0) && cont; --i) {
            switch(tokens[i].second) {
            case '(':
                if(parentheses_depth == 0) {
                    cont = false;
                } else {
                    parentheses_depth--;
                }
                break;
            case ')':
                parentheses_depth++;
                break;
            default:
                break;
            }
        }
    }

    vector<wxString> expression;
    std::vector<int> types;
    int depth = 0;
    cont = true;
    for(; (i >= 0) && cont; --i) {
        const wxString& text = tokens[i].first;
        int type = tokens[i].second;
        switch(type) {
        case '[':
        case '{':
        case '(':
        case '<':
            if(depth == 0) {
                cont = false;
            } else {
                PREPEND_STRING(t);
                depth--;
            }
            break;
        case '>':
        case ']':
        case ')':
            if(depth == 0 && LAST_TOKEN_IS(T_IDENTIFIER)) {
                cont = false;
            } else {
                PREPEND_STRING(t);
                depth++;
            }
            break;
        case '}':
            if(depth == 0) {
                cont = false;
            } else {
                PREPEND_STRING(t);
                depth++;
            }
            break;
        case ';':
        case T_PP_DEFINE:
        case T_PP_DEFINED:
        case T_PP_IF:
        case T_PP_IFNDEF:
        case T_PP_IFDEF:
        case T_PP_ELSE:
        case T_PP_ELIF:
        case T_PP_LINE:
        case T_PP_PRAGMA:
        case T_PP_UNDEF:
        case T_PP_ERROR:
        case T_PP_ENDIF:
        case T_PP_IDENTIFIER:
        case T_PP_DEC_NUMBER:
        case T_PP_OCTAL_NUMBER:
        case T_PP_HEX_NUMBER:
        case T_PP_FLOAT_NUMBER:
        case T_PP_STRING:
        case T_PP_AND:
        case T_PP_OR:
        case T_PP_STATE_EXIT:
        case T_PP_INCLUDE_FILENAME:
        case T_PP_INCLUDE:
        case T_PP_GT:
        case T_PP_GTEQ:
        case T_PP_LT:
        case T_PP_LTEQ:
            cont = false;
            break;
        case '=':
            if(depth == 0) {
                cont = false;
            } else {
                PREPEND_STRING(t);
            }
            break;
        case T_IDENTIFIER:
            if(LAST_TOKEN_IS(T_IDENTIFIER)) {
                // we do not allow two consecutive T_IDENTIFIER
                cont = false;
                break;
            } else {
                PREPEND_STRING(t);
            }
            break;
        case T_ALIGNAS:
        case T_ALIGNOF:
        case T_AND:
        case T_AND_EQ:
        case T_ASM:
        case T_AUTO:
        case T_BITAND:
        case T_BITOR:
        case T_BOOL:
        case T_BREAK:
        case T_CATCH:
        case T_CHAR:
        case T_CHAR16_T:
        case T_CHAR32_T:
        case T_CLASS:
        case T_COMPL:
        case T_CONST:
        case T_CONSTEXPR:
        case T_CONTINUE:
        case T_DECLTYPE:
        case T_DEFAULT:
        case T_DELETE:
        case T_DO:
        case T_DOUBLE:
        case T_ELSE:
        case T_ENUM:
        case T_EXPLICIT:
        case T_EXPORT:
        case T_EXTERN:
        case T_FALSE:
        case T_FINAL:
        case T_FLOAT:
        case T_FOR:
        case T_FRIEND:
        case T_GOTO:
        case T_IF:
        case T_INLINE:
        case T_INT:
        case T_LONG:
        case T_MUTABLE:
        case T_NAMESPACE:
        case T_NEW:
        case T_NOEXCEPT:
        case T_NOT:
        case T_NOT_EQ:
        case T_NULLPTR:
        case T_OPERATOR:
        case T_OR:
        case T_OR_EQ:
        case T_OVERRIDE:
        case T_PRIVATE:
        case T_PROTECTED:
        case T_PUBLIC:
        case T_REGISTER:
        case T_CASE:
        case T_SHORT:
        case T_SIGNED:
        case T_SIZEOF:
        case T_STATIC:
        case T_STATIC_ASSERT:
        case T_STRUCT:
        case T_SWITCH:
        case T_TEMPLATE:
        case T_THREAD_LOCAL:
        case T_THROW:
        case T_TRUE:
        case T_TRY:
        case T_TYPEDEF:
        case T_TYPEID:
        case T_TYPENAME:
        case T_UNION:
        case T_UNSIGNED:
        case T_USING:
        case T_VIRTUAL:
        case T_VOID:
        case T_VOLATILE:
        case T_WCHAR_T:
        case T_WHILE:
        case T_XOR:
        case T_XOR_EQ:
        case T_STRING:
        case T_DOT_STAR:
        case T_ARROW_STAR:
        case T_PLUS_PLUS:
        case T_MINUS_MINUS:
        case T_LS:
        case T_LE:
        case T_GE:
        case T_EQUAL:
        case T_NOT_EQUAL:
        case T_AND_AND:
        case T_OR_OR:
        case T_STAR_EQUAL:
        case T_SLASH_EQUAL:
        case T_DIV_EQUAL:
        case T_PLUS_EQUAL:
        case T_MINUS_EQUAL:
        case T_LS_ASSIGN:
        case T_RS_ASSIGN:
        case T_AND_EQUAL:
        case T_POW_EQUAL:
        case T_OR_EQUAL:
        case T_3_DOTS:
        case T_RETURN:
        case ',':
        case '*':
        case '&':
        case '!':
        case '~':
        case '+':
        case '^':
        case '|':
        case '%':
        case '?':
        case '/':
        case ':':
        case '-':
        case '@':
            if(depth <= 0) {
                cont = false;
            } else {
                PREPEND_STRING(t);
            }
            break;
        case T_CONST_CAST:
        case T_DYNAMIC_CAST:
        case T_REINTERPRET_CAST:
        case T_STATIC_CAST:
        default:
            PREPEND_STRING(t);
            break;
        }
        types.push_back(type);
    }

    // build the expression from the vector
    wxString expression_string;
    for(const auto& t : expression) {
        expression_string << t;
    }
    if(last_word && !expression.empty()) {
        *last_word = expression[expression.size() - 1];
    }
    return expression_string;
#undef LAST_TOKEN_IS
}

wxString CompletionHelper::truncate_file_to_location(const wxString& file_content, size_t line, size_t column,
                                                     size_t flags) const
{
    size_t curline = 0;
    size_t offset = 0;

    // locate the line
    for(const auto& ch : file_content) {
        if(curline == line) {
            break;
        }
        switch(ch.GetValue()) {
        case '\n':
            ++curline;
            ++offset;
            break;
        default:
            ++offset;
            break;
        }
    }

    if(curline != line) {
        return wxEmptyString;
    }

    // columns
    offset += column;

    if(offset < file_content.size()) {
        if(flags & (TRUNCATE_COMPLETE_WORDS | TRUNCATE_COMPLETE_LINES)) {
            while(true) {
                size_t next_pos = offset;

                if(next_pos < file_content.size()) {
                    wxChar next_char = file_content[next_pos];
                    if(flags & TRUNCATE_COMPLETE_WORDS) {
                        // complete words only
                        if(is_word_char(next_char)) {
                            offset += 1;
                        } else {
                            break;
                        }

                    } else {
                        // TRUNCATE_COMPLETE_LINES
                        if(next_char == '\n') {
                            break;
                        } else {
                            offset += 1;
                        }
                    }
                } else {
                    break;
                }
            }
        }
        return file_content.Mid(0, offset);
    }
    return wxEmptyString;
}

thread_local std::unordered_set<wxString> words;
namespace
{
void populate_keywords()
{
    if(words.empty()) {
        words.insert("auto");
        words.insert("break");
        words.insert("case");
        words.insert("char");
        words.insert("const");
        words.insert("continue");
        words.insert("default");
        words.insert("define");
        words.insert("defined");
        words.insert("do");
        words.insert("double");
        words.insert("elif");
        words.insert("else");
        words.insert("endif");
        words.insert("enum");
        words.insert("error");
        words.insert("extern");
        words.insert("float");
        words.insert("for");
        words.insert("goto");
        words.insert("if");
        words.insert("ifdef");
        words.insert("ifndef");
        words.insert("include");
        words.insert("int");
        words.insert("line");
        words.insert("long");
        words.insert("bool");
        words.insert("pragma");
        words.insert("register");
        words.insert("return");
        words.insert("short");
        words.insert("signed");
        words.insert("sizeof");
        words.insert("static");
        words.insert("struct");
        words.insert("switch");
        words.insert("typedef");
        words.insert("undef");
        words.insert("union");
        words.insert("unsigned");
        words.insert("void");
        words.insert("volatile");
        words.insert("while");
        words.insert("class");
        words.insert("namespace");
        words.insert("delete");
        words.insert("friend");
        words.insert("inline");
        words.insert("new");
        words.insert("operator");
        words.insert("overload");
        words.insert("protected");
        words.insert("private");
        words.insert("public");
        words.insert("this");
        words.insert("virtual");
        words.insert("template");
        words.insert("typename");
        words.insert("dynamic_cast");
        words.insert("static_cast");
        words.insert("const_cast");
        words.insert("reinterpret_cast");
        words.insert("using");
        words.insert("throw");
        words.insert("catch");
        words.insert("nullptr");
        words.insert("noexcept");
        words.insert("override");
        words.insert("explicit");
        words.insert("constexpr");
        words.insert("thread_local");
        words.insert("true");
        words.insert("false");
    }
}
} // namespace
bool CompletionHelper::is_cxx_keyword(const wxString& word)
{
    populate_keywords();
    return words.count(word) != 0;
}

vector<wxString> CompletionHelper::split_function_signature(const wxString& signature, wxString* return_value,
                                                            size_t flags) const
{
    // ---------------------------------------------------------------------------------------------
    // ----------------macros start-------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------
#define ADD_CURRENT_PARAM(current_param) \
    current_param->Trim().Trim(false);   \
    if(!current_param->empty()) {        \
        args.push_back(*current_param);  \
    }                                    \
    current_param->clear();
#define LAST_TOKEN_IS(token_type) (!types.empty() && (types[types.size() - 1] == token_type))
#define LAST_TOKEN_IS_ONE_OF_2(t1, t2) (LAST_TOKEN_IS(t1) || LAST_TOKEN_IS(t2))
#define LAST_TOKEN_IS_ONE_OF_3(t1, t2, t3) (LAST_TOKEN_IS_ONE_OF_2(t1, t2) || LAST_TOKEN_IS(t3))
#define LAST_TOKEN_IS_ONE_OF_4(t1, t2, t3, t4) (LAST_TOKEN_IS_ONE_OF_3(t1, t2, t3) || LAST_TOKEN_IS(t4))
#define LAST_TOKEN_IS_ONE_OF_5(t1, t2, t3, t4, t5) (LAST_TOKEN_IS_ONE_OF_4(t1, t2, t3, t4) || LAST_TOKEN_IS(t5))
#define LAST_TOKEN_IS_CLOSING_PARENTHESES() LAST_TOKEN_IS_ONE_OF_4('}', ']', '>', ')')
#define LAST_TOKEN_IS_OPEN_PARENTHESES() LAST_TOKEN_IS_ONE_OF_4('{', '[', '<', '(')
#define REMOVE_TRAILING_SPACE()                                                         \
    if(!current_param->empty() && (*current_param)[current_param->size() - 1] == ' ') { \
        current_param->RemoveLast();                                                    \
    }
#define APPEND_SPACE_IF_MISSING()                                                         \
    if(!current_param->empty() && ((*current_param)[current_param->size() - 1] != ' ')) { \
        current_param->Append(" ");                                                       \
    }

    // ---------------------------------------------------------------------------------------------
    // ----------------macros end-------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------
    CxxTokenizer tokenizer;
    tokenizer.Reset(signature);

    CxxLexerToken token;

    wxString cur_func_param;
    wxString* current_param = &cur_func_param;
    vector<wxString> args;
    vector<wxString> func_args;
    int depth = 0;

    vector<int> types;
    // search for the first opening brace
    while(tokenizer.NextToken(token)) {
        if(token.GetType() == '(') {
            depth = 1;
            break;
        }
    }

    bool done_collecting_args = false;
    constexpr int STATE_NORMAL = 0;
    constexpr int STATE_DEFAULT_VALUE = 1;
    int state = STATE_NORMAL;
    while(tokenizer.NextToken(token)) {
        switch(state) {
        case STATE_DEFAULT_VALUE:
            // consume everything until we reach signature end
            // or until we hit a "," (where depth==1)
            switch(token.GetType()) {
            case '<':
            case '{':
            case '(':
            case '[':
                depth++;
                break;
            case '>':
            case '}':
            case ']':
                depth--;
                break;
            case ')':
                depth--;
                if(depth == 0) {
                    // end of argument reading, switch back to the normal state
                    tokenizer.UngetToken();
                    // restore the depth
                    depth++;
                    state = STATE_NORMAL;
                }
                break;
            case ',':
                if(depth == 1) {
                    tokenizer.UngetToken();
                    state = STATE_NORMAL;
                }
                break;
            default:
                break;
            }
            break;
        case STATE_NORMAL:
            switch(token.GetType()) {
            case T_ALIGNAS:
            case T_ALIGNOF:
            case T_AND:
            case T_AND_EQ:
            case T_ASM:
            case T_AUTO:
            case T_BITAND:
            case T_BITOR:
            case T_BOOL:
            case T_BREAK:
            case T_CATCH:
            case T_CHAR:
            case T_CHAR16_T:
            case T_CHAR32_T:
            case T_CLASS:
            case T_COMPL:
            case T_CONST:
            case T_CONSTEXPR:
            case T_CONST_CAST:
            case T_CONTINUE:
            case T_DECLTYPE:
            case T_DEFAULT:
            case T_DELETE:
            case T_DO:
            case T_DOUBLE:
            case T_DYNAMIC_CAST:
            case T_ELSE:
            case T_ENUM:
            case T_EXPLICIT:
            case T_EXPORT:
            case T_EXTERN:
            case T_FALSE:
            case T_FINAL:
            case T_FLOAT:
            case T_FOR:
            case T_FRIEND:
            case T_GOTO:
            case T_IF:
            case T_INLINE:
            case T_INT:
            case T_LONG:
            case T_MUTABLE:
            case T_NAMESPACE:
            case T_NEW:
            case T_NOEXCEPT:
            case T_NOT:
            case T_NOT_EQ:
            case T_NULLPTR:
            case T_OPERATOR:
            case T_OR:
            case T_OR_EQ:
            case T_OVERRIDE:
            case T_PRIVATE:
            case T_PROTECTED:
            case T_PUBLIC:
            case T_REGISTER:
            case T_REINTERPRET_CAST:
            case T_CASE:
            case T_SHORT:
            case T_SIGNED:
            case T_SIZEOF:
            case T_STATIC:
            case T_STATIC_ASSERT:
            case T_STATIC_CAST:
            case T_STRUCT:
            case T_SWITCH:
            case T_TEMPLATE:
            case T_THREAD_LOCAL:
            case T_THROW:
            case T_TRUE:
            case T_TRY:
            case T_TYPEDEF:
            case T_TYPEID:
            case T_TYPENAME:
            case T_UNION:
            case T_UNSIGNED:
            case T_USING:
            case T_VIRTUAL:
            case T_VOID:
            case T_VOLATILE:
            case T_WCHAR_T:
            case T_WHILE:
            case T_XOR:
            case T_XOR_EQ:
            case T_STRING:
            case T_DOT_STAR:
            case T_ARROW_STAR:
            case T_PLUS_PLUS:
            case T_MINUS_MINUS:
            case T_LS:
            case T_LE:
            case T_GE:
            case T_EQUAL:
            case T_NOT_EQUAL:
            case T_AND_AND:
            case T_OR_OR:
            case T_STAR_EQUAL:
            case T_SLASH_EQUAL:
            case T_DIV_EQUAL:
            case T_PLUS_EQUAL:
            case T_MINUS_EQUAL:
            case T_LS_ASSIGN:
            case T_RS_ASSIGN:
            case T_AND_EQUAL:
            case T_POW_EQUAL:
            case T_OR_EQUAL:
            case T_3_DOTS:
            case '&':
            case ':':
                current_param->Append(token.GetWXString());
                APPEND_SPACE_IF_MISSING();
                break;
            case '*':
                if(LAST_TOKEN_IS('*')) {
                    REMOVE_TRAILING_SPACE();
                }
                current_param->Append(token.GetWXString());
                APPEND_SPACE_IF_MISSING();
                break;
            case T_IDENTIFIER: {
                wxString peeked_token_text;
                bool add_identifier = true;
                int next_token_type = tokenizer.PeekToken(peeked_token_text);
                // Check if we want to ignore the argument name
                if((depth == 1) && (next_token_type == ',' || next_token_type == '=' || next_token_type == ')') &&
                   (flags & STRIP_NO_NAME)) {
                    // two consecutive T_IDENTIFIER, dont add it
                    add_identifier = false;
                } else if(LAST_TOKEN_IS_CLOSING_PARENTHESES() || LAST_TOKEN_IS_ONE_OF_2(T_IDENTIFIER, '*')) {
                    APPEND_SPACE_IF_MISSING();
                }

                if(add_identifier) {
                    current_param->Append(token.GetWXString());
                }
            } break;
            case T_ARROW:
                if(done_collecting_args) {
                    // we are collecting function return value now, disregard it
                } else {
                    current_param->Append(token.GetWXString());
                }
                break;
            case ',':
                if(depth == 1) {
                    ADD_CURRENT_PARAM(current_param);
                } else {
                    current_param->Append(",");
                    APPEND_SPACE_IF_MISSING();
                }
                break;
            case '>':
            case ']':
            case '}':
                depth--;
                REMOVE_TRAILING_SPACE();
                current_param->Append(token.GetWXString());
                break;
            case ')':
                depth--;
                if(!done_collecting_args && depth == 0) {
                    // reached signature end
                    ADD_CURRENT_PARAM(current_param);
                    func_args.swap(args);
                    done_collecting_args = true;
                } else {
                    if(LAST_TOKEN_IS_CLOSING_PARENTHESES() || LAST_TOKEN_IS(T_IDENTIFIER)) {
                        REMOVE_TRAILING_SPACE();
                    }
                    current_param->Append(token.GetWXString());
                }
                break;
            case '[':
            case '<':
            case '(':
            case '{':
                depth++;
                REMOVE_TRAILING_SPACE();
                current_param->Append(token.GetWXString());
                break;
            case '=':
                if((depth == 1) && (flags & STRIP_NO_DEFAULT_VALUES)) {
                    state = STATE_DEFAULT_VALUE;
                } else {
                    APPEND_SPACE_IF_MISSING();
                    current_param->Append("= ");
                }
                break;
            default:
                current_param->Append(token.GetWXString());
                break;
            }

            break;
        }
        types.push_back(token.GetType());
    }

    if(!done_collecting_args) {
        // we did not complete
        func_args.swap(args);
    } else {
        // check if we have a return value
        ADD_CURRENT_PARAM(current_param);
        if(!args.empty() && return_value) {
            *return_value = args[0].Trim().Trim(false);
        }
    }
    return func_args;
#undef ADD_CURRENT_PARAM
#undef LAST_TOKEN_IS
#undef LAST_TOKEN_IS_ONE_OF_2
#undef LAST_TOKEN_IS_ONE_OF_3
#undef LAST_TOKEN_IS_ONE_OF_4
#undef LAST_TOKEN_IS_ONE_OF_5
#undef LAST_TOKEN_IS_CLOSING_PARENTHESES
#undef LAST_TOKEN_IS_OPEN_PARENTHESES
#undef REMOVE_TRAILING_SPACE
}

namespace
{
thread_local wxRegEx reDoxyParam("([@\\\\]{1}param)[ \t]+([_a-z][a-z0-9_]*)?", wxRE_DEFAULT | wxRE_ICASE);
thread_local wxRegEx reDoxyBrief("([@\\\\]{1}(brief|details))[ \t]*", wxRE_DEFAULT | wxRE_ICASE);
thread_local wxRegEx reDoxyThrow("([@\\\\]{1}(throw|throws))[ \t]*", wxRE_DEFAULT | wxRE_ICASE);
thread_local wxRegEx reDoxyReturn("([@\\\\]{1}(return|retval|returns))[ \t]*", wxRE_DEFAULT | wxRE_ICASE);
thread_local wxRegEx reDoxyToDo("([@\\\\]{1}todo)[ \t]*", wxRE_DEFAULT | wxRE_ICASE);
thread_local wxRegEx reDoxyRemark("([@\\\\]{1}(remarks|remark))[ \t]*", wxRE_DEFAULT | wxRE_ICASE);
thread_local wxRegEx reDate("([@\\\\]{1}date)[ \t]*", wxRE_DEFAULT | wxRE_ICASE);
thread_local wxRegEx reFN("([@\\\\]{1}fn)[ \t]*", wxRE_DEFAULT | wxRE_ICASE);
} // namespace

wxString CompletionHelper::format_comment(TagEntryPtr tag, const wxString& input_comment) const
{
    return format_comment(tag.Get(), input_comment);
}

wxString CompletionHelper::format_comment(TagEntry* tag, const wxString& input_comment) const
{
    wxString beautified_comment;
    if(tag) {
        if(tag->IsMethod()) {
            auto args = split_function_signature(tag->GetSignature(), nullptr);
            beautified_comment << "```\n";
            if(args.empty()) {
                beautified_comment << tag->GetName() << "()\n";
            } else {
                beautified_comment << tag->GetName() << "(\n";
                for(const wxString& arg : args) {
                    beautified_comment << "  " << arg << ",\n";
                }

                if(beautified_comment.EndsWith(",\n")) {
                    beautified_comment.RemoveLast(2);
                }
                beautified_comment << ")\n";
            }
            beautified_comment << "```\n";
        } else if(tag->GetKind() == "variable" || tag->GetKind() == "member" || tag->IsLocalVariable()) {
            wxString clean_pattern = tag->GetPatternClean();
            clean_pattern.Trim().Trim(false);

            if(!clean_pattern.empty()) {
                beautified_comment << "```\n";
                beautified_comment << clean_pattern << "\n";
                beautified_comment << "```\n";
            } else {
                beautified_comment << tag->GetKind() << "\n";
            }
        } else {
            // other
            wxString clean_pattern = tag->GetPatternClean();
            clean_pattern.Trim().Trim(false);
            if(!clean_pattern.empty()) {
                beautified_comment << "```\n";
                beautified_comment << clean_pattern << "\n";
                beautified_comment << "```\n";
            }
        }
    }
    wxString formatted_comment;
    if(!input_comment.empty()) {
        formatted_comment = wrap_lines(input_comment);

        if(reDoxyParam.IsValid() && reDoxyParam.Matches(formatted_comment)) {
            reDoxyParam.ReplaceAll(&formatted_comment, "\nParameter\n`\\2`");
        }

        if(reDoxyBrief.IsValid() && reDoxyBrief.Matches(formatted_comment)) {
            reDoxyBrief.ReplaceAll(&formatted_comment, "");
        }

        if(reDoxyThrow.IsValid() && reDoxyThrow.Matches(formatted_comment)) {
            reDoxyThrow.ReplaceAll(&formatted_comment, "\n`Throws:`\n");
        }

        if(reDoxyReturn.IsValid() && reDoxyReturn.Matches(formatted_comment)) {
            reDoxyReturn.ReplaceAll(&formatted_comment, "\n`Returns:`\n");
        }

        if(reDoxyToDo.IsValid() && reDoxyToDo.Matches(formatted_comment)) {
            reDoxyToDo.ReplaceAll(&formatted_comment, "\nTODO\n");
        }

        if(reDoxyRemark.IsValid() && reDoxyRemark.Matches(formatted_comment)) {
            reDoxyRemark.ReplaceAll(&formatted_comment, "\n  ");
        }

        if(reDate.IsValid() && reDate.Matches(formatted_comment)) {
            reDate.ReplaceAll(&formatted_comment, "Date ");
        }

        if(reFN.IsValid() && reFN.Matches(formatted_comment)) {
            size_t fnStart, fnLen, fnEnd;
            if(reFN.GetMatch(&fnStart, &fnLen)) {
                fnEnd = formatted_comment.find('\n', fnStart);
                if(fnEnd != wxString::npos) {
                    // remove the string from fnStart -> fnEnd (including ther terminating \n)
                    formatted_comment.Remove(fnStart, (fnEnd - fnStart) + 1);
                }
            }
        }

        // horizontal line
        if(!beautified_comment.empty()) {
            beautified_comment << "---\n";
        }
        beautified_comment << formatted_comment;
    }
    return beautified_comment;
}

thread_local wxRegEx reIncludeFile("include *[\\\"\\<]{1}([a-zA-Z0-9_/\\.\\+\\-]*)");

bool CompletionHelper::is_line_include_statement(const wxString& line, wxString* file_name, wxString* suffix) const
{
    wxString tmp_line = line;

    tmp_line.Trim().Trim(false);
    tmp_line.Replace("\t", " ");

    // search for the "#"
    wxString remainder;
    if(!tmp_line.StartsWith("#", &remainder)) {
        return false;
    }

    if(!reIncludeFile.Matches(remainder)) {
        return false;
    }

    if(file_name) {
        *file_name = reIncludeFile.GetMatch(remainder, 1);
    }

    if(suffix) {
        if(tmp_line.Contains("<")) {
            *suffix = ">";
        } else {
            *suffix = "\"";
        }
    }
    return true;
}

bool CompletionHelper::is_include_statement(const wxString& f_content, wxString* file_name, wxString* suffix) const
{
    // read backward until we find LF
    if(f_content.empty()) {
        return false;
    }

    int i = f_content.size() - 1;
    for(; i >= 0; --i) {
        if(f_content[i] == '\n') {
            break;
        }
    }

    wxString line = f_content.Mid(i);
    clDEBUG() << "Checking line:" << line << "for #include statement..." << endl;
    return is_line_include_statement(line, file_name, suffix);
}

wxString CompletionHelper::normalize_function(const TagEntry* tag, size_t flags)
{
    wxString return_value;
    wxString fullname;

    wxString name = tag->GetName();
    wxString signature = tag->GetSignature();
    fullname << name << "(";
    vector<wxString> args = split_function_signature(signature, &return_value, flags);
    wxString funcsig;
    for(const wxString& arg : args) {
        funcsig << arg << ", ";
    }

    if(funcsig.EndsWith(", ")) {
        funcsig.RemoveLast(2);
    }

    fullname << funcsig << ")";
    if(tag->is_const()) {
        fullname << " const";
    }
    return fullname;
}

wxString CompletionHelper::normalize_function(TagEntryPtr tag, size_t flags)
{
    return normalize_function(tag.Get(), flags);
}

void CompletionHelper::get_cxx_keywords(std::vector<wxString>& keywords)
{
    populate_keywords();
    keywords.reserve(words.size());
    for(const wxString& word : words) {
        keywords.push_back(word);
    }
}
