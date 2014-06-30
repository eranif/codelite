//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CompileCommandsCreateor.h
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

#ifndef COMPILECOMMANDSCREATEOR_H
#define COMPILECOMMANDSCREATEOR_H

#include "job.h"
#include <wx/thread.h>
#include "codelite_exports.h"
#include <wx/filename.h>
#include "cl_command_event.h"

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_COMPILE_COMMANDS_JSON_GENERATED, clCommandEvent);

class WXDLLIMPEXP_SDK CompileCommandsCreateor : public Job
{
    wxFileName m_filename;
public:
    CompileCommandsCreateor(const wxFileName& path);
    virtual ~CompileCommandsCreateor();

public:
    virtual void Process(wxThread* thread);
};

#endif // COMPILECOMMANDSCREATEOR_H
