#include "colour_picker_wrapper.h"
#include "allocator_mgr.h"
#include "color_property.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/clrpicker.h>
#include <wx/msgdlg.h>

ColourPickerWrapper::ColourPickerWrapper()
    : wxcWidget(ID_WXCOLORPICKER)
{

    PREPEND_STYLE(wxCLRP_DEFAULT_STYLE, true);
    PREPEND_STYLE(wxCLRP_USE_TEXTCTRL, false);
    PREPEND_STYLE(wxCLRP_SHOW_LABEL, false);

    SetPropertyString(_("Common Settings"), "wxColourPickerCtrl");
    AddProperty(new ColorProperty(PROP_VALUE, wxT("<Default>"), _("Colour")));
    RegisterEvent(wxT("wxEVT_COMMAND_COLOURPICKER_CHANGED"), wxT("wxColourPickerEvent"),
                  _("The user changed the colour selected in the control either using the button or using text "
                    "control\n(see wxCLRP_USE_TEXTCTRL; note that in this case the event is fired only if the user's "
                    "input is valid, i.e. recognizable)."));

    m_namePattern = wxT("m_colourPicker");
    SetName(GenerateName());
}

ColourPickerWrapper::~ColourPickerWrapper() {}

wxcWidget* ColourPickerWrapper::Clone() const { return new ColourPickerWrapper(); }

wxString ColourPickerWrapper::CppCtorCode() const
{
    wxString code;
    wxString color = wxCrafter::ColourToCpp(PropertyString(PROP_VALUE));
    if(color.IsEmpty()) { color << wxT("*wxBLACK"); }

    code << GetName() << wxT(" = new ") << GetRealClassName() << wxT("(") << GetWindowParent() << wxT(", ")
         << WindowID() << wxT(", ") << color << wxT(", ") << wxT("wxDefaultPosition, ") << SizeAsString() << wxT(", ")
         << StyleFlags(wxT("wxCLRP_DEFAULT_STYLE")) << wxT(");\n");

    code << CPPCommonAttributes();
    return code;
}

void ColourPickerWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/clrpicker.h>"));
}

wxString ColourPickerWrapper::GetWxClassName() const { return wxT("wxColourPickerCtrl"); }

void ColourPickerWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxString s;
    wxString value = PropertyString(PROP_VALUE);
    wxColour col(value);

    s << XRCPrefix() << XRCStyle() << XRCSize();

    if(value != wxT("<Default>")) { s << wxT("<value>") << wxCrafter::GetColourForXRC(value) << wxT("</value>"); }

    s << XRCCommonAttributes() << XRCSuffix();

    text << s;
}

void ColourPickerWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("value"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}

void ColourPickerWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("colour"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}

void ColourPickerWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "colour");
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}
