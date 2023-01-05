#include "CxxPreProcessorScanner.h"

#include "CxxPreProcessor.h"
#include "CxxPreProcessorExpression.h"
#include "CxxScannerTokens.h"
#include "file_logger.h"

#include <wx/sharedptr.h>

CxxPreProcessorScanner::CxxPreProcessorScanner(const wxFileName& filename, size_t options,
                                               std::unordered_set<wxString>& visitedFiles)
    : m_scanner(NULL)
    , m_filename(filename)
    , m_options(options)
    , m_visitedFiles(visitedFiles)
{
    m_scanner = ::LexerNew(m_filename, m_options);
    wxASSERT(m_scanner);
}

CxxPreProcessorScanner::~CxxPreProcessorScanner()
{
    if(m_scanner) {
        ::LexerDestroy(&m_scanner);
    }
}

void CxxPreProcessorScanner::GetRestOfPPLine(wxString& rest, bool collectNumberOnly)
{
    CxxLexerToken token;
    bool numberFound = false;
    while(m_scanner && ::LexerNext(m_scanner, token) && token.GetType() != T_PP_STATE_EXIT) {
        if(!numberFound && collectNumberOnly) {
            if(token.GetType() == T_PP_DEC_NUMBER || token.GetType() == T_PP_OCTAL_NUMBER ||
               token.GetType() == T_PP_HEX_NUMBER || token.GetType() == T_PP_FLOAT_NUMBER) {
                rest = token.GetWXString();
                numberFound = true;
            }
        } else if(!collectNumberOnly) {
            rest << " " << token.GetWXString();
        }
    }
    rest.Trim(false).Trim(true);
}

bool CxxPreProcessorScanner::ConsumeBlock()
{
    CxxLexerToken token;
    int depth = 1;
    while(m_scanner && ::LexerNext(m_scanner, token)) {
        switch(token.GetType()) {
        case T_PP_ENDIF:
            depth--;
            if(depth == 0) {
                return true;
            }
            break;
        case T_PP_IF:
        case T_PP_IFDEF:
        case T_PP_IFNDEF:
            depth++;
            break;
        default:
            break;
        }
    }
    // throw CxxLexerException("Invalid EOF");
    return false;
}

void CxxPreProcessorScanner::Parse(CxxPreProcessor* pp)
{
    CxxLexerToken token;
    bool searchingForBranch = false;
    CxxPreProcessorToken::Map_t& ppTable = pp->GetTokens();
    while(m_scanner && ::LexerNext(m_scanner, token)) {
        // Pre Processor state
        switch(token.GetType()) {
        case T_PP_INCLUDE_FILENAME: {
            // we found an include statement, recurse into it
            wxFileName include;
            if(pp->ExpandInclude(m_filename, token.GetWXString(), include) &&
               m_visitedFiles.count(include.GetFullPath()) == 0) {
                m_visitedFiles.insert(include.GetFullPath());
                CxxPreProcessorScanner scanner(include, pp->GetOptions(), m_visitedFiles);
                try {
                    if(!scanner.IsNull()) {
                        scanner.Parse(pp);
                    }
                } catch(CxxLexerException& e) {
                    // catch the exception
                    clDEBUG() << "Exception caught:" << e.message << endl;
                }
                // make sure we always delete the scanner
                LOG_IF_TRACE { clDEBUG1() << "<== Resuming parser on file:" << m_filename << clEndl; }
            }
            break;
        }
        case T_PP_IFDEF: {
            searchingForBranch = true;
            // read the identifier
            ReadUntilMatch(T_PP_IDENTIFIER, token);
            if(IsTokenExists(ppTable, token)) {
                searchingForBranch = false;
                // condition is true
                Parse(pp);
            } else {
                // skip until we find the next:
                // else, elif, endif (but do not consume these tokens)
                if(!ConsumeCurrentBranch())
                    return;
            }
            break;
        }
        case T_PP_IFNDEF: {
            searchingForBranch = true;
            // read the identifier
            ReadUntilMatch(T_PP_IDENTIFIER, token);
            if(!IsTokenExists(ppTable, token)) {
                searchingForBranch = false;
                // condition is true
                Parse(pp);
            } else {
                // skip until we find the next:
                // else, elif, endif (but do not consume these tokens)
                if(!ConsumeCurrentBranch())
                    return;
            }
            break;
        }
        case T_PP_IF:
            searchingForBranch = true;
        case T_PP_ELIF: {
            if(searchingForBranch) {
                // We expect a condition
                if(!CheckIf(ppTable)) {
                    // skip until we find the next:
                    // else, elif, endif (but do not consume these tokens)
                    if(!ConsumeCurrentBranch())
                        return;

                } else {
                    searchingForBranch = false;
                    // condition is true
                    Parse(pp);
                }
            } else {
                ConsumeBlock();
                return;
            }
            break;
        }
        case T_PP_ELSE: {
            if(searchingForBranch) {
                // if we reached an else, it means that we could not match
                // a if/elif/ifdef condition until now - enter it
                Parse(pp);
                searchingForBranch = false;
            } else {
                // we already found the branch for the current block
                // this means that the 'else' is a stop sign for us
                ConsumeBlock();
                return;
            }
            break;
        }
        case T_PP_ENDIF: {
            return;
        }
        case T_PP_DEFINE: {
            if(!::LexerNext(m_scanner, token) || token.GetType() != T_PP_IDENTIFIER) {
                // Recover
                wxString dummy;
                GetRestOfPPLine(dummy);
                break;
            }
            wxString macroName = token.GetWXString();

            wxString macroValue;
            // Optionally get the value
            GetRestOfPPLine(macroValue, m_options & kLexerOpt_CollectMacroValueNumbers);

            CxxPreProcessorToken pp;
            pp.name = macroName;
            pp.value = macroValue;
            // mark this token for deletion when the entire TU parsing is done
            pp.deleteOnExit = (m_options & kLexerOpt_DontCollectMacrosDefinedInThisFile);
            ppTable.insert(std::make_pair(pp.name, pp));
            break;
        }
        }
    }
}

bool CxxPreProcessorScanner::CheckIfDefined(const CxxPreProcessorToken::Map_t& table)
{
    CxxLexerToken token;
    if(m_scanner && ::LexerNext(m_scanner, token)) {
        if(token.GetType() == T_PP_STATE_EXIT) {
            return false;
        }
        switch(token.GetType()) {
        case T_PP_IDENTIFIER:
            return table.count(token.GetWXString());
        case '(':
            // ignore
            break;
        default:
            break;
        }
    }
    return false;
}

#define SET_CUR_EXPR_VALUE_RET_FALSE(v) \
    if(cur->IsValueSet())               \
        return false;                   \
    else                                \
        cur->SetValue((double)v);

struct ExpressionLocker {
    CxxPreProcessorExpression* m_expr;
    ExpressionLocker(CxxPreProcessorExpression* expr)
        : m_expr(expr)
    {
    }
    ~ExpressionLocker() { wxDELETE(m_expr); }
};

bool CxxPreProcessorScanner::CheckIf(const CxxPreProcessorToken::Map_t& table)
{
    // we currently support
    // #if IDENTIFIER
    // #if NUMBER
    // #if (cond) && (cond) || !(cond)
    // anything else, returns false
    CxxLexerToken token;
    CxxPreProcessorExpression* cur = new CxxPreProcessorExpression(false);
    ExpressionLocker locker(cur);
    CxxPreProcessorExpression* head = cur;
    while(m_scanner && ::LexerNext(m_scanner, token)) {
        if(token.GetType() == T_PP_STATE_EXIT) {
            bool res = head->IsTrue();
            return res;
        }
        switch(token.GetType()) {
        case '(':
        case ')':
            // ignore parenthesis
            break;

        case '!':
            cur->SetNot();
            break;

        case T_PP_DEC_NUMBER: {
            SET_CUR_EXPR_VALUE_RET_FALSE(atol(token.GetText()));
            break;
        }
        case T_PP_OCTAL_NUMBER: {
            SET_CUR_EXPR_VALUE_RET_FALSE(atol(token.GetText()));
            break;
        }
        case T_PP_HEX_NUMBER: {
            SET_CUR_EXPR_VALUE_RET_FALSE(atol(token.GetText()));
            break;
        }
        case T_PP_FLOAT_NUMBER: {
            SET_CUR_EXPR_VALUE_RET_FALSE(atof(token.GetText()));
            break;
        }
        case T_PP_IDENTIFIER: {
            wxString identifier = token.GetWXString();
            CxxPreProcessorToken::Map_t::const_iterator iter = table.find(identifier);
            if(iter == table.end()) {
                SET_CUR_EXPR_VALUE_RET_FALSE(0);
            } else {
                if(cur->IsDefined()) {
                    // no need to test further, it exists (the current expression
                    // if a 'defined' statement)
                    SET_CUR_EXPR_VALUE_RET_FALSE(1);
                } else {
                    wxString macroValue = iter->second.value;
                    if(macroValue.IsEmpty()) {
                        SET_CUR_EXPR_VALUE_RET_FALSE(0);
                    } else {
                        long v(0);
                        bool res = macroValue.ToCLong(&v);
                        if(!res) {
                            SET_CUR_EXPR_VALUE_RET_FALSE(0);

                        } else {
                            SET_CUR_EXPR_VALUE_RET_FALSE(v);
                        }
                    }
                }
            }
            break;
        }
        case T_PP_DEFINED:
            cur->SetDefined(true);
            break;
        case T_PP_AND:
            // And operand
            cur = cur->SetNext(CxxPreProcessorExpression::kAND, new CxxPreProcessorExpression(false));
            break;
        case T_PP_OR:
            // OR operand
            cur = cur->SetNext(CxxPreProcessorExpression::kOR, new CxxPreProcessorExpression(false));
            break;
        case T_PP_GT:
            cur = cur->SetNext(CxxPreProcessorExpression::kGreaterThan, new CxxPreProcessorExpression(0));
            break;
        case T_PP_GTEQ:
            cur = cur->SetNext(CxxPreProcessorExpression::kGreaterThanEqual, new CxxPreProcessorExpression(0));
            break;
        case T_PP_LT:
            cur = cur->SetNext(CxxPreProcessorExpression::kLowerThan, new CxxPreProcessorExpression(0));
            break;
        case T_PP_LTEQ:
            cur = cur->SetNext(CxxPreProcessorExpression::kLowerThanEqual, new CxxPreProcessorExpression(0));
            break;
        default:
            break;
        }
    }
    return false;
}

/**
 * @brief fast-forward until we hit one of the tokens in the stopTokens
 * Note that this function does not consume the 'stopToken'
 */
bool CxxPreProcessorScanner::ConsumeCurrentBranch()
{
    CxxLexerToken token;
    int depth = 1;
    // T_PP_ELIF
    // T_PP_ELSE
    // T_PP_ENDIF
    while(m_scanner && ::LexerNext(m_scanner, token)) {
        switch(token.GetType()) {
        case T_PP_IF:
        case T_PP_IFDEF:
        case T_PP_IFNDEF:
            depth++;
            break;
        case T_PP_ENDIF:
            if(depth == 1) {
                return true;
            }
            depth--;
            break;
        case T_PP_ELIF:
        case T_PP_ELSE:
            if(depth == 1) {
                ::LexerUnget(m_scanner);
                return true;
            }
            break;
        default:
            break;
        }
    }
    return false;
}

void CxxPreProcessorScanner::ReadUntilMatch(int type, CxxLexerToken& token)
{
    while(m_scanner && ::LexerNext(m_scanner, token)) {
        if(token.GetType() == type) {
            return;
        } else if(token.GetType() == T_PP_STATE_EXIT) {
            throw CxxLexerException(wxString() << "Could not find a match for type: " << type);
        }
    }
    throw CxxLexerException(wxString() << "<<EOF>> Could not find a match for type: " << type);
}

bool CxxPreProcessorScanner::IsTokenExists(const CxxPreProcessorToken::Map_t& table, const CxxLexerToken& token)
{
    return table.count(token.GetWXString());
}
