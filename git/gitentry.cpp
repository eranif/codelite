
//////////////////////////////////////////////////////////////////////////////

#include "gitentry.h"

GitEntry::GitEntry()
    : clConfigItem("git-settings")
    , m_flags(0)
{
}

GitEntry::~GitEntry()
{
}

void GitEntry::FromJSON(const JSONElement& json)
{
    m_entries           = json.namedObject("m_entries").toStringMap();
    m_colourTrackedFile = json.namedObject("m_colourTrackedFile").toString();
    m_colourDiffFile    = json.namedObject("m_colourDiffFile").toString();
    m_pathGIT           = json.namedObject("m_pathGIT").toString();
    m_pathGITK          = json.namedObject("m_pathGITK").toString();
    m_flags             = json.namedObject("m_flags").toSize_t(0);
}

JSONElement GitEntry::ToJSON() const
{
    JSONElement json = JSONElement::createObject(GetName());
    json.addProperty("m_entries",           m_entries);
    json.addProperty("m_colourTrackedFile", m_colourTrackedFile.GetAsString(wxC2S_HTML_SYNTAX));
    json.addProperty("m_colourDiffFile",    m_colourDiffFile.GetAsString(wxC2S_HTML_SYNTAX));
    json.addProperty("m_pathGIT",           m_pathGIT);
    json.addProperty("m_pathGITK",          m_pathGITK);
    json.addProperty("m_flags",             m_flags);
    return json;
}
