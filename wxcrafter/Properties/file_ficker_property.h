#ifndef FILEFICKERPROPERTY_H
#define FILEFICKERPROPERTY_H

#include "property_base.h" // Base class: PropertyBase

class FilePickerProperty : public PropertyBase
{
protected:
    wxString m_path;

public:
    FilePickerProperty(const wxString& label, const wxString& path, const wxString& tooltip);
    FilePickerProperty();
    ~FilePickerProperty() override = default;

public:
    void FixPaths(const wxString& cwd);

    wxString GetValue() const override;
    JSONItem Serialize() const override;
    void SetValue(const wxString& value) override;
    void UnSerialize(const JSONItem& json) override;
    PropertyeType GetType() override { return PT_FILE_PICKER; }
};

#endif // FILEFICKERPROPERTY_H
