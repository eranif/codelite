#include "scrolled_window_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/scrolwin.h>

ScrolledWindowWrapper::ScrolledWindowWrapper()
    : wxcWidget(ID_WXSCROLLEDWIN)
{
    EnableStyle(wxT("wxHSCROLL"), true);
    EnableStyle(wxT("wxVSCROLL"), true);

    SetPropertyString(_("Common Settings"), "wxScrolledWindow");
    AddProperty(
        new StringProperty(PROP_SCROLL_RATE_X, wxT("5"), _("Pixels per scroll unit in the horizontal direction")));
    AddProperty(
        new StringProperty(PROP_SCROLL_RATE_Y, wxT("5"), _("Pixels per scroll unit in the vertical direction.")));

    RegisterEvent("wxEVT_SCROLLWIN_TOP", "wxScrollWinEvent",
                  _("Since wxWidgets 2.9.X\nProcess wxEVT_SCROLLWIN_TOP scroll-to-top events"));
    RegisterEvent("wxEVT_SCROLLWIN_BOTTOM", "wxScrollWinEvent",
                  _("Since wxWidgets 2.9.X\nProcess wxEVT_SCROLLWIN_BOTTOM scroll-to-bottom events"));
    RegisterEvent("wxEVT_SCROLLWIN_LINEUP", "wxScrollWinEvent",
                  _("Since wxWidgets 2.9.X\nProcess wxEVT_SCROLLWIN_LINEUP line up events."));
    RegisterEvent("wxEVT_SCROLLWIN_LINEDOWN", "wxScrollWinEvent",
                  _("Since wxWidgets 2.9.X\nProcess wxEVT_SCROLLWIN_LINEDOWN line down events"));
    RegisterEvent("wxEVT_SCROLLWIN_PAGEUP", "wxScrollWinEvent",
                  _("Since wxWidgets 2.9.X\nProcess wxEVT_SCROLLWIN_PAGEUP page up events"));
    RegisterEvent("wxEVT_SCROLLWIN_PAGEDOWN", "wxScrollWinEvent",
                  _("Since wxWidgets 2.9.X\nProcess wxEVT_SCROLLWIN_PAGEDOWN page down events"));
    RegisterEvent("wxEVT_SCROLLWIN_THUMBTRACK", "wxScrollWinEvent",
                  _("Since wxWidgets 2.9.X\nProcess wxEVT_SCROLLWIN_THUMBTRACK thumbtrack events (frequent events sent "
                    "as the user drags the thumbtrack)"));
    RegisterEvent("wxEVT_SCROLLWIN_THUMBRELEASE", "wxScrollWinEvent",
                  _("Since wxWidgets 2.9.X\nProcess wxEVT_SCROLLWIN_THUMBRELEASE thumb release events"));
    m_namePattern = wxT("m_scrollWin");
    SetName(GenerateName());
}

ScrolledWindowWrapper::~ScrolledWindowWrapper() {}

wxcWidget* ScrolledWindowWrapper::Clone() const { return new ScrolledWindowWrapper(); }

wxString ScrolledWindowWrapper::CppCtorCode() const
{
    wxString code;
    code << CPPStandardWxCtor(wxT("wxHSCROLL | wxVSCROLL"));

    code << GetName() << wxT("->SetScrollRate(") << PropertyString(PROP_SCROLL_RATE_X) << wxT(", ")
         << PropertyString(PROP_SCROLL_RATE_Y) << wxT(");\n");
    return code;
}

void ScrolledWindowWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/scrolwin.h>"));
}

wxString ScrolledWindowWrapper::GetWxClassName() const { return wxT("wxScrolledWindow"); }

void ScrolledWindowWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    int xx = wxCrafter::ToNumber(PropertyString(PROP_SCROLL_RATE_X), 5);
    int yy = wxCrafter::ToNumber(PropertyString(PROP_SCROLL_RATE_Y), 5);
    text << XRCPrefix() << XRCCommonAttributes() << XRCStyle() << XRCSize() << wxT("<scrollrate>") << xx << wxT(",")
         << yy << wxT("</scrollrate>");

    ChildrenXRC(text, type);

    text << XRCSuffix();
}

void ScrolledWindowWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    // Neither XRCed nor wxFB nor wxSmith actually produce any property here, but the doc says:
    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("scrollrate"));
    if(propertynode) {
        wxSize size = wxCrafter::DecodeSize(propertynode->GetNodeContent());
        if(size.x != -1) { SetPropertyString(PROP_SCROLL_RATE_X, wxCrafter::ToString(size.x)); }
        if(size.y != -1) { SetPropertyString(PROP_SCROLL_RATE_Y, wxCrafter::ToString(size.y)); }
    }
}

void ScrolledWindowWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "scroll_rate_x");
    if(propertynode) { SetPropertyString(PROP_SCROLL_RATE_X, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "scroll_rate_y");
    if(propertynode) { SetPropertyString(PROP_SCROLL_RATE_Y, propertynode->GetNodeContent()); }
}
