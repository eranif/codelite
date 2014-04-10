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
}

JSONElement LLDBCommand::ToJSON() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty("m_commandType", m_commandType);
    json.addProperty("m_commandArguments", m_commandArguments);
    json.addProperty("m_workingDirectory", m_workingDirectory);
    json.addProperty("m_executable", m_executable);
    json.addProperty("m_redirectTTY", m_redirectTTY);
    return json;
}

