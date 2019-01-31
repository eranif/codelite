#include "static_bitmap_wrapper.h"
#include "allocator_mgr.h"
#include "file_ficker_property.h"
#include "import_from_wxFB.h"
#include "import_from_xrc.h"
#include "wxc_bitmap_code_generator.h"
#include "xmlutils.h"

StaticBitmapWrapper::StaticBitmapWrapper()
    : wxcWidget(ID_WXSTATICBITMAP)
{
    SetPropertyString(_("Common Settings"), "wxStaticBitmap");
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH, wxT(""), wxT("Select the bitmap file")));
    m_namePattern = wxT("m_staticBitmap");
    SetName(GenerateName());
}

StaticBitmapWrapper::~StaticBitmapWrapper() {}

wxcWidget* StaticBitmapWrapper::Clone() const { return new StaticBitmapWrapper(); }

void StaticBitmapWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/statbmp.h>")); }

wxString StaticBitmapWrapper::GetWxClassName() const { return wxT("wxStaticBitmap"); }

wxString StaticBitmapWrapper::CppCtorCode() const
{
    wxString code;
    wxcCodeGeneratorHelper::Get().AddBitmap(PropertyFile(PROP_BITMAP_PATH));
    code << GetName() << wxT(" = new ") << GetRealClassName() << "(" << GetWindowParent() << wxT(", ")
         << PropertyString(PROP_WINDOW_ID) << wxT(", ")
         << wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_BITMAP_PATH)) << wxT(", ")
         << wxT("wxDefaultPosition, ") << SizeAsString() << wxT(", 0 );\n");
    code << CPPCommonAttributes();
    return code;
}

void StaticBitmapWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCBitmap() << XRCSize() << XRCCommonAttributes() << XRCSuffix();
}

void StaticBitmapWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("bitmap"));
    if(propertynode) { ImportFromXrc::ProcessBitmapProperty(propertynode, this, PROP_BITMAP_PATH, "wxART_BUTTON"); }
}

void StaticBitmapWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("bitmap"));
    if(propertynode) { ImportFromXrc::ProcessBitmapProperty(propertynode, this, PROP_BITMAP_PATH, "wxART_BUTTON"); }
}

void StaticBitmapWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "bitmap");
    if(propertynode) {
        ImportFromwxFB::ProcessBitmapProperty(propertynode->GetNodeContent(), this, PROP_BITMAP_PATH, "wxART_BUTTON");
    }
}
