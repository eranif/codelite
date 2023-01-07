#include "category_property.h"

CategoryProperty::CategoryProperty(const wxString& name, const wxString& label)
    : PropertyBase(name)
    , m_value(name)
{
    if(label.IsEmpty())
        SetLabel(name);
    else
        SetLabel(label);
}

CategoryProperty::~CategoryProperty() {}

PropertyeType CategoryProperty::GetType() { return PT_CATEGORY; }

wxString CategoryProperty::GetValue() const { return m_value; }

JSONElement CategoryProperty::Serialize() const { return JSONElement(NULL); }

void CategoryProperty::SetValue(const wxString& value) { m_value = value; }

void CategoryProperty::UnSerialize(const JSONElement& json) { wxUnusedVar(json); }
