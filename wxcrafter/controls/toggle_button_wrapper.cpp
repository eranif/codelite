#include "toggle_button_wrapper.h"
#include "allocator_mgr.h"
#include "bool_property.h"
#include "string_property.h"
#include "wxgui_defs.h"
#include "xmlutils.h"
#include <wx/button.h>

ToggleButtonWrapper::ToggleButtonWrapper()
    : wxcWidget(ID_WXTOGGLEBUTTON)
{
    SetPropertyString(_("Common Settings"), "wxToggleButton");
    AddProperty(new StringProperty(PROP_LABEL, _("My Button"), _("The button label")));
    AddProperty(new BoolProperty(PROP_CHECKED, false, _("The button initial state")));

    PREPEND_STYLE(wxBU_BOTTOM, false);
    PREPEND_STYLE(wxBU_EXACTFIT, false);
    PREPEND_STYLE(wxBU_LEFT, false);
    PREPEND_STYLE(wxBU_RIGHT, false);
    PREPEND_STYLE(wxBU_TOP, false);

    RegisterEventCommand(wxT("wxEVT_COMMAND_TOGGLEBUTTON_CLICKED"), _("Handles a toggle button click event."));

    m_namePattern = wxT("m_toggleButton");
    SetName(GenerateName());
}

ToggleButtonWrapper::~ToggleButtonWrapper() {}

wxcWidget* ToggleButtonWrapper::Clone() const { return new ToggleButtonWrapper(); }

wxString ToggleButtonWrapper::CppCtorCode() const
{
    wxString cpp;
    cpp << CPPStandardWxCtorWithLabel(wxT("0"));
    cpp << GetName() << wxT("->SetValue(") << PropertyBool(PROP_CHECKED) << wxT(");\n");
    return cpp;
}

void ToggleButtonWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/tglbtn.h>"));
    headers.Add(wxT("#include <wx/button.h>"));
}

wxString ToggleButtonWrapper::GetWxClassName() const { return wxT("wxToggleButton"); }

void ToggleButtonWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCLabel() << XRCCommonAttributes() << wxT("<checked>")
         << PropertyString(PROP_CHECKED) << wxT("</checked>") << XRCSuffix();
}

void ToggleButtonWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("checked"));
    if(propertynode) { SetPropertyString(PROP_CHECKED, propertynode->GetNodeContent()); }
}

void ToggleButtonWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("checked"));
    if(propertynode) { SetPropertyString(PROP_CHECKED, propertynode->GetNodeContent()); }
}

void ToggleButtonWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "value");
    if(propertynode) { SetPropertyString(PROP_CHECKED, propertynode->GetNodeContent()); }
}
