// Headless implementation of the wxCrafter runtime shim.
// Linked only into the wxcgen executable.

#include "wxc_runtime.h"

namespace wxc_runtime
{

void LoadImages(const wxString&, std::map<wxString, wxBitmap>&, std::map<wxString, wxString>&) {}

} // namespace wxc_runtime
