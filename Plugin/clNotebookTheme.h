//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clNotebookTheme.h
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

#ifndef CLNOTEBOOKTHEME_H
#define CLNOTEBOOKTHEME_H

#include "codelite_exports.h"

#include <wx/colour.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK clNotebookTheme
{
public:
    enum eNotebookTheme { kDefault, kDark };

public:
    clNotebookTheme() {}

    static clNotebookTheme GetTheme(clNotebookTheme::eNotebookTheme theme);

    void SetActiveTabBgColour(const wxColour& activeTabBgColour) { this->m_activeTabBgColour = activeTabBgColour; }
    void SetActiveTabPenColour(const wxColour& activeTabPenColour) { this->m_activeTabPenColour = activeTabPenColour; }
    void SetActiveTabTextColour(const wxColour& activeTabTextColour)
    {
        this->m_activeTabTextColour = activeTabTextColour;
    }
    void SetBgColour(const wxColour& bgColour) { this->m_bgColour = bgColour; }
    void SetInnerPenColour(const wxColour& innerPenColour) { this->m_innerPenColour = innerPenColour; }
    void SetPenColour(const wxColour& penColour) { this->m_penColour = penColour; }
    void SetTabBgColour(const wxColour& tabBgColour) { this->m_tabBgColour = tabBgColour; }
    void SetTabTextColour(const wxColour& tabTextColour) { this->m_tabTextColour = tabTextColour; }
    const wxColour& GetActiveTabBgColour() const { return m_activeTabBgColour; }
    const wxColour& GetActiveTabPenColour() const { return m_activeTabPenColour; }
    const wxColour& GetActiveTabTextColour() const { return m_activeTabTextColour; }
    const wxColour& GetBgColour() const { return m_bgColour; }
    const wxColour& GetInnerPenColour() const { return m_innerPenColour; }
    const wxColour& GetPenColour() const { return m_penColour; }
    const wxColour& GetTabBgColour() const { return m_tabBgColour; }
    const wxColour& GetTabTextColour() const { return m_tabTextColour; }

protected:
    // The tab area background colour
    wxColour m_bgColour;

    // The tab area pen colour (used to mark the tab area borders)
    wxColour m_penColour;
    wxColour m_activeTabPenColour;
    wxColour m_innerPenColour;

    // The text colour
    wxColour m_activeTabTextColour;
    wxColour m_tabTextColour;

    // A singe tab background colour
    wxColour m_activeTabBgColour;
    wxColour m_tabBgColour;
};

#endif // CLNOTEBOOKTHEME_H
