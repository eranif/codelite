//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 The CodeLite Team
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

class clTreeCtrlPanelDefaultPage : public clTreeCtrlPanelDefaultPageBase
{
public:
    clTreeCtrlPanelDefaultPage(wxWindow* parent);
    virtual ~clTreeCtrlPanelDefaultPage();

protected:
    virtual void OnDefaultPageContextMenu(wxContextMenuEvent& event);
    void OnOpenFolder(wxCommandEvent& event);
};
#endif // CLTREECTRLPANELDEFAULTPAGE_H
