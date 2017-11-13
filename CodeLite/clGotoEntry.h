#ifndef CLGOTOENTRY_H
#define CLGOTOENTRY_H

#include "codelite_exports.h"
#include <wx/string.h>
#include <vector>
#include <wx/bitmap.h>

class WXDLLIMPEXP_CL clGotoEntry
{
private:
    wxString m_desc;
    wxString m_keyboardShortcut;
    int m_resourceID;
    wxBitmap m_bitmap;

public:
    typedef std::vector<clGotoEntry> Vec_t;

public:
    clGotoEntry(const wxString& desc, const wxString& shortcut, int id);
    clGotoEntry();

    void SetDesc(const wxString& desc) { this->m_desc = desc; }
    void SetKeyboardShortcut(const wxString& keyboardShortcut) { this->m_keyboardShortcut = keyboardShortcut; }
    void SetResourceID(int resourceID) { this->m_resourceID = resourceID; }
    const wxString& GetDesc() const { return m_desc; }
    const wxString& GetKeyboardShortcut() const { return m_keyboardShortcut; }
    int GetResourceID() const { return m_resourceID; }
    const wxBitmap& GetBitmap() const { return m_bitmap; }
    void SetBitmap(const wxBitmap& bitmap) { m_bitmap = bitmap; }
};
#endif // CLGOTOENTRY_H
