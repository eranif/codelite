#ifndef CLSCROLLBAR_H
#define CLSCROLLBAR_H

#include "codelite_exports.h"
#include <wx/scrolbar.h>
#include "clCustomScrollBar.h"
#include "clColours.h"

#if 1 // defined(__WXMSW__)||defined(__WXGTK__)
#define CL_USE_NATIVE_SCROLLBAR 0
#define CL_USE_CUSTOM_SCROLLBAR 1
#else
#define CL_USE_NATIVE_SCROLLBAR 1
#define CL_USE_CUSTOM_SCROLLBAR 0
#endif

class WXDLLIMPEXP_SDK clScrollBar : public wxScrollBar
{
protected:
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
    
    void SetColours(const clColours&) {}
};


#endif // CLSCROLLBAR_H
