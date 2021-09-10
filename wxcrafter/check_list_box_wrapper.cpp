#include "check_list_box_wrapper.h"
#include "allocator_mgr.h"
#include "import_from_wxFB.h"
#include "multi_strings_property.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"

CheckListBoxWrapper::CheckListBoxWrapper()
    : wxcWidget(ID_WXCHECKLISTBOX)
{
    PREPEND_STYLE(wxLB_SINGLE, true);
    PREPEND_STYLE(wxLB_MULTIPLE, false);
    PREPEND_STYLE(wxLB_EXTENDED, false);
    PREPEND_STYLE(wxLB_HSCROLL, false);
    PREPEND_STYLE(wxLB_ALWAYS_SB, false);
    PREPEND_STYLE(wxLB_NEEDED_SB, false);
    PREPEND_STYLE(wxLB_SORT, false);

    RegisterEventCommand(wxT("wxEVT_COMMAND_LISTBOX_SELECTED"),
                         _("Process a wxEVT_COMMAND_LISTBOX_SELECTED event\nwhen an item on the list is selected or "
                           "the selection changes."));
    RegisterEventCommand(
        wxT("wxEVT_COMMAND_LISTBOX_DOUBLECLICKED"),
        _("Process a wxEVT_COMMAND_LISTBOX_DOUBLECLICKED event\nwhen the listbox is double-clicked."));
    RegisterEventCommand(wxT("wxEVT_COMMAND_CHECKLISTBOX_TOGGLED"),
                         _("Process a wxEVT_COMMAND_CHECKLISTBOX_TOGGLED event\nwhen an item in the check list box "
                           "is checked or unchecked."));

    SetPropertyString(_("Common Settings"), "wxCheckListBox");
    AddProperty(new MultiStringsProperty(PROP_OPTIONS, _("The List Box Items. A semi-colon list of strings")));

    m_namePattern = wxT("m_checkListBox");
    SetName(GenerateName());
}

CheckListBoxWrapper::~CheckListBoxWrapper() {}

wxcWidget* CheckListBoxWrapper::Clone() const { return new CheckListBoxWrapper(); }

wxString CheckListBoxWrapper::CppCtorCode() const
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
    return code;
}

void CheckListBoxWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/checklst.h>")); }

wxString CheckListBoxWrapper::GetWxClassName() const { return wxT("wxCheckListBox"); }

void CheckListBoxWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxUnusedVar(type);
    text << XRCPrefix() << XRCCommonAttributes() << XRCSize() << XRCStyle();

    wxArrayString options = wxCrafter::Split(PropertyString(PROP_OPTIONS), wxT(";"));
    text << wxT("<content>");

    for(size_t i = 0; i < options.GetCount(); i++) {
        text << wxT("<item checked=\"0\">") << wxCrafter::XMLEncode(options.Item(i)) << wxT("</item>");
    }
    text << wxT("</content>");

    text << XRCSuffix();
}

void CheckListBoxWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("content"));
    if(propertynode) { SetPropertyString(PROP_OPTIONS, XmlUtils::ChildNodesContentToString(propertynode)); }
    // XRC also allows a 'checked' property for each item, but as neither XRCed, wxFB or (atm) wxC support this...
}

void CheckListBoxWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("content"));
    if(propertynode) {
        wxString t = XmlUtils::ChildNodesContentToString(propertynode);
        SetPropertyString(PROP_OPTIONS, XmlUtils::ChildNodesContentToString(propertynode));
    }
    // XRC also allows a 'checked' property for each item. wxSmith implements this, by:
    // <item checked="1">Content string</item>
    // but (atm) wxC doesn't support this
}

void CheckListBoxWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "choices");
    if(propertynode) {
        SetPropertyString(PROP_OPTIONS, ImportFromwxFB::ConvertFBOptionsString(propertynode->GetNodeContent()));
    }
}
