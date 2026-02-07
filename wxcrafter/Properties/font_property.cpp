#include "font_property.h"

const wxEventType wxEVT_FONT_UPDATE = wxNewEventType();

FontProperty::FontProperty(const wxString& label, const wxString& font, const wxString& tooltip)
    : PropertyBase(tooltip)
{
    SetLabel(label);
    m_value = font;
}

wxString FontProperty::GetValue() const { return m_value; }

JSONItem FontProperty::Serialize() const
{
    JSONItem json = JSONItem::createObject();
    json.addProperty(wxT("type"), wxT("font"));
    DoBaseSerialize(json);
    json.addProperty(wxT("m_value"), m_value);
    return json;
}

void FontProperty::UnSerialize(const JSONItem& json)
{
    DoBaseUnSerialize(json);
    m_value = json.namedObject(wxT("m_value")).toString();
}

void FontProperty::SetValue(const wxString& value) { m_value = value; }
