#include "php_parser_api.h"
#include "php_parser.cpp.h"
#include "php_entry.h"
#include "php_helpers.h"
#include <wx/tokenzr.h>

extern int php_lex();
extern int php_less(int);
extern int php_lineno;

wxString GetAttributes(size_t flags)
{
    wxString attr;
    if(flags & PHP_Attr_Static)
        attr << wxT("Static ");
    if(flags & PHP_Attr_Abstract)
        attr << wxT("Abstract ");
    if(flags & PHP_Attr_Final)
        attr << wxT("Final ");
    if(flags & PHP_Access_Protected)
        attr << wxT("Protected ");
    if(flags & PHP_Access_Private)
        attr << wxT("Private ");
    if(flags & PHP_Access_Public)
        attr << wxT("Public ");
    if(flags & PHP_Attr_Const)
        attr << wxT("Const ");
    if(flags & PHP_Attr_Reference)
        attr << wxT("Reference ");
    if(flags & PHP_Attr_Global)
        attr << wxT("Global ");
    return attr;
}

PHPGlobals PHPParser;

PHPGlobals::PHPGlobals()
    : m_docCommentLine(-1)
    , globalClass(new PHPEntry("<global>"))
    , parseOutlineOnly(false)
{
    setGlobalScope();
    m_entriesStack.push( PHPEntry() );
    
}

PHPGlobals::~PHPGlobals()
{
    reset();
    wxDELETE(globalClass);
}

void PHPGlobals::reset()
{
    this->php_heredoc_label.clear();
    this->m_yytext.clear();
    this->m_docComment.clear();
    std::vector<PHPEntry*>::iterator iter = m_classes.begin();
    for(; iter != m_classes.end(); iter++) {
        delete (*iter);
    }
    m_classes.clear();
    while ( !m_entriesStack.empty() ) {
        m_entriesStack.pop();
    }
    m_entriesStack.push( PHPEntry() );
    //currentEntry.reset();
    globalClass->reset();
    globalClass->setName("<global>");
    setGlobalScope();
    setLineNumber(-1);
    m_currentNameSpace.Clear();
    getCurchain().clear();
    getCurChainEntry().reset();
    currentFileName.Clear();
    parseOutlineOnly = false;
    m_useTable.clear();
    m_namespaceVisited.clear();
    m_namespaceVisitedList.clear();
}

void PHPGlobals::print()
{
    wxPrintf(wxT("=== GLOBAL SCOPE ===\n"));
    globalClass->print();
    wxPrintf(wxT("=== GLOBAL SCOPE END ===\n"));

    std::vector<PHPEntry*>::iterator iter = m_classes.begin();
    for(; iter != m_classes.end(); iter++) {
        (*iter)->print();
    }

    wxPrintf(wxT("\n\n=== > SUMMARY < ===\n\n"));

    wxPrintf(wxT("Current scope is: %s\n"), getCurrentScope().c_str());
    wxPrintf(wxT("Current function: %s\n"), getCurrentEntry().getName().c_str());

    // we got an un-terminated scope entry
    if(getCurrentEntry().isOk()) {
        getCurrentEntry().print();
    }
}

wxString PHPGlobals::getCurrentScope() const
{
    wxString curnamespace = getCurrentNamespace();
    const PHPEntry *cls = getCurrentClass();
    if(cls->isGlobalScopeClass()) {
        return curnamespace;

    } else if (cls->isClass()) {
        return wxString() << curnamespace << "\\" << cls->getName();

    } else {
        return "";
    }
}

void PHPGlobals::newLocal(const wxString& name, const wxString& type, PHPModifier attr)
{
    if(getCurrentEntry().getKind() == PHP_Kind_Function) {

        wxString actualType = type;
        if(attr & PHP_Attr_Global) {
            // See if we can locate the type of the variable in the global scope
            PHPGlobals::EntryMap_t globals = getGlobalVariables();
            PHPGlobals::EntryMap_t::iterator iter = globals.find(name);
            if(iter != globals.end()) {
                actualType = iter->second.getType();
            }
        }

        // we are inside a function
        getCurrentEntry().addNewLocal(name, actualType, attr);

    } else if(getCurrentClass()->isGlobalScopeClass()) {
        bool b = type.find_first_not_of(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_\\-$>")) == wxString::npos;
        bool validType = b && type.find_first_of(wxT("0123456789")) != 0; // first char must not start with a number

        PHPEntry local;
        local.setName(name);
        local.setType(validType ? type : wxString());
        local.setKind(PHP_Kind_Variable);
        local.addAttribute(attr);
        local.setLineNumber( getLineNumber());
        local.setFileName( getCurrentFileName());
        local.setNameSpace( getCurrentNamespace() );

        // if the current line number is one line below the
        // collected doc comment, attach the doc comment to this
        // function
        if(getLineNumber() - 1 == getDocCommentLine()) {
            local.setDocComment( getDocComment() );

        } else {
            clearDocComment();

        }

        getCurrentClass()->addEntry(local);

    }
}

bool PHPGlobals::parseFile(bool lock, const wxString& filename, int type, bool outlineOnly)
{
    if ( lock && !m_cs.TryEnter() ) {
        // parser is locked
        return false;
    }
    
    if(type == Type_Normal) {
        reset();
        this->parseOutlineOnly = outlineOnly;
        setCurrentFileName(filename);
        PHP_Scan(filename);
        this->parseOutlineOnly = false; // the default

    } else {
        resetChain();
        PHP_Expr_SetInputFile(filename);
        DoParseChain();
        PHP_Expr_Cleanup();
    }
    
    if ( lock ) {
        m_cs.Leave();
    }
    return true;
}

bool PHPGlobals::parseString(bool lock, const wxString& str, int type, bool outlineOnly, const wxString &filename)
{
    if ( lock ) {
        if (  !m_cs.TryEnter() ) {
            // parser is locked
            return false;
        }
    }
    
    if(type == Type_Normal) {
        reset();

        this->parseOutlineOnly = outlineOnly;
        this->setCurrentFileName(filename);

        PHP_ScanString(str);
        this->parseOutlineOnly = false;

    } else {
        PHPParser.resetChain();
        PHP_Expr_SetInputString(str);
        DoParseChain();
        PHP_Expr_Cleanup();
    }
    if ( lock ) {
        m_cs.Leave();
    }
    return true;
}

// Accessors
PHPGlobals::EntryMap_t PHPGlobals::getLocals() const
{
    static PHPGlobals::EntryMap_t emptyMap;
    if(getCurrentEntry().getKind() == PHP_Kind_Function) {
        return getCurrentEntry().getLocals();

    } else if(getCurrentClass()->isGlobalScopeClass()) {
        const std::vector<PHPEntry> & constEntries = getCurrentClass()->getEntries();
        PHPGlobals::EntryMap_t entries;
        for(size_t i=0; i<constEntries.size(); i++) {
            if(constEntries.at(i).getKind() == PHP_Kind_Variable && entries.find(constEntries.at(i).getName()) == entries.end()) {
                entries[constEntries.at(i).getName()] = constEntries.at(i);
            }
        }
        return entries;
    }
    return emptyMap;
}

wxString PHPGlobals::getCurrentFunction() const
{
    if(getCurrentEntry().getKind() == PHP_Kind_Function) {
        return getCurrentEntry().getName();
    }
    return wxT("");
}

std::map<wxString,PHPEntry> PHPGlobals::getGlobalConstants() const
{
    const std::vector<PHPEntry>& constEntries = globalClass->getEntries();
    PHPGlobals::EntryMap_t entries;
    for(size_t i=0; i<constEntries.size(); i++) {
        if(constEntries.at(i).getKind() == PHP_Kind_Constant && entries.find(constEntries.at(i).getName()) == entries.end()) {
            entries[constEntries.at(i).getName().c_str()] = constEntries.at(i);
        }
    }
    return entries;
}

std::map<wxString,PHPEntry> PHPGlobals::getGlobalVariables() const
{
    const std::vector<PHPEntry>& constEntries = globalClass->getEntries();
    PHPGlobals::EntryMap_t entries;
    for(size_t i=0; i<constEntries.size(); i++) {
        if(constEntries.at(i).getKind() == PHP_Kind_Variable && entries.find(constEntries.at(i).getName()) == entries.end()) {
            entries[constEntries.at(i).getName().c_str()] = constEntries.at(i);
        }
    }
    return entries;
}

void PHPGlobals::printChain()
{
    for(size_t i=0; i<curchain.size(); i++) {
        curchain.at(i).print();
    }
}

std::map<wxString,PHPEntry> PHPGlobals::getGlobalFunctions() const
{
    const std::vector<PHPEntry>& constEntries = globalClass->getEntries();
    PHPGlobals::EntryMap_t entries;
    for(size_t i=0; i<constEntries.size(); i++) {
        if(constEntries.at(i).getKind() == PHP_Kind_Function && entries.find(constEntries.at(i).getName()) == entries.end()) {
            entries[constEntries.at(i).getName().c_str()] = constEntries.at(i);
        }
    }
    return entries;
}

std::map<wxString,PHPEntry> PHPGlobals::getGlobals() const
{
    const std::vector<PHPEntry>& constEntries = globalClass->getEntries();
    PHPGlobals::EntryMap_t entries;
    for(size_t i=0; i<constEntries.size(); i++) {
        if(entries.find(constEntries.at(i).getName()) == entries.end()) {
            entries[constEntries.at(i).getName().c_str()] = constEntries.at(i);
        }
    }
    return entries;
}

// Helper method
static bool skip_method_call_args()
{
    int depth = 1;
    while(depth > 0) {
        int ch = php_expr_lex();
        //printf("ch=%d\n", ch);
        //fflush(stdout);
        if(ch ==0) {
            return false;
        }

        if(ch == ')') {
            depth--;
            if(depth == 0)
                php_expr_less(0);
            continue;

        } else if(ch == '(') {
            depth ++ ;
            continue;

        }
    }
    return true;
}

#define NEXT_TOKEN() type = php_expr_lex(); if(type == 0) break;
#define NEXT_TOKEN_NO_BREAK() type = php_expr_lex();
#define PEEK_TOKEN() type = php_expr_lex(); php_expr_less(0);
#define TOKEN_EOF()  type == 0

#define NEXT_TOKEN_WITH_EOF_CHECK() {\
        PEEK_TOKEN();\
        if(TOKEN_EOF()) {\
            getCurchain().push_back(entry);\
        }\
        NEXT_TOKEN();\
    }

#define TOKEN_IS(x) ((type == x) ? true : false)
#define TOKEN_IS2(x, y) ((type == x || type == y) ? true : false)
void PHPGlobals::DoParseChain()
{
    int type(0);
    PHPEntry entry;
    PEEK_TOKEN();
    if ( TOKEN_IS(T_NS_SEPARATOR) ) {
        NEXT_TOKEN_NO_BREAK(); // consume it
        entry.setKind( PHP_Kind_Namespace );
        entry.setPrefixOperator(T_NS_SEPARATOR);
    }

    while( true ) {
        NEXT_TOKEN();
        if(TOKEN_IS2(T_VARIABLE, T_STRING)) {
            entry.setName(php_expr_lval);
            if(entry.getName() == wxT("$this") || entry.getName() == wxT("self")) {
                entry.setKind(PHP_Kind_This);
            } else {
                entry.setKind(PHP_Kind_Variable);
            }
        } else if ( TOKEN_IS(T_NS_SEPARATOR) ) {
            continue;

        } else {
            // Token must be variable or string
            resetChain();
            break;
        }

        NEXT_TOKEN_WITH_EOF_CHECK();

        if(TOKEN_IS('(')) {
            entry.setKind(PHP_Kind_Function);
            if(!skip_method_call_args()) {
                resetChain();
                break;
            }
            NEXT_TOKEN();
            if(!TOKEN_IS(')')) {
                // parse error
                break;
            }

            NEXT_TOKEN_WITH_EOF_CHECK();
        }

        if(TOKEN_IS2(T_PAAMAYIM_NEKUDOTAYIM, T_OBJECT_OPERATOR)) {
            entry.setTerminatingOperator(type);

        } else if ( TOKEN_IS(T_NS_SEPARATOR) ) {
            entry.setTerminatingOperator(T_NS_SEPARATOR);
            entry.setKind( PHP_Kind_Namespace );

        } else {
            // Parse error
            resetChain();
            break;
        }

        getCurchain().push_back(entry);
        entry.reset();
    }
}

void PHPGlobals::addCurrentEntryToClass()
{
    PHPEntry currentEntry = m_entriesStack.top();
    while ( !m_entriesStack.empty() )  {
        m_entriesStack.pop();
    }
    m_entriesStack.push( PHPEntry() );
    currentEntry.setParent(getCurrentClass()->getName());
    if(currentEntry.isGlobal()) {
        // apply the namespace only to global functions / constant / variables
        currentEntry.setNameSpace(getCurrentNamespace());
    }
    currentEntry.setFileName(PHPParser.getCurrentFileName());
    getCurrentClass()->addEntry(currentEntry);
}

void PHPGlobals::newClass(const wxString& name)
{
    // set the current class pointer to point to the newly added class
    currentClass = new PHPEntry();
    currentClass->setName(name);
    currentClass->setKind(PHP_Kind_Class);
    currentClass->setFileName(PHPParser.getCurrentFileName());
    currentClass->setNameSpace(getCurrentNamespace());
    currentClass->setLineNumber(getLineNumber());
    m_classes.push_back(currentClass);
}

void PHPGlobals::assignDocComment()
{
    // if the current line number is one line below the
    // collected doc comment, attach the doc comment to this
    // function
    if(getLineNumber() - 1 == getDocCommentLine()) {
        getCurrentEntry().setDocComment( PHPParser.getDocComment() );

    } else {
        clearDocComment();

    }
}

bool PHPGlobals::checkForLocal(int ch, ScanLocalState &curstate, PHPEntry &local)
{
    switch(curstate) {
    case State_Local_Waiting_For_Variable:
        switch(ch) {
        case T_VARIABLE:
            local.setName(getYytext());
            local.setLineNumber(getLineNumber());
            curstate = State_Local_Found_Variable;
            return false;
        default:
            return false;
        }
        break;
    case State_Local_Found_Variable:
        switch(ch) {
        case ';':
            return true;
        case '=':
            curstate = State_Local_Found_Equal;
            return false;
        }
        break;
    case State_Local_Found_Equal:
        switch(ch) {
        case T_STRING:
            local.setType(getYytext());
            return true;
        case T_NEW:
            curstate = State_Local_Found_New;
            return false;
        default:
            return false;
        }
        break;
    case State_Local_Found_New:
        switch(ch) {
        case T_STRING:
            local.setType(getYytext());
            return true;
        default:
            return false;
        }
        break;
    }
    return false;
}

void PHPGlobals::skipMethodBodyKeepReturnStatement(wxString &type, wxString &typeScope)
{
    // TODO :: Collect local variables here
    ScanReturnState internalState = State_Waiting_For_Return;
    ScanLocalState  localState    = State_Local_Waiting_For_Variable;
    PHPEntry        localVariable;

    type.Clear();
    typeScope.Clear();
    int depth = 1;
    while(depth > 0) {
        int ch = php_lex();
        if(ch ==0) {
            break;
        }

        switch(ch) {
        case '}':
            depth--;
            if(depth == 0)
                php_less(0);
            break;
        case '{':
            depth++;
            break;
        default:
            break;
        }

        switch(internalState) {
            // While we are "waiting for T_RETURN'
            // we parse local variables as well
        case State_Waiting_For_Return: {
            switch(ch) {
            case T_RETURN:
                internalState = State_Found_Return;
                break;
            default:
                if(checkForLocal(ch, localState, localVariable)) {
                    localState = State_Local_Waiting_For_Variable;
                    getCurrentEntry().addNewLocal(localVariable.getName(), localVariable.getType());
                    localVariable.reset();
                }
                break;
            }
        }
        break;
        case State_Found_Return: {
            switch(ch) {
            case T_STRING: {
                // Returning value of a function
                type = getYytext();
                internalState = State_Consume_All;
            }
            break;
            case T_VARIABLE: {
                if(getYytext() == wxT("$this")) {
                    internalState = State_Found_This;
                    typeScope = getCurrentClass()->getName();

                } else {
                    type = getYytext();

                    // See if we can expand the local variable with its type...
                    PHPGlobals::EntryMap_t locals = getLocals();
                    PHPGlobals::EntryMap_t::iterator iter = locals.find(type);
                    if(iter != locals.end()) {
                        type = iter->second.getType();
                    }

                    internalState = State_Consume_All;
                }
            }
            break;
            case T_NEW:
                internalState = State_Found_New;
                break;
            default:
                break;
            }
        }
        break;

        case State_Consume_All:
            break;

        case State_Found_New: {
            switch(ch) {
            case T_STRING:
                internalState = State_Consume_All;
                type = this->getYytext();
                break;
            default:
                break;
            }
        }
        break;
        case State_Found_This: {
            switch(ch) {
            case T_OBJECT_OPERATOR:
                break;
            case T_STRING: {
                PHPEntry* entry = getCurrentClass()->findChild(getYytext(), PHP_Kind_Function);
                if(entry) {
                    type = entry->getType();
                } else {
                    entry = getCurrentClass()->findChild(getYytext(), PHP_Kind_Member);
                    if(entry) {
                        type = entry->getType();
                    }
                }
                internalState = State_Consume_All;
            }
            break;
            default:
                internalState = State_Consume_All;
                break;
            }
        }
        break;
        }
    }
}

wxFileName PHPGlobals::parseIncludeStatement(const wxString& str, const wxString &filename)
{
    // Prepare the input string
    setIncludeFile(wxT(""));
    wxString inputString = str;
    inputString.Trim().Trim(false);
    if(!inputString.StartsWith(PHP_PREFIX)) {
        inputString.Prepend(PHP_PREFIX_WITH_SPACE);
    }

    setCurrentFileName(filename);
    if ( !parseFile(true, filename, PHPGlobals::Type_Normal, true) ) {
        // parser is busy
        return wxFileName();
    }

    PHPGlobals::EntryMap_t constantsTable = PHPParser.getGlobalConstants();
    PHPGlobals::EntryMap_t::iterator iter = constantsTable.begin();
    for(; iter != constantsTable.end(); iter++) {
        inputString.Replace(iter->first, iter->second.getDefaultValue());
    }

    PHP_Expr_ParseIncludeStatement(inputString);
    PHP_Expr_Cleanup();

    return wxFileName(getIncludeFile());
}

Token* PHPGlobals::makeTokenList(const wxString& expr, const PHPGlobals::EntryMap_t& locals)
{
    wxString mod_expr;
    wxString new_expr = expr;
    int retryCount = 0;

    while ( retryCount < 10 ) {
        Token* t = doMakeTokenList(new_expr, locals, retryCount, mod_expr);
        if ( !t && !mod_expr.IsEmpty() ) {
            new_expr.swap( mod_expr );
            ++retryCount;

        } else {
            // join all namespace parts into a single namespace
            t->shrink();
            return t;
        }
    }
    return NULL;
}

Token* PHPGlobals::doMakeTokenList(const wxString& expr, const PHPGlobals::EntryMap_t& locals, int& retryCount, wxString& new_expr)
{
    new_expr.Clear();
    PHP_Expr_SetInputString(expr);

    int type(0);
    Token* head = new Token();
    head->setExpression( expr );
    Token* cur = head;
    bool firstToken = true;
    while ( true ) {
        NEXT_TOKEN();

        if ( TOKEN_IS(T_NS_SEPARATOR) && firstToken ) {
            firstToken = false;
            // the expression starts with a backslash - this means a full path no, relative namespace
            // don't add new token, continue to work with the currnet one
            cur->setStartsWithNSOperator(true);
            continue;
        }

        firstToken = false;
        wxString token_name = php_expr_lval;
        if ( TOKEN_IS(T_VARIABLE) || TOKEN_IS(T_STRING) || TOKEN_IS(T_STATIC) ) {
            if ( php_expr_lval == "$this" ) {
                // replace the $this with the actual class type
                // but remember that we originated from a "$this"
                cur->setName( getCurrentClass()->getName() );
                cur->setFlags( cur->getFlags() | Token::kThis );

            } else if ( php_expr_lval == "self" ) {
                // replace the self with the actual class type
                // but remember that we originated from a "self"
                cur->setName( getCurrentClass()->getName() );
                cur->setFlags( cur->getFlags() | Token::kSelf );

            } else if ( php_expr_lval == "static" ) {
                // replace the "static" with the actual class type
                // but remember that we originated from a "static"
                cur->setName( getCurrentClass()->getName() );
                cur->setFlags( cur->getFlags() | Token::kStatic );

            } else if ( token_name.StartsWith("$") && cur->isHead() && locals.count(token_name) ) {
                PHPEntry e = locals.find(token_name)->second;
                wxString e_type = e.getType();

                if ( !e_type.IsEmpty() ) {
                    // this token is:
                    // - the first token
                    // - is a local variable
                    // - the local variable has a type
                    // ==> use the type instead of the local name
                    // we need to restart the parsing but this time we use the variable type
                    new_expr = expr;
                    new_expr.Replace(e.getName(), e_type, false);
                    break;

                } else {
                    cur->setName( php_expr_lval );
                }
            } else {
                cur->setName( php_expr_lval );
            }
            continue;

        } else if ( TOKEN_IS(T_PAAMAYIM_NEKUDOTAYIM) ) {
            cur->setOperator(T_PAAMAYIM_NEKUDOTAYIM);
            cur = cur->append();
            continue;

        } else if ( TOKEN_IS(T_OBJECT_OPERATOR) ) {
            cur->setOperator(T_OBJECT_OPERATOR);
            cur = cur->append();
            continue;

        } else if ( TOKEN_IS(T_NS_SEPARATOR) ) {
            cur->setOperator(T_NS_SEPARATOR);
            cur = cur->append();
            continue;

        } else if ( TOKEN_IS('(') ) {
            // consume function body
            int depth = 1;
            while ( true ) {
                type = php_expr_lex();
                if(type == 0) {
                    wxDELETE( head );
                    return NULL;

                } else if ( type == '(' ) {
                    ++depth;

                } else if ( type == ')' ) {
                    --depth;
                    if ( depth == 0 ) {
                        break;
                    }
                }
                // else continue
            }

        } else {
            // end of parsing
            break;
        }
    }

    PHP_Expr_Cleanup();

    if ( !new_expr.IsEmpty() ) {
        wxDELETE(head);
    }
    return head;
}

void PHPGlobals::setNameSpace(const wxString& ns)
{
    m_currentNameSpace = ns;
    m_currentNameSpace.Trim().Trim(false);
    if ( !m_currentNameSpace.IsEmpty() && !m_currentNameSpace.StartsWith("\\") && m_currentNameSpace != "<global>" ) {
        m_currentNameSpace.Prepend("\\");
    }
    
    // Add namespace for each level found
    // For example, if the namespace found is:
    // A\B\C
    // Then we add the following namespaces:
    // \A
    // \A\B
    // \A\C\C
    wxArrayString namespaces = ::wxStringTokenize(m_currentNameSpace, "\\", wxTOKEN_STRTOK);
    wxString curns;
    for(size_t i=0; i<namespaces.GetCount(); ++i) {
        curns << "\\" << namespaces.Item(i);
        if ( m_namespaceVisited.count(curns) == 0 ) {
            // new namespace
            m_namespaceVisited.insert( curns );
            
            wxString namespaceParent, namespaceName;
            PHPEntry::CorrectClassName(curns, "", namespaceName, namespaceParent);
            
            PHPEntry nsEntry;
            nsEntry.setKind(PHP_Kind_Namespace);
            nsEntry.setName(namespaceName);
            nsEntry.setNameSpace(namespaceParent);
            nsEntry.setLineNumber( getLineNumber() );
            nsEntry.setFileName( getCurrentFileName() );
            m_namespaceVisitedList.push_back( nsEntry );
        }
    }
}

void PHPGlobals::addUseStatement(const wxString& namespaceName)
{
    // extract the class name from the fullpath
    wxString out_name, out_ns;
    PHPEntry::CorrectClassName(namespaceName, "", out_name, out_ns);
    out_ns << "\\" << out_name;
    addUseStatement( out_ns, out_name );
}

void PHPGlobals::addUseStatement(const wxString& namespaceName, const wxString& asName)
{
    wxString out_name, out_ns;
    PHPEntry::CorrectClassName( asName, namespaceName, out_name, out_ns );
    m_useTable.insert( std::make_pair(out_name, out_ns) );
}

void PHPGlobals::lockParser()
{
    m_cs.Enter();
}

void PHPGlobals::unlockParser()
{
    m_cs.Leave();
}

wxString PHPGlobals::expandUseStatement(const wxString& name) const
{
    if ( m_useTable.count(name) ) {
        return m_useTable.find(name)->second;
    }
    return name;
}

void PHPGlobals::pushAnonymousFunction()
{
    m_entriesStack.push( PHPEntry() );
    PHPEntry &currentEntry = getCurrentEntry();
    static int anonymousFunctionCount = 0;
    currentEntry.setKind( PHP_Kind_Function );
    currentEntry.setName( wxString() << "<anonymous-function-" << (++anonymousFunctionCount) << ">" );
    currentEntry.setFileName( getCurrentFileName() );
    currentEntry.setLineNumber( getLineNumber() );
}

void PHPGlobals::popAnonymousFunction()
{
    // TODO:: do we want to keep this information?
    
    // Make sure we don't pop the last entry
    if ( m_entriesStack.size() > 1 ) {
        m_entriesStack.pop();
    }
}
