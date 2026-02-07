#ifndef INTPROPERTY_H
#define INTPROPERTY_H

#include "property_base.h" // Base class: PropertyBase

class IntProperty : public PropertyBase
{
    int m_value;
    bool m_uninitialized;

public:
    IntProperty()
        : PropertyBase("")
        , m_value(-1)
        , m_uninitialized(true)
    {
    }

    IntProperty(const wxString& label, int value, const wxString& tooltip);
    IntProperty(const wxString& label, const wxString& tooltip);
    ~IntProperty() override = default;

    bool IsUninitialized() const { return m_uninitialized; }

public:
    PropertyeType GetType() override;
    wxString GetValue() const override;
    JSONItem Serialize() const override;
    void SetValue(const wxString& value) override;
    void UnSerialize(const JSONItem& json) override;
};

#endif // INTPROPERTY_H
