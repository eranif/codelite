#include "file_ficker_property.h"

#include "json_utils.h"
#include "wxc_project_metadata.h"

#include <wx/filename.h>

FilePickerProperty::FilePickerProperty(const wxString& label, const wxString& path, const wxString& tooltip)
    : PropertyBase(tooltip)
{
    wxString tip;
    tip << GetTooltip();

    SetTooltip(tip);
    SetLabel(label);
    SetValue(path);
}

FilePickerProperty::FilePickerProperty()
    : PropertyBase(wxT(""))
{
}

wxString FilePickerProperty::GetValue() const { return m_path; }

nlohmann::json FilePickerProperty::Serialize() const
{
    nlohmann::json json = {{"type", "filePicker"}};
    DoBaseSerialize(json);
    json["m_path"] = m_path;
    return json;
}

void FilePickerProperty::SetValue(const wxString& value) { m_path = value; }

void FilePickerProperty::UnSerialize(const nlohmann::json& json)
{
    if (!json.is_object()) {
        return;
    }
    DoBaseUnSerialize(json);
    m_path = JsonUtils::ToString(json["m_path"]);
}

void FilePickerProperty::FixPaths(const wxString& cwd)
{
    wxString wd = cwd;
    if(wd.IsEmpty()) { wd = wxcProjectMetadata::Get().GetProjectPath(); }

    wxFileName fn(m_path);
    if(fn.IsAbsolute() && wd.IsEmpty() == false) {
        fn.MakeRelativeTo(wd);
        m_path = fn.GetFullPath();
    }
}
