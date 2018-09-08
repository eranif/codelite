#ifndef CLHEADERITEM_H
#define CLHEADERITEM_H

#include "clColours.h"
#include "codelite_exports.h"
#include <vector>
#include <wx/bitmap.h>
#include <wx/gdicmn.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK clHeaderItem
{
    wxString m_label;
    wxBitmap m_bitmap;
    wxRect m_rect;

public:
    typedef std::vector<clHeaderItem> Vect_t;
    static const int X_SPACER = 5;
    static const int Y_SPACER = 2;

public:
    clHeaderItem();
    clHeaderItem(const wxString& label, const wxBitmap& bmp = wxNullBitmap);
    virtual ~clHeaderItem();

    void Render(wxDC& dc, const clColours& colours);
    void SetBitmap(const wxBitmap& bitmap) { this->m_bitmap = bitmap; }
    void SetLabel(const wxString& label) { this->m_label = label; }
    void SetRect(const wxRect& rect) { this->m_rect = rect; }
    const wxRect& GetRect() const { return m_rect; }
    const wxBitmap& GetBitmap() const { return m_bitmap; }
    const wxString& GetLabel() const { return m_label; }
    void SetWidth(size_t width) { m_rect.SetWidth(width); }
    size_t GetWidth() const { return m_rect.GetWidth(); }
    void SetX(size_t x) { m_rect.SetX(x); }
};

#endif // CLHEADERITEM_H
