#ifndef BITMAPPICKERPROPERTY_H
#define BITMAPPICKERPROPERTY_H

#include "property_base.h" // Base class: PropertyBase

class BitmapPickerProperty : public PropertyBase
{
    wxString m_path;

public:
    BitmapPickerProperty(const wxString& label, const wxString& path, const wxString& tooltip);
    virtual ~BitmapPickerProperty();

public:
    virtual PropertyeType GetType();
    virtual wxString GetValue() const;
    virtual JSONElement Serialize() const;
    virtual void SetValue(const wxString& value);
    virtual void UnSerialize(const JSONElement& json);
};

#endif // BITMAPPICKERPROPERTY_H
