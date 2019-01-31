#ifndef CATEGORYPROPERTY_H
#define CATEGORYPROPERTY_H

#include "property_base.h" // Base class: PropertyBase

class CategoryProperty : public PropertyBase
{
    wxString m_value;

public:
    CategoryProperty(const wxString& name, const wxString& label = "");
    virtual ~CategoryProperty();

public:
    virtual PropertyeType GetType();
    virtual wxString GetValue() const;
    virtual JSONElement Serialize() const;
    virtual void SetValue(const wxString& value);
    virtual void UnSerialize(const JSONElement& json);
};

#endif // CATEGORYPROPERTY_H
