#include "wxCustomControls.hpp"

int clMessageBox(const wxString& message, const wxString& caption, long style, wxWindow* parent, int x, int y)
{
    return wxMessageBox(message, caption, style, parent, x, y);
}
