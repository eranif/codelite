#ifndef BITMAPPICKERPROPERTY_H
#define BITMAPPICKERPROPERTY_H

#include "property_base.h" // Base class: PropertyBase

class BitmapPickerProperty : public PropertyBase
{
    wxString m_path;

public:
    BitmapPickerProperty(const wxString& label, const wxString& path, const wxString& tooltip);
    ~BitmapPickerProperty() override = default;

public:
    PropertyeType GetType() override;
    wxString GetValue() const override;
    JSONElement Serialize() const override;
    void SetValue(const wxString& value) override;
    void UnSerialize(const JSONElement& json) override;
};

#endif // BITMAPPICKERPROPERTY_H
