//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : cl_exception.h
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

#ifndef CLEXCEPTION_H
#define CLEXCEPTION_H

#include <wx/string.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clException
{
    wxString m_message;
    int      m_errorCode;
public:
    clException(const wxString& message) : m_message(message), m_errorCode(0) {}
    clException(const wxString& message, int errorCode) : m_message(message), m_errorCode(errorCode) {}
    virtual ~clException() = default;
    
    const wxString& What() const {
        return m_message;
    }
    int ErrorCode() const {
        return m_errorCode;
    }
};

#endif // CLEXCEPTION_H
