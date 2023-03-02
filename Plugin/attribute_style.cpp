#include "attribute_style.h"

#include "FontUtils.hpp"
#include "macros.h"

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

void StyleProperty::FromJSON(const JSONItem& json)
{
    auto M = json.GetAsMap();
    m_id = M["Id"].toInt(0);
    m_name = M["Name"].toString("DEFAULT");
    m_flags = M["Flags"].toSize_t(0);
    m_fontDesc = M["FontDesc"].toString();
    m_fgColour = M["Colour"].toString("BLACK");
    m_bgColour = M["BgColour"].toString("WHITE");
    m_fontSize = M["Size"].toInt(wxNOT_FOUND);
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
    json.addProperty("Size", m_fontSize);
    return json;
}

void StyleProperty::FromAttributes(wxFont* font) const
{
    CHECK_PTR_RET(font);
    if(HasFontInfoDesc()) {
        font->SetNativeFontInfo(GetFontInfoDesc());
    } else {
        font->SetUnderlined(GetUnderlined());
        font->SetWeight(IsBold() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
        font->SetStyle(GetItalic() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
        if(m_fontSize != wxNOT_FOUND) {
            font->SetPointSize(m_fontSize);
        }
    }
}

wxString StyleProperty::GetFontInfoDesc() const { return FontUtils::GetFontInfo(m_fontDesc); }

void StyleProperty::SetFontInfoDesc(const wxString& desc) { m_fontDesc = FontUtils::GetFontInfo(desc); }