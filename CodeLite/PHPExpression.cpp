#include "PHPExpression.h"
#include "PHPSourceFile.h"
#include "PHPEntityClass.h"
#include "PHPEntityVariable.h"
#include "PHPEntityFunction.h"
#include <stack>
#include <algorithm>
#include <set>
#include "PHPEntityNamespace.h"
#include "PHPEntityFunctionAlias.h"

PHPExpression::PHPExpression(const wxString& fulltext, const wxString& exprText, bool functionCalltipExpr)
    : m_type(kNone)
    , m_text(fulltext)
    , m_functionCalltipExpr(functionCalltipExpr)
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
    m_exprStartsWithOpenTag = false;
    // Extract the expression at the end of the input text
    std::stack<phpLexerToken::Vet_t> stack;
    phpLexerToken::Vet_t tokens;
    stack.push(tokens);

    phpLexerToken::Vet_t* current = &stack.top();

    // strip the text from any comments
    PHPScanner_t scanner = ::phpLexerNew(text);
    phpLexerToken token, lastToken;
    while(::phpLexerNext(scanner, token)) {
        lastToken = token;
        switch(token.type) {
        case kPHP_T_OPEN_TAG:
            if(current) current->push_back(token);
            break;
        // the following are tokens that once seen
        // we should start a new expression:
        case kPHP_T_NEW:
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
        case kPHP_T_FUNCTION:
        case kPHP_T_ABSTRACT:
        case kPHP_T_PUBLIC:
        case kPHP_T_PROTECTED:
        case kPHP_T_PRIVATE:
        case kPHP_T_FINAL:
        case kPHP_T_CONST:
        case kPHP_T_REQUIRE:
        case kPHP_T_REQUIRE_ONCE:
        case kPHP_T_USE:
        case kPHP_T_INT_CAST:
        case kPHP_T_DOUBLE_CAST:
        case kPHP_T_STRING_CAST:
        case kPHP_T_OBJECT_CAST:
        case kPHP_T_ARRAY_CAST:
        case kPHP_T_BOOL_CAST:
        case kPHP_T_UNSET_CAST:
        case kPHP_T_INCLUDE:
        case kPHP_T_INCLUDE_ONCE:
        case '.':
        case ';':
        case '{':
        case '}':
        case '=':
        case ':':
        case ',':
        case '!':
        case '?':
        case '|':
        case '@':
        case '<':
        case '*':
            if(current) current->clear();
            break;
        case '(':
        case '[':
            if(current) current->push_back(token);
            stack.push(phpLexerToken::Vet_t());
            current = &stack.top();
            break;
        case ')':
        case ']':
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

    if(m_functionCalltipExpr) {
        // the expression parser was constructed for the purpose of
        // function-calltip so replace the current expression with the previous one from the stack
        // i.e. the one before the first open brace
        if(stack.size() > 1) {
            stack.pop(); // remove the last token sequence
            current = &stack.top();
        }
    }

    if(current && !current->empty()) {
        if(current->at(0).type == kPHP_T_OPEN_TAG) {
            if(current->at(0).Text() == "<?") {
                m_exprStartsWithOpenTag = true;
            }
            current->erase(current->begin());
        }
        result.swap(*current);
    }
    return result;
}

PHPEntityBase::Ptr_t PHPExpression::Resolve(PHPLookupTable& lookpTable, const wxString& sourceFileName)
{
    if(m_expression.empty()) return PHPEntityBase::Ptr_t(NULL);

    m_sourceFile.reset(new PHPSourceFile(m_text, &lookpTable));
    m_sourceFile->SetParseFunctionBody(true);
    m_sourceFile->SetFilename(sourceFileName);
    m_sourceFile->Parse();

    if(m_expression.size() == 1 && m_expression.at(0).type == kPHP_T_NS_SEPARATOR) {
        // user typed '\'
        return lookpTable.FindScope("\\");
    }

    wxString asString = DoSimplifyExpression(0, m_sourceFile);
    wxUnusedVar(asString);

    if(m_parts.empty() && !m_filter.IsEmpty()) {

        // We have no expression, but the user did type something...
        // Return the parent scope of what the user typed so far
        if(m_filter.Contains("\\")) {
            // A namespace separator was found in the filter, break
            // the filter into 2:
            // scope + filter
            wxString scopePart = m_filter.BeforeLast('\\');
            if(!scopePart.StartsWith("\\")) {
                scopePart.Prepend("\\");
            }

            wxString filterPart = m_filter.AfterLast('\\');

            // Adjust the filter
            m_filter.swap(filterPart);

            // Fix the scope part
            scopePart = m_sourceFile->MakeIdentifierAbsolute(scopePart);
            return lookpTable.FindScope(scopePart);

        } else {
            // No namespace separator was typed
            // try to guess:
            // if the m_filter contains "(" -> user wants a global functions
            // else we use the current file scope
            return lookpTable.FindScope(m_sourceFile->Namespace()->GetFullName());
        }
    }

    // Now, use the lookup table
    std::list<PHPExpression::Part>::iterator iter = m_parts.begin();
    PHPEntityBase::Ptr_t currentToken(NULL);
    PHPEntityBase::Ptr_t parentToken(NULL);
    for(; iter != m_parts.end(); ++iter) {
        Part& part = *iter;
        if(!currentToken) {
            // first token
            // Check locks first
            if(part.m_text.StartsWith("$") && m_sourceFile->CurrentScope()) {
                // a variable
                currentToken = m_sourceFile->CurrentScope()->FindChild(part.m_text);
            }
            if(!currentToken) {
                currentToken = lookpTable.FindScope(part.m_text);
                if(!currentToken) {
                    // If we are inside a namespace, try prepending the namespace
                    // to the first token
                    if(m_sourceFile->Namespace() && m_sourceFile->Namespace()->GetFullName() != "\\") {
                        // Not the global namespace
                        wxString fullns =
                            PHPEntityNamespace::BuildNamespace(m_sourceFile->Namespace()->GetFullName(), part.m_text);
                        // Check if it exists
                        PHPEntityBase::Ptr_t pGuess = lookpTable.FindScope(fullns);
                        if(pGuess) {
                            currentToken = pGuess;
                            part.m_text = fullns;
                        } else {
                            // Maybe its a global function..
                            currentToken = lookpTable.FindFunction(part.m_text);
                        }
                    } else {
                        // Maybe its a global function..
                        currentToken = lookpTable.FindFunction(part.m_text);
                    }
                }
            }

        } else {
            // load the children of the current token (optionally, filter by the text)
            currentToken = lookpTable.FindMemberOf(currentToken->GetDbId(), part.m_text);
            if(currentToken && currentToken->Is(kEntityTypeFunctionAlias)) {
                // If the member is a function-alias, use the actual function instead
                currentToken = currentToken->Cast<PHPEntityFunctionAlias>()->GetFunc();
            }
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
                    
                    if((actualType == "self" || actualType == "\\self") && parentToken) {
                        // Resolve self to the actual class name
                        actualType = parentToken->GetFullName();
                    }
                    
                } else if(currentToken->Is(kEntityTypeVariable)) {
                    // return the type hint
                    actualType = currentToken->Cast<PHPEntityVariable>()->GetTypeHint();
                }

                wxString fixedpath;
                if(!actualType.IsEmpty() && FixReturnValueNamespace(lookpTable, parentToken, actualType, fixedpath)) {
                    actualType.swap(fixedpath);
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
        parentToken = currentToken;
    }
    return currentToken;
}

wxString PHPExpression::DoSimplifyExpression(int depth, PHPSourceFile::Ptr_t sourceFile)
{
    if(depth > 5 || sourceFile == NULL) {
        // avoid infinite recursion, by limiting the nest level to 5
        return "";
    }

    // Use the provided sourceFile if 'm_sourceFile' is NULL
    if(!m_sourceFile) {
        m_sourceFile = sourceFile;
    }

    // Parse the input source file
    PHPEntityBase::Ptr_t scope = sourceFile->CurrentScope();
    const PHPEntityBase* innerClass = sourceFile->Class();

    // Check the first token

    // Phase 1:
    // Loop over the expression and resolve as much as we can here for the first token
    // (mainly, replace $this, self, static, $variable)

    phpLexerToken token;
    wxString newExpr;
    wxString firstToken;
    int firstTokenType = wxNOT_FOUND;
    for(size_t i = 0; i < m_expression.size(); ++i) {
        token = m_expression.at(i);
        if(i == 0) {
            // Perform basic replacements that we can conduct here without the need of the global
            // lookup table
            if(token.type == kPHP_T_PARENT) {
                if(!innerClass) return "";
                firstToken = innerClass->GetFullName();
                firstTokenType = kPHP_T_PARENT;

            } else if(token.type == kPHP_T_THIS) {
                // the first token is $this
                // replace it with the current class absolute path
                if(!innerClass) return "";
                firstToken = innerClass->GetFullName(); // Is always in absolute path

            } else if(token.type == kPHP_T_SELF) {
                // Same as $this: replace it with the current class absolute path
                if(!innerClass) return "";
                firstToken = innerClass->GetFullName(); // Is always in absolute path
                firstTokenType = kPHP_T_SELF;

            } else if(token.type == kPHP_T_STATIC) {
                // Same as $this: replace it with the current class absolute path
                if(!innerClass) return "";
                firstToken = innerClass->GetFullName(); // Is always in absolute path
                firstTokenType = kPHP_T_STATIC;

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
                PHPEntityBase::Ptr_t local = scope->FindChild(token.Text());
                if(local && local->Cast<PHPEntityVariable>()) {
                    if(!local->Cast<PHPEntityVariable>()->GetTypeHint().IsEmpty()) {
                        // we have type hint! - use it
                        firstToken = local->Cast<PHPEntityVariable>()->GetTypeHint();

                    } else if(!local->Cast<PHPEntityVariable>()->GetExpressionHint().IsEmpty()) {
                        // we have an expression hint - use it
                        // append the "->" to the expression to make sure that the parser will understand it
                        // as an expression
                        PHPExpression e(m_text, local->Cast<PHPEntityVariable>()->GetExpressionHint() + "->");
                        firstToken = e.DoSimplifyExpression(depth + 1, m_sourceFile);
                        if(firstToken.EndsWith("->")) {
                            // remove the last 2 characters
                            firstToken.RemoveLast(2);
                        }
                    }
                } else {
                    // this local variable does not exist in the current scope
                    // This is probably a word-completion for local variable
                    m_filter = token.Text();
                    return "";
                }

            } else if(token.type == kPHP_T_IDENTIFIER) {
                // an identifier, convert it to the fullpath
                firstToken = sourceFile->MakeIdentifierAbsolute(token.Text());
            }
        }

        if(!firstToken.IsEmpty()) {
            newExpr = firstToken;
            firstToken.Clear();
        } else {
            newExpr << " " << token.Text();
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
                    part.m_text = sourceFile->MakeIdentifierAbsolute(currentText);
                } else {
                    part.m_text = currentText;
                }
            }

            if(m_parts.empty()) {
                // If the first token before the simplication was 'parent'
                // keyword, we need to carry this over
                part.m_textType = firstTokenType;
            }

            part.m_operator = token.type;
            part.m_operatorText = token.Text();
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
            currentText << token.Text();
            break;
        default:
            currentText << token.Text();
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
        expr << m_expression.at(i).Text();
    }
    return expr;
}

size_t PHPExpression::GetLookupFlags() const
{
    size_t flags(0);
    if(m_parts.empty()) return flags;

    if(m_parts.size() == 1 && m_parts.back().m_textType == kPHP_T_PARENT) {
        Part firstPart = m_parts.back();
        if(firstPart.m_textType == kPHP_T_PARENT) {
            flags |= PHPLookupTable::kLookupFlags_Parent;
        }
    } else {
        Part lastExpressionPart = m_parts.back();
        if(lastExpressionPart.m_operator == kPHP_T_PAAMAYIM_NEKUDOTAYIM) {
            if(lastExpressionPart.m_textType == kPHP_T_SELF)
                flags |= PHPLookupTable::kLookupFlags_Self;
            else
                flags |= PHPLookupTable::kLookupFlags_Static;
        }
    }
    return flags;
}

void PHPExpression::Suggest(PHPEntityBase::Ptr_t resolved, PHPLookupTable& lookup, PHPEntityBase::List_t& matches)
{
    // sanity
    if(!resolved) return;
    PHPEntityBase::Ptr_t currentScope = GetSourceFile()->CurrentScope();

    // GetCount() == 0 && !GetFilter().IsEmpty() i.e. a word completion is required.
    // We enhance the list with the following:
    // - PHP keywords
    // - Global functions
    // - Global constants
    // - Function arguments
    // - Local variables (of the current scope)
    // - And aliases e.g. 'use foo\bar as Bar;'
    if(GetCount() == 0 && !GetFilter().IsEmpty()) {

        // For functions and constants, PHP will fall back to global functions or constants if a
        // namespaced function or constant does not exist.
        PHPEntityBase::List_t globals =
            lookup.FindGlobalFunctionAndConsts(PHPLookupTable::kLookupFlags_Contains, GetFilter());
        matches.insert(matches.end(), globals.begin(), globals.end());

        if(currentScope && (currentScope->Is(kEntityTypeFunction) || currentScope->Is(kEntityTypeNamespace))) {
            // If the current scope is a function
            // add the local variables + function arguments to the current list of matches
            const PHPEntityBase::List_t& children = currentScope->GetChildren();
            PHPEntityBase::List_t::const_iterator iter = children.begin();
            for(; iter != children.end(); ++iter) {
                PHPEntityBase::Ptr_t child = *iter;
                if(child->Is(kEntityTypeVariable) && child->GetShortName().Contains(GetFilter()) &&
                   child->GetShortName() != GetFilter()) {
                    matches.push_back(child);
                }
            }
        }

        {
            // Add aliases
            PHPEntityBase::List_t aliases = GetSourceFile()->GetAliases();
            PHPEntityBase::List_t::iterator iter = aliases.begin();
            for(; iter != aliases.end(); ++iter) {
                if((*iter)->GetShortName().Contains(GetFilter())) {
                    matches.push_back(*iter);
                }
            }
        }

        {
            // Add $this incase we are inside a class (but only if '$this' contains the filter string)
            wxString lcFilter = GetFilter().Lower();
            if(GetSourceFile()->Class() && wxString("$this").Contains(lcFilter)) {
                PHPEntityBase::Ptr_t thiz(new PHPEntityVariable());
                thiz->SetFullName("$this");
                thiz->SetShortName("$this");
                thiz->SetFilename(currentScope->GetFilename());
                matches.push_back(thiz);
            }
        }
    }

    // Add the scoped matches
    // for the code completion
    size_t flags = PHPLookupTable::kLookupFlags_Contains | GetLookupFlags();
    if(resolved->Is(kEntityTypeClass)) {
        if(resolved->Cast<PHPEntityClass>()->IsInterface() || resolved->Cast<PHPEntityClass>()->IsAbstractClass()) {
            flags |= PHPLookupTable::kLookupFlags_IncludeAbstractMethods;
        }
    }
    
    PHPEntityBase::List_t scopeChildren = lookup.FindChildren(resolved->GetDbId(), flags, GetFilter());
    matches.insert(matches.end(), scopeChildren.begin(), scopeChildren.end());

    // Incase the resolved is a namespace, suggest all children namespaces
    if(resolved->Is(kEntityTypeNamespace)) {
        PHPEntityBase::List_t namespaces = lookup.FindNamespaces(resolved->GetFullName(), GetFilter());
        matches.insert(matches.end(), namespaces.begin(), namespaces.end());
    }

    // and make the list unique
    DoMakeUnique(matches);
}

void PHPExpression::DoMakeUnique(PHPEntityBase::List_t& matches)
{
    std::set<wxString> uniqueNames;
    PHPEntityBase::List_t uniqueList;
    for(PHPEntityBase::List_t::iterator iter = matches.begin(); iter != matches.end(); ++iter) {
        if(uniqueNames.count((*iter)->GetFullName()) == 0) {
            uniqueNames.insert((*iter)->GetFullName());
            uniqueList.push_back(*iter);
        }
    }
    matches.swap(uniqueList);
}

bool PHPExpression::FixReturnValueNamespace(PHPLookupTable& lookup,
                                            PHPEntityBase::Ptr_t parent,
                                            const wxString& classFullpath,
                                            wxString& fixedpath)
{
    if(!parent) return false;
    PHPEntityBase::Ptr_t pClass = lookup.FindClass(classFullpath);
    if(!pClass) {
        // classFullpath does not exist
        // prepend the parent namespace to its path and check again
        wxString parentNamespace = parent->GetFullName().BeforeLast('\\');
        wxString returnValueNamespace = classFullpath.BeforeLast('\\');
        wxString returnValueName = classFullpath.AfterLast('\\');
        wxString newType = PHPEntityNamespace::BuildNamespace(parentNamespace, returnValueNamespace);
        newType << "\\" << returnValueName;
        pClass = lookup.FindClass(newType);
        if(pClass) {
            fixedpath = newType;
            return true;
        }
    }
    return false;
}
