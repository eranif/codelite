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

#ifdef __GNUG__
#pragma implementation "wxMD5.h"
#endif

// for compilers that support precompilation, includes "wx.h"
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wxmd5.h"
#include "md5.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

wxMD5::wxMD5()
{
	m_bCalculatedDigest = false;
	m_pszDigestString[32] = '\0';
}

wxMD5::wxMD5(const wxString& szText)
{
	m_bCalculatedDigest = false;
	m_pszDigestString[32] = '\0';
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
	m_bCalculatedDigest = false;
	m_szText = szText;
}

const wxString wxMD5::GetDigest()
{
	if (m_bCalculatedDigest) {
		const wxString szRetVal(m_pszDigestString, wxConvUTF8);
		return szRetVal;
	} else if (m_szText.IsEmpty()) {
		return wxT("");
	} else {
		MD5_CTX md5Context;
		MD5Init(&md5Context);

#if wxVERSION_NUMBER < 2900
		MD5Update(&md5Context, (unsigned char*)(m_szText.c_str()), m_szText.Len());
#else
		MD5Update(&md5Context, (unsigned char*)m_szText.char_str().data(), m_szText.Len());
#endif
		MD5Final(m_arrDigest, &md5Context);

		int j = 0;
		for (int i = 0; i < 16; i++) {
			sprintf(&m_pszDigestString[j], "%02x", m_arrDigest[j]);
			j += 2;
		}

		const wxString szRetVal(m_pszDigestString, wxConvUTF8);

		return szRetVal;
	}
}

//////////////////////////////////////////////////////////////////////
// Static Methods
//////////////////////////////////////////////////////////////////////

const wxString wxMD5::GetDigest(const wxString& szText)
{
	wxMD5 md5(szText);

	return md5.GetDigest();
}
