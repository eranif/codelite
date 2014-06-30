//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CompileCommandsCreateor.cpp
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

#include "CompileCommandsCreateor.h"
#include "event_notifier.h"
#include "workspace.h"

wxDEFINE_EVENT(wxEVT_COMPILE_COMMANDS_JSON_GENERATED, clCommandEvent);

CompileCommandsCreateor::CompileCommandsCreateor(const wxFileName& path)
    : m_filename(path)
{
}

CompileCommandsCreateor::~CompileCommandsCreateor()
{
}

void CompileCommandsCreateor::Process(wxThread* thread)
{
    wxString errMsg;
    wxUnusedVar(thread);
    Workspace workspace;
    workspace.OpenReadOnly(m_filename.GetFullPath(), errMsg);
    
    JSONRoot json(cJSON_Array);
    JSONElement compile_commands = json.toElement();
    workspace.CreateCompileCommandsJSON(compile_commands);
    
    // Save the file
    wxFileName compileCommandsFile(m_filename.GetPath(), "compile_commands.json");
    json.save( compileCommandsFile );
    
    clCommandEvent eventCompileCommandsGenerated(wxEVT_COMPILE_COMMANDS_JSON_GENERATED);
    EventNotifier::Get()->AddPendingEvent( eventCompileCommandsGenerated );
}
