#include "font_picker_ctrl_wrapper.h"
#include "allocator_mgr.h"
#include "font_property.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/fontpicker.h>

FontPickerCtrlWrapper::FontPickerCtrlWrapper()
    : wxcWidget(ID_WXFONTPICKER)
{
    SetPropertyString(_("Common Settings"), "wxFontPickerCtrl");
    DelProperty(PROP_FONT);

    PREPEND_STYLE(wxFNTP_USE_TEXTCTRL, false);
    PREPEND_STYLE(wxFNTP_FONTDESC_AS_LABEL, false);
    PREPEND_STYLE(wxFNTP_USEFONT_FOR_LABEL, false);
    PREPEND_STYLE(wxFNTP_DEFAULT_STYLE, true);

    RegisterEvent(wxT("wxEVT_COMMAND_FONTPICKER_CHANGED"), wxT("wxFontPickerEvent"),
                  _("Generated whenever the selected font changes."));
    AddProperty(new FontProperty(PROP_VALUE, wxEmptyString, _("Initial font")));
    m_namePattern = wxT("m_fontPicker");
    SetName(GenerateName());
}

FontPickerCtrlWrapper::~FontPickerCtrlWrapper() {}

wxcWidget* FontPickerCtrlWrapper::Clone() const { return new FontPickerCtrlWrapper(); }

wxString FontPickerCtrlWrapper::CppCtorCode() const
{
    wxString code;
    wxString fontCode;
    wxString fontMemberName = GetName() + wxT("Font");
    if(wxCrafter::FontToCpp(PropertyString(PROP_VALUE), fontMemberName) != wxT("wxNullFont")) {
        code << wxCrafter::FontToCpp(PropertyString(PROP_VALUE), fontMemberName);
        fontCode = fontMemberName;

    } else {
        fontCode = wxT("wxNullFont");
    }

    code << GetName() << wxT(" = new ") << GetRealClassName() << wxT("(") << GetWindowParent() << wxT(", ")
         << WindowID() << wxT(", ") << fontCode << wxT(", ") << wxT("wxDefaultPosition, ") << SizeAsString()
         << wxT(", ") << StyleFlags(wxT("wxFNTP_DEFAULT_STYLE")) << wxT(");\n");

    code << CPPCommonAttributes();
    return code;
}

void FontPickerCtrlWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/fontpicker.h>"));
}

wxString FontPickerCtrlWrapper::GetWxClassName() const { return wxT("wxFontPickerCtrl"); }

void FontPickerCtrlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes() << wxT("<value>")
         << wxCrafter::FontToXRC(PropertyString(PROP_VALUE)) << wxT("</value>") << XRCSuffix();
}

void FontPickerCtrlWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("value"));
    if(propertynode) { SetPropertyString(PROP_VALUE, wxCrafter::XRCToFontstring(propertynode)); }
}

void FontPickerCtrlWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("value"));
    if(propertynode) { SetPropertyString(PROP_VALUE, wxCrafter::XRCToFontstring(propertynode)); }
}

void FontPickerCtrlWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "value");
    if(propertynode) {
        wxString fontasstring = wxCrafter::FBToFontstring(propertynode->GetNodeContent());
        if(!fontasstring.empty()) { SetPropertyString(PROP_VALUE, fontasstring); }
    }
}
