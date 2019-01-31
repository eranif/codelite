#include "string_property.h"
#include <wx/textctrl.h>

StringProperty::StringProperty()
    : MultiStringsProperty("", "", "\n", "")
{
}

StringProperty::~StringProperty() {}

wxString StringProperty::GetValue() const { return m_value; }

void StringProperty::SetValue(const wxString& value) { m_value = value; }

JSONElement StringProperty::Serialize() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty(wxT("type"), wxT("string"));
    DoBaseSerialize(json);
    json.addProperty(wxT("m_value"), m_value);
    return json;
}

void StringProperty::UnSerialize(const JSONElement& json)
{
    DoBaseUnSerialize(json);
    m_value = json.namedObject(wxT("m_value")).toString();
}
