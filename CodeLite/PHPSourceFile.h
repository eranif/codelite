//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPSourceFile.h
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

#ifndef PHPPARSER_H
#define PHPPARSER_H

#include "PHPEntityBase.h"
#include "PhpLexerAPI.h"
#include "codelite_exports.h"
#include <vector>
#include <wx/filename.h>

class WXDLLIMPEXP_CL PHPLookupTable;

class WXDLLIMPEXP_CL PHPSourceFile
{
    wxString m_text;
    PHPEntityBase::List_t m_scopes;
    PHPEntityBase::List_t m_defines;
    PHPScanner_t m_scanner;
    wxFileName m_filename;
    std::vector<phpLexerToken> m_lookBackTokens;
    std::vector<phpLexerToken> m_comments;
    bool m_parseFunctionBody;
    int m_depth;
    bool m_reachedEOF;
    // aliases defined by the 'use' operator
    std::map<wxString, wxString> m_aliases;
    PHPSourceFile* m_converter = nullptr;
    PHPLookupTable* m_lookup = nullptr;
    PHPEntityBase::List_t m_allMatchesInorder;

public:
    typedef wxSharedPtr<PHPSourceFile> Ptr_t;

protected:
    /**
     * @brief the lexer just read $something = ...
     * read the "..." and create a variable hint from it
     */
    bool ReadVariableInitialization(PHPEntityBase::Ptr_t var);

    /**
     * @brief calls phpLexerNextToken. Use this call instead of the global phpLexerNextToken
     * since this function will handle all PHP comments found
     */
    bool NextToken(phpLexerToken& token);

    /**
     * @brief return the previous token
     */
    phpLexerToken& GetPreviousToken();

    /**
     * @brief read until we found 'delim' (consume it)
     */
    bool ReadUntilFound(int delim, phpLexerToken& token);

    /**
     * @brief read until we found 'delim1' or delim2 (return which one was found)
     * or wxNOT_FOUND if non found
     */
    int ReadUntilFoundOneOf(int delim1, int delim2, phpLexerToken& token);

    /**
     * @brief read expression from the current position until we hit
     * semi colon
     * @param tokens [output]
     */
    bool ReadExpression(wxString& expression);

    /**
     * @brief unget the token
     */
    void UngetToken(const phpLexerToken& token);

    /**
     * @brief run the lexer until we find 'delim' - consume delim as well
     */
    bool ConsumeUntil(int delim);

    /**
     * @brief read list of identifiers separated by comma until we find 'delim'
     * note that this function does not consume the 'delim' token
     */
    bool ReadCommaSeparatedIdentifiers(int delim, wxArrayString& list);

    /**
     * @brief read the type
     */
    wxString ReadType();

    /**
     * @brief PHP7 style: read the return value
     */
    wxString ReadFunctionReturnValueFromSignature();

    /**
     * @brief read the value that comes after the 'extends' keyword
     */
    wxString ReadExtends();

    /**
     * @brief read the tokens after the implements keyword
     */
    void ReadImplements(wxArrayString& impls);

    /**
     * @brief parse 'use' statements (outer scope, for aliasing)
     */
    void OnUse();

    /**
     * @brief parse 'use' statements inside class (usually this is done for 'traits')
     */
    void OnUseTrait();

    /**
     * @brief we are looking at a case like:
     * use A, B {
     *  B::bigTalk as talk;
     * }
     * parse the inner block and scan for aliases
     */
    void ParseUseTraitsBody();

    /**
     * @brief found 'foreach' statement
     */
    void OnForEach();

    /**
     * @brief 'namespace' keyword found
     */
    void OnNamespace();

    /**
     * @brief 'function' keyword found
     */
    void OnFunction();

    /**
     * @brief found catch keyword
     */
    void OnCatch();

    /**
     * @brief found a global variable
     */
    void OnVariable(const phpLexerToken& token);

    /**
     * @brief 'class' keyword found
     */
    void OnClass(const phpLexerToken& tok);

    /**
     * @brief a define keyword was found
     */
    void OnDefine(const phpLexerToken& tok);

    /**
     * @brief found constant
     */
    void OnConstant(const phpLexerToken& tok);

    /**
     * @brief go over the look back tokens and extract all function flags
     */
    size_t LookBackForFunctionFlags();

    /**
     * @brief go over the look back tokens and extract all variable flags
     */
    size_t LookBackForVariablesFlags();

    /**
     * @brief return true of the look back tokens contains 'type'
     */
    bool LookBackTokensContains(int type) const;
    /**
     * @brief parse function signature
     */
    void ParseFunctionSignature(int startingDepth);

    /**
     * @brief consume the function body
     */
    void ConsumeFunctionBody();

    /**
     * @brief parse function body and collect local variables +
     * possible return statements
     */
    void ParseFunctionBody();

    /**
     * @brief go over the loop back tokens and construct the type hint
     */
    wxString LookBackForTypeHint();

    /**
     * @brief parse phase 2.
     * On this stage, all phpdoc comments are assigned to the proper PHP entity
     */
    void PhaseTwo();

    wxString DoMakeIdentifierAbsolute(const wxString& type, bool exactMatch);

public:
    PHPSourceFile(const wxFileName& filename, PHPLookupTable* lookup);
    PHPSourceFile(const wxString& content, PHPLookupTable* lookup);

    virtual ~PHPSourceFile();

    /**
     * @brief return a serialized list of all entries found in this source file
     * sorted by line number
     */
    const PHPEntityBase::List_t& GetAllMatchesInOrder();

    /**
     * @brief use a different PHPSourceFile class for converting types to their
     * absolute path
     * @param converter
     */
    void SetTypeAbsoluteConverter(PHPSourceFile* converter) { m_converter = converter; }

    /**
     * @brief check if we are inside a PHP block at the end of the given buffer
     */
    static bool IsInPHPSection(const wxString& buffer);

    /**
     * @brief return list of aliases (their short name) that appears on this file
     */
    PHPEntityBase::List_t GetAliases() const;

    /**
     * @brief return list of defines defined in this source file
     */
    const PHPEntityBase::List_t& GetDefines() const { return m_defines; }

    /**
     * @brief attempt to resolve 'type' to its full path
     */
    wxString MakeIdentifierAbsolute(const wxString& type);

    /**
     * @brief attempt to resolve 'type' to its full path
     */
    wxString MakeTypehintAbsolute(const wxString& type);

    /**
     * @brief prepend the current scope to the class name (or trait, interface...)
     */
    wxString PrependCurrentScope(const wxString& className);

    /**
     * @brief return the source file text
     */
    const wxString& GetText() const { return m_text; }

    /**
     * @brief parse the source file
     */
    void Parse(int exitDepth = -1);

    /**
     * @brief return the current scope
     */
    PHPEntityBase::Ptr_t CurrentScope();

    /**
     * @brief return the inner most class of the 'CurrentScope'
     * Note that this function return a const raw pointer to the
     * class
     */
    const PHPEntityBase* Class();

    /**
     * @brief return the namespace scope of this file
     */
    PHPEntityBase::Ptr_t Namespace();

    // Accessors
    void SetFilename(const wxFileName& filename) { this->m_filename = filename; }
    const wxFileName& GetFilename() const { return m_filename; }
    void SetParseFunctionBody(bool parseFunctionBody) { this->m_parseFunctionBody = parseFunctionBody; }
    bool IsParseFunctionBody() const { return m_parseFunctionBody; }
    void PrintStdout();
};

#endif // PHPPARSER_H
