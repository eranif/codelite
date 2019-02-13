#ifndef CLCUSTOMSCROLLBAR_H
#define CLCUSTOMSCROLLBAR_H

#include "codelite_exports.h"
#include <wx/panel.h>
#include "clColours.h"

class WXDLLIMPEXP_SDK clCustomScrollBar : public wxPanel
{
    double m_thumbSize = 0.0;
    double m_pageSize = 0.0;
    double m_range = 0.0;
    double m_thumbPosition = 0.0;
    long m_sbStyle = wxSB_VERTICAL;
    wxRect m_thumbRect;
    wxPoint m_mouseCapturePoint;
    wxPoint m_thumbCapturePoint;
    bool m_dragging = false;
    clColours m_colours;

protected:
    void OnPaint(wxPaintEvent& e);
    void OnMouseLeftDown(wxMouseEvent& e);
    void OnMouseLeftUp(wxMouseEvent& e);
    void OnMotion(wxMouseEvent& e);
    void OnSize(wxSizeEvent& e);
    void UpdateDrag(const wxPoint& pt);

public:
    clCustomScrollBar(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize, long style = wxSB_VERTICAL);
    virtual ~clCustomScrollBar();

    void SetScrollbar(int position, int thumbSize, int range, int pageSize, bool refresh);

    bool IsVertical() const { return m_sbStyle & wxSB_VERTICAL; }
    bool IsHorizontal() const { return m_sbStyle & wxSB_HORIZONTAL; }

    /// Accessors
    void SetPageSize(int pageSize) { this->m_pageSize = pageSize; }
    void SetRange(int range) { this->m_range = range; }
    void SetThumbPosition(int thumbPosition) { this->m_thumbPosition = thumbPosition; }
    void SetThumbSize(int thumbSize) { this->m_thumbSize = thumbSize; }
    int GetPageSize() const { return m_pageSize; }
    int GetRange() const { return m_range; }
    int GetThumbPosition() const { return m_thumbPosition; }
    int GetThumbSize() const { return m_thumbSize; }

    /// Override's parent
    virtual void SetColours(const clColours& colours);
};

#endif // CLCUSTOMSCROLLBAR_H
