#include "PropertyPreview.h"
#include "RemoteObject.h"

RemoteObject::RemoteObject() {}

RemoteObject::~RemoteObject() {}

void RemoteObject::FromJSON(const JSONItem& json)
{
    m_type = json.namedObject("type").toString();
    m_subtype = json.namedObject("subtype").toString();
    m_className = json.namedObject("className").toString();
    m_objectId = json.namedObject("objectId").toString();
    JSONItem value = json.namedObject("value");
    if(value.isNull()) {
        m_value = "null";
    } else if(value.isBool()) {
        m_value << (value.toBool() ? "true" : "false");
    } else if(value.isNumber()) {
        m_value << value.toInt();
    } else {
        m_value << value.toString();
    }
    if(json.hasNamedObject("preview")) { m_preview.FromJSON(json.namedObject("preview")); }
}

JSONItem RemoteObject::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("type", m_type);
    json.addProperty("subtype", m_subtype);
    json.addProperty("className", m_className);
    json.addProperty("value", m_value);
    json.addProperty("objectId", m_objectId);
    if(!m_preview.IsEmpty()) { json.append(m_preview.ToJSON("preview")); }
    return json;
}

wxString RemoteObject::ToString() const
{
    wxString str;
    if(IsFunction()) {
        return "Function";
    } else if(IsObject()) {
        str << this->GetClassName() << " : " << GetPreview().ToString();

    } else if(IsString()) {
        // Simple type
        str << "\"" << GetValue() << "\"";
    } else if(IsUndefined()) {
        str << "undefined";
    } else {
        str << GetValue();
    }
    return str;
}

wxString RemoteObject::GetTextPreview() const
{
    if(IsObject()) { return "{...}"; }
    return ToString();
}

bool RemoteObject::HasChildren() const { return IsObject(); }
