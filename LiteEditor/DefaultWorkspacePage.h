//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : DefaultWorkspacePage.h
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

#ifndef DEFAULTWORKSPACEPAGE_H
#define DEFAULTWORKSPACEPAGE_H

#include "clColours.h"
#include "cl_command_event.h"
#include "wxcrafter.h"

class DefaultWorkspacePage : public DefaultWorkspacePageBase
{
    clColours m_colours;

protected:
    void OnFolderDropped(clCommandEvent& event);
    void DoDropFolders(const wxArrayString& folders);
    void OnColoursChanged(clCommandEvent& event);

public:
    DefaultWorkspacePage(wxWindow* parent);
    virtual ~DefaultWorkspacePage();
};
#endif // DEFAULTWORKSPACEPAGE_H
