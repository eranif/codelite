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
    virtual ~StringProperty();

public:
    virtual wxString GetValue() const;
    virtual void SetValue(const wxString& value);
    virtual PropertyeType GetType() { return PT_MUTLI_STRING_PICKER; }
    void operator=(const wxString& src) { m_value = src; }
    virtual JSONElement Serialize() const;
    virtual void UnSerialize(const JSONElement& json);
};

#endif // STRINGPROPERTY_H
