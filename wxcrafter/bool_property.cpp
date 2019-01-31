#include "bool_property.h"
#include <wx/checkbox.h>

BoolProperty::BoolProperty(const wxString& label, bool checked, const wxString& tooltip)
    : PropertyBase(tooltip)
    , m_value(checked)
{
    SetLabel(label);
}

BoolProperty::BoolProperty()
    : PropertyBase(wxT(""))
{
}

BoolProperty::~BoolProperty() {}

wxString BoolProperty::GetValue() const
{
    if(m_value) {
        return wxT("1");

    } else {
        return wxT("0");
    }
}

void BoolProperty::SetValue(const wxString& value)
{
    if(value == wxT("1")) {
        m_value = true;
    } else {
        m_value = false;
    }
    NotifyChanged();
}

JSONElement BoolProperty::Serialize() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty(wxT("type"), wxT("bool"));
    DoBaseSerialize(json);
    json.addProperty(wxT("m_value"), m_value);
    return json;
}

void BoolProperty::UnSerialize(const JSONElement& json)
{
    DoBaseUnSerialize(json);
    m_value = json.namedObject(wxT("m_value")).toBool();
}
