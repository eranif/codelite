//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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
#include "drawingutils.h"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "event_notifier.h"

clAuiGlossyTabArt::clAuiGlossyTabArt()
    : clAuiMainNotebookTabArt()
{
}

clAuiGlossyTabArt::~clAuiGlossyTabArt() {}

void clAuiGlossyTabArt::DoSetColours()
{
    // And finally let the plugins override the colours
    clColourEvent tabColourEvent(wxEVT_COLOUR_TAB);
    if(EventNotifier::Get()->ProcessEvent(tabColourEvent)) {
        m_activeTabBgColour = tabColourEvent.GetBgColour();
        m_activeTabTextColour = tabColourEvent.GetFgColour();
        m_tabTextColour = tabColourEvent.GetFgColour();
        m_tabBgColour = m_activeTabBgColour.ChangeLightness(120);
    }

    clColourEvent tabPenColour(wxEVT_GET_TAB_BORDER_COLOUR);
    if(EventNotifier::Get()->ProcessEvent(tabPenColour)) {
        m_activeTabPenColour = tabPenColour.GetBorderColour();
        m_penColour = m_activeTabPenColour.ChangeLightness(120);
    }
}
