#include "clToolBarToggleButton.h"

clToolBarToggleButton::clToolBarToggleButton(clToolBarGeneric* parent, wxWindowID id, size_t bitmapId,
                                             const wxString& label)
    : clToolBarButton(parent, id, bitmapId, label)
{
    m_flags = kToggleButton;
}
