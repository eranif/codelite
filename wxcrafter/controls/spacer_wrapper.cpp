#include "spacer_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"

SpacerWrapper::SpacerWrapper()
    : wxcWidget(ID_WXSPACER)
{
    m_styles.Clear();
    m_properties.DeleteValues();

    AddProperty(new CategoryProperty(_("Spacer")));
    AddProperty(new StringProperty(PROP_NAME, wxT("Spacer"), wxT("")));
    // This was removed above because sizers don't do 'size'. But wxSpacer does...
    AddProperty(new StringProperty(PROP_SIZE, wxT("0,0"), _("The spacer's size: width,height")));

    m_namePattern = wxT("Spacer");
    SetName(GenerateName());
}

SpacerWrapper::~SpacerWrapper() {}

wxcWidget* SpacerWrapper::Clone() const { return new SpacerWrapper(); }

wxString SpacerWrapper::CppCtorCode() const
{
    wxString cpp;
    wxSize size = GetSize();

    if(GetParent()->GetType() == ID_WXGRIDBAGSIZER) {
        cpp << GetParent()->GetName() << wxT("->Add(") << wxCrafter::ToString(size.x) << wxT(", ")
            << wxCrafter::ToString(size.y) << wxT(", ") << wxT("wxGBPosition(") << GetGbPos() << wxT("), ")
            << wxT("wxGBSpan(") << GetGbSpan() << wxT("), ") << SizerFlags(wxT("0")) << wxT(", ")
            << m_sizerItem.GetBorderScaled() << wxT(");\n");
    } else {
        cpp << GetParent()->GetName() << wxT("->Add(") << wxCrafter::ToString(size.x) << wxT(", ")
            << wxCrafter::ToString(size.y) << wxT(", ") << m_sizerItem.GetProportion() << wxT(", ")
            << SizerFlags(wxT("0")) << wxT(", ") << m_sizerItem.GetBorderScaled() << wxT(");\n");
    }
    return cpp;
}

void SpacerWrapper::GetIncludeFile(wxArrayString& headers) const { wxUnusedVar(headers); }

wxString SpacerWrapper::GetWxClassName() const { return wxT(""); }

void SpacerWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxUnusedVar(type);
    wxUnusedVar(text);
}

void SpacerWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    // For some curious reason, wxFB stores spacer size in separate height and width nodes :/
    wxString ht("0"), wd("0");
    wxXmlNode* child = node->GetChildren();
    while(child) {
        wxString childname(child->GetName());
        if(childname == wxT("property")) {
            wxString attr(XmlUtils::ReadString(child, wxT("name")));

            if(attr == "height") { ht = child->GetNodeContent(); }
        }
        if(childname == wxT("property")) {
            wxString attr(XmlUtils::ReadString(child, wxT("name")));

            if(attr == "width") { wd = child->GetNodeContent(); }
        }
        child = child->GetNext();
    }

    PropertyBase* prop = GetProperty(PROP_SIZE);
    if(prop) { prop->SetValue(wd + "," + ht); }
}
