
//////////////////////////////////////////////////////////////////////////////

#include "gitentry.h"
const wxEventType wxEVT_GIT_CONFIG_CHANGED = ::wxNewEventType();

GitEntry::GitEntry()
    : clConfigItem("git-settings")
#ifdef __WXMSW__    
    , m_flags(Git_Show_Terminal)
#else
    , m_flags(0)
#endif    
    , m_gitDiffDlgSashPos(wxNOT_FOUND)
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
    m_flags             = json.namedObject("m_flags").toSize_t(m_flags);
    m_gitDiffDlgSashPos = json.namedObject("m_gitDiffDlgSashPos").toInt(m_gitDiffDlgSashPos);
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
    json.addProperty("m_gitDiffDlgSashPos", m_gitDiffDlgSashPos);
    return json;
}
