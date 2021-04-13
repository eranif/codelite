#include "bitmap_button_wrapper.h"
#include "allocator_mgr.h"
#include "bitmap_picker_property.h"
#include "bool_property.h"
#include "file_ficker_property.h"
#include "import_from_wxFB.h"
#include "import_from_xrc.h"
#include "wxc_bitmap_code_generator.h"
#include "wxgui_bitmaploader.h"
#include "xmlutils.h"
#include <wx/bmpbuttn.h>

BitmapButtonWrapper::BitmapButtonWrapper()
    : wxcWidget(ID_WXBITMAPBUTTON)
{
    PREPEND_STYLE(wxBU_AUTODRAW, true);
    PREPEND_STYLE(wxBU_LEFT, false);
    PREPEND_STYLE(wxBU_RIGHT, false);
    PREPEND_STYLE(wxBU_TOP, false);

    RegisterEvent(wxT("wxEVT_COMMAND_BUTTON_CLICKED"), wxT("wxCommandEvent"),
                  _("Process a wxEVT_COMMAND_BUTTON_CLICKED event, when the button is clicked."),
                  wxT("wxCommandEventHandler"));

    m_namePattern = wxT("m_bmpButton");
    SetPropertyString(_("Common Settings"), "wxBitmapButton");
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH, wxT(""), _("Select the bitmap file")));
    AddProperty(new BoolProperty(PROP_DEFAULT_BUTTON, false, _("Make this button the default button")));

    wxCrafter::ResourceLoader bl;
    m_properties.Item(PROP_BITMAP_PATH)->SetValue(bl.GetPlaceHolder16ImagePath().GetFullPath());

    SetName(GenerateName());
}

BitmapButtonWrapper::~BitmapButtonWrapper() {}

wxString BitmapButtonWrapper::GetWxClassName() const { return wxT("wxBitmapButton"); }

wxString BitmapButtonWrapper::CppCtorCode() const
{
    wxcCodeGeneratorHelper::Get().AddBitmap(PropertyFile(PROP_BITMAP_PATH));

    wxString code;
    code << GetName() << wxT(" = new ") << GetRealClassName() << "(" << GetWindowParent() << wxT(", ") << WindowID()
         << wxT(", ") << wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_BITMAP_PATH))
         << wxT(", wxDefaultPosition, ") << SizeAsString() << wxT(", ") << StyleFlags(wxT("0")) << wxT(");\n");

    if(PropertyString(PROP_DEFAULT_BUTTON) == wxT("1")) { code << GetName() << wxT("->SetDefault();"); }
    code << CPPCommonAttributes();
    return code;
}

void BitmapButtonWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCBitmap() << XRCSize() << XRCStyle() << XRCCommonAttributes() << wxT("<default>")
         << PropertyString(PROP_DEFAULT_BUTTON) << wxT("</default>") << wxT("</object>");
}

void BitmapButtonWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/bmpbuttn.h>")); }

void BitmapButtonWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("bitmap"));
    if(propertynode) { ImportFromXrc::ProcessBitmapProperty(propertynode, this, PROP_BITMAP_PATH, "wxART_BUTTON"); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("default"));
    if(propertynode) { SetPropertyString(PROP_DEFAULT_BUTTON, propertynode->GetNodeContent()); }
}

void BitmapButtonWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("bitmap"));
    if(propertynode) { ImportFromXrc::ProcessBitmapProperty(propertynode, this, PROP_BITMAP_PATH, "wxART_BUTTON"); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("default"));
    if(propertynode) { SetPropertyString(PROP_DEFAULT_BUTTON, propertynode->GetNodeContent()); }
}

void BitmapButtonWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "bitmap");
    if(propertynode) {
        ImportFromwxFB::ProcessBitmapProperty(propertynode->GetNodeContent(), this, PROP_BITMAP_PATH, "wxART_BUTTON");
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", "default");
    if(propertynode) { SetPropertyString(PROP_DEFAULT_BUTTON, propertynode->GetNodeContent()); }
}
