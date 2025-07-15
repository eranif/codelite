#ifndef BITMAPTEXTARRAYPROPERTY_H
#define BITMAPTEXTARRAYPROPERTY_H

#include "property_base.h"
class BitmapTextArrayProperty : public PropertyBase
{
protected:
    wxString m_value;

public:
    BitmapTextArrayProperty(const wxString& label, const wxString& value, const wxString& tooltip);
    ~BitmapTextArrayProperty() override = default;

    PropertyeType GetType() override;
    wxString GetValue() const override;
    JSONElement Serialize() const override;
    void SetValue(const wxString& value) override;
    void UnSerialize(const JSONElement& json) override;
};

#endif // BITMAPTEXTARRAYPROPERTY_H
