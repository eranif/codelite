#include "clGotoEntry.h"

clGotoEntry::clGotoEntry(const wxString& desc, const wxString& shortcut, int id)
    : m_desc(desc)
    , m_keyboardShortcut(shortcut)
    , m_resourceID(id)
    , m_flags(0)
{
}

clGotoEntry::clGotoEntry()
    : m_resourceID(wxNOT_FOUND)
{
}