#include "LLDBReply.h"

LLDBReply::~LLDBReply()
{
}

void LLDBReply::FromJSON(const JSONElement& json)
{
    m_transcationId = json.namedObject("m_transcationId").toInt(0);
    m_reasonCode = json.namedObject("m_reasonCode").toInt(kReasonInvalid);
}

JSONElement LLDBReply::ToJSON() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty("m_transcationId", m_transcationId);
    json.addProperty("m_reasonCode", m_reasonCode);
    return json;
}

