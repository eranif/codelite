#ifndef COLORPROPERTY_H
#define COLORPROPERTY_H

#include "property_base.h" // Base class: PropertyBase

extern const wxEventType wxEVT_COLOR_UPDATE;

class ColorProperty : public PropertyBase
{
    wxString m_colourName;

public:
    ColorProperty(const wxString& label, const wxString& colour = wxT("<Default>"), const wxString& tooltip = "");
    virtual ~ColorProperty();

public:
    virtual wxString GetValue() const;
    virtual JSONElement Serialize() const;
    virtual void SetValue(const wxString& value);
    virtual void UnSerialize(const JSONElement& json);
    virtual PropertyeType GetType() { return PT_COLOR_PICKER; }
};

#endif // COLORPROPERTY_H
