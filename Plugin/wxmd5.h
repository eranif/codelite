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

#ifdef __GNUG__
#pragma interface "wxMD5.h"
#endif

#include "wx/string.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK wxMD5
{
public:
	wxMD5();
	wxMD5(const wxString& szText);
	virtual ~wxMD5();

	// Other Methods
	void SetText(const wxString& szText);

	const wxString GetDigest();

	// Static Methods
	static const wxString GetDigest(const wxString& szText);

protected:
	bool      m_bCalculatedDigest;
	wxUint8   m_arrDigest[16];
	char      m_pszDigestString[33];
	wxString  m_szText;
};

#endif // _WXMD5_H__
