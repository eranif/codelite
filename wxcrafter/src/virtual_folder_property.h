#ifndef VIRTUALFOLDERPROPERTY_H
#define VIRTUALFOLDERPROPERTY_H

#include "property_base.h" // Base class: PropertyBase

class VirtualFolderProperty : public PropertyBase
{
    wxString m_path;

public:
    VirtualFolderProperty(const wxString& label, const wxString& path, const wxString& tooltip);
    virtual ~VirtualFolderProperty();

public:
    virtual wxString GetValue() const;
    virtual JSONElement Serialize() const;
    virtual void SetValue(const wxString& value);
    virtual void UnSerialize(const JSONElement& json);
    virtual PropertyeType GetType() { return PT_VIRTUAL_FOLDER_PICKER; }
};

#endif // VIRTUALFOLDERPROPERTY_H
