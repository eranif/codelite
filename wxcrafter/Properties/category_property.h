#ifndef CATEGORYPROPERTY_H
#define CATEGORYPROPERTY_H

#include "property_base.h" // Base class: PropertyBase

class CategoryProperty : public PropertyBase
{
    wxString m_value;

public:
    CategoryProperty(const wxString& name, const wxString& label = "");
    ~CategoryProperty() override = default;

public:
    PropertyeType GetType() override;
    wxString GetValue() const override;
    JSONElement Serialize() const override;
    void SetValue(const wxString& value) override;
    void UnSerialize(const JSONElement& json) override;
};

#endif // CATEGORYPROPERTY_H
