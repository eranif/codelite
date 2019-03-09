//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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
#include "file_logger.h"
#include "asyncprocess.h"
#include "globals.h"

wxDEFINE_EVENT(wxEVT_COMPILE_COMMANDS_JSON_GENERATED, clCommandEvent);

CompileCommandsCreateor::CompileCommandsCreateor(const wxFileName& path, const wxString& config)
    : m_filename(path)
    , m_configName(config)
{
}

CompileCommandsCreateor::~CompileCommandsCreateor() {}

void CompileCommandsCreateor::Process(wxThread* thread)
{
    wxString errMsg;
    wxUnusedVar(thread);
    
    wxFileName codeliteMake(clStandardPaths::Get().GetBinFolder(), "codelite-make");
#ifdef __WXMSW__
    codeliteMake.SetExt("exe");
#endif

    if(!codeliteMake.FileExists()) {
        clWARNING() << "Could not find" << codeliteMake;
        return;
    }
    
    wxString command;
    command << codeliteMake.GetFullPath();
    ::WrapWithQuotes(command);
    
    wxString workspaceFile = m_filename.GetFullPath();
    ::WrapWithQuotes(workspaceFile);
    
    command << " --workspace=" << workspaceFile << " --verbose --json --config=" << m_configName;
    
    wxString output;
    IProcess::Ptr_t process(::CreateSyncProcess(command, IProcessCreateDefault, m_filename.GetPath()));
    process->WaitForTerminate(output);
    
    clDEBUG() << output;
    
    wxFileName compileCommandsFile = m_filename;
    compileCommandsFile.SetFullName("compile_commands.json");
    
    clCommandEvent eventCompileCommandsGenerated(wxEVT_COMPILE_COMMANDS_JSON_GENERATED);
    eventCompileCommandsGenerated.SetFileName(compileCommandsFile.GetFullPath());
    EventNotifier::Get()->AddPendingEvent(eventCompileCommandsGenerated);
}
