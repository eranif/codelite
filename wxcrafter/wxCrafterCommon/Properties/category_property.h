#pragma once

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
    JSONItem Serialize() const override;
    void SetValue(const wxString& value) override;
    void UnSerialize(const JSONItem& json) override;
};
