#include "attribute_style.h"

StyleProperty::StyleProperty(int id, const wxString& fgColour, const wxString& bgColour, const int fontSize,
                             const wxString& name, const wxString& face, bool bold, bool italic, bool underline,
                             bool eolFilled, int alpha)
    : m_id(id)
    , m_fgColour(fgColour)
    , m_bgColour(bgColour)
    , m_fontSize(fontSize)
    , m_name(name)
    , m_faceName(face)
    , m_flags(0)
    , m_alpha(alpha)
{
    EnableFlag(kBold, bold);
    EnableFlag(kItalic, italic);
    EnableFlag(kUnderline, underline);
    EnableFlag(kEolFilled, eolFilled);
}

StyleProperty::StyleProperty()
    : m_id(0)
    , m_fgColour(_T("BLACK"))
    , m_bgColour(_T("WHITE"))
    , m_fontSize(10)
    , m_name(wxEmptyString)
    , m_faceName(_T("Courier"))
    , m_flags(0)
    , m_alpha(0)
{
}

StyleProperty& StyleProperty::operator=(const StyleProperty& rhs)
{
    m_fgColour = rhs.m_fgColour;
    m_bgColour = rhs.m_bgColour;
    m_faceName = rhs.m_faceName;
    m_fontSize = rhs.m_fontSize;
    m_name = rhs.m_name;
    m_id = rhs.m_id;
    m_alpha = rhs.m_alpha;
    m_flags = rhs.m_flags;
    return *this;
}

void StyleProperty::FromJSON(JSONItem json)
{
    m_id = json.namedObject("Id").toInt(0);
    m_name = json.namedObject("Name").toString("DEFAULT");
    m_flags = json.namedObject("Flags").toSize_t(0);
    m_alpha = json.namedObject("Alpha").toInt(50);
    m_faceName = json.namedObject("Face").toString("Courier");
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
    json.addProperty("Alpha", GetAlpha());
    json.addProperty("Face", portable ? wxString() : GetFaceName());
    json.addProperty("Colour", GetFgColour());
    json.addProperty("BgColour", GetBgColour());
    json.addProperty("Size", GetFontSize());
    return json;
}
