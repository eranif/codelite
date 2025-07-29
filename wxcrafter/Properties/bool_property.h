#ifndef BOOLPROPERTY_H
#define BOOLPROPERTY_H

#include "property_base.h" // Base class: PropertyBase

class BoolProperty : public PropertyBase
{
    bool m_value;

public:
    BoolProperty(const wxString& label, bool checked, const wxString& tooltip);
    BoolProperty();
    ~BoolProperty() override = default;

public:
    wxString GetValue() const override;
    void SetValue(const wxString& value) override;
    JSONElement Serialize() const override;
    void UnSerialize(const JSONElement& json) override;
    PropertyeType GetType() override { return PT_BOOL; }
};

#endif // BOOLPROPERTY_H
