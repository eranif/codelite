#include "SimpleHtmlListBoxWrapper.h"
#include "allocator_mgr.h"
#include "wxgui_helpers.h"
#include <wx/htmllbox.h>

SimpleHtmlListBoxWrapper::SimpleHtmlListBoxWrapper()
    : wxcWidget(ID_WXSIMPLEHTMLLISTBOX)
{
    // Styles
    PREPEND_STYLE(wxHLB_DEFAULT_STYLE, true);
    PREPEND_STYLE(wxHLB_MULTIPLE, false);

    // Events
    RegisterEventCommand("wxEVT_LISTBOX", _("Process a wxEVT_LISTBOX event, when an item on the list is selected"));
    RegisterEventCommand("wxEVT_LISTBOX_DCLICK",
                         _("Process a wxEVT_LISTBOX_DCLICK event, when the listbox is double-clicked"));
    RegisterEvent("wxEVT_HTML_CELL_CLICKED", "wxHtmlCellEvent", _("A wxHtmlCell was clicked. See wxHtmlCellEvent"));
    RegisterEvent("wxEVT_HTML_CELL_HOVER", "wxHtmlCellEvent",
                  _("The mouse passed over a wxHtmlCell. See wxHtmlCellEvent"));
    RegisterEvent("wxEVT_HTML_LINK_CLICKED", "wxHtmlLinkEvent",
                  _("A wxHtmlCell which contains an hyperlink was clicked. See wxHtmlLinkEvent"));
    SetPropertyString(_("Common Settings"), "wxSimpleHtmlListBox");
    AddProperty(new MultiStringsProperty(
        PROP_OPTIONS, wxT("The List Box Items. A semi-colon list of strings. This list may contain HTML fragements")));
    AddProperty(new StringProperty(
        PROP_SELECTION, wxT("-1"),
        wxT("The zero-based position of any initially selected string, or -1 if none are to be selected")));

    // Basic name pattern
    m_namePattern = "m_htmlListBox";
    SetName(GenerateName());
}

SimpleHtmlListBoxWrapper::~SimpleHtmlListBoxWrapper() {}

void SimpleHtmlListBoxWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add("#include <wx/htmllbox.h>"); }

wxString SimpleHtmlListBoxWrapper::CppCtorCode() const
{
    wxString code;
    wxArrayString options = wxCrafter::Split(PropertyString(PROP_OPTIONS), wxT(";"));

    wxString optionsArr;
    optionsArr << GetName() << wxT("Arr");

    code << wxT("wxArrayString ") << optionsArr << wxT(";\n");
    for(size_t i = 0; i < options.GetCount(); i++) {

        code << optionsArr << wxT(".Add(_(\"") << wxCrafter::ESCAPE(options.Item(i)) << wxT("\"));\n");
    }

    code << GetName() << wxT(" = new ") << GetRealClassName() << "(" << GetWindowParent() << wxT(", ") << WindowID()
         << wxT(", ") << wxT("wxDefaultPosition, ") << SizeAsString() << wxT(", ") << optionsArr << wxT(", ")
         << StyleFlags(wxT("0")) << wxT(");\n");
    code << CPPCommonAttributes();

    int sel = wxCrafter::ToNumber(PropertyString(PROP_SELECTION), -1);
    if(sel >= 0 && sel < (int)options.GetCount()) {
        code << GetName() << wxT("->SetSelection(") << PropertyString(PROP_SELECTION) << wxT(");\n");
    }
    return code;
}

wxString SimpleHtmlListBoxWrapper::GetWxClassName() const { return "wxSimpleHtmlListBox"; }

void SimpleHtmlListBoxWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxArrayString options = wxCrafter::Split(PropertyString(PROP_OPTIONS), wxT(";"));
    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes() << XRCContentItems() << XRCSelection()
         << XRCSuffix();
}
