//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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
#include <wx/filename.h>
#include "fileutils.h"
#include "file_logger.h"

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
    
    // Delete the commit message file
    wxFileName tmpFile(clStandardPaths::Get().GetTempDir(), ".svn-commit");
    clDEBUG() << "Svn commit handler: deleting commit file" << tmpFile << clEndl;
    FileUtils::Deleter d(tmpFile);
}
