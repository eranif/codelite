#include "zn_config_item.h"

znConfigItem::znConfigItem()
    : clConfigItem("ZoomNavigator")
    , m_highlightColour("LIGHT GREY")
    , m_enabled(true)
    , m_zoomFactor(-10)
{
}

znConfigItem::~znConfigItem()
{
}

void znConfigItem::FromJSON(const JSONElement& json)
{
    m_highlightColour = json.namedObject("m_highlightColour").toString();
    m_enabled         = json.namedObject("m_enabled").toBool();
    m_zoomFactor      = json.namedObject("m_zoomFactor").toInt(-10);
}

JSONElement znConfigItem::ToJSON() const
{
    JSONElement element = JSONElement::createObject(GetName());
    element.addProperty("m_highlightColour", m_highlightColour);
    element.addProperty("m_enabled", m_enabled);
    element.addProperty("m_zoomFactor", m_zoomFactor);
    return element;
}
