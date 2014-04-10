#include "LLDBReply.h"
#include "LLDBEnums.h"

LLDBReply::~LLDBReply()
{
}

LLDBReply::LLDBReply(const wxString& str)
{
    JSONRoot root(str);
    FromJSON( root.toElement() );
}

void LLDBReply::FromJSON(const JSONElement& json)
{
    m_transcationId   = json.namedObject("m_transcationId").toInt(0);
    m_replyType       = json.namedObject("m_replyType").toInt(kTypeInvalid);
    m_interruptResaon = json.namedObject("m_stopResaon").toInt(kInterruptReasonNone);
    m_line            = json.namedObject("m_line").toInt(wxNOT_FOUND);
    m_filename        = json.namedObject("m_filename").toString();
}

JSONElement LLDBReply::ToJSON() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty("m_transcationId", m_transcationId);
    json.addProperty("m_replyType",     m_replyType);
    json.addProperty("m_stopResaon",    m_interruptResaon);
    json.addProperty("m_line",          m_line);
    json.addProperty("m_filename",      m_filename);
    return json;
}

