//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : commitmessagescache.h
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

#ifndef COMMITMESSAGESCACHE_H
#define COMMITMESSAGESCACHE_H

#include <wx/arrstr.h>

class CommitMessagesCache
{
    wxArrayString m_messages;

protected:
    wxString FormatMessage(const wxString& message);

public:
    CommitMessagesCache();
    virtual ~CommitMessagesCache();

    /**
     * @brief return the messages
     * @param messages array containing the messages
     * @param previews array containing preview (first line) of the messages
     */
    void GetMessages(wxArrayString& messages, wxArrayString& previews);
    /**
     * @brief add message to the cache. duplicate messages are ignored
     * @param message
     */
    void AddMessage(const wxString& message);
    
    /**
     * @brief is the cache empty?
     */
    bool IsEmpty() const;
    /**
     * @brief clear the cache
     */
    void Clear();
};

#endif // COMMITMESSAGESCACHE_H
