#include "BitmapTextArrayProperty.h"
#include <json_node.h>

BitmapTextArrayProperty::BitmapTextArrayProperty(const wxString& label, const wxString& value, const wxString& tooltip)
    : PropertyBase(tooltip)
{
    SetLabel(label);
    m_value = value;
}

BitmapTextArrayProperty::~BitmapTextArrayProperty() {}

PropertyeType BitmapTextArrayProperty::GetType() { return PT_BITMAPTEXT_TABLE; }

wxString BitmapTextArrayProperty::GetValue() const { return m_value; }

JSONElement BitmapTextArrayProperty::Serialize() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty(wxT("type"), wxT("BitmapTextArray"));
    DoBaseSerialize(json);
    json.addProperty(wxT("m_value"), m_value);
    return json;
}

void BitmapTextArrayProperty::SetValue(const wxString& value) { m_value = value; }

void BitmapTextArrayProperty::UnSerialize(const JSONElement& json)
{
    DoBaseUnSerialize(json);
    m_value = json.namedObject(wxT("m_value")).toString();
}
