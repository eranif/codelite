//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : buildtabsettingsdata.cpp
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
#include "buildtabsettingsdata.h"

#include "wx/settings.h"

BuildTabSettingsData::BuildTabSettingsData() {}

BuildTabSettingsData::~BuildTabSettingsData() {}

void BuildTabSettingsData::Serialize(Archive& arch)
{
    arch.Write("m_skipWarnings", m_skipWarnings);
    arch.Write("m_autoHide", m_autoHide);
    arch.Write("m_autoShow", m_autoShow);
    arch.Write("m_scroll_to", (int)m_scroll_to);
    arch.Write("m_errorWarningStyle", (int)m_errorWarningStyle);
}

void BuildTabSettingsData::DeSerialize(Archive& arch)
{
    arch.Read("m_skipWarnings", m_skipWarnings);
    arch.Read("m_autoHide", m_autoHide);
    arch.Read("m_autoShow", m_autoShow);

    int _scroll_to = BuildTabSettingsData::SCROLL_TO_FIRST_ERROR;
    arch.Read("m_scroll_to", _scroll_to);
    m_scroll_to = (ScrollToLine)_scroll_to;

    int _m_errorWarningStyle = BuildTabSettingsData::MARKER_BOOKMARKS;
    arch.Read("m_errorWarningStyle", _m_errorWarningStyle);
    m_errorWarningStyle = (ErrorsWarningStyle)_m_errorWarningStyle;
}
