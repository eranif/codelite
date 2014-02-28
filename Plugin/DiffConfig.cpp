#include "DiffConfig.h"

DiffConfig::DiffConfig()
    : clConfigItem("CodeLiteDiff")
    , m_flags(0)
    , m_viewFlags(kViewVerticalSplit)
{
}

DiffConfig::~DiffConfig()
{
}

void DiffConfig::FromJSON(const JSONElement& json)
{
    m_flags     = json.namedObject("m_flags").toSize_t(0);
    m_viewFlags = json.namedObject("m_viewFlags").toSize_t(kViewVerticalSplit);
}

JSONElement DiffConfig::ToJSON() const
{
    JSONElement element = JSONElement::createObject(GetName());
    element.addProperty("m_flags",      m_flags);
    element.addProperty("m_viewFlags",  m_viewFlags);
    return element;
}

DiffConfig& DiffConfig::Load()
{
    clConfig::Get().ReadItem( this );
    return *this;
}

void DiffConfig::Save()
{
    clConfig::Get().WriteItem( this );
}
