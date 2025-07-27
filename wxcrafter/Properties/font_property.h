#ifndef FONTPROPERTY_H
#define FONTPROPERTY_H

#include "property_base.h"

extern const wxEventType wxEVT_FONT_UPDATE;

class FontProperty : public PropertyBase
{
protected:
    wxString m_value;

public:
    FontProperty(const wxString& label, const wxString& font, const wxString& tooltip);
    ~FontProperty() override = default;

public:
    wxString GetValue() const override;
    JSONElement Serialize() const override;
    void SetValue(const wxString& value) override;
    void UnSerialize(const JSONElement& json) override;
    PropertyeType GetType() override { return PT_FONT_PICKER; }
};

#endif // FONTPROPERTY_H
