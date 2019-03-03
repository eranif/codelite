#ifndef CLHEADERITEM_H
#define CLHEADERITEM_H

#include "clColours.h"
#include "codelite_exports.h"
#include <vector>
#include <wx/bitmap.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/window.h>

enum clHeaderFlags {
    kHeaderNative = (1 << 0),
    kHeaderColWidthFitData = (1 << 1),
    kHeaderColWidthFitHeader = (1 << 2),
    kHeaderColWidthUser = (1 << 3),
    kHeaderColWidthMask = (kHeaderColWidthUser | kHeaderColWidthFitHeader | kHeaderColWidthFitData),
};

class WXDLLIMPEXP_SDK clHeaderItem
{
    wxString m_label;
    wxBitmap m_bitmap;
    wxRect m_rect;
    wxWindow* m_parent = nullptr;
    size_t m_flags = kHeaderColWidthFitData;

protected:
    void EnableFlag(int flag, bool b)
    {
        if(b) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }

public:
    typedef std::vector<clHeaderItem> Vect_t;
    static const int X_SPACER = 5;
    static const int Y_SPACER = 5;

public:
    clHeaderItem();
    clHeaderItem(wxWindow* parent, const wxString& label, const wxBitmap& bmp = wxNullBitmap);
    virtual ~clHeaderItem();

    void Render(wxDC& dc, const clColours& colours, int flags);
    void SetBitmap(const wxBitmap& bitmap) { this->m_bitmap = bitmap; }
    void SetLabel(const wxString& label) { this->m_label = label; }
    void SetRect(const wxRect& rect) { this->m_rect = rect; }
    const wxRect& GetRect() const { return m_rect; }
    const wxBitmap& GetBitmap() const { return m_bitmap; }
    const wxString& GetLabel() const { return m_label; }
    void SetWidthValue(int width);
    void UpdateWidth(int width);
    size_t GetWidth() const { return m_rect.GetWidth(); }
    void SetX(size_t x) { m_rect.SetX(x); }
    bool IsAutoResize() const { return m_flags & kHeaderColWidthFitData; }
};

#endif // CLHEADERITEM_H
