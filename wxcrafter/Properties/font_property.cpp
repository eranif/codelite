#include "font_property.h"

#include "json_utils.h"

const wxEventType wxEVT_FONT_UPDATE = wxNewEventType();

FontProperty::FontProperty(const wxString& label, const wxString& font, const wxString& tooltip)
    : PropertyBase(tooltip)
{
    SetLabel(label);
    m_value = font;
}

wxString FontProperty::GetValue() const { return m_value; }

nlohmann::json FontProperty::Serialize() const
{
    nlohmann::json json = {{"type", "font"}};
    DoBaseSerialize(json);
    json["m_value"] = m_value;
    return json;
}

void FontProperty::UnSerialize(const nlohmann::json& json)
{
    if (!json.is_object()) {
        return;
    }
    DoBaseUnSerialize(json);
    m_value = JsonUtils::ToString(json["m_value"]);
}

void FontProperty::SetValue(const wxString& value) { m_value = value; }
