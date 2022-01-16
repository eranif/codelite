#include "image_list_wrapper.h"

#include "allocator_mgr.h"
#include "bool_property.h"
#include "category_property.h"
#include "int_property.h"
#include "virtual_folder_property.h"

ImageListWrapper::ImageListWrapper()
    : TopLevelWinWrapper(ID_WXIMAGELIST)
{
    m_properties.DeleteValues();
    m_styles.Clear();
    m_sizerFlags.Clear();

    SetPropertyString(_("Common Settings"), "wxImageList");
    AddProperty(new CategoryProperty(_("Inherited C++ Class Properties")));
    AddProperty(new StringProperty(PROP_NAME, "", _("The generated C++ class name")));
    AddProperty(new StringProperty(PROP_FILE, "", _("The filenames for the generated files")));
    AddProperty(
        new VirtualFolderProperty(PROP_VIRTUAL_FOLDER, "", _("codelite's virtual folder for the generated files")));
    AddProperty(new StringProperty(PROP_CLASS_DECORATOR, "",
                                   _("MSW Only\nC++ macro decorator - allows exporting this class from a DLL")));
    AddProperty(new CategoryProperty(_("wxImageList")));
    AddProperty(new IntProperty(PROP_BITMAP_SIZE, 16, _("The bitmaps size")));
    AddProperty(new BoolProperty(PROP_IMGLIST_MASK, true, _("True if masks should be created for all images")));
}

ImageListWrapper::~ImageListWrapper() {}

wxcWidget* ImageListWrapper::Clone() const { return new ImageListWrapper(); }

wxString ImageListWrapper::BaseCtorDecl() const
{
    wxString code;
    code << "    " << CreateBaseclassName() << "();\n";
    code << "    const wxBitmap& Bitmap(const wxString &name) const {\n"
         << "        if ( !m_bitmaps.count(name + m_resolution) )\n"
         << "            return wxNullBitmap;\n"
         << "        return m_bitmaps.find(name + m_resolution)->second;\n"
         << "    }\n\n";

    code << "    void SetBitmapResolution(const wxString &res = wxEmptyString) {\n"
         << "        m_resolution = res;\n"
         << "    }\n\n";

    return code;
}

wxString ImageListWrapper::BaseCtorImplPrefix() const
{
    wxString code;

    code << CreateBaseclassName() << "::" << CreateBaseclassName() << "()\n";
    code << "    : " << GetRealClassName() << "(" << PropertyInt(PROP_BITMAP_SIZE) << ", "
         << PropertyInt(PROP_BITMAP_SIZE) << ", " << PropertyBool(PROP_IMGLIST_MASK) << ")\n"
         << "    , m_imagesWidth(" << PropertyInt(PROP_BITMAP_SIZE) << ")\n"
         << "    , m_imagesHeight(" << PropertyInt(PROP_BITMAP_SIZE) << ")\n";
    return code;
}

wxString ImageListWrapper::DesignerXRC(bool forPreviewDialog) const
{
    wxUnusedVar(forPreviewDialog);
    wxString text;
    text << "<object class=\"wxPanel\" name=\"PreviewPanel\">"
         << "<size>-1,-1</size>"
         // Fake a sizer
         << "<object class=\"wxBoxSizer\">"
         << "<orient>wxVERTICAL</orient>";

    ChildrenXRC(text, XRC_DESIGNER);
    text << "</object>"  // wxBoxSizer
         << "</object>"; // PreviewPanel

    WrapXRC(text);
    return text;
}

wxString ImageListWrapper::CppCtorCode() const { return ""; }

void ImageListWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add("#include <wx/imaglist.h>");
    headers.Add("#include <wx/bitmap.h>");
    headers.Add("#include <wx/artprov.h>");
    headers.Add("#include <map>");
}

wxString ImageListWrapper::DoGenerateClassMember() const
{
    wxString memberCode;
    memberCode << "    // Maintain a map of all bitmaps representd by their name\n";
    memberCode << "    std::map<wxString, wxBitmap> m_bitmaps;\n";
    memberCode
        << "    // The requested image resolution (can be one of @2x, @1.5x, @1.25x or an empty string (the default)\n";
    memberCode << "    wxString m_resolution;\n";
    memberCode << "    int m_imagesWidth;\n";
    memberCode << "    int m_imagesHeight;\n";
    return memberCode;
}

wxString ImageListWrapper::GetWxClassName() const { return "wxImageList"; }

void ImageListWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type == XRC_DESIGNER) {
        ChildrenXRC(text, type);
        WrapXRC(text);
    }
}
