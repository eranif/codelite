//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
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
    , m_flags(0)
    , m_viewFlags(kViewVerticalSplit)
{
}

DiffConfig::~DiffConfig()
{
}

void DiffConfig::FromJSON(const JSONElement& json)
{
    m_flags     = json.namedObject("m_flags").toSize_t(0);
    m_viewFlags = json.namedObject("m_viewFlags").toSize_t(kViewVerticalSplit);
}

JSONElement DiffConfig::ToJSON() const
{
    JSONElement element = JSONElement::createObject(GetName());
    element.addProperty("m_flags",      m_flags);
    element.addProperty("m_viewFlags",  m_viewFlags);
    return element;
}

DiffConfig& DiffConfig::Load()
{
    clConfig::Get().ReadItem( this );
    return *this;
}

void DiffConfig::Save()
{
    clConfig::Get().WriteItem( this );
}
