#include "html_window_wrapper.h"
#include "allocator_mgr.h"
#include "string_property.h"
#include "wxgui_defs.h"
#include "xmlutils.h"
#include <wx/html/htmlwin.h>

HtmlWindowWrapper::HtmlWindowWrapper()
    : wxcWidget(ID_WXHTMLWIN)
{
    PREPEND_STYLE(wxHW_SCROLLBAR_NEVER, false);
    PREPEND_STYLE(wxHW_SCROLLBAR_AUTO, true);
    PREPEND_STYLE(wxHW_NO_SELECTION, false);

    SetPropertyString(_("Common Settings"), "wxHtmlWindow");
    AddProperty(new StringProperty(PROP_HTMLCODE, wxT("<b>wxHtmlWindow control!</b>"), wxT("HTML code to load")));
    AddProperty(new StringProperty(PROP_URL, wxT(""), wxT("URL to load")));

    RegisterEvent(wxT("wxEVT_COMMAND_HTML_CELL_CLICKED"), wxT("wxHtmlCellEvent"), wxT("A wxHtmlCell was clicked."));
    RegisterEvent(wxT("wxEVT_COMMAND_HTML_CELL_HOVER"), wxT("wxHtmlCellEvent"),
                  wxT("The mouse passed over a wxHtmlCell."));
    RegisterEvent(wxT("wxEVT_COMMAND_HTML_LINK_CLICKED"), wxT("wxHtmlLinkEvent"),
                  wxT("A wxHtmlCell which contains an hyperlink was clicked."));

    m_namePattern = wxT("m_htmlWin");
    SetName(GenerateName());
}

HtmlWindowWrapper::~HtmlWindowWrapper() {}

wxcWidget* HtmlWindowWrapper::Clone() const { return new HtmlWindowWrapper(); }

wxString HtmlWindowWrapper::CppCtorCode() const
{
    wxString code;
    code << CPPStandardWxCtor(wxT("wxHW_DEFAULT_STYLE"));

    wxString htmlCode = PropertyString(PROP_HTMLCODE);
    htmlCode.Trim().Trim(false);
    htmlCode.Prepend(wxT("wxT(\"")).Append(wxT("\")"));

    if(htmlCode.IsEmpty() == false) { code << GetName() << wxT("->SetPage(") << htmlCode << wxT(");\n"); }

    wxString url = PropertyFile(PROP_URL);
    url.Trim().Trim(false);
    if(url.IsEmpty() == false) { code << GetName() << wxT("->LoadPage(") << url << wxT(");\n"); }
    code << CPPCommonAttributes();
    return code;
}

void HtmlWindowWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/html/htmlwin.h>"));
}

wxString HtmlWindowWrapper::GetWxClassName() const { return wxT("wxHtmlWindow"); }

void HtmlWindowWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes();

    wxString html = PropertyString(PROP_HTMLCODE);
    html.Trim().Trim(false);
    if(html.IsEmpty() == false) { text << wxT("<htmlcode><![CDATA[") << html << wxT("]]></htmlcode>"); }
    text << XRCSuffix();
}

void HtmlWindowWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("htmlcode"));
    if(propertynode) { SetPropertyString(PROP_HTMLCODE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("url"));
    if(propertynode) { SetPropertyString(PROP_URL, propertynode->GetNodeContent()); }
}

void HtmlWindowWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("htmlcode"));
    if(propertynode) { SetPropertyString(PROP_HTMLCODE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("url"));
    if(propertynode) { SetPropertyString(PROP_URL, propertynode->GetNodeContent()); }
}

// wxFB doesn't do anything useful for this
