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
#include <wx/settings.h>

GeneralInfo::GeneralInfo()
{
    // build the default frame size
    wxRect display_rect =
        wxRect(0, 0, wxSystemSettings::GetMetric(wxSYS_SCREEN_X), wxSystemSettings::GetMetric(wxSYS_SCREEN_Y));
    wxRect frame_rect = display_rect;
    frame_rect.Deflate(frame_rect.GetWidth() / 10, frame_rect.GetHeight() / 10);
    frame_rect = frame_rect.CenterIn(display_rect);
    m_framePos = frame_rect.GetTopLeft();
    m_frameSize = frame_rect.GetSize();
    m_defaultRect = frame_rect;
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
