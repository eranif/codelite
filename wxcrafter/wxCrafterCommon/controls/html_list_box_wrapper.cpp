#include "html_list_box_wrapper.h"

#include "wxgui_defs.h"

#include <wx/htmllbox.h>

HtmlListBoxWrapper::HtmlListBoxWrapper()
    : VListBoxWrapper(ID_WXHTMLLISTBOX, "wxHtmlListBox", "m_htmlListBox")
{
    PREPEND_STYLE(wxHLB_DEFAULT_STYLE, true);
    PREPEND_STYLE(wxHLB_MULTIPLE, false);

    RegisterEvent("wxEVT_HTML_CELL_CLICKED", "wxHtmlCellEvent", _("A wxHtmlCell was clicked. See wxHtmlCellEvent"));
    RegisterEvent(
        "wxEVT_HTML_CELL_HOVER", "wxHtmlCellEvent", _("The mouse passed over a wxHtmlCell. See wxHtmlCellEvent"));
    RegisterEvent("wxEVT_HTML_LINK_CLICKED",
                  "wxHtmlLinkEvent",
                  _("A wxHtmlCell which contains an hyperlink was clicked. See wxHtmlLinkEvent"));
}

void HtmlListBoxWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add("#include <wx/htmllbox.h>"); }

wxString HtmlListBoxWrapper::GetWxClassName() const { return "wxHtmlListBox"; }

void HtmlListBoxWrapper::DefaultImplMethods(wxString& decl, wxString& impl, const wxString& qualifiedName) const
{
    decl << "        wxString OnGetItem(size_t n) const override;\n";

    impl << "wxString " << qualifiedName << "::OnGetItem(size_t n) const\n"
         << "{\n"
         << "    return wxString::Format(\"<b>Item %zu</b>\", n);\n"
         << "}\n\n";
}
