#include "string_property.h"

#include "json_utils.h"

StringProperty::StringProperty()
    : MultiStringsProperty("", "", "\n", "")
{
}

wxString StringProperty::GetValue() const { return m_value; }

void StringProperty::SetValue(const wxString& value) { m_value = value; }

nlohmann::json StringProperty::Serialize() const
{
    nlohmann::json json = {{"type", "string"}};
    DoBaseSerialize(json);
    json["m_value"] = m_value;
    return json;
}

void StringProperty::UnSerialize(const nlohmann::json& json)
{
    if (!json.is_object()) {
        return;
    }
    DoBaseUnSerialize(json);
    m_value = JsonUtils::ToString(json["m_value"]);
}
