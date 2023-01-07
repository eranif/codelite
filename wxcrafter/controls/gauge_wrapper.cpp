#include "gauge_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/gauge.h>

GaugeWrapper::GaugeWrapper()
    : wxcWidget(ID_WXGAUGE)
{
    PREPEND_STYLE(wxGA_HORIZONTAL, true);
    PREPEND_STYLE(wxGA_VERTICAL, false);
    PREPEND_STYLE(wxGA_SMOOTH, false);

    AddProperty(new StringProperty(
        PROP_RANGE, wxT("100"),
        _("Integer range (maximum value) of the gauge. It is ignored when the gauge is used in indeterminate mode.")));
    AddProperty(new StringProperty(PROP_VALUE, wxT("10"), _("Sets the position of the gauge")));

    m_namePattern = wxT("m_gauge");
    SetName(GenerateName());
}

GaugeWrapper::~GaugeWrapper() {}

wxcWidget* GaugeWrapper::Clone() const { return new GaugeWrapper(); }

wxString GaugeWrapper::CppCtorCode() const
{
    // wxGauge(wxWindow* parent, wxWindowID id, int range, const wxPoint& pos = wxDefaultPosition, const wxSize& size =
    // wxDefaultSize, long style = wxGA_HORIZONTAL, const wxValidator& validator = wxDefaultValidator, const wxString&
    // name = "gauge")
    wxString cpp;
    cpp << GetName() << wxT(" = new ") << GetRealClassName() << "(" << GetWindowParent() << wxT(", ") << WindowID()
        << wxT(", ") << PropertyString(PROP_RANGE) << wxT(", ") << wxT("wxDefaultPosition, ") << SizeAsString()
        << wxT(", ") << StyleFlags(wxT("wxGA_HORIZONTAL")) << wxT(");\n");
    cpp << CPPCommonAttributes();
    int pos = wxCrafter::ToNumber(PropertyString(PROP_VALUE), 0);
    cpp << GetName() << wxT("->SetValue(") << pos << wxT(");\n");
    return cpp;
}

void GaugeWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/gauge.h>")); }

wxString GaugeWrapper::GetWxClassName() const { return wxT("wxGauge"); }

void GaugeWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << wxT("<range>") << wxCrafter::ToNumber(PropertyString(PROP_RANGE), 100) << wxT("</range>")
         << wxT("<value>") << wxCrafter::ToNumber(PropertyString(PROP_VALUE), 10) << wxT("</value>") << XRCStyle()
         << XRCCommonAttributes() << XRCSize() << XRCSuffix();
}

void GaugeWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("range"));
    if(propertynode) { SetPropertyString(PROP_RANGE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("value"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}

void GaugeWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("range"));
    if(propertynode) { SetPropertyString(PROP_RANGE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("value"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}

void GaugeWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "range");
    if(propertynode) { SetPropertyString(PROP_RANGE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "value");
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}
