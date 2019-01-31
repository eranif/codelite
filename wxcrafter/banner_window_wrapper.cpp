#include "banner_window_wrapper.h"
#include "allocator_mgr.h"
#include "choice_property.h"
#include "color_property.h"
#include "file_ficker_property.h"
#include "multi_strings_property.h"
#include "wxc_bitmap_code_generator.h"
#include "wxc_settings.h"
#include "wxgui_helpers.h"
#include <wx/bannerwindow.h>

BannerWindowWrapper::BannerWindowWrapper()
    : wxcWidget(ID_WXBANNERWINDOW)
{
    wxArrayString options;
    options.Add("wxTOP");
    options.Add("wxBOTTOM");
    options.Add("wxLEFT");
    options.Add("wxRIGHT");

    SetPropertyString(_("Common Settings"), "wxBannerWindow");
    AddProperty(new MultiStringsProperty(
        PROP_TITLE, _("The Title\nTitle is rendered in bold and should be single line"), "\\n", ""));
    AddProperty(new MultiStringsProperty(PROP_MESSAGE,
                                         _("Message can have multiple lines but is not wrapped automatically\ninclude "
                                           "explicit line breaks in the string if you want to have multiple lines"),
                                         "\\n", ""));
    AddProperty(
        new ChoiceProperty(PROP_ORIENTATION, options, 2,
                           _("The banner orientation changes how the text in it is displayed and also defines where is "
                             "the bitmap truncated if it's too big to fit\nbut doesn't do anything for the banner "
                             "position, this is supposed to be taken care of in the usual way, e.g. using sizers")));
    AddProperty(new BitmapPickerProperty(
        PROP_BITMAP_PATH, wxT(""),
        wxT("Select the bitmap file\nImportant: You can set text and title OR a bitmap, but not both")));
    AddProperty(new ColorProperty(PROP_COLOR_GRADIENT_START));
    AddProperty(new ColorProperty(PROP_COLOR_GRADIENT_END));

    SetPropertyString(PROP_TITLE, "Title");
    SetPropertyString(PROP_MESSAGE, "Descriptive message");
    m_namePattern = "m_banner";
    SetName(GenerateName());
}

BannerWindowWrapper::~BannerWindowWrapper() {}

wxcWidget* BannerWindowWrapper::Clone() const { return new BannerWindowWrapper(); }

wxString BannerWindowWrapper::CppCtorCode() const
{
    wxcCodeGeneratorHelper::Get().AddBitmap(PropertyFile(PROP_BITMAP_PATH));
    wxString cppCode;
    cppCode << GetName() << " = new " << GetRealClassName() << "(" << GetWindowParent() << ", " << WindowID() << ", "
            << PropertyString(PROP_ORIENTATION) << ", "
            << "wxDefaultPosition, " << SizeAsString() << ", " << StyleFlags("0") << ");\n";

    cppCode << GetName() << "->SetBitmap(" << wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_BITMAP_PATH))
            << ");\n";
    cppCode << GetName() << "->SetText(" << wxCrafter::UNDERSCORE(PropertyString(PROP_TITLE)) << ", "
            << wxCrafter::UNDERSCORE(PropertyString(PROP_MESSAGE)) << ");\n";

    wxString col1, col2;
    col1 = wxCrafter::ColourToCpp(PropertyString(PROP_COLOR_GRADIENT_START));
    col2 = wxCrafter::ColourToCpp(PropertyString(PROP_COLOR_GRADIENT_END));

    if(!col1.IsEmpty() && !col2.IsEmpty()) {
        cppCode << GetName() << "->SetGradient(" << col1 << ", " << col2 << ");\n";
    }

    cppCode << CPPCommonAttributes();
    return cppCode;
}

void BannerWindowWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add("#include <wx/bannerwindow.h>"); }
wxString BannerWindowWrapper::GetWxClassName() const { return "wxBannerWindow"; }

void BannerWindowWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxUnusedVar(type);
    text << XRCPrefix() << XRCSize() << XRCStyle() << XRCCommonAttributes() << XRCBitmap() << "<title>"
         << wxCrafter::CDATA(PropertyString(PROP_TITLE)) << "</title>"
         << "<message>" << wxCrafter::CDATA(PropertyString(PROP_MESSAGE)) << "</message>"
         << "<direction>" << PropertyString(PROP_ORIENTATION) << "</direction>"
         << "<gradient-start>" << wxCrafter::GetColourForXRC(PropertyString(PROP_COLOR_GRADIENT_START))
         << "</gradient-start>"
         << "<gradient-end>" << wxCrafter::GetColourForXRC(PropertyString(PROP_COLOR_GRADIENT_END)) << "</gradient-end>"
         << XRCSuffix();
}

bool BannerWindowWrapper::IsLicensed() const { return wxcSettings::Get().IsLicensed(); }
