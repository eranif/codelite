#include "PHPFormatterBuffer.h"
#include "PHPScannerTokens.h"
#include <wx/tokenzr.h>

PHPFormatterBuffer::PHPFormatterBuffer(const wxString& buffer, const PHPFormatterOptions& options)
    : m_scanner(NULL)
    , m_options(options)
    , m_openTagWithEcho(false)
    , m_forDepth(0)
    , m_insideForStatement(false)
    , m_insideHereDoc(false)
    , m_depth(0)
    , m_lastCommentLine(-1)
{
    m_scanner = ::phpLexerNew(buffer, kPhpLexerOpt_ReturnComments | kPhpLexerOpt_ReturnAllNonPhp);
}

PHPFormatterBuffer::~PHPFormatterBuffer()
{
    if(m_scanner) {
        ::phpLexerDestroy(&m_scanner);
    }
}

PHPFormatterBuffer& PHPFormatterBuffer::ProcessToken(const phpLexerToken& token)
{
    if(::phpLexerIsPHPCode(m_scanner)) {

        // Inside PHP tags
        if(m_insideHereDoc) {
            m_buffer << token.text;
            if(token.type == kPHP_T_END_HEREDOC) {
                AppendEOL();
                if(m_options.flags & kPFF_BreakAfterHeredoc) {
                    AppendEOL();
                }
                m_insideHereDoc = false;
            }
        } else if(token.type == kPHP_T_OPEN_TAG) {
            m_openTagWithEcho = false;
            m_buffer << token.text;
            AppendEOL();
            AppendEOL();

        } else if(token.type == kPHP_T_OPEN_TAG_WITH_ECHO) {
            m_openTagWithEcho = true;
            m_buffer << token.text << " ";

        } else if(token.type == '{') {
            HandleOpenCurlyBrace();

        } else if(token.type == kPHP_T_VARIABLE) {
            m_buffer << token.text << " ";

        } else if(token.type == kPHP_T_FOREACH) {
            m_buffer << token.text << " ";
            if(m_options.flags & kPFF_BreakBeforeForeach) {
                InsertSeparatorLine();
            }
        } else if(token.type == kPHP_T_ELSE || token.type == kPHP_T_ELSEIF) {
            if(m_options.flags & kPFF_ElseOnSameLineAsClosingCurlyBrace && m_lastToken.type == '}') {
                ReverseClearUntilFind("}");
                m_buffer << " " << token.text << " ";
            } else {
                m_buffer << token.text << " ";
            }

        } else if(token.type == kPHP_T_WHILE) {
            m_buffer << token.text << " ";
            if(m_options.flags & kPFF_BreakBeforeWhile) {
                InsertSeparatorLine();
            }
        } else if(token.type == kPHP_T_CLASS) {
            if(m_options.flags & kPFF_BreakBeforeClass) {
                InsertSeparatorLine();
            }
            m_buffer << token.text << " ";

        } else if(token.type == kPHP_T_FUNCTION) {
            // Found a function
            m_buffer << token.text << " ";
            if(m_lastCommentLine != token.lineNumber) {
                // this function has no comment associated with it
                if(m_options.flags & kPFF_BreakBeforeFunction) {
                    // But don't insert new line separator if this keyword
                    // was found in a statement like:
                    // use function A as B;
                    if(m_lastToken.type != kPHP_T_USE) {
                        // Look backward until we find the last EOL
                        // and insert another one...
                        InsertSeparatorLine();
                    }
                }
            }
        } else if(token.type == kPHP_T_START_HEREDOC) {
            RemoveLastSpace();
            m_buffer << token.text;
            m_insideHereDoc = true;

        } else if(token.type == kPHP_T_FOR) {
            // for(;;) is a special case where
            // we don't insert new line after semi-colon
            m_insideForStatement = true;
            m_buffer << token.text;

        } else if(token.type == '(' && m_insideForStatement) {
            m_forDepth++;
            m_buffer << token.text;

        } else if(token.type == ')' && m_insideForStatement) {
            m_forDepth--;
            if(m_forDepth == 0) {
                m_insideForStatement = false;
            }
            m_buffer << token.text;

        } else if(token.type == '(' || token.type == '[') {
            RemoveLastSpace();
            m_buffer << token.text;

        } else if(token.type == ')') {
            RemoveLastSpace();
            m_buffer << token.text;

        } else if(token.type == ']') {
            RemoveLastSpace();
            m_buffer << token.text << " ";

        } else if(token.type == ';') {
            RemoveLastSpace();
            m_buffer << token.text;
            AppendEOL();

        } else if(token.type == '}') {
            UnIndent();
            m_buffer << token.text;
            AppendEOL(kDepthDec);

        } else if(token.type == ',') {
            RemoveLastSpace();
            m_buffer << token.text << " ";

        } else if(token.type == '&') {
            // attach reference to the next token
            m_buffer << token.text;

        } else if(token.type == kPHP_T_CXX_COMMENT) {
            // C++ style comment ("//")
            // AppendEOL();
            m_buffer << token.text;
            AppendEOL();
            m_lastCommentLine = token.lineNumber + 1;

        } else if(token.type == kPHP_T_C_COMMENT) {
            // Doxygen style comment: we first format it to match
            // the current indentation + line ending before adding
            // it to the current buffer
            AppendEOL();
            m_buffer << FormatDoxyComment(token.text);
            AppendEOL();
            m_lastCommentLine = token.endLineNumber + 1;

        } else if(token.type == kPHP_T_OBJECT_OPERATOR || token.type == kPHP_T_PAAMAYIM_NEKUDOTAYIM) {
            // -> operator or ::
            RemoveLastSpace();
            m_buffer << token.text;

        } else if(token.type == '!') {
            // dont add extrace space after the NOT operator
            m_buffer << token.text;

        } else if(token.type == kPHP_T_NS_SEPARATOR) {
            if(m_lastToken.type == kPHP_T_IDENTIFIER) {
                RemoveLastSpace();
            }
            m_buffer << token.text;

        } else {
            // by default, add the token text and a space after it
            m_buffer << token.text << " ";
        }

        m_lastToken = token;

    } else {
        if(token.type == kPHP_T_CLOSE_TAG && !m_openTagWithEcho) {
            AppendEOL();
        }
        m_buffer << token.text;
    }
    return *this;
}

void PHPFormatterBuffer::HandleOpenCurlyBrace()
{
    RemoveLastSpace();
    m_buffer << " {";
    AppendEOL(kDepthInc);
}

void PHPFormatterBuffer::AppendEOL(eDepthCommand depth)
{
    m_buffer << m_options.eol;
    switch(depth) {
    case kDepthDec:
        --m_depth;
        break;
    case kDepthIncTemporarily:
    case kDepthInc:
        ++m_depth;
        break;
    default:
        break;
    }
    m_buffer << GetIndent();
    if(kDepthIncTemporarily == depth) {
        --m_depth;
    }
}

void PHPFormatterBuffer::UnIndent()
{

    if(IsUseTabs() && !m_buffer.IsEmpty() && m_buffer.Last() == '\t') {
        m_buffer.RemoveLast();

    } else if(!IsUseTabs() && (m_buffer.length() >= m_options.indentSize) &&
              m_buffer.Mid(m_buffer.length() - m_options.indentSize) == wxString(' ', m_options.indentSize)) {
        m_buffer.RemoveLast(m_options.indentSize);
    }
}

void PHPFormatterBuffer::RemoveLastSpace()
{
    if(!m_buffer.IsEmpty() && m_buffer.Last() == ' ') {
        m_buffer.RemoveLast();
    }
}

wxString PHPFormatterBuffer::FormatDoxyComment(const wxString& comment)
{
    wxString formattedBlock;
    wxString indent = GetIndent();
    wxArrayString lines = ::wxStringTokenize(comment, "\n", wxTOKEN_STRTOK);
    for(size_t i = 0; i < lines.GetCount(); ++i) {
        lines.Item(i).Trim().Trim(false);
        if(i) {
            // preprend space + the indent string for every line except for the first line
            lines.Item(i).Prepend(" ");
            lines.Item(i).Prepend(indent);
        }
        formattedBlock << lines.Item(i) << m_options.eol;
    }

    formattedBlock.RemoveLast(m_options.eol.length());
    return formattedBlock;
}
wxString& PHPFormatterBuffer::GetIndent()
{
    if(IsUseTabs()) {
        m_indentString = wxString('\t', m_depth);
    } else {
        m_indentString = wxString(' ', m_depth * m_options.indentSize);
    }
    return m_indentString;
}

void PHPFormatterBuffer::InsertSeparatorLine()
{
    size_t where = m_buffer.rfind(m_options.eol);
    if(where != wxString::npos) {
        m_buffer.insert(where, m_options.eol);
    }
}

void PHPFormatterBuffer::ReverseClearUntilFind(const wxString& delim)
{
    size_t where = m_buffer.rfind(delim);
    if(where != wxString::npos) {
        m_buffer = m_buffer.Mid(0, where + delim.length());
    }
}

void PHPFormatterBuffer::format()
{
    phpLexerToken token;
    phpLexerToken::Vet_t sequence;
    m_stack.push(sequence);
    m_sequence = &m_stack.top();
    while(NextToken(token)) {
        phpLexerToken nextToken;
        if(::phpLexerIsPHPCode(m_scanner)) {
            ProcessToken(token);

            //
            // Special indentation cases
            // Handle the following:
            // for(..) <statement> -> for(..)
            //                            <statement>
            // if(..) <statement> -> if(..)
            //                            <statement>
            // etc.
            // In addtion, we also handle here the following:
            // if(something) {} else <statement> =>
            //                                      if(something) {
            //                                      } else
            //                                          <statement>
            //

            if(token.type == '(') {
                // Create new stack
                m_stack.push(phpLexerToken::Vet_t());
                m_sequence = &m_stack.top();
            }
            if(token.type == ')') {
                // switch back to the previous sequence
                if(m_stack.size() >= 2) {
                    m_stack.pop();
                    m_sequence = &m_stack.top();
                }

                if(!m_sequence->empty()) {
                    phpLexerToken lastToken = m_sequence->at(m_sequence->size() - 1);
                    // The following tokens are usually followed by an open brace
                    if(lastToken.type == kPHP_T_IF || lastToken.type == kPHP_T_FOR || lastToken.type == kPHP_T_ELSEIF ||
                       lastToken.type == kPHP_T_FOREACH || lastToken.type == kPHP_T_WHILE) {
                        // Peek at the next char
                        if(PeekToken(nextToken)) {
                            if(nextToken.type != '{' && !nextToken.IsAnyComment()) {
                                // Increase the depth but only temporarily for the next statement
                                AppendEOL(kDepthIncTemporarily);
                            }
                        }
                    }
                }
            } else if(token.type == kPHP_T_ELSE) {
                // Check for 'else <statement>' (without an open  curly braces)
                if(PeekToken(nextToken) && nextToken.type != '{' && nextToken.type != kPHP_T_IF) {
                    AppendEOL(kDepthIncTemporarily);
                }

            } else {
                m_sequence->push_back(token);
            }
        } else {
            ProcessToken(token);
        }
    }
}

bool PHPFormatterBuffer::NextToken(phpLexerToken& token)
{
    if(m_tokensBuffer.empty()) {
        return ::phpLexerNext(m_scanner, token);
    } else {
        token = *m_tokensBuffer.begin();
        m_tokensBuffer.erase(m_tokensBuffer.begin());
        return true;
    }
}

bool PHPFormatterBuffer::PeekToken(phpLexerToken& token)
{
    if(!::phpLexerNext(m_scanner, token)) return false;
    m_tokensBuffer.push_back(token);
    return true;
}
