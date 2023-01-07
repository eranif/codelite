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
    virtual ~FilePickerProperty();

public:
    void FixPaths(const wxString& cwd);
    virtual wxString GetValue() const;
    virtual JSONElement Serialize() const;
    virtual void SetValue(const wxString& value);
    virtual void UnSerialize(const JSONElement& json);
    virtual PropertyeType GetType() { return PT_FILE_PICKER; }
};

#endif // FILEFICKERPROPERTY_H
