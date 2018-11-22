#include "PropertyDescriptor.h"

PropertyDescriptor::PropertyDescriptor() {}

PropertyDescriptor::~PropertyDescriptor() {}

void PropertyDescriptor::FromJSON(const JSONElement& json)
{
    m_name = json.namedObject("name").toString();
    if(json.hasNamedObject("value")) { m_value.FromJSON(json.namedObject("value")); }
}

JSONElement PropertyDescriptor::ToJSON(const wxString& name) const
{
    JSONElement json = JSONElement::createObject(name);
    json.addProperty("name", m_name);
    if(!m_value.IsEmpty()) { json.append(m_value.ToJSON("value")); }
    return json;
}

wxString PropertyDescriptor::ToString() const
{
    wxString str = GetName();
    if(!GetValue().IsEmpty()) { str << ": " << GetValue().ToString(); }
    return str;
}

bool PropertyDescriptor::IsEmpty() const { return GetName().IsEmpty(); }

wxString PropertyDescriptor::GetTextPreview() const
{
    if(GetValue().IsObject()) { return "{...}"; }
    return GetValue().ToString();
}

bool PropertyDescriptor::HasChildren() const { return GetValue().IsObject(); }
