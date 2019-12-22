//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : queuecommand.cpp
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

#include "queuecommand.h"
#include "workspace.h"
#include "build_config.h"

QueueCommand::QueueCommand(const wxString& project, const wxString& configuration, bool projectOnly, int kind)
    : m_project(project)
    , m_configuration(configuration)
    , m_projectOnly(projectOnly)
    , m_kind(kind)
    , m_cleanLog(true)
    , m_checkBuildSuccess(false)
{
}

QueueCommand::QueueCommand(int kind)
    : m_projectOnly(false)
    , m_kind(kind)
    , m_cleanLog(true)
    , m_checkBuildSuccess(false)
{
    // Fill with default values
    if ( clCxxWorkspaceST::Get()->IsOpen() ) {
        m_project = clCxxWorkspaceST::Get()->GetActiveProjectName();
        BuildConfigPtr buildPtr = clCxxWorkspaceST::Get()->GetProjBuildConf(m_project, "");
        wxCHECK_RET( buildPtr, "No active project" );
        
        // If a 'Build' or 'Clean' kinds where requested 
        // and the project build configuration is Custom build
        // change the kind to CustomBuild and set the proper build
        // targets
        if ( m_kind == kBuild && buildPtr->IsCustomBuild() ) {
            // change the type to CustomBuild
            m_kind = kCustomBuild;
            SetCustomBuildTarget("Build");
            
        } else if ( m_kind == kClean && buildPtr->IsCustomBuild() ) {
            // change the type to CustomBuild
            m_kind = kCustomBuild;
            SetCustomBuildTarget("Clean");
        
        } else {
            m_configuration = buildPtr->GetName();
            
        }
    }
}

QueueCommand::~QueueCommand()
{
}


void QueueCommand::SetConfiguration(const wxString& configuration)
{
    this->m_configuration = configuration;
}

void QueueCommand::SetProject(const wxString& project)
{
    this->m_project = project;
}

const wxString& QueueCommand::GetConfiguration() const
{
    return m_configuration;
}

const wxString& QueueCommand::GetProject() const
{
    return m_project;
}

void QueueCommand::SetProjectOnly(const bool& projectOnly)
{
    this->m_projectOnly = projectOnly;
}

const bool& QueueCommand::GetProjectOnly() const
{
    return m_projectOnly;
}

void QueueCommand::SetKind(const int& kind)
{
    this->m_kind = kind;
}

const int& QueueCommand::GetKind() const
{
    return m_kind;
}

void QueueCommand::SetCleanLog(const bool& cleanLog)
{
    this->m_cleanLog = cleanLog;
}

const bool& QueueCommand::GetCleanLog() const
{
    return m_cleanLog;
}

wxString QueueCommand::DeriveSynopsis() const
{
    wxString synopsis;
    switch (m_kind) {
    case kBuild:
        synopsis << wxT("Building ");
        break;
    case kClean:
        synopsis << wxT("Cleaning ");
        break;
    case kCustomBuild:
        synopsis << wxT("Making '") << m_customBuildTarget << wxT("' In ");
        break;
    case kDebug:
        synopsis << wxT("Debugging ");
        break;
    case kExecuteNoDebug:
        synopsis << "Executing ";
        break;
    default:
        synopsis << wxT("In ");
        break;
    }
    synopsis << m_project << wxT(" (") << m_configuration << wxT(")");
    return synopsis;
}
