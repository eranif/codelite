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

BuildTabSettingsData::BuildTabSettingsData()
    : m_skipWarnings(false)
    , m_boldErrFont(false)
    , m_boldWarnFont(false)
    , m_showBuildPane(0)
    , m_autoHide(false)
    , m_autoShow(false)
    , m_errorsFirstLine(false)
    , m_errorWarningStyle(EWS_Bookmarks)
    , m_buildpaneScrollTo(0)
{
    wxColour errColour(wxT("RED"));
    wxColour wrnColour(wxT("#AA9B49"));

    m_errorColour = errColour.GetAsString(wxC2S_HTML_SYNTAX);
    m_warnColour = wrnColour.GetAsString(wxC2S_HTML_SYNTAX);

    m_warnColourBg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW).GetAsString();
    m_errorColourBg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW).GetAsString();
}

BuildTabSettingsData::~BuildTabSettingsData() {}

void BuildTabSettingsData::Serialize(Archive& arch)
{
    arch.Write(wxT("m_errorColour"), m_errorColour);
    arch.Write(wxT("m_warnColour"), m_warnColour);
    arch.Write(wxT("m_errorColourBg"), m_errorColourBg);
    arch.Write(wxT("m_warnColourBg"), m_warnColourBg);
    arch.Write(wxT("m_skipWarnings"), m_skipWarnings);
    arch.Write(wxT("m_boldErrFont"), m_boldErrFont);
    arch.Write(wxT("m_boldWarnFont"), m_boldWarnFont);
    arch.Write(wxT("m_autoHide"), m_autoHide);
    arch.Write(wxT("m_autoShow"), m_autoShow);
    arch.Write(wxT("m_errorsFirstLine"), m_errorsFirstLine);
    arch.Write(wxT("m_showBuildPane"), m_showBuildPane);
    arch.Write(wxT("m_errorWarningStyle"), m_errorWarningStyle);
    arch.Write(wxT("m_buildpaneScrollTo"), m_buildpaneScrollTo);
    arch.Write("m_buildFont", m_buildFont);
}

void BuildTabSettingsData::DeSerialize(Archive& arch)
{
    arch.Read(wxT("m_errorColour"), m_errorColour);
    arch.Read(wxT("m_warnColour"), m_warnColour);
    arch.Read(wxT("m_errorColourBg"), m_errorColourBg);
    arch.Read(wxT("m_warnColourBg"), m_warnColourBg);
    arch.Read(wxT("m_skipWarnings"), m_skipWarnings);
    arch.Read(wxT("m_boldErrFont"), m_boldErrFont);
    arch.Read(wxT("m_boldWarnFont"), m_boldWarnFont);
    arch.Read(wxT("m_autoHide"), m_autoHide);
    arch.Read(wxT("m_autoShow"), m_autoShow);
    arch.Read(wxT("m_errorsFirstLine"), m_errorsFirstLine);
    arch.Read(wxT("m_showBuildPane"), m_showBuildPane);
    arch.Read(wxT("m_errorWarningStyle"), m_errorWarningStyle);
    arch.Read(wxT("m_buildpaneScrollTo"), m_buildpaneScrollTo);
    arch.Read("m_buildFont", m_buildFont);
}
