#include "static_text_wrapper.h"
#include "allocator_mgr.h"
#include "multi_strings_property.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"

StaticTextWrapper::StaticTextWrapper()
    : wxcWidget(ID_WXSTATICTEXT)
{
    PREPEND_STYLE(wxALIGN_LEFT, false);
    PREPEND_STYLE(wxALIGN_RIGHT, false);
    PREPEND_STYLE(wxALIGN_CENTRE, false);
    PREPEND_STYLE(wxST_NO_AUTORESIZE, false);

    SetPropertyString(_("Common Settings"), "wxStaticText");
    AddProperty(new MultiStringsProperty(PROP_LABEL, _("Static Text Label"), wxT("\\n"), _("Label:")));
    m_properties.Item(PROP_LABEL)->SetValue(_("Static Text Label"));

    AddProperty(
        new StringProperty(PROP_WRAP, wxT("-1"), _("Wrap the text after N pixels. If set to -1, don't wrap")));

    m_namePattern = wxT("m_staticText");
    SetName(GenerateName());
}

StaticTextWrapper::~StaticTextWrapper() {}

wxString StaticTextWrapper::GetWxClassName() const { return wxT("wxStaticText"); }

wxString StaticTextWrapper::CppCtorCode() const
{
    // ( this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
    wxString code;
    code << GetName() << wxT(" = new ") << GetRealClassName() << "(" << GetWindowParent() << wxT(", ") << WindowID()
         << wxT(", ") << Label() << wxT(", wxDefaultPosition, ") << SizeAsString() << wxT(", ") << StyleFlags(wxT("0"))
         << wxT(");\n");

    if(wxCrafter::ToNumber(PropertyString(PROP_WRAP), -1) >= 0) { // The default value is -1
        code << GetName() << "->Wrap(" << PropertyString(PROP_WRAP) << ");\n";
    }

    code << CPPCommonAttributes();
    return code;
}

void StaticTextWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxString xrc;
    xrc << XRCPrefix() << XRCLabel() << XRCStyle() << XRCSize() << XRCCommonAttributes();
    if(wxCrafter::ToNumber(PropertyString(PROP_WRAP), -1) >= 0) { // The default value is -1
        xrc << wxT("<wrap>") << PropertyString(PROP_WRAP) << wxT("</wrap>");
    }
    xrc << XRCSuffix();
    text << xrc;
}

void StaticTextWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("wrap"));
    if(propertynode) { SetPropertyString(PROP_WRAP, propertynode->GetNodeContent()); }
}

// wxSmith doesn't wrap

void StaticTextWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "wrap");
    if(propertynode) { SetPropertyString(PROP_WRAP, propertynode->GetNodeContent()); }
}

wxcWidget* StaticTextWrapper::Clone() const { return new StaticTextWrapper(); }

void StaticTextWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/stattext.h>")); }
