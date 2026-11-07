#include "wxgui_bitmaploader.h"

#include "Zip/clZipReader.h"
#include "clSystemSettings.h"
#include "wxc_project_metadata.h"

#include <wx/app.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

std::map<wxString, std::string> wxCrafter::ResourceLoader::m_svgsDark;
std::map<wxString, std::string> wxCrafter::ResourceLoader::m_svgsLight;
std::map<wxString, wxBitmapBundle> wxCrafter::ResourceLoader::m_bundles;
std::map<wxString, wxBitmap> wxCrafter::ResourceLoader::m_bitmaps;

namespace
{

wxSize GetDefaultSize()
{
    static std::optional<wxSize> defaultSize{std::nullopt};
    if (!defaultSize) {
        auto* frame = wxTheApp->GetTopWindow();
        defaultSize = frame ? frame->FromDIP(wxSize(16, 16)) : wxSize(16, 16);
    }
    return *defaultSize;
}

void LoadImages(const wxString& skin,
                std::map<wxString, std::string>& svgsDark,
                std::map<wxString, std::string>& svgsLight)
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
        clZipReader::Entry d = entry.second;
        if (d.len && d.buffer) {
            if (fn.GetExt() == "svg") {
                // bitmaps/dark/<name>.svg or bitmaps/light/<name>.svg
                const wxString name = fn.GetName();
                const wxArrayString& dirs = fn.GetDirs();
                if (dirs.Index("dark") != wxNOT_FOUND) {
                    svgsDark[name] = std::string(reinterpret_cast<const char*>(d.buffer), d.len);
                } else if (dirs.Index("light") != wxNOT_FOUND) {
                    svgsLight[name] = std::string(reinterpret_cast<const char*>(d.buffer), d.len);
                }
            }
            // release the memory
            free(d.buffer);
        }
    }
    entries.clear();
}
} // namespace

wxCrafter::ResourceLoader::ResourceLoader(const wxString& skin)
{
    if (m_svgsDark.empty() && m_svgsLight.empty()) {
        LoadImages(skin, m_svgsDark, m_svgsLight);
    }
}

wxBitmapBundle wxCrafter::ResourceLoader::Bundle(const wxString& name) const
{
    const bool isDark = clSystemSettings::IsDark();
    const auto& svgs = isDark ? m_svgsDark : m_svgsLight;
    auto svg = svgs.find(name);
    if (svg == svgs.end()) {
        return wxBitmapBundle();
    }

    // Parse it once for each theme
    const wxString key = (isDark ? wxT("dark:") : wxT("light:")) + name;
    auto iter = m_bundles.find(key);
    if (iter == m_bundles.end()) {
        iter = m_bundles.emplace(key, wxBitmapBundle::FromSVG(svg->second.c_str(), GetDefaultSize())).first;
    }
    return iter->second;
}

wxBitmap wxCrafter::ResourceLoader::Bitmap(const wxString& name, const wxSize& size) const
{
    const wxBitmapBundle bundle = Bundle(name);
    return bundle.IsOk() ? bundle.GetBitmap(size) : wxBitmap();
}

const wxBitmap& wxCrafter::ResourceLoader::Bitmap(const wxString& name) const
{
    static wxBitmap s_nullBitmap;
    const wxString key = (clSystemSettings::IsDark() ? wxT("dark:") : wxT("light:")) + name;
    auto iter = m_bitmaps.find(key);
    if (iter == m_bitmaps.end()) {
        const wxBitmap bmp = Bitmap(name, GetDefaultSize());
        if (!bmp.IsOk()) {
            return s_nullBitmap;
        }
        iter = m_bitmaps.emplace(key, bmp).first;
    }
    return iter->second;
}

wxFileName wxCrafter::ResourceLoader::SaveAsTempPng(const wxString& name, const wxSize& size) const
{
    const wxBitmap bmp = Bitmap(name, size);
    if (bmp.IsOk()) {
        wxFileName path(wxFileName::GetTempDir(), name + wxT(".png"));
        if (bmp.SaveFile(path.GetFullPath(), wxBITMAP_TYPE_PNG)) {
            path.MakeRelativeTo(wxcProjectMetadata::Get().GetProjectPath());
            return path;
        }
    }
    return wxFileName();
}

wxFileName wxCrafter::ResourceLoader::GetPlaceHolderImagePath() const
{
    // This image is bigger than the others (it was a PNG file of 32x32)
    return SaveAsTempPng(wxT("placeholder"), wxSize(32, 32));
}

wxFileName wxCrafter::ResourceLoader::GetPlaceHolder16ImagePath() const
{
    return SaveAsTempPng(wxT("placeholder16"), wxSize(16, 16));
}
