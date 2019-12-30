//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : generalinfo.cpp
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
#include "generalinfo.h"

GeneralInfo::GeneralInfo()
    : m_frameSize(800, 600)
    , m_framePos(30, 30)
{
}

GeneralInfo::~GeneralInfo() {}

void GeneralInfo::DeSerialize(Archive& arch)
{
    arch.Read(wxT("m_frameSize"), m_frameSize);
    arch.Read(wxT("m_framePos"), m_framePos);
    arch.Read(wxT("m_flags"), m_flags);
}

void GeneralInfo::Serialize(Archive& arch)
{
    arch.Write(wxT("m_frameSize"), m_frameSize);
    arch.Write(wxT("m_framePos"), m_framePos);
    arch.Write(wxT("m_flags"), m_flags);
}
