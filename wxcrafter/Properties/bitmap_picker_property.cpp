#include "bitmap_picker_property.h"

#include "json_utils.h"

BitmapPickerProperty::BitmapPickerProperty(const wxString& label, const wxString& path, const wxString& tooltip)
    : PropertyBase(tooltip)
{
    wxString tip;
    tip << GetTooltip();

    SetTooltip(tip);
    SetLabel(label);
    SetValue(path);
}

PropertyeType BitmapPickerProperty::GetType() { return PT_BITMAP; }

wxString BitmapPickerProperty::GetValue() const { return m_path; }

nlohmann::json BitmapPickerProperty::Serialize() const
{
    nlohmann::json json = {{"type", "bitmapPicker"}};
    DoBaseSerialize(json);
    json["m_path"] = m_path;
    return json;
}

void BitmapPickerProperty::SetValue(const wxString& value) { m_path = value; }

void BitmapPickerProperty::UnSerialize(const nlohmann::json& json)
{
    if (!json.is_object()) {
        return;
    }
    DoBaseUnSerialize(json);
    m_path = JsonUtils::ToString(json["m_path"]);
}
