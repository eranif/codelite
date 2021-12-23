#ifndef CLCUSTOMSCROLLBAR_H
#define CLCUSTOMSCROLLBAR_H

#include "clColours.h"
#include "codelite_exports.h"

#include <wx/event.h>
#include <wx/panel.h>

/// a wxCommandEvent that takes ownership of the clientData
class WXDLLIMPEXP_SDK clScrollEvent : public wxCommandEvent
{
protected:
    int m_position = wxNOT_FOUND;

public:
    clScrollEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clScrollEvent(const clScrollEvent& event);
    clScrollEvent& operator=(const clScrollEvent& src);
    virtual ~clScrollEvent();
    virtual wxEvent* Clone() const;
    void SetPosition(int position) { this->m_position = position; }
    int GetPosition() const { return m_position; }
};

typedef void (wxEvtHandler::*clScrollEventFunction)(clScrollEvent&);
#define clScrollEventHandler(func) wxEVENT_HANDLER_CAST(clScrollEventFunction, func)

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
    bool m_notifyScroll = false;

protected:
    void OnIdle(wxIdleEvent& event);
    void OnPaint(wxPaintEvent& e);
    void OnMouseLeftDown(wxMouseEvent& e);
    void OnMouseLeftUp(wxMouseEvent& e);
    void OnMotion(wxMouseEvent& e);
    void OnSize(wxSizeEvent& e);
    void UpdateDrag(const wxPoint& pt);
    int GetPositionFromPoint(const wxPoint& pt) const;

public:
    clCustomScrollBar(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize, long style = wxSB_VERTICAL);
    virtual ~clCustomScrollBar();

    bool ShouldShow() const;
    bool CanScollDown() const { return (GetThumbPosition() + GetThumbSize()) < GetRange(); }

    void SetPosition(int pos, bool notify = true);
    void UpdateScroll(int thumbSize, int range, int pageSize, int position);

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

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_CUSTOM_SCROLL, clScrollEvent);
#endif // CLCUSTOMSCROLLBAR_H
