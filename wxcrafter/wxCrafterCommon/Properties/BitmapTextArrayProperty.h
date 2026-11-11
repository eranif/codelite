#pragma once

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
    JSONItem Serialize() const override;
    void SetValue(const wxString& value) override;
    void UnSerialize(const JSONItem& json) override;
};
