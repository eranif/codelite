#include "check_box_wrapper.h"
#include "allocator_mgr.h"
#include "bool_property.h"
#include "string_property.h"
#include "wxgui_defs.h"
#include "xmlutils.h"
#include <wx/checkbox.h>

CheckBoxWrapper::CheckBoxWrapper()
    : wxcWidget(ID_WXCHECKBOX)
{
    SetPropertyString(_("Common Settings"), "wxCheckBox");
    AddProperty(new StringProperty(PROP_LABEL, _("My CheckBox"), _("The Checkbox label")));
    AddProperty(new BoolProperty(PROP_VALUE, false, _("Value")));

    PREPEND_STYLE(wxCHK_2STATE, false);
    PREPEND_STYLE(wxCHK_3STATE, false);
    PREPEND_STYLE(wxCHK_ALLOW_3RD_STATE_FOR_USER, false);
    PREPEND_STYLE(wxALIGN_RIGHT, false);

    RegisterEventCommand(wxT("wxEVT_COMMAND_CHECKBOX_CLICKED"),
                         _("Process a wxEVT_COMMAND_CHECKBOX_CLICKED event, when the checkbox is clicked."));
    m_namePattern = wxT("m_checkBox");
    SetName(GenerateName());
}

CheckBoxWrapper::~CheckBoxWrapper() {}

wxcWidget* CheckBoxWrapper::Clone() const { return new CheckBoxWrapper(); }
wxString CheckBoxWrapper::CppCtorCode() const
{
    wxString code;
    code << GetName() << wxT(" = new ") << GetRealClassName() << "(" << GetWindowParent() << wxT(", ") << WindowID()
         << wxT(", ") << Label() << wxT(", ") << wxT("wxDefaultPosition") << wxT(", ") << SizeAsString() << wxT(", ")
         << StyleFlags(wxT("0")) << wxT(");\n");

    if(PropertyString(PROP_VALUE) == wxT("1")) {
        code << GetName() << wxT("->SetValue(true);\n");

    } else {
        code << GetName() << wxT("->SetValue(false);\n");
    }
    code << CPPCommonAttributes();
    return code;
}

void CheckBoxWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/checkbox.h>")); }

wxString CheckBoxWrapper::GetWxClassName() const { return wxT("wxCheckBox"); }

void CheckBoxWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCCommonAttributes() << XRCStyle() << XRCSize() << XRCLabel() << wxT("<checked>")
         << PropertyString(PROP_VALUE) << wxT("</checked>") << XRCSuffix();
}

void CheckBoxWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("checked"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}

void CheckBoxWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("checked"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}

void CheckBoxWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", wxT("checked"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}
