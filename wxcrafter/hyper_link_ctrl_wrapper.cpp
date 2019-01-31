#include "hyper_link_ctrl_wrapper.h"
#include "allocator_mgr.h"
#include "color_property.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/hyperlink.h>

HyperLinkCtrlWrapper::HyperLinkCtrlWrapper()
    : wxcWidget(ID_WXHYPERLINK)
{
    PREPEND_STYLE_FALSE(wxHL_ALIGN_LEFT);
    PREPEND_STYLE_FALSE(wxHL_ALIGN_RIGHT);
    PREPEND_STYLE_FALSE(wxHL_ALIGN_CENTRE);
    PREPEND_STYLE_FALSE(wxHL_CONTEXTMENU);
    PREPEND_STYLE_TRUE(wxHL_DEFAULT_STYLE);

    RegisterEvent(wxT("wxEVT_COMMAND_HYPERLINK"), wxT("wxHyperlinkEvent"),
                  _("The hyperlink was (left) clicked. If this event is not handled in user's code (or it's skipped; "
                    "see wxEvent::Skip)\nthen a call to wxLaunchDefaultBrowser is done with the hyperlink's URL."));

    SetPropertyString(_("Common Settings"), "wxHyperLinkCtrl");
    AddProperty(new StringProperty(PROP_LABEL, wxT("codelite IDE"), _("The label of the hyperlink")));
    AddProperty(
        new StringProperty(PROP_URL, wxT("http://www.codelite.org"), _("The URL associated with the given label")));
    AddProperty(
        new ColorProperty(PROP_NORMAL_COLOR, wxT("#0000FF"),
                          _("Sets the colour used to print the label when the link has never been clicked "
                            "before\n(i.e. the link has not been visited) and the mouse is not over the control.")));
    AddProperty(new ColorProperty(PROP_VISITED_COLOR, wxT("#FF0000"),
                                  _("Sets the colour used to print the label when the mouse is not over the control "
                                    "and the link has already been clicked before\n(i.e. the link has been visited)")));
    AddProperty(new ColorProperty(
        PROP_HOVER_COLOR, wxT("#0000FF"),
        _("Sets the colour used to print the label of the hyperlink when the mouse is over the control")));

    m_namePattern = wxT("m_hyperLink");
    SetName(GenerateName());
}

HyperLinkCtrlWrapper::~HyperLinkCtrlWrapper() {}

wxcWidget* HyperLinkCtrlWrapper::Clone() const { return new HyperLinkCtrlWrapper(); }

wxString HyperLinkCtrlWrapper::CppCtorCode() const
{
    wxString code;
    code << GetName() << wxT(" = new ") << GetRealClassName() << wxT("(") << GetWindowParent() << wxT(", ")
         << WindowID() << wxT(", ") << wxCrafter::UNDERSCORE(PropertyString(PROP_LABEL)) << wxT(", ")
         << wxCrafter::WXT(PropertyString(PROP_URL)) << wxT(", ") << wxT("wxDefaultPosition, ") << SizeAsString()
         << wxT(", ") << StyleFlags(wxT("wxHL_DEFAULT_STYLE")) << wxT(");\n");

    code << CPPCommonAttributes();
    wxString color;
    color = wxCrafter::ColourToCpp(PropertyString(PROP_NORMAL_COLOR));
    if(color.IsEmpty() == false) { code << GetName() << wxT("->SetNormalColour(") << color << wxT(");\n"); }

    color = wxCrafter::ColourToCpp(PropertyString(PROP_HOVER_COLOR));
    if(color.IsEmpty() == false) { code << GetName() << wxT("->SetHoverColour(") << color << wxT(");\n"); }

    color = wxCrafter::ColourToCpp(PropertyString(PROP_VISITED_COLOR));
    if(color.IsEmpty() == false) { code << GetName() << wxT("->SetVisitedColour(") << color << wxT(");\n"); }
    return code;
}

void HyperLinkCtrlWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/hyperlink.h>"));
}

wxString HyperLinkCtrlWrapper::GetWxClassName() const { return wxT("wxHyperlinkCtrl"); }

void HyperLinkCtrlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCCommonAttributes() << XRCStyle() << XRCSize() << XRCLabel() << wxT("<url>")
         << wxCrafter::XMLEncode(PropertyString(PROP_URL)) << wxT("</url>") << XRCSuffix();
}

void HyperLinkCtrlWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    wxcWidget::LoadPropertiesFromXRC(node); // Baseclass stuff first

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("url"));
    if(propertynode) { SetPropertyString(PROP_URL, propertynode->GetNodeContent()); }
    // XRC doesn't seem to do the other properties
}

void HyperLinkCtrlWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    wxcWidget::LoadPropertiesFromwxSmith(node); // Baseclass stuff first

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("url"));
    if(propertynode) { SetPropertyString(PROP_URL, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("normal_color"));
    if(propertynode) { SetPropertyString(PROP_NORMAL_COLOR, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("hover_color"));
    if(propertynode) { SetPropertyString(PROP_HOVER_COLOR, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("visited_color"));
    if(propertynode) { SetPropertyString(PROP_VISITED_COLOR, propertynode->GetNodeContent()); }
}

void HyperLinkCtrlWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "url");
    if(propertynode) { SetPropertyString(PROP_URL, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "hover_color");
    if(propertynode) {
        wxString col = wxCrafter::ValueToColourString(propertynode->GetNodeContent());
        if(!col.empty()) { SetPropertyString(PROP_HOVER_COLOR, col); }
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", "normal_color");
    if(propertynode) {
        wxString col = wxCrafter::ValueToColourString(propertynode->GetNodeContent());
        if(!col.empty()) { SetPropertyString(PROP_NORMAL_COLOR, col); }
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", "visited_color");
    if(propertynode) {
        wxString col = wxCrafter::ValueToColourString(propertynode->GetNodeContent());
        if(!col.empty()) { SetPropertyString(PROP_VISITED_COLOR, col); }
    }
}
