#include "attribute_style.h"

StyleProperty::StyleProperty(int id, const wxString& name, const wxString& fgColour, const wxString& bgColour,
                             const int fontSize, bool bold, bool italic, bool underline, bool eolFilled)
    : m_id(id)
    , m_name(name)
    , m_fgColour(fgColour)
    , m_bgColour(bgColour)
    , m_fontSize(fontSize)
{
    EnableFlag(kBold, bold);
    EnableFlag(kItalic, italic);
    EnableFlag(kUnderline, underline);
    EnableFlag(kEolFilled, eolFilled);
}

StyleProperty::StyleProperty()
    : m_fgColour("BLACK")
    , m_bgColour("WHITE")
{
}

StyleProperty::StyleProperty(int id, const wxString& name, const wxString& fontDesc, const wxString& fgColour,
                             const wxString& bgColour, bool eolFilled)
    : m_id(id)
    , m_name(name)
    , m_fontDesc(fontDesc)
    , m_fgColour(fgColour)
    , m_bgColour(bgColour)
{
    EnableFlag(kEolFilled, eolFilled);
}

void StyleProperty::FromJSON(JSONItem json)
{
    m_id = json.namedObject("Id").toInt(0);
    m_name = json.namedObject("Name").toString("DEFAULT");
    m_flags = json.namedObject("Flags").toSize_t(0);
    m_fontDesc = json.namedObject("FontDesc").toString();
    m_fgColour = json.namedObject("Colour").toString("BLACK");
    m_bgColour = json.namedObject("BgColour").toString("WHITE");
    m_fontSize = json.namedObject("Size").toInt(10);
}

JSONItem StyleProperty::ToJSON(bool portable) const
{
    JSONItem json = JSONItem::createObject();
    json.addProperty("Id", GetId());
    json.addProperty("Name", GetName());
    json.addProperty("Flags", m_flags);
    json.addProperty("FontDesc", portable ? wxString() : GetFontInfoDesc());
    json.addProperty("Colour", GetFgColour());
    json.addProperty("BgColour", GetBgColour());
    json.addProperty("Size", GetFontSize());
    return json;
}

void StyleProperty::FromAttributes(wxFont* font) const
{
    if(GetFontInfoDesc().empty()) {
        font->SetUnderlined(GetUnderlined());
        font->SetWeight(IsBold() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
        font->SetStyle(GetItalic() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
        font->SetPointSize(GetFontSize());
    } else {
        font->SetNativeFontInfo(GetFontInfoDesc());
    }
}
