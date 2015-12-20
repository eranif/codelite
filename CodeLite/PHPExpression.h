//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPExpression.h
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

#ifndef PHPEXPRESSION_H
#define PHPEXPRESSION_H

#include "codelite_exports.h"
#include <wx/string.h>
#include "PhpLexerAPI.h"
#include <list>
#include "PHPSourceFile.h"
#include "PHPLookupTable.h"

class WXDLLIMPEXP_CL PHPExpression
{
public:
    enum eType {
        kNone = -1,
        kThis,
        kSelf,
        kStatic,
    };
    struct Part {
        wxString m_text;
        int m_operator;
        int m_textType;
        wxString m_operatorText;

        Part()
            : m_operator(kPHP_T_OBJECT_OPERATOR)
            , m_textType(wxNOT_FOUND)
        {
        }
    };
    typedef std::list<PHPExpression::Part> List_t;
    typedef wxSharedPtr<PHPExpression> Ptr_t;

protected:
    eType m_type;
    wxString m_text;
    phpLexerToken::Vet_t m_expression;
    PHPExpression::List_t m_parts;
    wxString m_filter; // Part of the word that was typed by the user
                       // but will do no good to resolve the expression
    PHPSourceFile::Ptr_t m_sourceFile;
    bool m_functionCalltipExpr;
    bool m_exprStartsWithOpenTag;

protected:
    wxString DoSimplifyExpression(int depth, PHPSourceFile::Ptr_t sourceFile);
    /**
     * @brief make 'matches' a unique list by removing duplicates
     */
    void DoMakeUnique(PHPEntityBase::List_t& matches);

protected:
    phpLexerToken::Vet_t CreateExpression(const wxString& text);
    /**
     * @brief fix the return value full path
     */
    bool FixReturnValueNamespace(
        PHPLookupTable& lookup, PHPEntityBase::Ptr_t parent, const wxString& classFullpath, wxString& fixedpath);

public:
    PHPExpression(const wxString& fulltext, const wxString& exprText = wxString(), bool functionCalltipExpr = false);
    virtual ~PHPExpression();

    const phpLexerToken::Vet_t& GetExpression() const { return m_expression; }
    /**
     * @brief return the parse expression as string. Useful for debuggin purposes
     */
    wxString GetExpressionAsString() const;

    /**
     * @brief suggest matches for this expression.
     * This function must be called after a successfull call to 'Resolve'
     * @param resolved the resolved object from the previous call to 'Resolve'
     * @param matches [output]
     */
    void Suggest(PHPEntityBase::Ptr_t resolved, PHPLookupTable& lookup, PHPEntityBase::List_t& matches);
    
    /**
     * @brief return true of the token before the expression is "<?"
     */
    bool IsExprStartsWithOpenTag() const { return m_exprStartsWithOpenTag; }
    
    /**
     * @brief return the elements count in the expression.
     * For example:
     * $a->foo()->bar()->b // Code complete here
     * will return 3 ($a, foo, bar)
     */
    size_t GetCount() const { return m_parts.size(); }

    /**
     * @brief return the source file used to create and parse this expression
     */
    PHPSourceFile::Ptr_t GetSourceFile() { return m_sourceFile; }

    /**
     * @brief resolve the expression and return the entity of the last part
     * of the expression
     */
    PHPEntityBase::Ptr_t Resolve(PHPLookupTable& lookpTable, const wxString& sourceFileName);
    /**
     * @brief return the filter for this expression
     */
    const wxString& GetFilter() const { return m_filter; }

    /**
     * @brief get the lookup flags to pass to the lookup table for feteching members
     */
    size_t GetLookupFlags() const;
};

#endif // PHPEXPRESSION_H
