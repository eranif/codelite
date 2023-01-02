#include "clToolBarToggleButton.h"
#if !wxUSE_NATIVE_TOOLBAR
clToolBarToggleButton::clToolBarToggleButton(clToolBar* parent, wxWindowID id, size_t bitmapId, const wxString& label)
    : clToolBarButton(parent, id, bitmapId, label)
{
    m_flags = kToggleButton;
}

clToolBarToggleButton::~clToolBarToggleButton() {}
#endif // #if !wxUSE_NATIVE_TOOLBAR
