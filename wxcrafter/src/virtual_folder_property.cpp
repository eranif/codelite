#include "virtual_folder_property.h"
#include "virtual_folder_picker_ctrl.h"

VirtualFolderProperty::VirtualFolderProperty(const wxString& label, const wxString& path, const wxString& tooltip)
    : PropertyBase(tooltip)
{
    wxString tip;
    tip << GetTooltip() << _("\n ** Click to select different virtual folder **");

    SetTooltip(tip);
    SetLabel(label);
    SetValue(path);
}

VirtualFolderProperty::~VirtualFolderProperty() {}

wxString VirtualFolderProperty::GetValue() const { return m_path; }

JSONElement VirtualFolderProperty::Serialize() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty(wxT("type"), wxT("virtualFolderPicker"));
    DoBaseSerialize(json);
    json.addProperty(wxT("m_path"), m_path);
    return json;
}

void VirtualFolderProperty::SetValue(const wxString& value) { m_path = value; }

void VirtualFolderProperty::UnSerialize(const JSONElement& json)
{
    DoBaseUnSerialize(json);
    m_path = json.namedObject(wxT("m_path")).toString();
}
