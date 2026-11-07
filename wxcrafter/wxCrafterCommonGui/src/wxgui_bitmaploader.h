#ifndef BITMAPLOADER_H
#define BITMAPLOADER_H

#include <map>
#include <string>
#include <wx/bitmap.h>
#include <wx/bmpbndl.h>
#include <wx/filename.h>
#include <wx/string.h>
#include <wx/window.h>

namespace wxCrafter
{
/// Loads the images of wxCrafter from the skin file (wxgui.zip).
///
/// The images are SVG files, one for each theme of the application: bitmaps/dark/<name>.svg and
/// bitmaps/light/<name>.svg. The theme is the one of the application when the image is asked for.
class ResourceLoader
{
protected:
    static std::map<wxString, std::string> m_svgsDark;   // The content of the SVG files of the dark theme
    static std::map<wxString, std::string> m_svgsLight;  // The content of the SVG files of the light theme
    static std::map<wxString, wxBitmapBundle> m_bundles; // The SVG files that were parsed, by theme and name
    static std::map<wxString, wxBitmap> m_bitmaps;       // The same as bitmaps of 16x16

public:
    ResourceLoader(const wxString& skin = wxT("wxgui"));
    virtual ~ResourceLoader() = default;

    /// The size of the images in pixels: 16x16 DIP
    static wxSize GetDefaultSize();

    /// The image as a bitmap of 16x16 (a null bitmap if there is no such image)
    const wxBitmap& Bitmap(const wxString& name) const;

    /// The image as a bitmap of `size` (a null bitmap if there is no such image)
    wxBitmap Bitmap(const wxString& name, const wxSize& size) const;

    /// The image for the current theme as a bundle: it keeps its quality on high DPI screens (an empty bundle if
    /// there is no such image)
    wxBitmapBundle Bundle(const wxString& name) const;

    /**
     * @brief return the path to a temporary image place holder (a PNG file of 32x32, drawn from its SVG file)
     */
    wxFileName GetPlaceHolderImagePath() const;
    /// The same, with an image of 16x16
    wxFileName GetPlaceHolder16ImagePath() const;

private:
    /// Draw the image `name` with `size` and save it as a PNG file in the temp folder. Return the path of the file,
    /// relative to the folder of the project
    wxFileName SaveAsTempPng(const wxString& name, const wxSize& size) const;
};
} // namespace wxCrafter

#endif // BITMAPLOADER_H
