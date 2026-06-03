// wxCrafter runtime shim.
//
// The wxCrafter library is built once (wxCrafterLib) and linked into both the
// GUI executable (wxcrafter / wxCrafterPlugin) and the headless code-generator
// (wxcgen). A few code paths inside the library touch GUI-only APIs (image
// lists, status bar text, busy cursor). To avoid sprinkling #ifdefs through
// the lib, those calls go through this shim. Two implementations are linked
// per-target: src/wxc_runtime_gui.cpp for GUI builds, src/wxc_runtime_headless.cpp
// for the headless tool.

#ifndef WXC_RUNTIME_H
#define WXC_RUNTIME_H

#include <wx/string.h>

class wxImageList;
class wxcWidget;
namespace wxCrafter
{
class ResourceLoader;
}

namespace wxc_runtime
{

// Allocate the wxImageList used by Allocator for the widget tree.
// GUI: returns `new wxImageList(16, 16, true)`. Headless: returns nullptr.
wxImageList* CreateAllocatorImageList();

// Add a bitmap to the allocator's image list and return the new image id.
// GUI: list->Add(loader.Bitmap(name)). Headless: returns -1; arguments unused.
int AddImageToAllocator(wxImageList* list, const wxCrafter::ResourceLoader& loader, const wxString& bmpName);

// RAII scope shown around bitmap-code generation.
// GUI: ctor sets the top frame status text and pushes wxBusyCursor;
//      dtor pops the cursor and restores the status text.
// Headless: both are no-ops.
class BitmapGenStatusScope
{
public:
    BitmapGenStatusScope();
    ~BitmapGenStatusScope();

    BitmapGenStatusScope(const BitmapGenStatusScope&) = delete;
    BitmapGenStatusScope& operator=(const BitmapGenStatusScope&) = delete;
};

// Set the application's status-bar message.
// GUI: forwards to MainFrame::SetStatusMessage. Headless: no-op.
// (This is the implementation behind wxCrafter::SetStatusMessage; that wrapper
// stays in wxgui_helpers.h so existing callers keep working.)
void SetStatusMessage(const wxString& msg);

// True iff `page` is the wizard page currently selected in the GUI designer.
// GUI: forwards to GUICraftMainPanel::m_MainPanel->GetActiveWizardPage().
// Headless: returns false — the designer preview path that uses this is
//           never hit by the headless XRC_LIVE pipeline.
bool IsActiveWizardPage(const wxcWidget* page);

// Notify the GUI designer that bitmaps changed and the preview should refresh.
// GUI: posts wxEVT_REFRESH_DESIGNER on EventNotifier. Headless: no-op.
void RequestDesignerRefresh();

} // namespace wxc_runtime

#endif // WXC_RUNTIME_H
