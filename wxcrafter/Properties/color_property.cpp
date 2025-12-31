#include "color_property.h"

#include "json_utils.h"

const wxEventType wxEVT_COLOR_UPDATE = wxNewEventType();

ColorProperty::ColorProperty(const wxString& label, const wxString& colour, const wxString& tooltip)
    : PropertyBase(tooltip)
{
    SetLabel(label);
    m_colourName = colour;
}

wxString ColorProperty::GetValue() const { return m_colourName; }

nlohmann::json ColorProperty::Serialize() const
{
    nlohmann::json json = {{"type", "colour"}};
    DoBaseSerialize(json);
    json["colour"] = m_colourName;
    return json;
}

void ColorProperty::SetValue(const wxString& value) { m_colourName = value; }

void ColorProperty::UnSerialize(const nlohmann::json& json)
{
    if (!json.is_object()) {
        return;
    }
    DoBaseUnSerialize(json);
    SetValue(JsonUtils::ToString(json["colour"]));
}
