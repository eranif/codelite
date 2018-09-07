#ifndef CLSCROLLEDPANEL_H
#define CLSCROLLEDPANEL_H

#include "codelite_exports.h"
#include <wx/panel.h>
#include <wx/scrolbar.h>

class WXDLLIMPEXP_SDK clScrolledPanel : public wxWindow
{
private:
    wxScrollBar* m_vsb = nullptr;
    int m_pageSize = 0;

protected:
    virtual void OnVScroll(wxScrollEvent& event);
    virtual void OnCharHook(wxKeyEvent& event);

public:
    clScrolledPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~clScrolledPanel();

    /**
     * @brief return the number lines can fit into the page (vertically)
     */
    int GetPageSize() const;

    /**
     * @brief override this method to scroll the view
     * @param steps number of lines to scroll. If 'numLines' is set to 0, then scroll to top or bottom of the view
     * depending on the direction. otherwise, scroll 'steps' into the correct 'direction'
     * @param direction direction to scroll
     */
    virtual void ScrollLines(int steps, wxDirection direction)
    {
        wxUnusedVar(steps);
        wxUnusedVar(direction);
    }

    /**
     * @brief scroll to set 'firstLine' as the first visible line in the view
     */
    virtual void ScrollToLine(int firstLine) { wxUnusedVar(firstLine); }

    /**
     * @brief whenver the view changes (i.e. there is a new top line) call this method so the scrollbar
     * will adjust its position
     */
    void UpdateVScrollBar(int position, int thumbSize, int rangeSize, int pageSize);

    /**
     * @brief called by the scrolled window whenver a key is down
     * return true if the key was processed and we should stop the processing of this event
     */
    virtual bool DoKeyDown(const wxKeyEvent& event)
    {
        wxUnusedVar(event);
        return false;
    }
};

#endif // CLSCROLLEDPANEL_H
