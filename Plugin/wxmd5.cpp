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
#include "md5_header.h"
#include <wx/string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

wxMD5::wxMD5()
{
}

wxMD5::wxMD5(const wxString& szText)
{
	m_szText = szText;
}

wxMD5::~wxMD5()
{
}

//////////////////////////////////////////////////////////////////////
// Other Methods
//////////////////////////////////////////////////////////////////////

void wxMD5::SetText(const wxString& szText)
{
	m_szText = szText;
}

const wxString wxMD5::GetDigest()
{
	MD5 context;
	context.update((unsigned char*)m_szText.mb_str().data(), m_szText.Len());
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
