#include "radio_button_wrapper.h"
#include "allocator_mgr.h"
#include "bool_property.h"
#include "string_property.h"
#include "xmlutils.h"

RadioButtonWrapper::RadioButtonWrapper()
    : wxcWidget(ID_WXRADIOBUTTON)
{
    PREPEND_STYLE(wxRB_GROUP, false);
    PREPEND_STYLE(wxRB_SINGLE, false);

    SetPropertyString(_("Common Settings"), "wxRadioButton");
    AddProperty(new StringProperty(PROP_LABEL, wxT("My RadioButton"), wxT("Label")));
    AddProperty(new BoolProperty(PROP_VALUE, true, wxT("Initial value")));

    RegisterEventCommand(wxT("wxEVT_COMMAND_RADIOBUTTON_SELECTED"),
                         wxT("Process a wxEVT_COMMAND_RADIOBUTTON_SELECTED event, when the radiobutton is clicked."));

    m_namePattern = wxT("m_radioButton");
    SetName(GenerateName());
}

RadioButtonWrapper::~RadioButtonWrapper() {}

wxcWidget* RadioButtonWrapper::Clone() const { return new RadioButtonWrapper(); }

wxString RadioButtonWrapper::CppCtorCode() const
{
    // m_radioBtn3 = new wxRadioButton( m_panel11, wxID_ANY, wxT("RadioBtn"), wxDefaultPosition, wxDefaultSize,
    // wxRB_USE_CHECKBOX ); m_radioBtn3->SetValue( true );

    wxString code;
    code << GetName() << wxT(" = new ") << GetRealClassName() << "(" << GetWindowParent() << wxT(", ") << WindowID()
         << wxT(", ") << Label() << wxT(", ") << wxT("wxDefaultPosition, ") << SizeAsString() << wxT(", ")
         << StyleFlags(wxT("0")) << wxT(");\n");
    code << CPPCommonAttributes();
    code << GetName() << wxT("->SetValue(") << PropertyString(PROP_VALUE) << wxT(");\n");
    return code;
}

void RadioButtonWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/radiobut.h>")); }

wxString RadioButtonWrapper::GetWxClassName() const { return wxT("wxRadioButton"); }

void RadioButtonWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCLabel() << XRCCommonAttributes() << XRCValue() << XRCSuffix();
}

void RadioButtonWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("value"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}

void RadioButtonWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("label"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
    propertynode = XmlUtils::FindFirstByTagName(node, wxT("selected"));
    if(propertynode) { SetPropertyString(PROP_SELECTION, propertynode->GetNodeContent()); }
}

void RadioButtonWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "value");
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}
