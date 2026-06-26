#include "wxgui_bitmaploader.h"

#include "wxc_project_metadata.h"
#include "wxc_runtime.h"

#include <wx/filename.h>

std::map<wxString, wxBitmap> wxCrafter::ResourceLoader::m_bitmaps;

wxCrafter::ResourceLoader::ResourceLoader(const wxString& skin)
{
    if (m_bitmaps.empty()) {
        wxc_runtime::LoadImages(skin, m_bitmaps);
    }
}

const wxBitmap& wxCrafter::ResourceLoader::Bitmap(const wxString& name) const
{
    static wxBitmap s_nullBitmap;
    std::map<wxString, wxBitmap>::const_iterator iter = m_bitmaps.find(name);
    if (iter == m_bitmaps.end()) {
        return s_nullBitmap;
    }
    return iter->second;
}

wxFileName wxCrafter::ResourceLoader::GetPlaceHolderImagePath() const
{
    static const wxString PLACEHOLDER_IMG = wxT("placeholder");
    const wxBitmap& bmp = Bitmap(PLACEHOLDER_IMG);
    if (bmp.IsOk()) {
        wxFileName path(wxFileName::GetTempDir(), wxT("placeholder.png"));
        if (bmp.SaveFile(path.GetFullPath(), wxBITMAP_TYPE_PNG)) {
            path.MakeRelativeTo(wxcProjectMetadata::Get().GetProjectPath());
            return path;
        }
    }
    return wxFileName();
}

wxFileName wxCrafter::ResourceLoader::GetPlaceHolder16ImagePath() const
{
    static const wxString PLACEHOLDER_IMG16 = wxT("placeholder16");
    const wxBitmap& bmp = Bitmap(PLACEHOLDER_IMG16);
    if (bmp.IsOk()) {
        wxFileName path(wxFileName::GetTempDir(), wxT("placeholder16.png"));
        if (bmp.SaveFile(path.GetFullPath(), wxBITMAP_TYPE_PNG)) {
            path.MakeRelativeTo(wxcProjectMetadata::Get().GetProjectPath());
            return path;
        }
    }
    return wxFileName();
}
