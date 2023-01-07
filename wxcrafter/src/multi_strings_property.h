#ifndef MULTISTRINGSPROPERTY_H
#define MULTISTRINGSPROPERTY_H

#include "property_base.h" // Base class: PropertyBase

class MultiStringsProperty : public PropertyBase
{
    wxString m_delim;
    wxString m_msg;

protected:
    wxString m_value;

public:
    MultiStringsProperty(const wxString& label, const wxString& tooltip, const wxString& delim = wxT(";"),
                         const wxString& msg = wxT(""));
    virtual ~MultiStringsProperty();

public:
    virtual wxString GetValue() const;
    virtual JSONElement Serialize() const;
    virtual void SetValue(const wxString& value);
    virtual void UnSerialize(const JSONElement& json);
    virtual PropertyeType GetType() { return PT_MUTLI_STRING_PICKER; }
};

#endif // MULTISTRINGSPROPERTY_H
