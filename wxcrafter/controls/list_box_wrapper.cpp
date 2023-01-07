#include "list_box_wrapper.h"
#include "allocator_mgr.h"
#include "import_from_wxFB.h"
#include "multi_strings_property.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"

ListBoxWrapper::ListBoxWrapper()
    : wxcWidget(ID_WXLISTBOX)
{
    PREPEND_STYLE(wxLB_SINGLE, true);
    PREPEND_STYLE(wxLB_MULTIPLE, false);
    PREPEND_STYLE(wxLB_EXTENDED, false);
    PREPEND_STYLE(wxLB_HSCROLL, false);
    PREPEND_STYLE(wxLB_ALWAYS_SB, false);
    PREPEND_STYLE(wxLB_NEEDED_SB, false);
    PREPEND_STYLE(wxLB_SORT, false);

    RegisterEventCommand(wxT("wxEVT_COMMAND_LISTBOX_SELECTED"),
                         _("Process a wxEVT_COMMAND_LISTBOX_SELECTED event, when an item on the list is selected or "
                           "the selection changes."));
    RegisterEventCommand(
        wxT("wxEVT_COMMAND_LISTBOX_DOUBLECLICKED"),
        _("Process a wxEVT_COMMAND_LISTBOX_DOUBLECLICKED event, when the listbox is double-clicked."));

    SetPropertyString(_("Common Settings"), "wxListBox");
    AddProperty(new MultiStringsProperty(PROP_OPTIONS, _("The List Box Items. A semi-colon list of strings")));
    AddProperty(new StringProperty(
        PROP_SELECTION, wxT("-1"),
        _("The zero-based position of any initially selected string, or -1 if none are to be selected")));

    m_namePattern = wxT("m_listBox");
    SetName(GenerateName());
}

ListBoxWrapper::~ListBoxWrapper() {}

wxcWidget* ListBoxWrapper::Clone() const { return new ListBoxWrapper(); }

wxString ListBoxWrapper::CppCtorCode() const
{
    wxString code;
    wxArrayString options = wxCrafter::Split(PropertyString(PROP_OPTIONS), wxT(";"));

    wxString optionsArr;
    optionsArr << GetName() << wxT("Arr");

    code << wxT("wxArrayString ") << optionsArr << wxT(";\n");
    for(size_t i = 0; i < options.GetCount(); i++) {
        code << optionsArr << wxT(".Add(") << wxCrafter::UNDERSCORE(options.Item(i)) << wxT(");\n");
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

void ListBoxWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/listbox.h>")); }

wxString ListBoxWrapper::GetWxClassName() const { return wxT("wxListBox"); }

void ListBoxWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxArrayString options = wxCrafter::Split(PropertyString(PROP_OPTIONS), wxT(";"));
    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes() << XRCContentItems() << XRCSelection()
         << XRCSuffix();
}

void ListBoxWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("content"));
    if(propertynode) { SetPropertyString(PROP_OPTIONS, XmlUtils::ChildNodesContentToString(propertynode)); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("selection"));
    if(propertynode) { SetPropertyString(PROP_SELECTION, propertynode->GetNodeContent()); }
}

void ListBoxWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("default"));
    if(propertynode) { SetPropertyString(PROP_OPTIONS, XmlUtils::ChildNodesContentToString(propertynode)); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("selection"));
    if(propertynode) { SetPropertyString(PROP_SELECTION, propertynode->GetNodeContent()); }
}

void ListBoxWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "choices");
    if(propertynode) {
        SetPropertyString(PROP_OPTIONS, ImportFromwxFB::ConvertFBOptionsString(propertynode->GetNodeContent()));
    }

    // NB wxFb doesn't do 'selection' for listboxes
}
