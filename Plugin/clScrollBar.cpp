#include "clScrollBar.h"

clScrollBar::clScrollBar(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : ScrollBarBase(parent, id, pos, size, style)
{
    Hide();
}

clScrollBar::~clScrollBar() {}

void clScrollBar::SetScrollbar(int position, int thumbSize, int range, int pageSize, bool refresh)
{
    m_thumb_size = thumbSize;
    m_range_size = range;
    ScrollBarBase::SetScrollbar(position, thumbSize, range, pageSize, refresh);
}

void clScrollBar::SetColours(const clColours& colours)
{
#if defined(__WXMSW__) || defined(__WXGTK__)
    ScrollBarBase::SetColours(colours);
#else
    wxUnusedVar(colours);
#endif
}
