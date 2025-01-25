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

#include "Cxx/CxxVariable.h"
#include "Cxx/cpp_scanner.h"
#include "codelite_exports.h"
#include "database/entry.h"
#include "function.h"
#include "macros.h"
#include "variable.h"

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

    void SetTemplateInstantiation(const wxString& templateInstantiation);

    void SetTemplateDeclaration(const wxArrayString& templateDeclaration)
    {
        this->templateDeclaration = templateDeclaration;
    }

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
    wxString m_expression;
    TagsManager* m_tm;
    CxxVariable::Map_t m_locals;

public:

    /**
     * @brief given fileContent, locate the best line to place a class forward declaration
     * statement
     */
    int GetBestLineForForwardDecl(const wxString& fileContent) const;

    /**
     * @brief set the tags manager to be used by this language instance
     * @param tm
     */
    void SetTagsManager(TagsManager* tm);

    /**
     * @brief return the available tags manager
     * @return
     */
    TagsManager* GetTagsManager();

    //==========================================================
    // New API based on the yacc grammar files
    //==========================================================

    /**
     * return scope name from given input string
     * @param in input string
     * @return scope name or empty string
     */
    wxString GetScopeName(const wxString& in);

    /**
     * Collect local variables from given scope text (in (a function signature))
     * @param in scope to search for
     * @return since we dont have full information about each token,
     * all local variables returned are of type 'variable' with public access
     */
    std::vector<TagEntryPtr> GetLocalVariables(const wxString& in);

    bool VariableFromPattern(const wxString& pattern, const wxString& name, Variable& var);

    /**
     * @brief insert functionBody into clsname. This function will search for best location
     * to place the function body. set visibility to 0 for 'public' function, 1 for 'protected' and 2 for private
     * return true if this function succeeded, false otherwise
     */
    bool InsertFunctionDecl(const wxString& clsname, const wxString& functionDecl, wxString& sourceContent,
                            int visibility = 0);

private:
    /**
     * Private constructor
     */
    Language();

    /**
     * Private destructor
     */
    virtual ~Language();

    void ParseTemplateArgs(const wxString& argListStr, wxArrayString& argsList);
    void ParseTemplateInitList(const wxString& argListStr, wxArrayString& argsList);
    wxArrayString DoExtractTemplateDeclarationArgs(TagEntryPtr tag);

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
