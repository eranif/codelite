#include "multi_strings_property.h"
#include "multi_string_ctrl.h"

MultiStringsProperty::MultiStringsProperty(const wxString& label, const wxString& tooltip, const wxString& delim,
                                           const wxString& msg)
    : PropertyBase(tooltip)
    , m_delim(delim)
    , m_msg(msg)
{
    SetLabel(label);
}

MultiStringsProperty::~MultiStringsProperty() {}

wxString MultiStringsProperty::GetValue() const { return m_value; }

JSONElement MultiStringsProperty::Serialize() const
{
    JSONElement element = JSONElement::createObject();
    element.addProperty(wxT("type"), wxT("multi-string"));
    DoBaseSerialize(element);
    element.addProperty(wxT("m_value"), m_value);
    return element;
}

void MultiStringsProperty::SetValue(const wxString& value) { m_value = value; }

void MultiStringsProperty::UnSerialize(const JSONElement& json)
{
    DoBaseUnSerialize(json);
    m_value = json.namedObject(wxT("m_value")).toString();
}
