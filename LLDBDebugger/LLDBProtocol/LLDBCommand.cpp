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

    JSONElement arr = json.namedObject("m_breakpoints");
    for(int i=0; i<arr.arraySize(); ++i) {
        LLDBBreakpoint bp;
        bp.FromJSON( arr.arrayItem(i) );
        m_breakpoints.push_back( bp );
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
    
    JSONElement bparr = JSONElement::createArray("m_breakpoints");
    json.append( bparr );
    
    for(size_t i=0; i<m_breakpoints.size(); ++i) {
        bparr.arrayAppend( m_breakpoints.at(i)->ToJSON() );
    }
    return json;
}

