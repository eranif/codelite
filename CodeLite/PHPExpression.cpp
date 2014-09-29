#include "PHPExpression.h"
#include "PHPSourceFile.h"
#include "PHPEntityClass.h"
#include "PHPEntityVariable.h"

PHPExpression::PHPExpression(const wxString& fulltext, const wxString& exprText)
    : m_type(kNone)
    , m_text(fulltext)
{
    if(exprText.IsEmpty()) {
        // use the full text to extract the expression
        m_expression = CreateExpression(fulltext);

    } else {
        wxString phpExprText = exprText;
        if(!exprText.StartsWith("<?php")) {
            // without this, the parser will refuse to work :)
            phpExprText.Prepend("<?php ");
        }
        m_expression = CreateExpression(phpExprText);
    }
}

PHPExpression::~PHPExpression() {}

wxVector<phpLexerToken> PHPExpression::CreateExpression(const wxString& text)
{
    // strip the text from any comments
    PHPScanner_t scanner = ::phpLexerNew(text);
    phpLexerToken token;
    wxVector<phpLexerToken> tokens;
    int depth(0);
    while(::phpLexerNext(scanner, token)) {
        switch(token.type) {
        case kPHP_T_OPEN_TAG:
            // skip the open tag
            break;
        case ';':
        case '{':
        case '}':
        case '=':
            if(depth == 0) {
                tokens.clear();
            }
            break;
        case '(':
            if(depth == 0) {
                tokens.push_back(token);
            }
            depth++;
            break;
        case ')':
            depth--;
            if(depth == 0) {
                // test the next token
                phpLexerToken nextToken;
                if(::phpLexerNext(scanner, nextToken)) ::phpLexerUnget(scanner);

                if(nextToken.type == kPHP_T_OBJECT_OPERATOR) {
                    tokens.push_back(token);
                } else {
                    tokens.clear();
                }
            }
            break;
        default:
            if(depth == 0) {
                tokens.push_back(token);
            }
            break;
        }
    }
    ::phpLexerDestroy(&scanner);
    return tokens;
}

PHPEntityBase::Ptr_t PHPExpression::Resolve(PHPLookupTable& lookpTable)
{
    if(m_expression.empty()) return PHPEntityBase::Ptr_t(NULL);

    PHPSourceFile source(m_text);
    source.SetParseFunctionBody(true);
    source.Parse();
    wxString asString = SimplifyExpression(source, 0);
    
    // Now, use the lookup table
    std::list<PHPExpression::Part>::iterator iter = m_parts.begin();
    PHPEntityBase::Ptr_t currentToken(NULL);
    for(; iter != m_parts.end(); ++iter) {
        if(!currentToken) {
            // first token
            currentToken = lookpTable.FindScope(iter->m_text);
            
        } else {
            // load the children of the current token (optionally, filter by the text)
            currentToken = lookpTable.FindMemberOf(currentToken->GetDbId(), iter->m_text);
        }
        if(!currentToken) {
            // return NULL
            return currentToken;
        }
    }
    return currentToken;
}

wxString PHPExpression::SimplifyExpression(PHPSourceFile& source, int depth)
{
    if(depth > 5) {
        // avoid infinite recursion, by limiting the nest level to 5
        return "";
    }
    // Parse the input source file
    PHPEntityBase::Ptr_t scope = source.CurrentScope();
    const PHPEntityBase* innerClass = source.Class();

    // Check the first token

    // Phase 1:
    // Loop over the expression and resolve as much as we can here for the first token
    // (mainly, replace $this, self, static, $variable)

    phpLexerToken token;
    wxString newExpr;
    wxString firstToken;
    for(size_t i = 0; i < m_expression.size(); ++i) {
        token = m_expression.at(i);
        if(i == 0) {
            // Perform basic replacements that we can conduct here without the need of the global
            // lookup table
            if(token.type == kPHP_T_THIS) {
                // the first token is $this
                // replace it with the current class absolute path
                if(!innerClass) return "";
                firstToken = innerClass->Cast<PHPEntityClass>()->GetName(); // Is always in absolute path

            } else if(token.type == kPHP_T_SELF) {
                // Same as $this: replace it with the current class absolute path
                if(!innerClass) return "";
                firstToken = innerClass->Cast<PHPEntityClass>()->GetName(); // Is always in absolute path

            } else if(token.type == kPHP_T_STATIC) {
                // Same as $this: replace it with the current class absolute path
                if(!innerClass) return "";
                firstToken = innerClass->Cast<PHPEntityClass>()->GetName(); // Is always in absolute path

            } else if(token.type == kPHP_T_VARIABLE) {
                // the expression being evaluated starts with a variable (e.g. $a->something()->)
                // in this case, use the current scope ('scope') and replace it with the real type
                // Note that the type can be another expression
                // e.g.:
                // $a = $this->getQuery();
                // $b = $a->fetchAll()->
                // so if the expression being evaluated is "$a->fetchAll()->"
                // we first replace $a with '$this->getQuery()' so it becomes:
                // $this->getQuery()->fetchAll()->
                // However, $this also need a replacement so eventually, it becomes like this:
                // \MyClass->getQuery->fetchAll-> and this is something that we can evaluate easily using
                // our lookup tables (note that the parenthessis are missing on purpose)
                PHPEntityBase::Ptr_t local = scope->FindChild(token.text);
                if(local && local->Cast<PHPEntityVariable>()) {
                    if(!local->Cast<PHPEntityVariable>()->GetTypeHint().IsEmpty()) {
                        // we have type hint! - use it
                        firstToken = local->Cast<PHPEntityVariable>()->GetTypeHint();

                    } else if(!local->Cast<PHPEntityVariable>()->GetExpressionHint().IsEmpty()) {
                        // we have an expression hint - use it
                        // append the "->" to the expression to make sure that the parser will understand it
                        // as an expression
                        PHPExpression e(m_text, local->Cast<PHPEntityVariable>()->GetExpressionHint() + "->");
                        firstToken = e.SimplifyExpression(source, depth + 1);
                        if(firstToken.EndsWith("->")) {
                            // remove the last 2 characters
                            firstToken.RemoveLast(2);
                        }
                    }
                } else {
                    // this local variable does not exist in the current scope
                    return "";
                }
            }
        }
        if(!firstToken.IsEmpty()) {
            newExpr = firstToken;
            firstToken.Clear();
        } else {
            newExpr << " " << token.text;
        }
    }

    // Phase 2:
    // Now break the tokens from the lexers into something that we can work with
    // when using the lookup table.

    // The split in this phase is done by searching for kPHP_T_OBJECT_OPERATOR and kPHP_T_PAAMAYIM_NEKUDOTAYIM
    // separators
    m_expression = CreateExpression("<?php " + newExpr);
    Part part;
    wxString currentText;
    for(size_t i = 0; i < m_expression.size(); ++i) {
        token = m_expression.at(i);
        // Remove any braces and split by object kPHP_T_OBJECT_OPERATOR and kPHP_T_PAAMAYIM_NEKUDOTAYIM
        switch(token.type) {
        case kPHP_T_OPEN_TAG:
            break;
        case '(':
            if(!currentText.IsEmpty()) {
                part.m_text = currentText;
            }
            break;
        case ')':
            // skip it
            break;
        case kPHP_T_PAAMAYIM_NEKUDOTAYIM:
        case kPHP_T_OBJECT_OPERATOR:
            if(!currentText.IsEmpty() && part.m_text.IsEmpty()) {
                part.m_text = currentText;
            }
            part.m_operator = token.type;
            part.m_operatorText = token.text;
            m_parts.push_back(part);

            // cleanup
            currentText.clear();
            part.m_text.clear();
            part.m_operatorText.clear();
            part.m_operator = wxNOT_FOUND;
            break;
        default:
            currentText << token.text;
            break;
        }
    }

    if(!currentText.IsEmpty()) {
        m_filter = currentText;
    }

    wxString simplified;
    List_t::iterator iter = m_parts.begin();
    for(; iter != m_parts.end(); ++iter) {
        simplified << iter->m_text << iter->m_operatorText;
    }
    return simplified.Trim().Trim(false);
}

wxString PHPExpression::GetExpressionAsString() const
{
    wxString expr;
    for(size_t i = 0; i < m_expression.size(); ++i) {
        expr << m_expression.at(i).text << " ";
    }
    return expr;
}
