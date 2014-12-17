//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : cl_aui_notebook_art.cpp
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

#include "cl_aui_notebook_art.h"

clAuiGlossyTabArt::clAuiGlossyTabArt()
    : clAuiMainNotebookTabArt(NULL)
{
    m_tabRadius = 0.0;
}

clAuiGlossyTabArt::~clAuiGlossyTabArt() {}

void clAuiGlossyTabArt::DoSetColours()
{
    // Set the colours
    // based on the selected book theme
    DoInitializeColoursFromTheme();
#ifdef __WXMSW__
    wxColour baseColour = wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION);
    m_tabBgColour = baseColour.ChangeLightness(150);
    m_penColour = baseColour.ChangeLightness(80);
    m_innerPenColour = baseColour.ChangeLightness(175);
#else
    m_tabTextColour = wxColour("rgb(60, 60, 60)");
#endif
}
