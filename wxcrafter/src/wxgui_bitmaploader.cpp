#include "wxgui_bitmaploader.h"

#include "Zip/clZipReader.h"
#include "clFilesCollector.h"
#include "wxc_project_metadata.h"
#include "wxgui_globals.h"

#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

std::map<wxString, wxBitmap> wxCrafter::ResourceLoader::m_bitmaps;
std::map<wxString, wxString> wxCrafter::ResourceLoader::m_files;

wxCrafter::ResourceLoader::ResourceLoader(const wxString& skin)
{
    if(m_bitmaps.empty()) {
        wxString zipFile;
        zipFile << wxStandardPaths::Get().GetDataDir() << wxFileName::GetPathSeparator() << skin << wxT(".zip");
        clZipReader zip(zipFile);

        std::unordered_map<wxString, clZipReader::Entry> entries;
        zip.ExtractAll(entries);
        if(entries.empty()) { return; }

        // Loop over the files
        for(const auto& entry : entries) {
            wxFileName fn = wxFileName(entry.first);
            wxString name = fn.GetName();
            clZipReader::Entry d = entry.second;
            if(d.len && d.buffer) {
                // Avoid wxAsserts by checking it's likely to be a png before creating the image
                if(fn.GetExt() == "png") {
                    wxMemoryInputStream is(d.buffer, d.len);
                    wxImage img(is, wxBITMAP_TYPE_PNG);
                    wxBitmap bmp(img);
                    if(bmp.IsOk()) { m_bitmaps[name] = bmp; }
                } else {
                    wxString fileContent((const char*)d.buffer, d.len);
                    m_files.insert({ fn.GetFullName(), fileContent });
                }
                // release the memory
                free(d.buffer);
            }
        }
        entries.clear();
    }
}

wxCrafter::ResourceLoader::~ResourceLoader() {}

const wxBitmap& wxCrafter::ResourceLoader::Bitmap(const wxString& name) const
{
    static wxBitmap s_nullBitmap;
    std::map<wxString, wxBitmap>::const_iterator iter = m_bitmaps.find(name);
    if(iter == m_bitmaps.end()) { return s_nullBitmap; }
    return iter->second;
}

wxFileName wxCrafter::ResourceLoader::GetPlaceHolderImagePath() const
{
    static const wxString PLACEHOLDER_IMG = wxT("placeholder");
    const wxBitmap& bmp = Bitmap(PLACEHOLDER_IMG);
    if(bmp.IsOk()) {
        wxFileName path(wxFileName::GetTempDir(), wxT("placeholder.png"));
        if(bmp.SaveFile(path.GetFullPath(), wxBITMAP_TYPE_PNG)) {
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
    if(bmp.IsOk()) {
        wxFileName path(wxFileName::GetTempDir(), wxT("placeholder16.png"));
        if(bmp.SaveFile(path.GetFullPath(), wxBITMAP_TYPE_PNG)) {
            path.MakeRelativeTo(wxcProjectMetadata::Get().GetProjectPath());
            return path;
        }
    }
    return wxFileName();
}

wxString wxCrafter::ResourceLoader::File(const wxString& name) const
{
    std::map<wxString, wxString>::const_iterator iter = m_files.find(name);
    if(iter == m_files.end()) { return wxT(""); }
    return iter->second;
}
