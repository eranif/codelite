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
    virtual ~IntProperty();

    bool IsUninitialized() const { return m_uninitialized; }

public:
    virtual PropertyeType GetType();
    virtual wxString GetValue() const;
    virtual JSONElement Serialize() const;
    virtual void SetValue(const wxString& value);
    virtual void UnSerialize(const JSONElement& json);
};

#endif // INTPROPERTY_H
