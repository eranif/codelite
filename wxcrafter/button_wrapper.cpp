#include "button_wrapper.h"
#include "allocator_mgr.h"
#include "bool_property.h"
#include "choice_property.h"
#include "file_ficker_property.h"
#include "import_from_wxFB.h"
#include "import_from_xrc.h"
#include "wxc_bitmap_code_generator.h"
#include "wxgui_bitmaploader.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include "xy_pair.h"
#include <wx/button.h>

ButtonWrapper::ButtonWrapper()
    : wxcWidget(ID_WXBUTTON)
{
    PREPEND_STYLE(wxBU_BOTTOM, false);
    PREPEND_STYLE(wxBU_EXACTFIT, false);
    PREPEND_STYLE(wxBU_LEFT, false);
    PREPEND_STYLE(wxBU_RIGHT, false);
    PREPEND_STYLE(wxBU_TOP, false);

    wxArrayString directions;
    directions.Add("wxLEFT");
    directions.Add("wxRIGHT");
    directions.Add("wxTOP");
    directions.Add("wxBOTTOM");

    RegisterEvent(wxT("wxEVT_COMMAND_BUTTON_CLICKED"), wxT("wxCommandEvent"),
                  _("Process a wxEVT_COMMAND_BUTTON_CLICKED event, when the button is clicked."),
                  wxT("wxCommandEventHandler"));

    m_namePattern = wxT("m_button");
    SetPropertyString(_("Common Settings"), "wxButton");
    AddProperty(new StringProperty(PROP_LABEL, _("My Button"), _("The button label")));
    AddProperty(new BoolProperty(PROP_DEFAULT_BUTTON, false, _("Make this button the default button")));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH, wxT(""), _("Select the bitmap file (wx2.9.X and later)")));
    AddProperty(new ChoiceProperty(PROP_DIRECTION, directions, 0,
                                   _("The position of the bitmap inside the button. By default it is positioned to "
                                     "the left of the text, near to the left button border. ")));
    AddProperty(new StringProperty(PROP_MARGINS, "2,2",
                                   _("Sets x/y margins between the bitmap and the button label (wx2.9.X and later)")));

    // wxCrafter::ResourceLoader bl;
    // m_properties.Item(PROP_BITMAP_PATH)->SetValue( bl.GetPlaceHolder16ImagePath().GetFullPath() );

    SetName(GenerateName());
}

ButtonWrapper::~ButtonWrapper() {}

wxString ButtonWrapper::CppCtorCode() const
{
    wxcCodeGeneratorHelper::Get().AddBitmap(PropertyFile(PROP_BITMAP_PATH));

    XYPair xy(PropertyString(PROP_MARGINS), 2, 2);

    wxString code;
    code << GetName() << wxT(" = new ") << GetRealClassName() << "(" << GetWindowParent() << wxT(", ") << WindowID()
         << wxT(", ") << Label() << wxT(", wxDefaultPosition, ") << SizeAsString() << wxT(", ") << StyleFlags(wxT("0"))
         << wxT(");\n");

    if(PropertyString(PROP_DEFAULT_BUTTON) == wxT("1")) { code << GetName() << wxT("->SetDefault();\n"); }

    wxString bmpCpp = wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_BITMAP_PATH));
    if(bmpCpp != "wxNullBitmap") {
        code << wxCrafter::WX294_BLOCK_START();
        code << GetName() << "->SetBitmap(" << bmpCpp << ", " << PropertyString(PROP_DIRECTION) << ");\n";
        code << GetName() << "->SetBitmapMargins(" << xy.ToString() << ");\n";
        code << wxCrafter::WXVER_CHECK_BLOCK_END();
    }
    code << CPPCommonAttributes();
    return code;
}

void ButtonWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCLabel() << XRCStyle() << XRCSize() << XRCCommonAttributes() << XRCBitmap()
         << "<bitmapposition>" << PropertyString(PROP_DIRECTION) << "</bitmapposition>" << wxT("<default>")
         << PropertyString(PROP_DEFAULT_BUTTON) << wxT("</default>") << XRCSuffix();
}

wxString ButtonWrapper::GetWxClassName() const { return wxT("wxButton"); }

void ButtonWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/button.h>")); }

void ButtonWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("bitmap"));
    if(propertynode) { ImportFromXrc::ProcessBitmapProperty(propertynode, this, PROP_BITMAP_PATH, "wxART_BUTTON"); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("default"));
    if(propertynode) { SetPropertyString(PROP_DEFAULT_BUTTON, propertynode->GetNodeContent()); }
}

void ButtonWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("bitmap"));
    if(propertynode) { ImportFromXrc::ProcessBitmapProperty(propertynode, this, PROP_BITMAP_PATH, "wxART_BUTTON"); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("default"));
    if(propertynode) { SetPropertyString(PROP_DEFAULT_BUTTON, propertynode->GetNodeContent()); }
}

void ButtonWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
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
