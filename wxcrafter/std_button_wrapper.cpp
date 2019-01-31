#include "std_button_wrapper.h"
#include "allocator_mgr.h"
#include "bool_property.h"
#include "choice_property.h"
#include "multi_strings_property.h"
#include "wxgui_defs.h"
#include "xmlutils.h"

StdButtonWrapper::StdButtonWrapper()
    : wxcWidget(ID_WXSTDBUTTON)
{
    m_properties.DeleteValues();
    wxArrayString ids;
    ids.Add("wxID_OK");
    ids.Add("wxID_YES");
    ids.Add("wxID_SAVE");
    ids.Add("wxID_APPLY");
    ids.Add("wxID_CLOSE");
    ids.Add("wxID_NO");
    ids.Add("wxID_CANCEL");
    ids.Add("wxID_HELP");
    ids.Add("wxID_CONTEXT_HELP");

    RegisterEvent(wxT("wxEVT_COMMAND_BUTTON_CLICKED"), wxT("wxCommandEvent"),
                  wxT("Process a wxEVT_COMMAND_BUTTON_CLICKED event, when the button is clicked."),
                  wxT("wxCommandEventHandler"));

    AddProperty(new CategoryProperty(_("Standard wxButton")));
    AddProperty(new ChoiceProperty(PROP_WINDOW_ID, ids, 0, _("Button ID")));
    AddProperty(new StringProperty(PROP_NAME, wxT(""), _("C++ member name")));
    AddProperty(new MultiStringsProperty(PROP_TOOLTIP, _("Tooltip"), wxT("\\n"), _("Tooltip text:")));
    AddProperty(new BoolProperty(PROP_DEFAULT_BUTTON, false, wxT("Make this button the default button")));
    m_namePattern = "m_button";
    SetName(GenerateName());
}

StdButtonWrapper::~StdButtonWrapper() {}

wxcWidget* StdButtonWrapper::Clone() const { return new StdButtonWrapper(); }

wxString StdButtonWrapper::CppCtorCode() const
{
    wxString code;
    code << CPPStandardWxCtorWithLabel("0");
    if(PropertyString(PROP_DEFAULT_BUTTON) == wxT("1")) { code << GetName() << wxT("->SetDefault();\n"); }
    code << CPPCommonAttributes();
    return code;
}

void StdButtonWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/button.h>")); }

wxString StdButtonWrapper::GetWxClassName() const { return "wxButton"; }

void StdButtonWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << "<object class=\"button\">" << wxT("<object class=\"") << GetWxClassName() << wxT("\" name=\"") << GetId()
         << wxT("\">") << XRCLabel() << XRCStyle() << XRCSize() << XRCCommonAttributes() << wxT("<default>")
         << PropertyString(PROP_DEFAULT_BUTTON) << wxT("</default>") << XRCSuffix() // wxButton
         << XRCSuffix();                                                            // button
}

void StdButtonWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("default"));
    if(propertynode) { SetPropertyString(PROP_DEFAULT_BUTTON, propertynode->GetNodeContent()); }
}

// wxFB doesn't do anything here
