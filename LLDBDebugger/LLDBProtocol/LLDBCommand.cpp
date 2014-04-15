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
    
    JSONElement arr = json.namedObject("m_breakpoints");
    for(int i=0; i<arr.arraySize(); ++i) {
        LLDBBreakpoint::Ptr_t bp(new LLDBBreakpoint() );
        bp->FromJSON( arr.arrayItem(i) );
        m_breakpoints.push_back( bp );
    }
    
    if ( m_commandType == kCommandStart ) {
        // In the "Start" command we should also receive the settings data
        m_settings.FromJSON( json.namedObject("m_settings") );
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
    
    JSONElement bparr = JSONElement::createArray("m_breakpoints");
    json.append( bparr );
    
    for(size_t i=0; i<m_breakpoints.size(); ++i) {
        bparr.arrayAppend( m_breakpoints.at(i)->ToJSON() );
    }
    
    if ( m_commandType == kCommandStart ) {
        // In the "Start" command we should also send the settings data
        json.addProperty("m_settings", m_settings.ToJSON());
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
    
    JSONElement::wxStringMap_t::const_iterator iter = m_env.begin();
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
