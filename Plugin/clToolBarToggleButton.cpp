#include "clToolBarToggleButton.h"

clToolBarToggleButton::clToolBarToggleButton(
    clToolBar* parent, wxWindowID id, const wxBitmap& bmp, const wxString& label)
    : clToolBarButton(parent, id, bmp, label)
{
    m_flags = kToggleButton;
}

clToolBarToggleButton::~clToolBarToggleButton() {}

