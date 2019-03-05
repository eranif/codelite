//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SFTPBookmark.cpp
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

#include "SFTPBookmark.h"

SFTPBookmark::SFTPBookmark()
{
}

SFTPBookmark::~SFTPBookmark()
{
}

void SFTPBookmark::FromJSON(const JSONItem& json)
{
    m_account.FromJSON( json.namedObject("m_account") );
    m_name      = json.namedObject("m_name").toString();
    m_folder    = json.namedObject("m_folder").toString();
}

JSONItem SFTPBookmark::ToJSON() const
{
    JSONItem json = JSONItem::createObject();
    json.addProperty("m_account", m_account.ToJSON());
    json.addProperty("m_name", m_name);
    json.addProperty("m_folder", m_folder);
    return json;
}
