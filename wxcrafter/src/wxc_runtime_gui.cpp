// GUI implementation of the wxCrafter runtime shim.
// Linked into the wxcrafter executable and the wxCrafterPlugin shared library.

#include "Zip/clZipReader.h"
#include "event_notifier.h"
#include "main.h"
#include "wxc_runtime.h"
#include "wxgui_bitmaploader.h"

#include <wx/frame.h>
#include <wx/imaglist.h>

namespace wxc_runtime
{

wxImageList* CreateAllocatorImageList() { return new wxImageList(16, 16, true); }

int AddImageToAllocator(wxImageList* list, const wxCrafter::ResourceLoader& loader, const wxString& bmpName)
{
    if (!list) {
        return -1;
    }
    return list->Add(loader.Bitmap(bmpName));
}

void LoadImages(const wxString& skin, std::map<wxString, wxBitmap>& bitmaps, std::map<wxString, wxString>& files)
{
    wxString zipFile;
#ifdef __WXMSW__
    wxFileName zip_path{wxStandardPaths::Get().GetExecutablePath()};
    zip_path.SetName(skin);
    zip_path.SetExt("zip");
    zip_path.RemoveLastDir();
    zipFile = zip_path.GetFullPath();
#else
    zipFile << wxStandardPaths::Get().GetDataDir() << wxFileName::GetPathSeparator() << skin << wxT(".zip");
#endif
    clZipReader zip(zipFile);

    std::unordered_map<wxString, clZipReader::Entry> entries;
    zip.ExtractAll(entries);
    if (entries.empty()) {
        return;
    }

    // Loop over the files
    for (const auto& entry : entries) {
        wxFileName fn = wxFileName(entry.first);
        wxString name = fn.GetName();
        clZipReader::Entry d = entry.second;
        if (d.len && d.buffer) {
            // Avoid wxAsserts by checking it's likely to be a png before creating the image
            if (fn.GetExt() == "png") {
                wxMemoryInputStream is(d.buffer, d.len);
                wxImage img(is, wxBITMAP_TYPE_PNG);
                wxBitmap bmp(img);
                if (bmp.IsOk()) {
                    bitmaps[name] = bmp;
                }
            } else {
                wxString fileContent((const char*)d.buffer, d.len);
                files.insert({fn.GetFullName(), fileContent});
            }
            // release the memory
            free(d.buffer);
        }
    }
    entries.clear();
}
} // namespace wxc_runtime
