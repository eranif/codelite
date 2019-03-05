//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPEntityVariable.h
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

#ifndef PHPENTITYVARIABLE_H
#define PHPENTITYVARIABLE_H

#include "codelite_exports.h"
#include "PHPEntityBase.h" // Base class: PHPEntityBase
#include <wx/string.h>

class WXDLLIMPEXP_CL PHPEntityVariable : public PHPEntityBase
{
public:
    virtual wxString FormatPhpDoc(const CommentConfigData& data) const;
    virtual wxString GetDisplayName() const;
    virtual bool Is(eEntityType type) const;
    virtual wxString Type() const;
    virtual void FromResultSet(wxSQLite3ResultSet& res);

protected:
    wxString m_typeHint;

    // Incase this variable is instantiated with an expression
    // keep it
    wxString m_expressionHint;
    wxString m_defaultValue;

public:
    /**
     * @brief
     * @return
     */
    wxString GetScope() const;
    virtual void Store(PHPLookupTable* lookup);
    virtual void PrintStdout(int indent) const;
    
    void FromJSON(const JSONItem& json);
    JSONItem ToJSON() const;
    
    /**
     * @brief format this variable
     */
    wxString ToFuncArgString() const;
    PHPEntityVariable();
    virtual ~PHPEntityVariable();

    void SetExpressionHint(const wxString& expressionHint) { this->m_expressionHint = expressionHint; }
    const wxString& GetExpressionHint() const { return m_expressionHint; }
    void SetVisibility(int visibility);
    void SetTypeHint(const wxString& typeHint) { this->m_typeHint = typeHint; }
    const wxString& GetTypeHint() const { return m_typeHint; }
    void SetDefaultValue(const wxString& defaultValue) { this->m_defaultValue = defaultValue; }
    const wxString& GetDefaultValue() const { return m_defaultValue; }
    wxString GetNameNoDollar() const;
    virtual wxString ToTooltip() const;
    
    // Aliases
    void SetIsReference(bool isReference) { SetFlag(kVar_Reference, isReference); }
    bool IsMember() const { return HasFlag(kVar_Member); }
    bool IsPublic() const { return HasFlag(kVar_Public); }
    bool IsPrivate() const { return HasFlag(kVar_Private); }
    bool IsProtected() const { return HasFlag(kVar_Protected); }
    bool IsFunctionArg() const { return HasFlag(kVar_FunctionArg); }
    bool IsConst() const { return HasFlag(kVar_Const); }
    bool IsReference() const { return HasFlag(kVar_Reference); }
    bool IsStatic() const { return HasFlag(kVar_Static); }
    bool IsDefine() const { return HasFlag(kVar_Define); }
    bool IsBoolean() const { return GetTypeHint() == "boolean" || GetTypeHint() == "bool"; }
};

#endif // PHPENTITYVARIABLE_H
