#include "static_box_sizer_wrapper.h"

#include "allocator_mgr.h"
#include "choice_property.h"
#include "string_property.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"

StaticBoxSizerWrapper::StaticBoxSizerWrapper()
{
    m_type = ID_WXSTATICBOXSIZER;
    m_styles.Clear(); // Sizer has no styles

    wxArrayString arr;
    arr.Add("Vertical");
    arr.Add("Horizontal");

    SetPropertyString(_("Common Settings"), "wxStaticBoxSizer");
    AddProperty(new ChoiceProperty(PROP_ORIENTATION, arr, 0, _("Sizer orientation")));
    AddProperty(new StringProperty(PROP_LABEL, _("My Label"), _("Label")));

    m_namePattern = "staticBoxSizer";
    SetName(GenerateName());
}

StaticBoxSizerWrapper::~StaticBoxSizerWrapper() {}

wxcWidget* StaticBoxSizerWrapper::Clone() const { return new StaticBoxSizerWrapper(); }

wxString StaticBoxSizerWrapper::CppCtorCode() const
{
    wxString code;
    wxString orient;
    PropertyString(PROP_ORIENTATION) == "Horizontal" ? orient = "wxHORIZONTAL" : orient = "wxVERTICAL";

    wxString staticBox;
    staticBox << " new wxStaticBox(" << GetWindowParent() << ", wxID_ANY, " << Label() << ")";

    if(!KeepAsClassMember()) {
        code << "wxStaticBoxSizer* ";
    }

    code << GetName() << " = new wxStaticBoxSizer(" << staticBox << ", " << orient << ");\n";
    code << GenerateMinSizeCode();

    if(IsMainSizer()) {
        if(GetParent()->IsTopWindow()) {
            code << "this->SetSizer(" << GetName() << ");\n";

        } else {
            code << GetParent()->GetName() << "->SetSizer(" << GetName() << ");\n";
        }
    }
    return code;
}

wxString StaticBoxSizerWrapper::GetWxClassName() const { return "wxStaticBoxSizer"; }

void StaticBoxSizerWrapper::SetOrientation(const wxString& orient)
{
    DoSetPropertyStringValue(PROP_ORIENTATION, orient);
}

void StaticBoxSizerWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxString orient;
    PropertyString(PROP_ORIENTATION) == "Horizontal" ? orient = "wxHORIZONTAL" : orient = "wxVERTICAL";
    text << XRCPrefix() << "<sizeritem><object class=\"spacer\"/></sizeritem>" << GenerateMinSizeXRC() << "<orient>"
         << orient << "</orient>" << XRCLabel();

    ChildrenXRC(text, type);
    text << XRCSuffix();
}

void StaticBoxSizerWrapper::GetIncludeFile(wxArrayString& headers) const
{
    SizerWrapperBase::GetIncludeFile(headers);
    headers.Add("#include <wx/statbox.h>");
}

void StaticBoxSizerWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, "orient");
    if(propertynode) {
        // BoxSizerWrapper uses wxVERTICAL/wxHORIZONTAL but for some reason StaticBoxSizerWrapper has
        // Vertical/Horizontal
        bool horizontal = propertynode->GetNodeContent().Lower().Contains("horizontal");
        SetPropertyString(PROP_ORIENTATION, horizontal ? "Horizontal" : "Vertical");
    }
}

void StaticBoxSizerWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, "orient");
    if(propertynode) {
        bool horizontal = propertynode->GetNodeContent().Lower().Contains("horizontal");
        SetPropertyString(PROP_ORIENTATION, horizontal ? "Horizontal" : "Vertical");
    }
}

void StaticBoxSizerWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "orient");
    if(propertynode) {
        bool horizontal = propertynode->GetNodeContent().Lower().Contains("horizontal");
        SetPropertyString(PROP_ORIENTATION, horizontal ? "Horizontal" : "Vertical");
    }
}
