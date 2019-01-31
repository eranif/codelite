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
    virtual ~FontProperty();

public:
    wxString GetValue() const;
    JSONElement Serialize() const;
    void SetValue(const wxString& value);
    void UnSerialize(const JSONElement& json);
    virtual PropertyeType GetType() { return PT_FONT_PICKER; }
};

#endif // FONTPROPERTY_H
