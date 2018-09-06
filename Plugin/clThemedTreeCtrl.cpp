#include "clThemedTreeCtrl.h"
#include "clTreeCtrlNode.h"

clThemedTreeCtrl::clThemedTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : clTreeCtrl(parent, id, pos, size, style | wxTR_ROW_LINES)
{
    clTreeCtrlColours colours;
    colours.InitDefaults();
    SetColours(colours);
}

clThemedTreeCtrl::~clThemedTreeCtrl() {}
