#include "string_property.h"

StringProperty::StringProperty()
    : MultiStringsProperty("", "", "\n", "")
{
}

wxString StringProperty::GetValue() const { return m_value; }

void StringProperty::SetValue(const wxString& value) { m_value = value; }

JSONItem StringProperty::Serialize() const
{
    JSONItem json = JSONItem::createObject();
    json.addProperty(wxT("type"), wxT("string"));
    DoBaseSerialize(json);
    json.addProperty(wxT("m_value"), m_value);
    return json;
}

void StringProperty::UnSerialize(const JSONItem& json)
{
    DoBaseUnSerialize(json);
    m_value = json.namedObject(wxT("m_value")).toString();
}
