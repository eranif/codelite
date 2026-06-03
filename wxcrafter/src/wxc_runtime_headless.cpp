// Headless implementation of the wxCrafter runtime shim.
// Linked only into the wxcgen executable.

#include "wxc_runtime.h"

namespace wxc_runtime
{

wxImageList* CreateAllocatorImageList() { return nullptr; }

int AddImageToAllocator(wxImageList* /*list*/, const wxCrafter::ResourceLoader& /*loader*/,
                        const wxString& /*bmpName*/)
{
    return -1;
}

BitmapGenStatusScope::BitmapGenStatusScope() = default;
BitmapGenStatusScope::~BitmapGenStatusScope() = default;

void SetStatusMessage(const wxString& /*msg*/) {}

bool IsActiveWizardPage(const wxcWidget* /*page*/)
{
    // Headless never enters the XRC_DESIGNER preview path that calls this.
    return false;
}

void RequestDesignerRefresh()
{
    // No designer to refresh in the headless build, and no event loop to drain
    // the pending event — drop it on the floor.
}

} // namespace wxc_runtime
