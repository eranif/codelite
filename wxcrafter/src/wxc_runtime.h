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
