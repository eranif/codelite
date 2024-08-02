//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : wxmd5.cpp
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
//      Name:          wxMD5.cpp
//      Purpose:       wxMD5 Class
//      Author:        Casey O'Donnell
//      Creator:       See Internet RFC 1321
//                       Copyright (C) 1991 - 1992
//                       RSA Data Security, Inc.  Created 1991
//      Created:       07/02/2003
//      Last modified: 07/02/2003
//      Licence:       wxWindows license
//////////////////////////////////////////////////////////////////////

// wxMD5.cpp: implementation of the wxMD5 class.
//
//////////////////////////////////////////////////////////////////////

#include "wxmd5.h"

#include "fileutils.h"
#include "md5_header.h"

#include <wx/string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

wxMD5::wxMD5() {}

wxMD5::wxMD5(const wxString& szText) { m_string = szText.mb_str().data(); }
wxMD5::wxMD5(const std::string& szText) { m_string = szText; }

wxMD5::~wxMD5() {}

//////////////////////////////////////////////////////////////////////
// Other Methods
//////////////////////////////////////////////////////////////////////

void wxMD5::SetText(const wxString& szText) { m_string = szText.mb_str().data(); }

const wxString wxMD5::GetDigest()
{
    MD5 context;
    context.update((unsigned char*)m_string.c_str(), m_string.length());
    context.finalize();

    wxString md5(context.hex_digest());
    md5.MakeUpper();
    return md5;
}

//////////////////////////////////////////////////////////////////////
// Static Methods
//////////////////////////////////////////////////////////////////////

const wxString wxMD5::GetDigest(const wxString& szText)
{
    wxMD5 md5(szText);
    return md5.GetDigest();
}

wxMD5::wxMD5(const wxFileName& filename)
{
    wxString fileContent;
    FileUtils::ReadFileContent(filename, fileContent);
    m_string = fileContent.mb_str().data();
}

const wxString wxMD5::GetDigest(const wxFileName& filename)
{
    wxMD5 md5(filename);
    return md5.GetDigest();
}

const wxString wxMD5::GetDigest(const std::string& str)
{
    wxMD5 md5(str);
    return md5.GetDigest();
}
