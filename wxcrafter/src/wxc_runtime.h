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

#include <map>
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/settings.h>
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

// Look up a wxSystemSettings font (wxSYS_DEFAULT_GUI_FONT etc.).
// GUI: forwards to wxSystemSettings::GetFont. Headless: returns a default-
//      constructed wxFont so the caller doesn't trigger gtk_init / pango
//      lookups that fail without a DISPLAY. Codegen only inspects style/
//      weight/underlined attributes which it sets explicitly afterwards
//      from the project string — the system-derived face/size are discarded.
wxFont GetSystemFont(wxSystemFont index);

// True iff the OS-level dark-mode appearance is active.
// GUI: forwards to wxSystemSettings::GetAppearance().IsDark().
// Headless: returns false (codegen falls back to the light-theme branch).
bool IsDarkAppearance();

/**
 * @brief Loads bitmap images and text files from a skin ZIP archive into the provided maps.
 *
 * This function locates a "{skin}.zip" archive, extracts all entries, and stores PNG images as
 * wxBitmap objects in bitmaps while storing other file contents as strings in files. On Windows,
 * the archive is resolved relative to the application executable; on other platforms, it is
 * resolved relative to the application's data directory.
 *
 * @param skin const wxString& The skin name used to build the ZIP archive name.
 * @param bitmaps std::map<wxString, wxBitmap>& Output map that receives bitmap entries keyed by
 *        the file base name without extension.
 * @param files std::map<wxString, wxString>& Output map that receives non-PNG file contents keyed
 *        by the full file name.
 *
 * @return void
 */
void LoadImages(const wxString& skin, std::map<wxString, wxBitmap>& bitmaps, std::map<wxString, wxString>& files);
} // namespace wxc_runtime

#endif // WXC_RUNTIME_H
