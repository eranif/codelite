//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : svnstatushandler.h
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

#ifndef SVNSTATUSHANDLER_H
#define SVNSTATUSHANDLER_H

#include "svncommandhandler.h" // Base class

class SubversionView;
class Subversion2;

class SvnStatusHandler : public SvnCommandHandler
{
    bool m_fileExplorerOnly;
    wxString m_rootDir;

public:
    SvnStatusHandler(Subversion2* plugin, int commandId, wxEvtHandler* owner, bool fileExplorerOnly = false,
                     const wxString& rootDir = wxT(""));
    virtual ~SvnStatusHandler();

public:
    virtual void Process(const wxString& output);
};

#endif // SVNSTATUSHANDLER_H
