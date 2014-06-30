//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : svn_default_command_handler.cpp
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

#include "svn_default_command_handler.h"
#include "subversion2.h"
#include "subversion_view.h"
#include "event_notifier.h"

SvnDefaultCommandHandler::SvnDefaultCommandHandler(Subversion2 *plugin, int commandId, wxEvtHandler *owner)
    : SvnCommandHandler(plugin, commandId, owner)
{
}

SvnDefaultCommandHandler::~SvnDefaultCommandHandler()
{
}

void SvnDefaultCommandHandler::Process(const wxString &output)
{
    wxUnusedVar(output);

    // Reload any modified files
    EventNotifier::Get()->PostReloadExternallyModifiedEvent(false);

    // Refresh the SVN output page
    GetPlugin()->GetSvnView()->BuildTree();
}
