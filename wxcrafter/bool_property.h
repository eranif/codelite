#ifndef BOOLPROPERTY_H
#define BOOLPROPERTY_H

#include "property_base.h" // Base class: PropertyBase

class BoolProperty : public PropertyBase
{
    bool m_value;

public:
    BoolProperty(const wxString& label, bool checked, const wxString& tooltip);
    BoolProperty();
    virtual ~BoolProperty();

public:
    virtual wxString GetValue() const;
    virtual void SetValue(const wxString& value);
    virtual JSONElement Serialize() const;
    virtual void UnSerialize(const JSONElement& json);
    virtual PropertyeType GetType() { return PT_BOOL; }
};

#endif // BOOLPROPERTY_H
