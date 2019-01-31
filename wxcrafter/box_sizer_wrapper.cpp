#include "box_sizer_wrapper.h"
#include "allocator_mgr.h"
#include "choice_property.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"

BoxSizerWrapper::BoxSizerWrapper()
{
    m_type = ID_WXBOXSIZER;
    m_styles.Clear(); // Sizer has no styles

    wxArrayString arr;
    arr.Add(wxT("wxVERTICAL"));
    arr.Add(wxT("wxHORIZONTAL"));

    SetPropertyString(_("Common Settings"), "wxBoxSizer");
    AddProperty(new ChoiceProperty(PROP_ORIENTATION, arr, 0, wxT("Sizer orientation")));

    EnableSizerFlag("wxEXPAND", true);
    m_sizerItem.SetProportion(1);

    m_namePattern = wxT("boxSizer");
    SetName(GenerateName());
}

BoxSizerWrapper::~BoxSizerWrapper() {}

wxString BoxSizerWrapper::GetWxClassName() const { return wxT("wxBoxSizer"); }

wxString BoxSizerWrapper::CppCtorCode() const
{
    wxString code;
    if(!wxcSettings::Get().HasFlag(wxcSettings::SIZERS_AS_MEMBERS)) code << "wxBoxSizer* ";

    code << GetName() << " = new wxBoxSizer(" << PropertyString(PROP_ORIENTATION) << ");\n";

    code << GenerateMinSizeCode();

    if(IsMainSizer()) {
        if(GetParent()->IsTopWindow()) {
            code << wxT("this->SetSizer(") << GetName() << wxT(");\n");

        } else {
            code << GetWindowParent() << wxT("->SetSizer(") << GetName() << wxT(");\n");
        }
    }
    return code;
}

void BoxSizerWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << wxT("<object class=\"wxBoxSizer\">") << wxT("<orient>") << PropertyString(PROP_ORIENTATION)
         << wxT("</orient>\n");
    text << GenerateMinSizeXRC();
    ChildrenXRC(text, type);
    text << wxT("</object>");
}

wxcWidget* BoxSizerWrapper::Clone() const { return new BoxSizerWrapper(); }

void BoxSizerWrapper::SetOrientation(const wxString& orient) { DoSetPropertyStringValue(PROP_ORIENTATION, orient); }

void BoxSizerWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("orient"));
    if(propertynode) { SetPropertyString(PROP_ORIENTATION, propertynode->GetNodeContent()); }
}

void BoxSizerWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    // wxSmith doesn't use "name" here, but "variable" means the same thing afaict
    wxString value = XmlUtils::ReadString(node, wxT("variable"));
    if(!value.empty()) { SetName(value); }

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("orient"));
    if(propertynode) { SetPropertyString(PROP_ORIENTATION, propertynode->GetNodeContent()); }
}

void BoxSizerWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "orient");
    if(propertynode) { SetPropertyString(PROP_ORIENTATION, propertynode->GetNodeContent()); }
}
