#ifndef CLSCROLLBAR_H
#define CLSCROLLBAR_H

#include "codelite_exports.h"
#include <wx/scrolbar.h>

class WXDLLIMPEXP_SDK clScrollBar : public wxScrollBar
{
    int m_thumb_size = 0;
    int m_range_size = 0;

public:
    clScrollBar(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = wxSB_VERTICAL);
    virtual ~clScrollBar();
    void SetScrollbar(int position, int thumbSize, int range, int pageSize, bool refresh = true);
    bool ShouldShow() const { return (m_thumb_size < m_range_size); }
    /**
     * @brief can we scroll up or left?
     */
    bool CanScrollUp() const { return GetThumbPosition() > 0; }
    /**
     * @brief can we scroll down or right?
     */
    bool CanScollDown() const { return (GetThumbPosition() + GetThumbSize()) < GetRange(); }
};

#endif // CLSCROLLBAR_H
