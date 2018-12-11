#include "clScrollBar.h"

clScrollBar::clScrollBar(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxScrollBar(parent, id, pos, size, style)
{
    Hide();
}

clScrollBar::~clScrollBar() {}

void clScrollBar::SetScrollbar(int position, int thumbSize, int range, int pageSize, bool refresh)
{
    m_thumb_size = thumbSize;
    m_range_size = range;
    wxScrollBar::SetScrollbar(position, thumbSize, range, pageSize, refresh);
}
