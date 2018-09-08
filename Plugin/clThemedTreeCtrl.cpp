#include "clThemedTreeCtrl.h"
#include <clColours.h>

clThemedTreeCtrl::clThemedTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : clTreeCtrl(parent, id, pos, size, style | wxTR_ROW_LINES)
{
    clColours colours;
    colours.InitDefaults();
    SetColours(colours);
}

clThemedTreeCtrl::~clThemedTreeCtrl() {}
