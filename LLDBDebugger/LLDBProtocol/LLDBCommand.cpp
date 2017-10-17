//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBCommand.cpp
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

#include "LLDBCommand.h"

LLDBCommand::~LLDBCommand()
{
}

LLDBCommand::LLDBCommand(const wxString& jsonString)
{
    JSONRoot root(jsonString);
    FromJSON( root.toElement() );
}

void LLDBCommand::FromJSON(const JSONElement& json)
{
    m_commandType = json.namedObject("m_commandType").toInt(kCommandInvalid);
    m_commandArguments = json.namedObject("m_commandArguments").toString();
    m_workingDirectory = json.namedObject("m_workingDirectory").toString();
    m_executable = json.namedObject("m_executable").toString();
    m_redirectTTY = json.namedObject("m_redirectTTY").toString();
    m_interruptReason = json.namedObject("m_interruptReason").toInt(kInterruptReasonNone);
    m_lldbId = json.namedObject("m_lldbId").toInt(wxNOT_FOUND);
    m_env = json.namedObject("m_env").toStringMap();
    m_frameId = json.namedObject("m_frameId").toInt(wxNOT_FOUND);
    m_threadId = json.namedObject("m_threadId").toInt(wxNOT_FOUND);
    m_expression = json.namedObject("m_expression").toString();
    m_startupCommands = json.namedObject("m_startupCommands").toString();
    

    JSONElement arr = json.namedObject("m_breakpoints");
    for(int i=0; i<arr.arraySize(); ++i) {
        LLDBBreakpoint::Ptr_t bp(new LLDBBreakpoint() );
        bp->FromJSON( arr.arrayItem(i) );
        m_breakpoints.push_back( bp );
    }

    if (    m_commandType == kCommandStart          || 
            m_commandType == kCommandDebugCoreFile  ||
            m_commandType == kCommandAttachProcess  )
    {
        m_settings.FromJSON( json.namedObject("m_settings") );
    }
    
    if ( m_commandType == kCommandDebugCoreFile ) {
        m_corefile = json.namedObject("m_corefile").toString();
    }
    if ( m_commandType == kCommandAttachProcess ) {
        m_processID = json.namedObject("m_processID").toInt();
    }
}

JSONElement LLDBCommand::ToJSON() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty("m_commandType", m_commandType);
    json.addProperty("m_commandArguments", m_commandArguments);
    json.addProperty("m_workingDirectory", m_workingDirectory);
    json.addProperty("m_executable", m_executable);
    json.addProperty("m_redirectTTY", m_redirectTTY);
    json.addProperty("m_interruptReason", m_interruptReason);
    json.addProperty("m_lldbId", m_lldbId);
    json.addProperty("m_env", m_env);
    json.addProperty("m_frameId", m_frameId);
    json.addProperty("m_threadId", m_threadId);
    json.addProperty("m_expression", m_expression);
    json.addProperty("m_startupCommands", m_startupCommands);

    JSONElement bparr = JSONElement::createArray("m_breakpoints");
    json.append( bparr );

    for(size_t i=0; i<m_breakpoints.size(); ++i) {
        bparr.arrayAppend( m_breakpoints.at(i)->ToJSON() );
    }

    if (    m_commandType == kCommandStart          || 
            m_commandType == kCommandDebugCoreFile  ||
            m_commandType == kCommandAttachProcess  )
    {
        json.addProperty("m_settings", m_settings.ToJSON());
    }
    
    if ( m_commandType == kCommandDebugCoreFile ) {
        json.addProperty("m_corefile", m_corefile);
    }
    
    if ( m_commandType == kCommandAttachProcess ) {
        json.addProperty("m_processID", m_processID);
    }
    return json;
}

void LLDBCommand::FillEnvFromMemory()
{
    // get an environment map from memory and copy into
    // m_env variable.
    m_env.clear();
    wxEnvVariableHashMap tmpEnvMap;
    ::wxGetEnvMap( &tmpEnvMap );

    wxEnvVariableHashMap::iterator iter = tmpEnvMap.begin();
    for(; iter != tmpEnvMap.end(); ++iter ) {
        m_env.insert( std::make_pair(iter->first, iter->second) );
    }
}

char** LLDBCommand::GetEnvArray() const
{
    if ( m_env.empty() ) {
        return NULL;
    }

    char **penv = new char*[m_env.size()+1];

    wxStringMap_t::const_iterator iter = m_env.begin();
    size_t index(0);
    for(; iter != m_env.end(); ++iter ) {
        // Convert the environment into C-array
        wxString entry;
        entry << iter->first << "=" << iter->second;
        std::string c_string = entry.mb_str(wxConvUTF8).data();
        char *pentry = new char[c_string.length()+1];
        strcpy(pentry, c_string.c_str());
        penv[index] = pentry;
        ++index;
    }
    penv[m_env.size()] = NULL;
    return penv;
}

void LLDBCommand::UpdatePaths(const LLDBPivot& pivot)
{
    if ( pivot.IsValid() ) {
        
        // Convert all paths:
        // m_workingDirectory
        // m_executable
        // m_breakpoints
#if 0
        m_workingDirectory = pivot.ToRemote( m_workingDirectory );
        m_executable       = pivot.ToRemote( m_executable );
#endif
        for(size_t i=0; i<m_breakpoints.size(); ++i) {
            m_breakpoints.at(i)->SetFilename( pivot.ToRemote( m_breakpoints.at(i)->GetFilename() ) );
        }
    }
}
