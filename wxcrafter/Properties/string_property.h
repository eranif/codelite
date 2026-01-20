#ifndef STRINGPROPERTY_H
#define STRINGPROPERTY_H

#include "multi_strings_property.h"
#include "property_base.h" // Base class: PropertyBase

class StringProperty : public MultiStringsProperty
{
public:
    StringProperty(const wxString& label, const wxString& value, const wxString& tooltip)
        : MultiStringsProperty(label, tooltip, "\n", "")
    {
        m_value = value;
    }

    StringProperty(const wxString& label, const wxString& tooltip)
        : MultiStringsProperty(label, tooltip, "\n", "")
    {
    }

    StringProperty();
    ~StringProperty() override = default;

    void operator=(const wxString& src) { m_value = src; }
public:
    wxString GetValue() const override;
    void SetValue(const wxString& value) override;
    PropertyeType GetType() override { return PT_MULTI_STRING_PICKER; }
    nlohmann::json Serialize() const override;
    void UnSerialize(const nlohmann::json& json) override;
};

#endif // STRINGPROPERTY_H
