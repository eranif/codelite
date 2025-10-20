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
    ~MultiStringsProperty() override = default;

public:
    wxString GetValue() const override;
    JSONElement Serialize() const override;
    void SetValue(const wxString& value) override;
    void UnSerialize(const JSONElement& json) override;
    PropertyeType GetType() override { return PT_MULTI_STRING_PICKER; }
};

#endif // MULTISTRINGSPROPERTY_H
