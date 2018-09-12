#ifndef CLSCROLLEDPANEL_H
#define CLSCROLLEDPANEL_H

#include "clScrollBar.h"
#include "codelite_exports.h"
#include <wx/bitmap.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/panel.h>
#include <wx/scrolbar.h>
#include <wx/treebase.h>

class WXDLLIMPEXP_SDK clScrolledPanel : public wxWindow
{
private:
    clScrollBar* m_vsb = nullptr;
    int m_pageSize = 0;
    int m_position = 0;
    int m_thumbSize = 0;
    int m_rangeSize = 0;

    wxBitmap m_tmpBmp;
    wxMemoryDC* m_memDC = nullptr;
    wxGCDC* m_gcdc = nullptr;
    bool m_showSBOnFocus = false;
    wxDateTime m_dragStartTime;
    wxPoint m_dragStartPos;
    bool m_dragging = false;

protected:
    virtual void OnVScroll(wxScrollEvent& event);
    virtual void OnCharHook(wxKeyEvent& event);
    virtual void OnIdle(wxIdleEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void OnMotion(wxMouseEvent& event);
    void OnLeaveWindow(wxMouseEvent& event);
    void DoBeginDrag();
    void DoCancelDrag();

protected:
    bool ShouldShowScrollBar() const;

    /**
     * @brief return true row from a position
     */
    virtual wxTreeItemId GetRow(const wxPoint& pt) const { return wxTreeItemId(); }

public:
    clScrolledPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~clScrolledPanel();
    
    /**
     * @brief return the system default font
     */
    static wxFont GetDefaultFont();
    
    /**
     * @brief when enabled, the scrollbar will only be shown (if needed at all) when this window has the focus (or any
     * of its decendants)
     */
    void SetShowScrollBarOnFocus(bool b) { m_showSBOnFocus = b; }
    wxDC& GetTempDC() const { return *m_gcdc; }

    /**
     * @brief return the number lines can fit into the page (vertically)
     */
    int GetPageSize() const;

    /**
     * @brief whenver the view changes (i.e. there is a new top line) call this method so the scrollbar
     * will adjust its position
     */
    void UpdateVScrollBar(int position, int thumbSize, int rangeSize, int pageSize);

    //===----------------------------------------------------
    // Overridables
    //===----------------------------------------------------

    /**
     * @brief override this method to scroll the view
     * @param steps number of lines to scroll. If 'numLines' is set to 0, then scroll to top or bottom of the view
     * depending on the direction. otherwise, scroll 'steps' into the correct 'direction'
     * @param direction direction to scroll
     */
    virtual void ScrollRows(int steps, wxDirection direction)
    {
        wxUnusedVar(steps);
        wxUnusedVar(direction);
    }

    /**
     * @brief scroll to set 'firstLine' as the first visible line in the view
     */
    virtual void ScrollToRow(int firstLine) { wxUnusedVar(firstLine); }

    /**
     * @brief called by the scrolled window whenver a key is down
     * return true if the key was processed and we should stop the processing of this event
     */
    virtual bool DoKeyDown(const wxKeyEvent& event)
    {
        wxUnusedVar(event);
        return false;
    }

    // Process idle events. Override this in the subclass
    virtual void ProcessIdle() {}
};

#endif // CLSCROLLEDPANEL_H
