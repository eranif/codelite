#include "attribute_style.h"

#include "FontUtils.hpp"
#include "macros.h"

StyleProperty::StyleProperty(int id,
                             const wxString& name,
                             const wxString& fgColour,
                             const wxString& bgColour,
                             const int fontSize,
                             bool bold,
                             bool italic,
                             bool underline,
                             bool eolFilled)
    : m_id(id)
    , m_name(name)
    , m_fgColour(fgColour)
    , m_bgColour(bgColour)
    , m_fontSize(fontSize)
{
    SetBold(bold);
    SetItalic(italic);
    SetUnderlined(underline);
    EnableFlag(kEolFilled, eolFilled);
}

StyleProperty::StyleProperty()
    : m_fgColour("BLACK")
    , m_bgColour("WHITE")
{
}

StyleProperty::StyleProperty(int id,
                             const wxString& name,
                             const wxString& fontDesc,
                             const wxString& fgColour,
                             const wxString& bgColour,
                             bool eolFilled)
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
    m_isBold = M["Bold"].toBool(m_isBold);
    m_isItalic = M["Italic"].toBool(m_isItalic);
    m_isUnderlined = M["Underlined"].toBool(m_isUnderlined);
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
    json.addProperty("Bold", m_isBold);
    json.addProperty("Italic", m_isItalic);
    json.addProperty("Underlined", m_isUnderlined);
    return json;
}

void StyleProperty::FromAttributes(wxFont* font) const
{
    CHECK_PTR_RET(font);
    if (HasFontInfoDesc()) {
        font->SetNativeFontInfo(GetFontInfoDesc());
    }
    font->SetUnderlined(GetUnderlined());
    font->SetWeight(IsBold() ? wxFONTWEIGHT_HEAVY : wxFONTWEIGHT_NORMAL);
    font->SetStyle(GetItalic() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
    if (m_fontSize != wxNOT_FOUND) {
        font->SetPointSize(m_fontSize);
    }
}

wxString StyleProperty::GetFontInfoDesc() const { return FontUtils::GetFontInfo(m_fontDesc); }

void StyleProperty::SetFontInfoDesc(const wxString& desc) { m_fontDesc = FontUtils::GetFontInfo(desc); }