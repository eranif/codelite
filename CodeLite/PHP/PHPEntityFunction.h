//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPEntityFunction.h
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

#ifndef PHPENTITYFUNCTION_H
#define PHPENTITYFUNCTION_H

#include "PHPEntityBase.h" // Base class: PHPEntityBase
#include "codelite_exports.h"

class WXDLLIMPEXP_CL PHPEntityFunction : public PHPEntityBase
{
public:
    wxString FormatPhpDoc(const CommentConfigData& data) const override;
    wxString GetDisplayName() const override;
    bool Is(eEntityType type) const override;
    wxString Type() const override;
    void FromResultSet(wxSQLite3ResultSet& res) override;
    void PrintStdout(int indent) const override;

    void FromJSON(const JSONItem& json) override;
    JSONItem ToJSON() const override;

protected:
    // The local variables defined in this function of type
    // PHPEntityVariable
    PHPEntityBase::List_t m_locals;

    // the return value as read from the database
    wxString m_strReturnValue;

    // the signature
    wxString m_strSignature;

public:
    PHPEntityFunction() = default;
    ~PHPEntityFunction() override = default;
    const wxString& GetReturnValue() const { return m_strReturnValue; }
    void SetReturnValue(const wxString& strReturnValue) { this->m_strReturnValue = strReturnValue; }

    wxString GetScope() const;
    /**
     * @brief format function signature
     */
    wxString GetSignature() const;

    /**
     * @brief return the full path for this function
     * Example: \path\toClass::functionName($a, $b)
     */
    wxString GetFullPath() const;

    /**
     * @brief write this object into the database
     * @param db
     */
    void Store(PHPLookupTable* lookup) override;

    wxString ToTooltip() const override;
};

#endif // PHPENTITYFUNCTION_H
