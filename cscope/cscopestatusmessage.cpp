//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cscopestatusmessage.cpp
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

#include "cscopestatusmessage.h"
CScopeStatusMessage::CScopeStatusMessage()
{
}

CScopeStatusMessage::~CScopeStatusMessage()
{
}

void CScopeStatusMessage::SetMessage(const wxString& message)
{
	this->m_message = message.c_str();
}

void CScopeStatusMessage::SetPercentage(int percentage)
{
	this->m_percentage = percentage;
}

const wxString& CScopeStatusMessage::GetMessage() const
{
	return m_message;
}

int CScopeStatusMessage::GetPercentage() const
{
	return m_percentage;
}

const wxString& CScopeStatusMessage::GetFindWhat() const
{
	return m_findWhat;
}

void CScopeStatusMessage::SetFindWhat(const wxString& findWhat)
{
	m_findWhat = findWhat.c_str();
}
