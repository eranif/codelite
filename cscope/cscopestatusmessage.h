//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cscopestatusmessage.h
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

#ifndef __cscopestatusmessage__
#define __cscopestatusmessage__

#include <wx/string.h>
class CScopeStatusMessage
{
	wxString m_message;
	int      m_percentage;
	wxString m_findWhat;

public:
	CScopeStatusMessage();
	virtual ~CScopeStatusMessage();

	// Setters
	void SetMessage   (const wxString& message ) ;
	void SetPercentage (int percentage);
	void SetFindWhat  (const wxString& findWhat);

	// Getters
	const wxString&   GetMessage   () const ;
	const wxString&   GetFindWhat  () const ;
	int GetPercentage() const;
};
#endif // __cscopestatusmessage__
