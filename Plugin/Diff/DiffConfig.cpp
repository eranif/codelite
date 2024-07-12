//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : DiffConfig.cpp
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

#include "DiffConfig.h"

DiffConfig::DiffConfig()
    : clConfigItem("CodeLiteDiff")
    , m_flags(kShowLineNumbers)
    , m_viewFlags(kViewVerticalSplit)
{
}

DiffConfig::~DiffConfig() {}

void DiffConfig::FromJSON(const JSONItem& json)
{
    m_flags = json.namedObject("m_flags").toSize_t(0);
    m_viewFlags = json.namedObject("m_viewFlags").toSize_t(kViewVerticalSplit);
    m_leftFile = json.namedObject("m_leftFile").toString();
    m_rightFile = json.namedObject("m_rightFile").toString();
}

JSONItem DiffConfig::ToJSON() const
{
    JSONItem element = JSONItem::createObject(GetName());
    element.addProperty("m_flags", m_flags);
    element.addProperty("m_viewFlags", m_viewFlags);
    element.addProperty("m_leftFile", m_leftFile);
    element.addProperty("m_rightFile", m_rightFile);
    return element;
}

DiffConfig& DiffConfig::Load()
{
    clConfig::Get().ReadItem(this);
    return *this;
}

void DiffConfig::Save() { clConfig::Get().WriteItem(this); }
