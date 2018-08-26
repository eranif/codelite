//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : svnstatushandler.cpp
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

#include "svnstatushandler.h"
#include "svn_console.h"
#include "subversion_view.h"
#include "subversion2.h"
#include "svnxml.h"

SvnStatusHandler::SvnStatusHandler(Subversion2* plugin, int commandId, wxEvtHandler* owner, bool fileExplorerOnly,
                                   const wxString& rootDir)
    : SvnCommandHandler(plugin, commandId, owner)
    , m_fileExplorerOnly(fileExplorerOnly)
    , m_rootDir(rootDir)
{
}

SvnStatusHandler::~SvnStatusHandler() {}

void SvnStatusHandler::Process(const wxString& output)
{
    wxArrayString modFiles, conflictedFiles, unversionedFiles, newFiles, deletedFiles, lockedFiles, ignoredFiles;
    SvnXML::GetFiles(output, modFiles, conflictedFiles, unversionedFiles, newFiles, deletedFiles, lockedFiles,
                     ignoredFiles);

    modFiles.Sort();
    conflictedFiles.Sort();
    unversionedFiles.Sort();
    newFiles.Sort();
    deletedFiles.Sort();
    lockedFiles.Sort();
    ignoredFiles.Sort();
    GetPlugin()->GetSvnView()->UpdateTree(modFiles, conflictedFiles, unversionedFiles, newFiles, deletedFiles,
                                          lockedFiles, ignoredFiles, m_fileExplorerOnly, m_rootDir);
}
