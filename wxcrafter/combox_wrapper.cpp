#include "combox_wrapper.h"
#include "allocator_mgr.h"
#include "import_from_wxFB.h"
#include "multi_strings_property.h"
#include "string_property.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/textctrl.h>

ComboxWrapper::ComboxWrapper()
    : wxcWidget(ID_WXCOMBOBOX)
{
    SetPropertyString(_("Common Settings"), "wxComboBox");
    AddProperty(new MultiStringsProperty(PROP_CB_CHOICES, wxT("Combobox drop down choices")));
    AddProperty(new StringProperty(PROP_HINT, "", _("Sets a hint shown in an empty unfocused text control")));
    AddProperty(new StringProperty(
        PROP_SELECTION, wxT("-1"),
        wxT("The zero-based position of any initially selected string, or -1 if none are to be selected")));
    AddProperty(new StringProperty(PROP_VALUE, "", wxT("The combobox initial value")));

    RegisterEventCommand(wxT("wxEVT_COMMAND_COMBOBOX_SELECTED"),
                         wxT("Process a wxEVT_COMMAND_COMBOBOX_SELECTED event, when an item on the list is selected. "
                             "Note that calling GetValue returns the new value of selection."));
    RegisterEventCommand(wxT("wxEVT_COMMAND_TEXT_UPDATED"),
                         wxT("Process a wxEVT_COMMAND_TEXT_UPDATED event, when the combobox text changes."));
    RegisterEventCommand(wxT("wxEVT_COMMAND_TEXT_ENTER"),
                         wxT("Process a wxEVT_COMMAND_TEXT_ENTER event, when <RETURN> is pressed in the combobox."));

    PREPEND_STYLE(wxCB_DROPDOWN, false);
    PREPEND_STYLE(wxCB_READONLY, false);
    PREPEND_STYLE(wxCB_SIMPLE, false);
    PREPEND_STYLE(wxCB_SORT, false);
    PREPEND_STYLE(wxTE_PROCESS_ENTER, false);

    m_namePattern = wxT("m_comboBox");
    SetName(GenerateName());
}

ComboxWrapper::~ComboxWrapper() {}

wxcWidget* ComboxWrapper::Clone() const { return new ComboxWrapper(); }

void ComboxWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/combobox.h>"));
    headers.Add(wxT("#include <wx/arrstr.h>"));
}

wxString ComboxWrapper::GetWxClassName() const { return wxT("wxComboBox"); }

wxString ComboxWrapper::CppCtorCode() const
{
    wxString code;
    wxArrayString options = wxCrafter::SplitByString(PropertyString(PROP_CB_CHOICES), wxT("\\n"));

    code << wxT("wxArrayString ") << GetName() << wxT("Arr;\n");
    for(size_t i = 0; i < options.GetCount(); i++) {
        code << GetName() << wxT("Arr.Add(wxT(\"") << options.Item(i) << wxT("\"));\n");
    }

    code << GetName() << wxT(" = new ") << GetRealClassName() << "(" << GetWindowParent() << wxT(", ") << WindowID()
         << wxT(", ") << ValueAsString() << wxT(", ") << wxT("wxDefaultPosition, ") << SizeAsString() << wxT(", ")
         << GetName() << wxT("Arr") << wxT(", ") << StyleFlags(wxT("0")) << wxT(");\n");
    code << CPPCommonAttributes();

    if(!HasStyle(wxCB_READONLY)) {
        code << wxCrafter::WX30_BLOCK_START();
        code << GetName() << "->SetHint(" << wxCrafter::UNDERSCORE(PropertyString(PROP_HINT)) << ");\n";
        code << wxCrafter::WXVER_CHECK_BLOCK_END();
    }

    int sel = wxCrafter::ToNumber(PropertyString(PROP_SELECTION), -1);
    if(sel >= 0 && sel < (int)options.GetCount()) {
        code << GetName() << wxT("->SetSelection(") << PropertyString(PROP_SELECTION) << wxT(");\n");
    }
    return code;
}

void ComboxWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxArrayString options = wxCrafter::SplitByString(PropertyString(PROP_CB_CHOICES), wxT("\\n"));
    wxString str;
    str << wxT("<content>");
    for(size_t i = 0; i < options.GetCount(); i++) {
        str << wxT("<item>") << wxCrafter::CDATA(options.Item(i)) << wxT("</item>");
    }
    str << wxT("</content>");

    text << XRCPrefix() << XRCValue() << XRCStyle() << XRCSize() << XRCCommonAttributes();
    if(!HasStyle(wxCB_READONLY) && !PropertyString(PROP_HINT).empty()) {
        // set hint for non-readonly combobox
        text << "<hint>" << wxCrafter::CDATA(PropertyString(PROP_HINT)) << "</hint>";
    }
    text << XRCSelection() << str << XRCSuffix();
}

void ComboxWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    // The Options (content) property has a default, so we must clear it even if the node is absent
    wxString multistring;
    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("content"));
    if(propertynode) {
        multistring = XmlUtils::ChildNodesContentToString(
            propertynode, "", "\\n"); // wxC stores combobox items as a \n-separated string, not ';' like wxChoice
    }
    SetPropertyString(PROP_CB_CHOICES, multistring);

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("selection"));
    if(propertynode) { SetPropertyString(PROP_SELECTION, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("value"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}

void ComboxWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    // The Options (content) property has a default, so we must clear it even if the node is absent
    wxString multistring;
    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("content"));
    if(propertynode) { multistring = XmlUtils::ChildNodesContentToString(propertynode, "", "\\n"); }
    SetPropertyString(PROP_CB_CHOICES, multistring);

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("selection"));
    if(propertynode) { SetPropertyString(PROP_SELECTION, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("value")); // wxSmith doesn't seem to do this, but...
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}

void ComboxWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    // The "choices" property has a default, so we should clear it even if the node was absent
    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "choices");
    wxString content;
    if(propertynode) { content = ImportFromwxFB::ConvertFBOptionsString(propertynode->GetNodeContent(), "\\n"); }
    SetPropertyString(PROP_CB_CHOICES, content);

    propertynode = XmlUtils::FindNodeByName(node, "property", "selection");
    if(propertynode) { SetPropertyString(PROP_SELECTION, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "value");
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }
}
