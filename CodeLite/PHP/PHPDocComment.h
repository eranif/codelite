//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPDocComment.h
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

#ifndef PHPDOCCOMMENT_H
#define PHPDOCCOMMENT_H

#include "codelite_exports.h"
#include <wx/string.h>
#include <wx/arrstr.h>
#include "PHPSourceFile.h"
#include "macros.h"
#include "wxStringHash.h"
#include "PHPDocParam.h"

class WXDLLIMPEXP_CL PHPDocComment
{
public:
    struct Property
    {
        wxString name;
        wxString type;
        wxString desc;
        typedef std::unordered_map<wxString, PHPDocComment::Property> Map_t;
    };

protected:
    PHPSourceFile& m_sourceFile;
    wxString m_comment;
    std::unordered_map<wxString, wxString> m_params;
    wxArrayString m_paramsArr;
    wxString m_returnValue;
    bool m_returnNullable;
    wxString m_varType;
    wxString m_varName;
    PHPDocComment::Property::Map_t m_properties; // @property, @property-read, @property-write
    PHPEntityBase::List_t m_methods;

    /**
     * @brief process @method php doc
     */
    void ProcessMethods();

public:
    PHPDocComment(PHPSourceFile& sourceFile, const wxString& comment);
    virtual ~PHPDocComment();

    void SetComment(const wxString& comment) { this->m_comment = comment; }

    const wxString& GetVar() const;
    const wxString& GetReturn() const;
    const bool IsReturnNullable() const;
    const wxString& GetParam(const wxString& name) const;
    const wxString& GetParam(size_t index) const;
    const PHPDocComment::Property::Map_t& GetProperties() const { return m_properties; }
    PHPDocComment::Property::Map_t& GetProperties() { return m_properties; }
    const PHPEntityBase::List_t& GetMethods() const { return m_methods; }
};

#endif // PHPDOCCOMMENT_H
