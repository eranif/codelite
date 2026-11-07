#ifndef BITMAPLOADER_H
#define BITMAPLOADER_H

#include <map>
#include <string>
#include <wx/bitmap.h>
#include <wx/bmpbndl.h>
#include <wx/filename.h>
#include <wx/string.h>

namespace wxCrafter
{
/// Loads the images of wxCrafter from the skin file (wxgui.zip).
///
/// An image can be an SVG (bitmaps/dark/<name>.svg and bitmaps/light/<name>.svg, one for each theme of the
/// application) or a PNG (<name>.png). When both exist, the SVG is used.
class ResourceLoader
{
protected:
    static std::map<wxString, wxBitmap> m_bitmaps;       // The PNG files
    static std::map<wxString, std::string> m_svgsDark;   // The content of the SVG files of the dark theme
    static std::map<wxString, std::string> m_svgsLight;  // The content of the SVG files of the light theme
    static std::map<wxString, wxBitmapBundle> m_bundles; // The SVG files that were rendered, by theme and name
    static std::map<wxString, wxBitmap> m_svgBitmaps;    // The same as bitmaps of 16x16

public:
    ResourceLoader(const wxString& skin = wxT("wxgui"));
    virtual ~ResourceLoader() = default;

    /// The image as a bitmap of 16x16 (a null bitmap if there is no such image)
    const wxBitmap& Bitmap(const wxString& name) const;

    /// The image for the current theme as a bundle: it keeps its quality on high DPI screens (an empty bundle if
    /// there is no such image)
    wxBitmapBundle Bundle(const wxString& name) const;

    /**
     * @brief return the path to a temporary image place holder
     */
    wxFileName GetPlaceHolderImagePath() const;
    wxFileName GetPlaceHolder16ImagePath() const;

private:
    /// The PNG file of `name` (a null bitmap if there is no such file)
    const wxBitmap& PngBitmap(const wxString& name) const;
    /// The bundle of the SVG file of `name` for the current theme (an empty bundle if there is no such file)
    wxBitmapBundle SvgBundle(const wxString& name) const;
};
} // namespace wxCrafter

#endif // BITMAPLOADER_H
