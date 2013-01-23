#include "zn_config_item.h"

znConfigItem::znConfigItem()
    : clConfigItem("ZoomNavigator")
{
}

znConfigItem::~znConfigItem()
{
}

void znConfigItem::FromJSON(const JSONElement& json)
{
    m_highlightColour = json.namedObject("m_highlightColour").toString();
    m_enabled         = json.namedObject("m_enabled").toBool();
}

JSONElement znConfigItem::ToJSON() const
{
    JSONElement element = JSONElement::createObject(GetName());
    element.addProperty("m_highlightColour", m_highlightColour);
    element.addProperty("m_enabled", m_enabled);
    return element;
}
