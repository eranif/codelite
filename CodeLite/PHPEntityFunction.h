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

#include "codelite_exports.h"
#include "PHPEntityBase.h" // Base class: PHPEntityBase
#include <vector>

class WXDLLIMPEXP_CL PHPEntityFunction : public PHPEntityBase
{
public:
    virtual wxString FormatPhpDoc(const CommentConfigData& data) const;
    virtual wxString GetDisplayName() const;
    virtual bool Is(eEntityType type) const;
    virtual wxString Type() const;
    virtual void FromResultSet(wxSQLite3ResultSet& res);
    virtual void PrintStdout(int indent) const;

protected:
    // The local variabels defined in this function of type
    // PHPEntityVariable
    PHPEntityBase::List_t m_locals;

    // the return value as read from the database
    wxString m_strReturnValue;

    // the signature
    wxString m_strSignature;

public:
    PHPEntityFunction();
    virtual ~PHPEntityFunction();
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
    virtual void Store(PHPLookupTable* lookup);
};

#endif // PHPENTITYFUNCTION_H
