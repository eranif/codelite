//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : svn_default_command_handler.h
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

#ifndef SVN_DEFAULT_COMMAND_HANDLER_H
#define SVN_DEFAULT_COMMAND_HANDLER_H

#include "svncommandhandler.h" // Base class

class SubversionView;

class SvnDefaultCommandHandler : public SvnCommandHandler {
public:
	SvnDefaultCommandHandler(Subversion2 *plugin, int commandId, wxEvtHandler *owner);
	virtual ~SvnDefaultCommandHandler() = default;

public:
	virtual void Process(const wxString &output);
};

#endif // SVN_DEFAULT_COMMAND_HANDLER_H
