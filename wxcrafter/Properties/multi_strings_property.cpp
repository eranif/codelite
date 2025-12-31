#include "multi_strings_property.h"

#include "json_utils.h"

MultiStringsProperty::MultiStringsProperty(const wxString& label, const wxString& tooltip, const wxString& delim,
                                           const wxString& msg)
    : PropertyBase(tooltip)
    , m_delim(delim)
    , m_msg(msg)
{
    SetLabel(label);
}

wxString MultiStringsProperty::GetValue() const { return m_value; }

nlohmann::json MultiStringsProperty::Serialize() const
{
    nlohmann::json element = {{"type", "multi-string"}};
    DoBaseSerialize(element);
    element["m_value"] = m_value;
    return element;
}

void MultiStringsProperty::SetValue(const wxString& value) { m_value = value; }

void MultiStringsProperty::UnSerialize(const nlohmann::json& json)
{
    if (!json.is_object()) {
        return;
    }
    DoBaseUnSerialize(json);
    m_value = JsonUtils::ToString(json["m_value"]);
}
