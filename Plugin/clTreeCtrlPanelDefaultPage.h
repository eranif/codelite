//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clTreeCtrlPanelDefaultPage.h
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

#ifndef CLTREECTRLPANELDEFAULTPAGE_H
#define CLTREECTRLPANELDEFAULTPAGE_H
#include "wxcrafter_plugin.h"
#include "cl_command_event.h"
#include "clColours.h"
#include <wx/event.h>

class clTreeCtrlPanelDefaultPage : public clTreeCtrlPanelDefaultPageBase
{
    clColours m_colours;

public:
    clTreeCtrlPanelDefaultPage(wxWindow* parent);
    virtual ~clTreeCtrlPanelDefaultPage();

protected:
    virtual void OnDefaultPageContextMenu(wxContextMenuEvent& event);
    void OnOpenFolder(wxCommandEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnColoursChanged(clCommandEvent& event);
};
#endif // CLTREECTRLPANELDEFAULTPAGE_H
