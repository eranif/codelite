#ifndef CLSCROLLBAR_H
#define CLSCROLLBAR_H

#include <wx/dc.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/window.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clScrollBarHelper : public wxEvtHandler
{
    wxWindow* m_parent = nullptr;
    int m_position = 0;
    int m_thumbSize = 0;
    int m_range = 0;
    int m_pageSize = 0;
    int m_lineInPixels = 0;
    int m_minButtonSize = 15;
#ifdef __WXOSX__
    int m_size = 10;
#else
    int m_size = 14;
#endif
    wxOrientation m_orientation = wxVERTICAL;
    wxRect m_thumbRect;
    wxColour m_buttonColour;
    wxColour m_bgColour;
    bool m_scrolling = false;
    wxPoint m_anchorPoint;
    int m_remainder = 0;

protected:
    wxWindow* GetParent() const { return m_parent; }
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseMotion(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void DoCancelScrolling();
    int DoGetButtonSize();
    int DoGetButtonPosition();
    wxRect GetVirtualRect() const;
    bool IsVertical() const { return m_orientation == wxVERTICAL; }

public:
    clScrollBarHelper(wxWindow* parent, wxOrientation orientation = wxVERTICAL);
    virtual ~clScrollBarHelper();
    
    /**
     * @brief return the scrollbar area
     */
    wxRect GetClientRect() const;
    
    /**
     * @brief set the scrollbar colours
     */
    void SetBgColour(const wxColour& bgColour) { this->m_bgColour = bgColour; }
    void SetButtonColour(const wxColour& buttonColour) { this->m_buttonColour = buttonColour; }
    const wxColour& GetBgColour() const { return m_bgColour; }
    const wxColour& GetButtonColour() const { return m_buttonColour; }

    void SetMinButtonSize(int minButtonSize) { this->m_minButtonSize = minButtonSize; }
    int GetMinButtonSize() const { return m_minButtonSize; }

    /**
     * @brief set the scrollbar width (when wxVERTICAL) or height (when wxHORIZONTAL)
     * @param size
     */
    void SetSize(int size) { this->m_size = size; }

    int GetSize() const { return m_size; }
    /**
     * @brief render the scrollbar on the given dc
     * @param event
     */
    void Render(wxDC& dc);
    /**
     * Sets the scrollbar properties.
     * @param position The position of the scrollbar in scroll units.
     * @param thumbSize The size of the thumb, or visible portion of the scrollbar, in scroll units.
     * @param range The maximum position of the scrollbar.
     * is paged up or down. Often it is the same as the thumb size. refresh	true to redraw the scrollbar, false
     * otherwise.
     */
    void SetScrollbar(int position, int thumbSize, int range, int lineInPixels);
};

#endif // CLSCROLLBAR_H
