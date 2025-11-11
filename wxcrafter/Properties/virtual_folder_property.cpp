#include "virtual_folder_property.h"

#include "json_utils.h"

VirtualFolderProperty::VirtualFolderProperty(const wxString& label, const wxString& path, const wxString& tooltip)
    : PropertyBase(tooltip)
{
    wxString tip;
    tip << GetTooltip() << _("\n ** Click to select different virtual folder **");

    SetTooltip(tip);
    SetLabel(label);
    SetValue(path);
}

wxString VirtualFolderProperty::GetValue() const { return m_path; }

nlohmann::json VirtualFolderProperty::Serialize() const
{
    nlohmann::json json = {{"type", "virtualFolderPicker"}};
    DoBaseSerialize(json);
    json["m_path"] = m_path;
    return json;
}

void VirtualFolderProperty::SetValue(const wxString& value) { m_path = value; }

void VirtualFolderProperty::UnSerialize(const nlohmann::json& json)
{
    if (!json.is_object()) {
        return;
    }
    DoBaseUnSerialize(json);
    m_path = JsonUtils::ToString(json["m_path"]);
}
