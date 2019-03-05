//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : abbreviationentry.cpp
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

#include "abbreviationentry.h"
AbbreviationEntry::AbbreviationEntry()
    : m_entries()
    , m_autoInsert(false)
{
}

AbbreviationEntry::~AbbreviationEntry() {}

void AbbreviationEntry::DeSerialize(Archive& arch)
{
    arch.Read(wxT("m_entries"), m_entries);
    arch.Read(wxT("m_autoInsert"), m_autoInsert);
}

void AbbreviationEntry::Serialize(Archive& arch)
{
    arch.Write(wxT("m_entries"), m_entries);
    arch.Write(wxT("m_autoInsert"), m_autoInsert);
}

////////////////////////////////////////////////////////////////
// JSON
////////////////////////////////////////////////////////////////

void AbbreviationJSONEntry::FromJSON(const JSONItem& json)
{
    m_entries = json.namedObject("entries").toStringMap();
    m_autoInsert = json.namedObject("autoInsert").toBool();
}

JSONItem AbbreviationJSONEntry::ToJSON() const
{
    JSONItem ele = JSONItem::createObject(GetName());
    ele.addProperty("entries", m_entries);
    ele.addProperty("autoInsert", m_autoInsert);
    return ele;
}
