#include "bitmap_picker_property.h"

#include "JSON.h"

BitmapPickerProperty::BitmapPickerProperty(const wxString& label, const wxString& path, const wxString& tooltip)
    : PropertyBase(tooltip)
{
    wxString tip;
    tip << GetTooltip();

    SetTooltip(tip);
    SetLabel(label);
    SetValue(path);
}

PropertyeType BitmapPickerProperty::GetType() { return PT_BITMAP; }

wxString BitmapPickerProperty::GetValue() const { return m_path; }

JSONItem BitmapPickerProperty::Serialize() const
{
    JSONItem json = JSONItem::createObject();
    json.addProperty(wxT("type"), wxT("bitmapPicker"));
    DoBaseSerialize(json);
    json.addProperty(wxT("m_path"), m_path);
    return json;
}

void BitmapPickerProperty::SetValue(const wxString& value) { m_path = value; }

void BitmapPickerProperty::UnSerialize(const JSONItem& json)
{
    DoBaseUnSerialize(json);
    m_path = json.namedObject(wxT("m_path")).toString();
}
