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
#include <map>
#include "PHPSourceFile.h"

class WXDLLIMPEXP_CL PHPDocComment
{
    wxString m_comment;
    std::map<wxString, wxString> m_params;
    wxArrayString m_paramsArr;
    wxString m_returnValue;
    wxString m_varType;
    wxString m_varName;
    
public:
    PHPDocComment(PHPSourceFile& sourceFile, const wxString& comment);
    virtual ~PHPDocComment();

    void SetComment(const wxString& comment) { this->m_comment = comment; }

    const wxString& GetVar() const;
    const wxString& GetReturn() const;
    const wxString& GetParam(const wxString &name) const;
    const wxString& GetParam(size_t index) const;
};

#endif // PHPDOCCOMMENT_H
