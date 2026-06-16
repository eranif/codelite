// Headless implementation of the wxCrafter runtime shim.
// Linked only into the wxcgen executable.

#include "wxc_runtime.h"

namespace wxc_runtime
{

wxImageList* CreateAllocatorImageList() { return nullptr; }

int AddImageToAllocator(wxImageList* /*list*/, const wxCrafter::ResourceLoader& /*loader*/, const wxString& /*bmpName*/)
{
    return -1;
}

void SetStatusMessage(const wxString& /*msg*/) {}

bool IsActiveWizardPage(const wxcWidget* /*page*/)
{
    // Headless never enters the XRC_DESIGNER preview path that calls this.
    return false;
}

bool IsDarkAppearance() { return false; }

void LoadImages(const wxString&, std::map<wxString, wxBitmap>&, std::map<wxString, wxString>&) {}

} // namespace wxc_runtime
