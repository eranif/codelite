#include "bitmap_wrapepr.h"
#include "allocator_mgr.h"
#include "file_ficker_property.h"
#include "wxc_bitmap_code_generator.h"
#include "wxc_project_metadata.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"

BitmapWrapepr::BitmapWrapepr()
    : wxcWidget(ID_WXBITMAP)
{
    m_properties.DeleteValues();
    m_styles.Clear();
    m_sizerFlags.Clear();

    AddProperty(new CategoryProperty(_("wxBitmap")));
    AddProperty(new StringProperty(
        PROP_NAME, wxT(""),
        _("A unique name for the bitmap (across your project)\nThis name can be used later to load the bitmap from the "
          "generated class\nby simply calling: wxBitmap bmp = myimglist.Bitmap(\"my-bitmap-name\"")));
    AddProperty(new FilePickerProperty(PROP_BITMAP_PATH, wxT(""), _("Select the bitmap file")));
    m_namePattern = "m_bmp";
    SetName(GenerateName());
}

BitmapWrapepr::~BitmapWrapepr() {}

wxcWidget* BitmapWrapepr::Clone() const { return new BitmapWrapepr(); }

wxString BitmapWrapepr::CppCtorCode() const
{
    wxArrayString exts;
    exts.Add("");
    exts.Add("@2x");
    exts.Add("@1.25x");
    exts.Add("@1.5x");

    wxString bmpPath = PropertyString(PROP_BITMAP_PATH);
    wxFileName fn(bmpPath);
    // Support for hi-res images
    // The logic:
    // If we find files with the following perfixes: @2x, @1.5x, @1.25x
    // add them to the resource files as well
    for(size_t i = 0; i < exts.size(); ++i) {
        wxFileName hiResImage = fn;
        hiResImage.MakeAbsolute(wxcProjectMetadata::Get().GetProjectPath());
        hiResImage.SetName(hiResImage.GetName() + exts.Item(i));
        if(hiResImage.FileExists()) {
            wxcCodeGeneratorHelper::Get().AddBitmap(hiResImage.GetFullPath(), GetName() + exts.Item(i));
        }
    }

    wxString cppCode;
    for(size_t i = 0; i < exts.size(); ++i) {
        if(wxcCodeGeneratorHelper::Get().Contains(GetName() + exts.Item(i))) {
            cppCode << "{\n";
            cppCode << "    wxBitmap bmp;\n";
            cppCode << "    wxIcon icn;\n";
            cppCode << "    bmp = "
                    << wxcCodeGeneratorHelper::Get().BitmapCode(PropertyString(PROP_BITMAP_PATH),
                                                                GetName() + exts.Item(i))
                    << ";\n";
            cppCode << "    if(bmp.IsOk()) {\n";
            cppCode << "        if((m_imagesWidth == bmp.GetWidth()) && (m_imagesHeight == bmp.GetHeight())){\n";
            cppCode << "            icn.CopyFromBitmap(bmp);\n";
            cppCode << "            this->Add(icn);\n";
            cppCode << "        }\n";
            cppCode << "        m_bitmaps.insert(std::make_pair(" << wxCrafter::WXT(GetName() + exts.Item(i))
                    << ", bmp));\n";
            cppCode << "    }\n";
            cppCode << "}\n";
        }
    }
    return cppCode;
}

void BitmapWrapepr::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add("#include <wx/bitmap.h>");
    headers.Add("#include <wx/icon.h>");
}

wxString BitmapWrapepr::GetWxClassName() const { return ""; }

void BitmapWrapepr::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type == XRC_DESIGNER) {
        wxString designerXRC;
        designerXRC << "<object class=\"sizeritem\">"
                    << "<border>5</border>"
                    << "<flag>wxALL</flag>"
                    << "<object class=\"wxStaticBitmap\" name=\"" << GetName() << "\">" << wxT("<bitmap>")
                    << PropertyFile(PROP_BITMAP_PATH) << wxT("</bitmap>") << XRCSuffix() // wxStaticBitmap
                    << XRCSuffix();                                                      // sizeritem
        text << designerXRC;

    } else if(type == XRC_LIVE) {
        text << XRCPrefix() << PropertyFile(PROP_BITMAP_PATH) << XRCSuffix();
    }
}
