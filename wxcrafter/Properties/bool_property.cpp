#include "bool_property.h"

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

nlohmann::json BoolProperty::Serialize() const
{
    nlohmann::json json = {{"type", "bool"}};
    DoBaseSerialize(json);
    json["m_value"] = m_value;
    return json;
}

void BoolProperty::UnSerialize(const nlohmann::json& json)
{
    if (!json.is_object()) {
        return;
    }
    DoBaseUnSerialize(json);
    m_value = json.value("m_value", m_value);
}
