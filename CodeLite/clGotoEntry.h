#ifndef CLGOTOENTRY_H
#define CLGOTOENTRY_H

#include "codelite_exports.h"
#include <vector>
#include <wx/bitmap.h>
#include <wx/string.h>

class WXDLLIMPEXP_CL clGotoEntry
{
private:
    wxString m_desc;
    wxString m_keyboardShortcut;
    int m_resourceID;
    wxBitmap m_bitmap;
    size_t m_flags;

public:
    enum eFlags {
        kItemNormal = (1 << 0),
        kItemRadio = (1 << 1),
        kItemCheck = (1 << 2),
        kItemStateChecked = (1 << 3),
    };

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
    void SetFlags(size_t flags) { this->m_flags = flags; }
    size_t GetFlags() const { return m_flags; }
    bool IsChecked() const { return m_flags & kItemStateChecked; }
    void SetChecked(bool b) { b ? m_flags |= kItemStateChecked : m_flags &= ~kItemStateChecked; }
    bool IsCheckable() const { return m_flags & kItemCheck; }
    void SetCheckable(bool b) { b ? m_flags |= kItemCheck : m_flags &= ~kItemCheck; }
};
#endif // CLGOTOENTRY_H
