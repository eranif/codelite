#ifndef VIRTUALFOLDERPROPERTY_H
#define VIRTUALFOLDERPROPERTY_H

#include "property_base.h" // Base class: PropertyBase

class VirtualFolderProperty : public PropertyBase
{
    wxString m_path;

public:
    VirtualFolderProperty(const wxString& label, const wxString& path, const wxString& tooltip);
    ~VirtualFolderProperty() override = default;

public:
    wxString GetValue() const override;
    nlohmann::json Serialize() const override;
    void SetValue(const wxString& value) override;
    void UnSerialize(const nlohmann::json& json) override;
    PropertyeType GetType() override { return PT_VIRTUAL_FOLDER_PICKER; }
};

#endif // VIRTUALFOLDERPROPERTY_H
