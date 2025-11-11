#include "BitmapTextArrayProperty.h"

#include "json_utils.h"

BitmapTextArrayProperty::BitmapTextArrayProperty(const wxString& label, const wxString& value, const wxString& tooltip)
    : PropertyBase(tooltip)
{
    SetLabel(label);
    m_value = value;
}

PropertyeType BitmapTextArrayProperty::GetType() { return PT_BITMAPTEXT_TABLE; }

wxString BitmapTextArrayProperty::GetValue() const { return m_value; }

nlohmann::json BitmapTextArrayProperty::Serialize() const
{
    nlohmann::json json = {{"type", "BitmapTextArray"}};
    DoBaseSerialize(json);
    json["m_value"] = m_value;
    return json;
}

void BitmapTextArrayProperty::SetValue(const wxString& value) { m_value = value; }

void BitmapTextArrayProperty::UnSerialize(const nlohmann::json& json)
{
    if (!json.is_object()) {
        return;
    }
    DoBaseUnSerialize(json);
    m_value = JsonUtils::ToString(json["m_value"]);
}
