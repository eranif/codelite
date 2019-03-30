//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : wxmd5.h
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

//////////////////////////////////////////////////////////////////////
//      Name:          wxMD5.h
//      Purpose:       wxMD5 Class
//      Author:        Casey O'Donnell
//      Creator:       Derived from the RSA Data Security, Inc.
//                       MD5 Message-Digest Algorithm
//                       See Internet RFC 1321
//                       Copyright (C) 1991 - 1992
//                       RSA Data Security, Inc.  Created 1991
//      Created:       07/02/2003
//      Last modified: 07/02/2003
//      Licence:       wxWindows license
//////////////////////////////////////////////////////////////////////

// wxMD5.h: interface for the wxMD5 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _WXMD5_H__
#define _WXMD5_H__

#include "precompiled_header.h"
#include "codelite_exports.h"
#include <wx/filename.h>

class WXDLLIMPEXP_SDK wxMD5
{
public:
    wxMD5();
    wxMD5(const wxString& szText);
    wxMD5(const wxFileName& filename);
    virtual ~wxMD5();

    // Other Methods
    void SetText(const wxString& szText);

    const wxString GetDigest();

    // Static Methods
    static const wxString GetDigest(const wxString& szText);
    static const wxString GetDigest(const wxFileName& filename);

protected:
    wxString m_szText;
};

#endif // _WXMD5_H__
