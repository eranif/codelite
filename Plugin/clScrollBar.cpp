#include "clScrollBar.h"

clScrollBarHelper::clScrollBarHelper(
    wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxScrollBar(parent, id, pos, size, style)
{
}

clScrollBarHelper::~clScrollBarHelper() {}

void clScrollBarHelper::SetScrollbar(int position, int thumbSize, int range, int pageSize, bool refresh)
{
    wxScrollBar::SetScrollbar(position, thumbSize, range, pageSize, refresh);
}
