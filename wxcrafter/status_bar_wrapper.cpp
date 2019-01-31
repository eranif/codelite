#include "status_bar_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/statusbr.h>

StatusBarWrapper::StatusBarWrapper()
    : wxcWidget(ID_WXSTATUSBAR)
{
    PREPEND_STYLE_FALSE(wxSTB_SIZEGRIP);
    PREPEND_STYLE_FALSE(wxSTB_SHOW_TIPS);
    PREPEND_STYLE_FALSE(wxSTB_ELLIPSIZE_START);
    PREPEND_STYLE_FALSE(wxSTB_ELLIPSIZE_MIDDLE);
    PREPEND_STYLE_FALSE(wxSTB_ELLIPSIZE_END);
    PREPEND_STYLE_TRUE(wxSTB_DEFAULT_STYLE);

    SetPropertyString(_("Common Settings"), "wxStatusBar");
    AddProperty(new StringProperty(PROP_FIELD_COUNT, wxT("1"), _("Sets the number of fields")));

    m_namePattern = wxT("m_statusBar");
    SetName(GenerateName());
}

StatusBarWrapper::~StatusBarWrapper() {}

wxcWidget* StatusBarWrapper::Clone() const { return new StatusBarWrapper(); }

wxString StatusBarWrapper::CppCtorCode() const
{
    // wxStatusBar(wxWindow* parent, wxWindowID id = wxID_ANY, long style = wxST_SIZEGRIP, const wxString& name =
    // "statusBar")
    wxString code;
    code << GetName() << wxT(" = new ") << GetRealClassName() << wxT("(") << GetWindowParent() << wxT(", ")
         << WindowID() << wxT(", ") << StyleFlags(wxT("wxST_SIZEGRIP")) << wxT(");\n");
    code << GetName() << wxT("->SetFieldsCount(") << wxCrafter::ToNumber(PropertyString(PROP_FIELD_COUNT), 1)
         << wxT(");\n");
    code << GetWindowParent() << wxT("->SetStatusBar(") << GetName() << wxT(");\n");
    return code;
}

void StatusBarWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/statusbr.h>")); }

wxString StatusBarWrapper::GetWxClassName() const { return wxT("wxStatusBar"); }

void StatusBarWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type == XRC_DESIGNER) { return; }

    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes() << wxT("<fields>")
         << wxCrafter::ToNumber(PropertyString(PROP_FIELD_COUNT), 1) << wxT("</fields>") << XRCSuffix();
}

wxString StatusBarWrapper::DesignerXRC() const
{
    wxString text;
    text << wxT("<object class=\"") << GetWxClassName() << wxT("\" name=\"STATUS_BAR_ID\">");
    text << wxT("<style>") << wxT("</style>") // No style
         << XRCSize() << XRCCommonAttributes() << wxT("<fields>")
         << wxCrafter::ToNumber(PropertyString(PROP_FIELD_COUNT), 1) << wxT("</fields>") << XRCSuffix();
    return text;
}

void StatusBarWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("fields"));
    if(propertynode) { SetPropertyString(PROP_FIELD_COUNT, propertynode->GetNodeContent()); }
}

void StatusBarWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("fields"));
    if(propertynode) { SetPropertyString(PROP_FIELD_COUNT, propertynode->GetNodeContent()); }
}

void StatusBarWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "fields");
    if(propertynode) { SetPropertyString(PROP_FIELD_COUNT, propertynode->GetNodeContent()); }
}
