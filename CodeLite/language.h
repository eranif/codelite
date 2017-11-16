//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : language.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#ifndef CODELITE_LANGUAGE_H
#define CODELITE_LANGUAGE_H

#include "CxxTokenizer.h"
#include "CxxVariable.h"
#include "codelite_exports.h"
#include "comment.h"
#include "cpp_scanner.h"
#include "entry.h"
#include "expression_result.h"
#include "function.h"
#include "macros.h"
#include "parsedtoken.h"
#include "singleton.h"
#include "tokenizer.h"
#include "variable.h"
#include "y.tab.h"
#include <set>
#include <vector>
#include <wx/filename.h>

enum SearchFlags {
    PartialMatch = 0x00000001,
    ExactMatch = 0x00000002,
    IgnoreCaseSensitive = 0x00000004,
    ReplaceTokens = 0x00000008,
};

class TagsManager;

class TemplateHelper
{
    std::vector<wxArrayString> templateInstantiationVector;
    wxArrayString templateDeclaration;
    wxString typeScope;
    wxString typeName;

public:
    TemplateHelper() {}
    ~TemplateHelper() {}

    void SetTemplateDeclaration(const wxString& templateDeclaration);
    void SetTemplateInstantiation(const wxString& tempalteInstantiation);

    void SetTemplateDeclaration(const wxArrayString& templateDeclaration)
    {
        this->templateDeclaration = templateDeclaration;
    }
    void SetTemplateInstantiation(const wxArrayString& templateInstantiation);

    const wxArrayString& GetTemplateDeclaration() const { return templateDeclaration; }
    bool IsTemplate() const { return templateDeclaration.IsEmpty() == false; }

    void SetTypeScope(const wxString& typeScope) { this->typeScope = typeScope; }
    const wxString& GetTypeScope() const { return typeScope; }
    void SetTypeName(const wxString& typeName) { this->typeName = typeName; }
    const wxString& GetTypeName() const { return typeName; }
    wxString Substitute(const wxString& name);

    void Clear();

    wxString GetPath() const;
};

class WXDLLIMPEXP_CL Language
{
    friend class LanguageST;
    friend class TagEntry;
    friend class TemplateHelper;
    friend class TagsManager;

private:
    std::map<char, char> m_braces;
    std::vector<wxString> m_delimArr;
    wxString m_expression;
    CppScannerPtr m_scanner;
    CxxTokenizer m_tokenScanner;
    TagsManager* m_tm;
    wxString m_visibleScope;
    wxString m_lastFunctionSignature;
    std::vector<wxString> m_additionalScopes;                           // collected by parsing 'using namespace XXX'
    std::map<wxString, std::vector<wxString> > m_additionalScopesCache; // collected by parsing 'using namespace XXX'
    TemplateHelper m_templateHelper;
    std::set<wxString> m_templateArgs;
    CxxVariable::Map_t m_locals;

protected:
    void SetVisibleScope(const wxString& visibleScope) { this->m_visibleScope = visibleScope; }
    const wxString& GetVisibleScope() const { return m_visibleScope; }
    CxxVariable::Ptr_t FindLocalVariable(const wxString& name);
    CxxVariable::Ptr_t FindVariableInScope(const wxString& scope, const wxString& name);

public:
    /**
     * Return the visible scope of syntax starting at depth zero.
     *
     * For example, given the following srcString:
     * \code
     * int main()
     * {
     *		{
     *			int i =0;
     *		}
     * }
     * \endcode
     *
     * the output will be:
     * \code
     * int main()
     * {
     *   ;
     * }
     * \code
     *
     * \note The scopes with depth greater then 0 will be removed.
     * This function is useful for searching for local members.
     * \param srcString Input string
     * \return visible scope
     */
    wxString OptimizeScope(const wxString& srcString, int lastFuncLine, wxString& localsScope);

    /**
     * @brief given fileContent, locate the best line to place a class forward declaration
     * statement
     */
    int GetBestLineForForwardDecl(const wxString& fileContent) const;

    void CheckForTemplateAndTypedef(ParsedToken* token);

    void SetLastFunctionSignature(const wxString& lastFunctionSignature)
    {
        this->m_lastFunctionSignature = lastFunctionSignature;
    }
    const wxString& GetLastFunctionSignature() const { return m_lastFunctionSignature; }

    void SetAdditionalScopes(const std::vector<wxString>& additionalScopes, const wxString& filename);

    /**
     * @brief update the 'additional scopes' cache with list of scopes
     * @param filename
     * @param additionalScopes
     */
    void UpdateAdditionalScopesCache(const wxString& filename, const std::vector<wxString>& additionalScopes);
    /**
     * @brief clear the additional scopes cache
     */
    void ClearAdditionalScopesCache();

    const std::vector<wxString>& GetAdditionalScopes() const;
    /**
     * Set the language specific auto completion delimeteres, for example: for C++ you should populate
     * the array with { . , -> , :: }
     * \param delimArr delimeter array
     */
    void SetAutoCompDeliemters(const std::vector<wxString>& delimArr);

    /**
     * \brief set the tags manager to be used by this language instance
     * \param tm
     */
    void SetTagsManager(TagsManager* tm);

    /**
     * \brief return the available tags manager
     * \return
     */
    TagsManager* GetTagsManager();

    /**
     * Parse comments from source file
     * \param name file name
     * \param comments [output] returned vector of comments
     */
    void ParseComments(const wxFileName& fileName, std::vector<CommentPtr>* comments);

    //==========================================================
    // New API based on the yacc grammar files
    //==========================================================

    /**
     * Evaluate a C++ expression. for example, the following expression: '((Notebook*)book)->'
     * will be processed into typeName=Notebook, and typeScope=<global> (assuming Notebook is not
     * placed within any namespace)
     * \param stmt c++ expression
     * \param text text where this expression was found
     * \param fn filename context
     * \param lineno current line number
     * \param typeName [output]
     * \param typeScope [output]
     * \param oper [output] return the operator used (::, ., ->)
     * \param scopeTemplateInitList [output] return the scope tempalte intialization (e.g. "std::auto_ptr<wxString>
     * str;" -> <wxString>
     * \return true on success, false otherwise. The output fields are only to be checked with the return
     * valus is 'true'
     */
    bool ProcessExpression(const wxString& stmt, const wxString& text, const wxFileName& fn, int lineno,
                           wxString& typeName, wxString& typeScope, wxString& oper, wxString& scopeTemplateInitList);

    /**
     * return scope name from given input string
     * \param in input string
     * \return scope name or empty string
     */
    wxString GetScopeName(const wxString& in, std::vector<wxString>* additionlNS);

    /**
     * parse an expression and return the result. this functions uses
     * the sqlite database as its symbol table
     * \param in input string expression
     * \return ExpressionResult, if it fails to parse it, check result.m_name.empty() for success
     */
    ExpressionResult ParseExpression(const wxString& in);

    /**
     * @brief process a token and resolve it into typeName / typeScope
     * @param token
     * @return
     */
    bool ProcessToken(TokenContainer* tokeContainer);

    /**
     * Collect local variables from given scope text (in) and an optional symbol name
     * \param in scope to search for
     * \param tags output, since we dont have full information about each token, all local variables returned are of
     *type
     *			   'variable' with public access
     * \param name optional name to look for (name can be partial).
     */
    void GetLocalVariables(const wxString& in, std::vector<TagEntryPtr>& tags, bool isFuncSignature,
                           const wxString& name = wxEmptyString, size_t flag = PartialMatch);

    wxString ApplyCtagsReplacementTokens(const wxString& in);

    bool VariableFromPattern(const wxString& pattern, const wxString& name, Variable& var);
    bool FunctionFromPattern(TagEntryPtr tag, clFunction& foo);

    /**
     * @brief insert functionBody into clsname. This function will search for best location
     * to place the function body. set visibility to 0 for 'pubilc' function, 1 for 'protected' and 2 for private
     * return true if this function succeeded, false otherwise
     */
    bool InsertFunctionDecl(const wxString& clsname, const wxString& functionDecl, wxString& sourceContent,
                            int visibility = 0);

    /**
     * @brief insert functionBody into clsname. This function will search for best location
     * to place the function body
     */
    void InsertFunctionImpl(const wxString& clsname, const wxString& functionImpl, const wxString& filename,
                            wxString& sourceContent, int& insertedLine);

private:
    bool DoSearchByNameAndScope(const wxString& name, const wxString& scopeName, std::vector<TagEntryPtr>& tags,
                                wxString& type, wxString& typeScope, bool testGlobalScope = true);

    bool CorrectUsingNamespace(wxString& type, wxString& typeScope, const wxString& parentScope,
                               std::vector<TagEntryPtr>& tags);
    /**
     * Private constructor
     */
    Language();

    /**
     * Private destructor
     */
    virtual ~Language();

    /**
     * Return the next token and the delimiter found, the source string is taken from the
     * m_tokenScanner member of this class
     * @param token next token
     * @param delim delimiter found
     * @param subscriptOperator subscript operator was found
     * @param funcArgList the first argument list passed (e.g. MyCall(a)->, funcArgList will contain (a))
     * @return true if token was found false otherwise
     */
    bool NextToken(wxString& token, wxString& delim, bool& subscriptOperator, wxString& funcArgList);

    /**
     * @brief construct a list of tokens based on an expression and a give scope
     * For example: parsing the expression: variable->GetName().AfterFirst()
     * will yield a list of 3 tokens
     * @param scopeName the current scope name of where the expression was found
     * @return ParsedToken ptr (allocated on the heap. Must be freed by calling to ParsedToke::DeleteTokens( head ) or
     * NULL incase
     * parsing fails
     */
    ParsedToken* ParseTokens(const wxString& scopeName);

    /**
     * @brief Attempt to fix template results
     * @param typeName the type name that was detected by the parser
     * @param typeScope the type scope
     */
    bool OnTemplates(ParsedToken* token);

    /**
     * \brief attempt to expand 'typedef' to their actual value
     */
    bool OnTypedef(ParsedToken* token);
    void DoSimpleTypedef(ParsedToken* token);

    /**
     * @brief expand reference operator (->) overloading
     * @param typeName [input/output] the type name to check for the reference operator overloading
     * @param typeScope [input/output] its scope
     * @return true on success, false otherwise
     */
    bool OnArrowOperatorOverloading(ParsedToken* token);

    /**
     * @brief expand subscript operator
     * @param typeName [input/output] the type name to check for the reference operator overloading
     * @param typeScope [input/output] its scope
     * @return true on success, false otherwise
     */
    bool OnSubscriptOperator(ParsedToken* token);

    /**
     * @brief run the user defined types as they appear in the 'Types' replacement table
     * @param token current token
     */
    bool RunUserTypes(ParsedToken* token, const wxString& entryPath = wxEmptyString);

    void ParseTemplateArgs(const wxString& argListStr, wxArrayString& argsList);
    void ParseTemplateInitList(const wxString& argListStr, wxArrayString& argsList);
    void DoRemoveTempalteInitialization(wxString& str, wxArrayString& tmplInitList);
    void DoResolveTemplateInitializationList(wxArrayString& tmpInitList);
    void DoFixFunctionUsingCtagsReturnValue(clFunction& foo, TagEntryPtr tag);
    void DoReplaceTokens(wxString& inStr, const wxStringTable_t& ignoreTokens);
    wxArrayString DoExtractTemplateDeclarationArgsFromScope();
    wxArrayString DoExtractTemplateDeclarationArgs(ParsedToken* token);
    wxArrayString DoExtractTemplateDeclarationArgs(TagEntryPtr tag);
    void DoExtractTemplateInitListFromInheritance(TagEntryPtr tag, ParsedToken* token);
    void DoExtractTemplateInitListFromInheritance(ParsedToken* token);
    void DoExtractTemplateArgsFromSelf(ParsedToken* token);
    void DoFixTokensFromVariable(TokenContainer* tokeContainer, const wxString& variableDecl);

    // Wrapper methods to help the transition from the [wxString,wxString] API into the new ParsedToken
    // code
    bool DoIsTypeAndScopeExist(ParsedToken* token);
    bool DoCorrectUsingNamespaces(ParsedToken* token, std::vector<TagEntryPtr>& tags);

    /**
     * @brief read the class name. This function assumes that the last token
     * consumed by the scanner is 'class' (type: lexCLASS)
     */
    int DoReadClassName(CppScanner& scanner, wxString& clsname) const;
};

class WXDLLIMPEXP_CL LanguageST
{
public:
    static void Free();
    static Language* Get();
};

#endif // CODELITE_LANGUAGE_H
