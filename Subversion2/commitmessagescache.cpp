//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : commitmessagescache.cpp
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

#include "commitmessagescache.h"

void CommitMessagesCache::GetMessages(wxArrayString& messages, wxArrayString& previews)
{
	for(size_t i=0; i<m_messages.GetCount(); i++) {
		messages.Add(m_messages.Item(i));
		previews.Add(m_messages.Item(i).BeforeFirst(wxT('\n')));
	}
}

void CommitMessagesCache::AddMessage(const wxString& message)
{
	wxString formattedMessage = FormatMessage(message);
	if(formattedMessage.IsEmpty())
		return;

	if(m_messages.Index(formattedMessage) == wxNOT_FOUND) {
		m_messages.Add(formattedMessage);
	}
}

wxString CommitMessagesCache::FormatMessage(const wxString& message)
{
	wxString formattedMessage ( message );
	formattedMessage.Replace(wxT("\r\n"), wxT("\n"));
	formattedMessage.Replace(wxT("\v"),   wxT("\n"));
	formattedMessage.Trim(false).Trim();
	return formattedMessage;
}
