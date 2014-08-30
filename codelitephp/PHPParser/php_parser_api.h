#ifndef __PHP_PARSER_API_H__
#define __PHP_PARSER_API_H__

#include "php_parser_constants.h"
#include "php_entry.h"
#include <wx/filename.h>
#include "token.h"
#include <map>
#include <macros.h>
#include <wx/thread.h>
#include <stack>

class PHPGlobals
{
    wxString              php_heredoc_label;
    wxString              m_yytext;
    int                   m_yytype;
    int                   m_lineNumber;
    wxString              m_docComment;
    int                   m_docCommentLine;
    std::vector<PHPEntry*>  m_classes;        // classes
    //PHPEntry              currentEntry;   // temporary holds the current entry (function/member)
    PHPEntry::Stack_t     m_entriesStack;
    PHPEntry            * globalClass;
    PHPEntry            * currentClass;
    PHPEntry              m_anonymousFunction;
    wxString              m_currentNameSpace;
    std::vector<PHPEntry> curchain;
    PHPEntry              curChainEntry;
    wxString              currentFileName;
    bool                  parseOutlineOnly;
    wxString              includeFile;    // Holds the result for the include statement parsing
    wxStringMap_t         m_useTable;
    wxCriticalSection     m_cs;
    PHPEntry::List_t      m_namespaceVisitedList;
    wxStringSet_t         m_namespaceVisited;

public:
    enum {
        Type_Normal,
        Type_Expr
    };
    typedef std::map<wxString, PHPEntry> EntryMap_t;

protected:
    // Internal parsing states
    enum ScanReturnState {
        State_Waiting_For_Return = 0,
        State_Found_Return,
        State_Found_New,
        State_Consume_All,
        State_Found_This
    };

    enum ScanLocalState {
        State_Local_Waiting_For_Variable = 0,
        State_Local_Found_Variable,
        State_Local_Found_Equal,
        State_Local_Found_New
    };

protected:
    void DoParseChain();
    bool checkForLocal(int ch, ScanLocalState &curstate, PHPEntry &local);
    Token* doMakeTokenList(const wxString &expr, const PHPGlobals::EntryMap_t& locals, int &retryCount, wxString &new_expr);

public:
    PHPGlobals();
    ~PHPGlobals();

    Token* makeTokenList(const wxString &expr, const PHPGlobals::EntryMap_t& locals);
    
    /**
     * @brief try to find a match for 'name' in the 'use' table
     */
    wxString expandUseStatement(const wxString &name) const;
    const wxStringMap_t& getUseTable() const {
        return m_useTable;
    }
    const PHPEntry::List_t& getNamespaceVisitedList() const {
        return m_namespaceVisitedList;
    }

    PHPEntry::List_t& getNamespaceVisitedList() {
        return m_namespaceVisitedList;
    }

    const wxStringSet_t& getNamespaceVisited() const {
        return m_namespaceVisited;
    }

    void setIncludeFile(const wxString& includeFile) {
        this->includeFile = includeFile;
    }
    const wxString& getIncludeFile() const {
        return includeFile;
    }
    /**
     * @brief call this method to parse from teh current location
     * and search for T_RETURN expr;
     * the return statement is them kept in the PHPEntry.type member
     */
    void skipMethodBodyKeepReturnStatement(wxString &type, wxString &typeScope);

    wxFileName parseIncludeStatement(const wxString &str, const wxString &filename);
    void setParseOutlineOnly(bool parseOutlineOnly) {
        this->parseOutlineOnly = parseOutlineOnly;
    }
    bool getParseOutlineOnly() const {
        return /*parseOutlineOnly*/ false;
    }
    void setCurrentFileName(const wxString& currentFileName) {
        this->currentFileName = currentFileName;
    }
    const wxString& getCurrentFileName() const {
        return currentFileName;
    }
    PHPEntry& getCurChainEntry() {
        return curChainEntry;
    }

    std::vector<PHPEntry>& getCurchain() {
        return curchain;
    }

    void resetChain() {
        getCurChainEntry().reset();
        curchain.clear();
    }
    void printChain();
    /**
     * @brief parse a file
     * @param filename
     * @param type
     * @param outlineOnly set to true if only the file layout is required (i.e. method body is skipped)
     * @return false if the parser is locked (busy)
     */
    bool       parseFile(bool lock, const wxString &filename, int type, bool outlineOnly = false);
    bool       parseString(bool lock, const wxString &str, int type, bool outlineOnly = false, const wxString &filename = wxEmptyString);
    /**
     * @brief manually request for parser locking
     * useful when we want to perform a batch operation
     */
    void lockParser();
    /**
     * @brief manually unlock the parser
     */
    void unlockParser();

    wxCriticalSection& GetLocker() {
        return m_cs;
    }

    void       newLocal(const wxString& name, const wxString &type = wxT(""), PHPModifier attr = PHP_Attr_None);

    void clearDocComment() {
        m_docComment.Clear();
        m_docCommentLine = -1;
    }

    void setDocCommentLine(int docCommentLine) {
        this->m_docCommentLine = docCommentLine;
    }
    int getDocCommentLine() const {
        return m_docCommentLine;
    }
    void setLineNumber(int lineNumber) {
        this->m_lineNumber = lineNumber;
    }
    int getLineNumber() const {
        return m_lineNumber;
    }

    /** namespace management **/
    void setNameSpace(const wxString &ns) ;

    void addUseStatement( const wxString &namespaceName );
    void addUseStatement( const wxString &namespaceName, const wxString &asName );

    const wxString &getCurrentNamespace() const {
        return m_currentNameSpace;
    }

    void clearNamespace() {
        m_currentNameSpace.Clear();
    }

    /**
     * @brief print the content to the stdout
     */
    void print();

    /**
     * @brief add new class to the class vector
     */
    void newClass(const wxString &name) ;

    /**
     * @brief point the current class back to the global class scope
     */
    void setGlobalScope() {
        currentClass = globalClass;
    }

    /**
     * @brief add the curretly active entry to the current class
     */
    void addCurrentEntryToClass() ;
    
    /**
     * @brief push an annonymous function to the stack
     */
    void pushAnonymousFunction();

    /**
     * @brief push an annonymous function to the stack
     */
    void popAnonymousFunction();
    
    /**
     * @brief return writeable reference to the current PHP entry
     */
    PHPEntry& getCurrentEntry() {
        // there is always at least one entry in the stack
        return this->m_entriesStack.top();
    }

    const PHPEntry& getCurrentEntry() const {
        // there is always at least one entry in the stack
        return this->m_entriesStack.top();
    }
    
    /**
     * @brief return the currently active class
     * @return this function always return a valid pointer
     */
    PHPEntry* getCurrentClass() {
        return currentClass;
    }
    const PHPEntry* getCurrentClass() const {
        return currentClass;
    }
    void reset();
    void setPhpHeredocLabel(const wxString& php_heredoc_label) {
        this->php_heredoc_label = php_heredoc_label.Mid(3); // Skip the '<<<'
        this->php_heredoc_label.Trim().Trim(false);
    }
    const wxString& getPhpHeredocLabel() const {
        return php_heredoc_label;
    }
    void setYytext(const wxString& yytext) {
        this->m_yytext = yytext.c_str();
    }
    const wxString& getYytext() const {
        return m_yytext;
    }
    void setYytype(int yytype) {
        this->m_yytype = yytype;
    }
    int getYytype() const {
        return m_yytype;
    }

    void assignDocComment();

    void setDocComment(const wxString& docComment) {
        this->m_docComment = docComment;
    }
    wxString& getDocComment() {
        return m_docComment;
    }
    void appendDocComment(const wxString &s) {
        m_docComment << s;
    }
    void appendDocComment(wxChar c) {
        if( ((!m_docComment.IsEmpty() && m_docComment.Last() == wxT('\n')) || // Not empty and the last char is a newline OR
             (m_docComment.IsEmpty()))                                     && // Or it is empty...
            (c == wxT(' ') || c == wxT('\t') || c == wxT('*') )) {            // AND the new char is a whitespace or a star
            // Dont add it
            // We want our document comment unstripped from any whitespace and stars
        } else {
            m_docComment << c;

        }
    }

    /////////////////////////////////////////////////////////////////
    // Public API
    /////////////////////////////////////////////////////////////////

    /**
     * @brief return the name of the current scope (at the end of the file)
     */
    wxString getCurrentScope() const;

    /**
     * @brief return the current local variables
     */
    PHPGlobals::EntryMap_t getLocals() const;
    /**
     * @brief return the name of the function at the end of the file
     */
    wxString getCurrentFunction() const;
    /**
     * @brief return a map for the global variables
     */
    PHPGlobals::EntryMap_t getGlobalVariables() const;
    /**
     * @brief return a map for the global variables
     */
    PHPGlobals::EntryMap_t getGlobalConstants() const;
    /**
     * @brief return a map for the global functions
     */
    PHPGlobals::EntryMap_t getGlobalFunctions() const;
    /**
     * @brief return a unified map for the global variables
     * and the global functions
     */
    PHPGlobals::EntryMap_t getGlobals() const;
    /**
     * @brief return a vector for all the classes found in the current file
     */
    const std::vector<PHPEntry*>& getClasses() const {
        return m_classes;
    }
    /** non const version of the getClasses() **/
    std::vector<PHPEntry*>& getClasses() {
        return m_classes;
    }
};

/// Ascending sorting function
struct PHPEntrySorter {
    bool operator()(const PHPEntry &item1, const PHPEntry &item2) {
        return item2.getName().Cmp(item1.getName()) > 0;
    }
};

struct PHPEntrySorterP {
    bool operator()(const PHPEntry *item1, const PHPEntry *item2) {
        return item2->getName().Cmp(item1->getName()) > 0;
    }
};

// externals

extern PHPGlobals PHPParser;
extern "C" int PHP_Scan( const wxString &filePath);
extern "C" int PHP_Lex( const wxString &filePath);
extern "C" int PHP_ScanString( const wxString &inputString );
extern "C" int PHP_Expr_ParseIncludeStatement( const wxString &inputString );
extern "C" int PHP_Expr_ParseIncludeStatementFile( const wxString &filepath );
extern "C" void PHP_Expr_SetInputFile(const wxString &filepath);
extern "C" void PHP_Expr_SetInputString(const wxString &inputString);
extern "C" int PHP_Expr_Cleanup();
extern "C" int php_expr_lex();
extern "C" void php_expr_less(int count);
extern "C" wxString php_expr_lval;

#endif // __PHP_PARSER_API_H__
