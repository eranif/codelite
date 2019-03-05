#include "ObjectPreview.h"
#include "PropertyPreview.h"

PropertyPreview::PropertyPreview() {}

PropertyPreview::~PropertyPreview() { wxDELETE(m_valuePreview); }

void PropertyPreview::FromJSON(const JSONItem& json)
{
    m_name = json.namedObject("name").toString();
    m_type = json.namedObject("type").toString();
    m_value = json.namedObject("value").toString();
    if(json.hasNamedObject("valuePreview")) {
        m_valuePreview = new ObjectPreview();
        m_valuePreview->FromJSON(json.namedObject("valuePreview"));
    }
}

JSONItem PropertyPreview::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("name", m_name);
    json.addProperty("type", m_type);
    json.addProperty("value", m_value);
    if(m_valuePreview) { json.append(m_valuePreview->ToJSON("valuePreview")); }
    return json;
}

wxString PropertyPreview::ToString() const
{
    wxString str;
    if(IsObject() && IsEmpty()) { return "{...}"; }
    str << GetName() << " : ";
    if(!GetSubtype().IsEmpty()) {
        str << GetSubtype();
    } else {
        str << GetType();
    }
    if(!GetValue().IsEmpty()) {
        if(IsString()) {
            str << ": \"" << GetValue() << "\"";
        } else {
            str << ": " << GetValue();
        }
    }
    if(GetValuePreview()) { str << "[ " << GetValuePreview()->ToString() << "]"; }
    return str;
}
