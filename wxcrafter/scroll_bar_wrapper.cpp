#include "scroll_bar_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/scrolbar.h>

ScrollBarWrapper::ScrollBarWrapper()
    : wxcWidget(ID_WXSCROLLBAR)
{
    PREPEND_STYLE_STR(wxT("wxSB_HORIZONTAL"), wxSB_HORIZONTAL, true);
    PREPEND_STYLE_STR(wxT("wxSB_VERTICAL"), wxSB_VERTICAL, false);

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

    SetPropertyString(_("Common Settings"), "wxScrollBar");
    AddProperty(new StringProperty(PROP_VALUE, wxT("0"), _("The position of the scrollbar in scroll units.")));
    AddProperty(new StringProperty(PROP_THUMBSIZE, wxT("1"),
                                   _("The size of the thumb, or visible portion of the scrollbar, in scroll units.")));
    AddProperty(new StringProperty(PROP_RANGE, wxT("10"), _("The maximum position of the scrollbar.")));
    AddProperty(
        new StringProperty(PROP_PAGESIZE, wxT("1"),
                           _("The size of the page size in scroll units. This is the number of units the scrollbar "
                             "will scroll when it is paged up or down.\nOften it is the same as the thumb size.")));

    m_namePattern = wxT("m_scrollBar");
    SetName(GenerateName());
}

ScrollBarWrapper::~ScrollBarWrapper() {}

wxcWidget* ScrollBarWrapper::Clone() const { return new ScrollBarWrapper(); }

wxString ScrollBarWrapper::CppCtorCode() const
{
    wxString code;
    code << CPPStandardWxCtor(wxT("wxSB_HORIZONTAL"));
    code << GetName() << wxT("->SetScrollbar(") << PropertyString(PROP_VALUE) << wxT(", ")
         << PropertyString(PROP_THUMBSIZE) << wxT(", ") << PropertyString(PROP_RANGE) << wxT(", ")
         << PropertyString(PROP_PAGESIZE) << wxT(");\n");
    return code;
}

void ScrollBarWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/scrolbar.h>")); }

wxString ScrollBarWrapper::GetWxClassName() const { return wxT("wxScrollBar"); }

void ScrollBarWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCSize() << XRCStyle() << XRCCommonAttributes() << wxT("<value>")
         << wxCrafter::XMLEncode(PropertyString(PROP_VALUE)) << wxT("</value>") << wxT("<thumbsize>")
         << wxCrafter::XMLEncode(PropertyString(PROP_THUMBSIZE)) << wxT("</thumbsize>") << wxT("<range>")
         << wxCrafter::XMLEncode(PropertyString(PROP_RANGE)) << wxT("</range>") << wxT("<pagesize>")
         << wxCrafter::XMLEncode(PropertyString(PROP_RANGE)) << wxT("</pagesize>") << XRCSuffix();
}

void ScrollBarWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("thumbsize"));
    if(propertynode) { SetPropertyString(PROP_THUMBSIZE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("range"));
    if(propertynode) { SetPropertyString(PROP_RANGE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("pagesize"));
    if(propertynode) { SetPropertyString(PROP_RANGE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("value"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}

void ScrollBarWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "thumbsize");
    if(propertynode) { SetPropertyString(PROP_THUMBSIZE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "range");
    if(propertynode) { SetPropertyString(PROP_RANGE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "pagesize");
    if(propertynode) { SetPropertyString(PROP_RANGE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "value");
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}
