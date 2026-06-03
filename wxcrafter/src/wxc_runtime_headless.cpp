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

wxFont GetSystemFont(wxSystemFont /*index*/)
{
    // Avoid wxSystemSettings::GetFont, which on GTK probes the live theme
    // (gtk_init / pango). Return a default-constructed wxFont — codegen sets
    // the attributes it cares about (style/weight/underlined) explicitly
    // afterwards from the project string.
    return wxFont();
}

bool IsDarkAppearance() { return false; }

void LoadImages(const wxString&, std::map<wxString, wxBitmap>&, std::map<wxString, wxString>&) {}

} // namespace wxc_runtime
