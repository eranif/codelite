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

PropertyeType CategoryProperty::GetType() { return PT_CATEGORY; }

wxString CategoryProperty::GetValue() const { return m_value; }

nlohmann::json CategoryProperty::Serialize() const { return nullptr; }

void CategoryProperty::SetValue(const wxString& value) { m_value = value; }

void CategoryProperty::UnSerialize(const nlohmann::json&) {}
