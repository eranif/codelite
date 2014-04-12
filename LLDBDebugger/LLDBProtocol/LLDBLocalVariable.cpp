#include "LLDBLocalVariable.h"
#include <lldb/API/SBValue.h>

LLDBLocalVariable::LLDBLocalVariable(lldb::SBValue value)
{
    DoInitFromLLDBValue( value );
}

LLDBLocalVariable::~LLDBLocalVariable()
{
}

void LLDBLocalVariable::DoInitFromLLDBValue(lldb::SBValue value)
{
    SetName( value.GetName() );
    SetType( value.GetTypeName() );
    SetSummary( value.GetSummary() );
    SetValue( value.GetValue() );
    SetValueChanged( value.GetValueDidChange() );
    SetLldbId( value.GetID() );
    
    if ( value.MightHaveChildren() ) {
        m_hasChildren = true;
    }
}

void LLDBLocalVariable::FromJSON(const JSONElement& json)
{
    m_name = json.namedObject("m_name").toString();
    m_value = json.namedObject("m_value").toString();
    m_summary = json.namedObject("m_summary").toString();
    m_type = json.namedObject("m_type").toString();
    m_valueChanged = json.namedObject("m_valueChanged").toBool(false);
    m_lldbId = json.namedObject("m_lldbId").toInt();
    m_hasChildren = json.namedObject("m_hasChildren").toBool(false);
}

JSONElement LLDBLocalVariable::ToJSON() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty("m_name", m_name);
    json.addProperty("m_value", m_value);
    json.addProperty("m_summary", m_summary);
    json.addProperty("m_type", m_type);
    json.addProperty("m_valueChanged", m_valueChanged);
    json.addProperty("m_lldbId", m_lldbId);
    json.addProperty("m_hasChildren", m_hasChildren);
    
    return json;
}
