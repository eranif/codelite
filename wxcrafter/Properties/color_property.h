#ifndef COLORPROPERTY_H
#define COLORPROPERTY_H

#include "property_base.h" // Base class: PropertyBase

extern const wxEventType wxEVT_COLOR_UPDATE;

class ColorProperty : public PropertyBase
{
    wxString m_colourName;

public:
    ColorProperty(const wxString& label, const wxString& colour = wxT("<Default>"), const wxString& tooltip = "");
    ~ColorProperty() override = default;

public:
    wxString GetValue() const override;
    JSONElement Serialize() const override;
    void SetValue(const wxString& value) override;
    void UnSerialize(const JSONElement& json) override;
    PropertyeType GetType() override { return PT_COLOR_PICKER; }
};

#endif // COLORPROPERTY_H
