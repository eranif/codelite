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
    m_transcationId = json.namedObject("m_transcationId").toInt(0);
    m_commandArguments = json.namedObject("m_commandArguments").toString();
    m_workingDirectory = json.namedObject("m_workingDirectory").toString();
}

JSONElement LLDBCommand::ToJSON() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty("m_commandType", m_commandType);
    json.addProperty("m_transcationId", m_transcationId);
    json.addProperty("m_commandArguments", m_commandArguments);
    json.addProperty("m_workingDirectory", m_workingDirectory);
    return json;
}

