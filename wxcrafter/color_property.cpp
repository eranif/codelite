#include "color_property.h"
#include "json_node.h"
#include "wxgui_globals.h"
#include "wxgui_helpers.h"
#include <wx/app.h>

const wxEventType wxEVT_COLOR_UPDATE = wxNewEventType();

ColorProperty::ColorProperty(const wxString& label, const wxString& colour, const wxString& tooltip)
    : PropertyBase(tooltip)
{
    SetLabel(label);
    m_colourName = colour;
}

ColorProperty::~ColorProperty() {}

wxString ColorProperty::GetValue() const { return m_colourName; }

JSONElement ColorProperty::Serialize() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty(wxT("type"), wxT("colour"));
    DoBaseSerialize(json);
    json.addProperty(wxT("colour"), m_colourName);
    return json;
}

void ColorProperty::SetValue(const wxString& value) { m_colourName = value; }

void ColorProperty::UnSerialize(const JSONElement& json)
{
    DoBaseUnSerialize(json);
    SetValue(json.namedObject(wxT("colour")).toString());
}
