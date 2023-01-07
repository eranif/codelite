#include "font_property.h"
#include "FontPickerDlg.h"
#include "wxgui_helpers.h"
#include <wx/app.h>

const wxEventType wxEVT_FONT_UPDATE = wxNewEventType();

FontProperty::FontProperty(const wxString& label, const wxString& font, const wxString& tooltip)
    : PropertyBase(tooltip)
{
    SetLabel(label);
    m_value = font;
}

FontProperty::~FontProperty() {}

wxString FontProperty::GetValue() const { return m_value; }

JSONElement FontProperty::Serialize() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty(wxT("type"), wxT("font"));
    DoBaseSerialize(json);
    json.addProperty(wxT("m_value"), m_value);
    return json;
}

void FontProperty::UnSerialize(const JSONElement& json)
{
    DoBaseUnSerialize(json);
    m_value = json.namedObject(wxT("m_value")).toString();
}

void FontProperty::SetValue(const wxString& value) { m_value = value; }
