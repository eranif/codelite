#include "slider_wrapper.h"
#include "allocator_mgr.h"
#include "xmlutils.h"
#include <wx/slider.h>

SliderWrapper::SliderWrapper()
    : wxcWidget(ID_WXSLIDER)
{
    SetPropertyString(_("Common Settings"), "wxSlider");
    AddProperty(new StringProperty(PROP_VALUE, wxT("50"), _("Value")));
    AddProperty(new StringProperty(PROP_MINVALUE, wxT("0"), _("Minimum slider value")));
    AddProperty(new StringProperty(PROP_MAXVALUE, wxT("100"), _("Maximum slider value")));

    PREPEND_STYLE(wxSL_HORIZONTAL, true);
    PREPEND_STYLE(wxSL_VERTICAL, false);
    PREPEND_STYLE(wxSL_AUTOTICKS, false);
    PREPEND_STYLE(wxSL_LABELS, false);
    PREPEND_STYLE(wxSL_LEFT, false);
    PREPEND_STYLE(wxSL_RIGHT, false);
    PREPEND_STYLE(wxSL_TOP, false);
    PREPEND_STYLE(wxSL_BOTTOM, false);
    PREPEND_STYLE(wxSL_SELRANGE, false);
    PREPEND_STYLE(wxSL_INVERSE, false);

    RegisterEvent(wxT("wxEVT_SCROLL_TOP"), wxT("wxScrollEvent"),
                  _("Process wxEVT_SCROLL_TOP scroll-to-top events (minimum position)"));
    RegisterEvent(wxT("wxEVT_SCROLL_BOTTOM"), wxT("wxScrollEvent"),
                  _("Process wxEVT_SCROLL_BOTTOM scroll-to-bottom events (maximum position)."));
    RegisterEvent(wxT("wxEVT_SCROLL_LINEUP"), wxT("wxScrollEvent"), _("Process wxEVT_SCROLL_LINEUP line up events."));
    RegisterEvent(wxT("wxEVT_SCROLL_LINEDOWN"), wxT("wxScrollEvent"),
                  _("Process wxEVT_SCROLL_LINEDOWN line down events."));
    RegisterEvent(wxT("wxEVT_SCROLL_PAGEUP"), wxT("wxScrollEvent"), _("Process wxEVT_SCROLL_PAGEUP page up events."));
    RegisterEvent(wxT("wxEVT_SCROLL_PAGEDOWN"), wxT("wxScrollEvent"),
                  _("Process wxEVT_SCROLL_PAGEDOWN page down events"));
    RegisterEvent(wxT("wxEVT_SCROLL_THUMBTRACK"), wxT("wxScrollEvent"),
                  _("Process wxEVT_SCROLL_THUMBTRACK thumbtrack events (frequent events sent as the user drags the "
                      "thumbtrack)."));
    RegisterEvent(wxT("wxEVT_SCROLL_THUMBRELEASE"), wxT("wxScrollEvent"),
                  _("Process wxEVT_SCROLL_THUMBRELEASE thumb release events."));
    RegisterEvent(wxT("wxEVT_SCROLL_CHANGED"), wxT("wxScrollEvent"),
                  _("Process wxEVT_SCROLL_CHANGED end of scrolling events (MSW only)."));

    m_namePattern = wxT("m_slider");
    SetName(GenerateName());
}

SliderWrapper::~SliderWrapper() {}

wxcWidget* SliderWrapper::Clone() const { return new SliderWrapper(); }

wxString SliderWrapper::CppCtorCode() const
{
    // m_slider3 = new wxSlider( m_panel11, wxID_ANY, 150, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    wxString code;
    code << GetName() << wxT(" = new ") << GetRealClassName() << "(" << GetWindowParent() << wxT(", ") << WindowID()
         << wxT(", ") << PropertyString(PROP_VALUE) << wxT(", ") << PropertyString(PROP_MINVALUE) << wxT(", ")
         << PropertyString(PROP_MAXVALUE) << wxT(", ") << wxT("wxDefaultPosition, ") << SizeAsString() << wxT(", ")
         << StyleFlags(wxT("0")) << wxT(");\n");
    code << CPPCommonAttributes();
    return code;
}

void SliderWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/slider.h>")); }

wxString SliderWrapper::GetWxClassName() const { return wxT("wxSlider"); }

void SliderWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCCommonAttributes() << wxT("<min>") << PropertyString(PROP_MINVALUE)
         << wxT("</min>") << wxT("<max>") << PropertyString(PROP_MAXVALUE) << wxT("</max>") << XRCValue() << XRCSize()
         << XRCSuffix();
}

void SliderWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("min"));
    if(propertynode) { SetPropertyString(PROP_MINVALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("max"));
    if(propertynode) { SetPropertyString(PROP_MAXVALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("value"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}

void SliderWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("min"));
    if(propertynode) { SetPropertyString(PROP_MINVALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("max"));
    if(propertynode) { SetPropertyString(PROP_MAXVALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("value"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}

void SliderWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "minValue");
    if(propertynode) { SetPropertyString(PROP_MINVALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "maxValue");
    if(propertynode) { SetPropertyString(PROP_MAXVALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "value");
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}
