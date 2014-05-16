#include "SFTPBookmark.h"

SFTPBookmark::SFTPBookmark()
{
}

SFTPBookmark::~SFTPBookmark()
{
}

void SFTPBookmark::FromJSON(const JSONElement& json)
{
    m_account.FromJSON( json.namedObject("m_account") );
    m_name      = json.namedObject("m_name").toString();
    m_folder    = json.namedObject("m_folder").toString();
}

JSONElement SFTPBookmark::ToJSON() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty("m_account", m_account.ToJSON());
    json.addProperty("m_name", m_name);
    json.addProperty("m_folder", m_folder);
    return json;
}
