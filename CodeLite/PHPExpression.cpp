#include "PHPExpression.h"
#include "PHPSourceFile.h"
#include "PHPEntityClass.h"
#include "PHPEntityVariable.h"
#include "PHPEntityFunction.h"
#include <stack>

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

phpLexerToken::Vet_t PHPExpression::CreateExpression(const wxString& text)
{
    // Extract the expression at the end of the input text
    std::stack<phpLexerToken::Vet_t> stack;
    phpLexerToken::Vet_t tokens;
    stack.push(tokens);

    phpLexerToken::Vet_t* current = &stack.top();

    // strip the text from any comments
    PHPScanner_t scanner = ::phpLexerNew(text);
    phpLexerToken token;
    while(::phpLexerNext(scanner, token)) {
        switch(token.type) {
        case kPHP_T_OPEN_TAG:
            // skip the open tag
            break;
        // the following are tokens that once seen
        // we should start a new expression:
        case kPHP_T_ECHO:
        case kPHP_T_CASE:
        case kPHP_T_RETURN:
        case kPHP_T_THROW:
        case kPHP_T_CLASS:
        case kPHP_T_TRAIT:
        case kPHP_T_INTERFACE:
        case kPHP_T_EXTENDS:
        case kPHP_T_IMPLEMENTS:
        case kPHP_T_DOUBLE_ARROW:
        case kPHP_T_NS_C:
        case kPHP_T_FUNC_C:
        case kPHP_T_METHOD_C:
        case kPHP_T_CLASS_C:
        case kPHP_T_TRAIT_C:
        case kPHP_T_LINE:
        case kPHP_T_FILE:
        case kPHP_T_DIR:
        case kPHP_T_YIELD:
        case kPHP_T_LOGICAL_OR:
        case kPHP_T_LOGICAL_XOR:
        case kPHP_T_LOGICAL_AND:
        case kPHP_T_PRINT:
        case kPHP_T_PLUS_EQUAL:
        case kPHP_T_MINUS_EQUAL:
        case kPHP_T_MUL_EQUAL:
        case kPHP_T_DIV_EQUAL:
        case kPHP_T_CONCAT_EQUAL:
        case kPHP_T_MOD_EQUAL:
        case kPHP_T_AND_EQUAL:
        case kPHP_T_OR_EQUAL:
        case kPHP_T_XOR_EQUAL:
        case kPHP_T_SL_EQUAL:
        case kPHP_T_SR_EQUAL:
        case kPHP_T_BOOLEAN_OR:
        case kPHP_T_BOOLEAN_AND:
        case kPHP_T_IS_EQUAL:
        case kPHP_T_IS_NOT_EQUAL:
        case kPHP_T_IS_IDENTICAL:
        case kPHP_T_IS_NOT_IDENTICAL:
        case kPHP_T_IS_SMALLER_OR_EQUAL:
        case kPHP_T_IS_GREATER_OR_EQUAL:
        case kPHP_T_SL:
        case kPHP_T_SR:
        case '.':
        case ';':
        case '{':
        case '}':
        case '=':
        case ':':
            if(current) current->clear();
            break;
        case '(':
            if(current) current->push_back(token);
            stack.push(phpLexerToken::Vet_t());
            current = &stack.top();
            break;
        case ')':
            if(stack.size() < 2) {
                // parse error...
                return phpLexerToken::Vet_t();
            }
            // switch back to the previous set of tokens
            stack.pop();
            current = &stack.top();
            if(current) current->push_back(token);
            break;
        default:
            if(current) current->push_back(token);
            break;
        }
    }

    ::phpLexerDestroy(&scanner);
    phpLexerToken::Vet_t result;
    if(current) {
        result.swap(*current);
    }
    return result;
}

PHPEntityBase::Ptr_t PHPExpression::Resolve(PHPLookupTable& lookpTable, const wxString& sourceFileName)
{
    if(m_expression.empty()) return PHPEntityBase::Ptr_t(NULL);

    PHPSourceFile source(m_text);
    source.SetParseFunctionBody(true);
    source.SetFilename(sourceFileName);
    source.Parse();
    wxString asString = SimplifyExpression(source, 0);
    wxUnusedVar(asString);

    // Now, use the lookup table
    std::list<PHPExpression::Part>::iterator iter = m_parts.begin();
    PHPEntityBase::Ptr_t currentToken(NULL);
    for(; iter != m_parts.end(); ++iter) {
        Part& part = *iter;
        if(!currentToken) {
            // first token
            currentToken = lookpTable.FindScope(part.m_text);

        } else {
            // load the children of the current token (optionally, filter by the text)
            currentToken = lookpTable.FindMemberOf(currentToken->GetDbId(), part.m_text);
        }

        // If the current "part" of the expression ends with a scope resolving operator ("::") or
        // an object operator ("->") we need to resolve the operator to the actual type (
        // incase of a functin it will be the return value, and in case of a variable it will be
        // the type hint)
        if(currentToken) {
            if(part.m_operator == kPHP_T_OBJECT_OPERATOR || part.m_operator == kPHP_T_PAAMAYIM_NEKUDOTAYIM) {
                wxString actualType;
                if(currentToken->Is(kEntityTypeFunction)) {
                    // return the function return value
                    actualType = currentToken->Cast<PHPEntityFunction>()->GetReturnValue();
                } else if(currentToken->Is(kEntityTypeVariable)) {
                    // return the type hint
                    actualType = currentToken->Cast<PHPEntityVariable>()->GetTypeHint();
                }

                if(!actualType.IsEmpty()) {
                    currentToken = lookpTable.FindScope(actualType);
                }
            }
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
                if(m_parts.empty() && token.type == kPHP_T_PAAMAYIM_NEKUDOTAYIM) {
                    // The first token in the "parts" list has a scope resolving operator ("::")
                    // we need to make sure that the indetifier is provided in fullpath
                    part.m_text = source.MakeIdentifierAbsolute(currentText);
                } else {
                    part.m_text = currentText;
                }
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
        case kPHP_T_THIS:
        case kPHP_T_SELF:
        case kPHP_T_STATIC:
            part.m_textType = token.type;
            currentText << token.text;
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
        expr << m_expression.at(i).text;
    }
    return expr;
}

size_t PHPExpression::GetLookupFlags() const
{
    size_t flags(0);
    if(m_parts.empty()) return flags;

    Part lastExpressionPart = m_parts.back();
    if(lastExpressionPart.m_operator == kPHP_T_PAAMAYIM_NEKUDOTAYIM) {
        if(lastExpressionPart.m_textType == kPHP_T_SELF)
            flags |= PHPLookupTable::kLookupFlags_SelfStaticMembers;
        else
            flags |= PHPLookupTable::kLookupFlags_StaticMembers;
    }
    return flags;
}
