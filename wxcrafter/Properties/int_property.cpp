#include "int_property.h"

IntProperty::IntProperty(const wxString& label, int value, const wxString& tooltip)
    : PropertyBase(tooltip)
    , m_value(value)
    , m_uninitialized(false)
{
    SetLabel(label);
}

IntProperty::IntProperty(const wxString& label, const wxString& tooltip)
    : PropertyBase(tooltip)
    , m_value(-1)
    , m_uninitialized(true)
{
    SetLabel(label);
}

PropertyeType IntProperty::GetType() { return PT_INT; }

wxString IntProperty::GetValue() const
{
    wxString v;
    v << m_value;
    return v;
}

nlohmann::json IntProperty::Serialize() const
{
    nlohmann::json json = {{"type", "integer"}};
    DoBaseSerialize(json);
    json["m_value"] = m_value;
    return json;
}

void IntProperty::SetValue(const wxString& value)
{
    long i(-1);
    if(value.ToLong(&i)) {
        m_value = i;
    } else {
        m_value = -1;
    }
}

void IntProperty::UnSerialize(const nlohmann::json& json)
{
    if (!json.is_object()) {
        return;
    }
    DoBaseUnSerialize(json);
    m_value = json.value("m_value", m_value);
}
