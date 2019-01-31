#ifndef BITMAPLOADER_H
#define BITMAPLOADER_H

#include <map>
#include <wx/bitmap.h>
#include <wx/filename.h>
#include <wx/string.h>

namespace wxCrafter
{
class ResourceLoader
{
protected:
    static std::map<wxString, wxBitmap> m_bitmaps;
    static std::map<wxString, wxString> m_files;

public:
    ResourceLoader(const wxString& skin = wxT("wxgui"));
    virtual ~ResourceLoader();

    const wxBitmap& Bitmap(const wxString& name) const;
    wxString File(const wxString& name) const;
    /**
     * @brief return the path to a temporary image place holder
     */
    wxFileName GetPlaceHolderImagePath() const;
    wxFileName GetPlaceHolder16ImagePath() const;
};
} // namespace wxCrafter

#endif // BITMAPLOADER_H
