#include "color_property.h"

#include "JSON.h"

const wxEventType wxEVT_COLOR_UPDATE = wxNewEventType();

ColorProperty::ColorProperty(const wxString& label, const wxString& colour, const wxString& tooltip)
    : PropertyBase(tooltip)
{
    SetLabel(label);
    m_colourName = colour;
}

wxString ColorProperty::GetValue() const { return m_colourName; }

JSONItem ColorProperty::Serialize() const
{
    JSONItem json = JSONItem::createObject();
    json.addProperty(wxT("type"), wxT("colour"));
    DoBaseSerialize(json);
    json.addProperty(wxT("colour"), m_colourName);
    return json;
}

void ColorProperty::SetValue(const wxString& value) { m_colourName = value; }

void ColorProperty::UnSerialize(const JSONItem& json)
{
    DoBaseUnSerialize(json);
    SetValue(json.namedObject(wxT("colour")).toString());
}
