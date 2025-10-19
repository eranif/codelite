#include "combobox_wrapper.h"

#include "Importer/import_from_wxFB.h"
#include "Properties/multi_strings_property.h"
#include "Properties/string_property.h"
#include "allocator_mgr.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"

ComboBoxWrapper::ComboBoxWrapper()
    : wxcWidget(ID_WXCOMBOBOX)
{
    SetPropertyString(_("Common Settings"), "wxComboBox");
    Add<MultiStringsProperty>(PROP_CB_CHOICES, _("ComboBox drop down choices"));
    Add<StringProperty>(PROP_HINT, "", _("Sets a hint shown in an empty unfocused text control"));
    Add<StringProperty>(
        PROP_SELECTION,
        wxT("-1"),
        _("The zero-based position of any initially selected string, or -1 if none are to be selected"));
    Add<StringProperty>(PROP_VALUE, "", _("The comboBox initial value"));

    RegisterEventCommand(wxT("wxEVT_COMMAND_COMBOBOX_SELECTED"),
                         _("Process a wxEVT_COMMAND_COMBOBOX_SELECTED event, when an item on the list is selected. "
                           "Note that calling GetValue returns the new value of selection."));
    RegisterEventCommand(wxT("wxEVT_COMMAND_TEXT_UPDATED"),
                         _("Process a wxEVT_COMMAND_TEXT_UPDATED event, when the comboBox text changes."));
    RegisterEventCommand(wxT("wxEVT_COMMAND_TEXT_ENTER"),
                         _("Process a wxEVT_COMMAND_TEXT_ENTER event, when <RETURN> is pressed in the comboBox."));

    PREPEND_STYLE(wxCB_DROPDOWN, false);
    PREPEND_STYLE(wxCB_READONLY, false);
    PREPEND_STYLE(wxCB_SIMPLE, false);
    PREPEND_STYLE(wxCB_SORT, false);
    PREPEND_STYLE(wxTE_PROCESS_ENTER, false);

    m_namePattern = wxT("m_comboBox");
    SetName(GenerateName());
}

wxcWidget* ComboBoxWrapper::Clone() const { return new ComboBoxWrapper(); }

void ComboBoxWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/combobox.h>"));
    headers.Add(wxT("#include <wx/arrstr.h>"));
}

wxString ComboBoxWrapper::GetWxClassName() const { return wxT("wxComboBox"); }

wxString ComboBoxWrapper::CppCtorCode() const
{
    wxString code;
    wxArrayString options = wxCrafter::SplitByString(PropertyString(PROP_CB_CHOICES), wxT("\\n"));

    code << wxT("wxArrayString ") << GetName() << wxT("Arr;\n");
    for (size_t i = 0; i < options.GetCount(); i++) {
        code << GetName() << wxT("Arr.Add(") << wxCrafter::UNDERSCORE(options.Item(i)) << wxT(");\n");
    }

    code << GetName() << wxT(" = new ") << GetRealClassName() << "(" << GetWindowParent() << wxT(", ") << WindowID()
         << wxT(", ") << ValueAsString() << wxT(", ") << wxT("wxDefaultPosition, ") << SizeAsString() << wxT(", ")
         << GetName() << wxT("Arr") << wxT(", ") << StyleFlags(wxT("0")) << wxT(");\n");
    code << CPPCommonAttributes();

    if (!HasStyle(wxCB_READONLY)) {
        code << wxCrafter::WX30_BLOCK_START();
        code << GetName() << "->SetHint(" << wxCrafter::UNDERSCORE(PropertyString(PROP_HINT)) << ");\n";
        code << wxCrafter::WXVER_CHECK_BLOCK_END();
    }

    int sel = wxCrafter::ToNumber(PropertyString(PROP_SELECTION), -1);
    if (sel >= 0 && sel < (int)options.GetCount()) {
        code << GetName() << wxT("->SetSelection(") << PropertyString(PROP_SELECTION) << wxT(");\n");
    }
    return code;
}

void ComboBoxWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxArrayString options = wxCrafter::SplitByString(PropertyString(PROP_CB_CHOICES), wxT("\\n"));
    wxString str;
    str << wxT("<content>");
    for (size_t i = 0; i < options.GetCount(); i++) {
        str << wxT("<item>") << wxCrafter::CDATA(options.Item(i)) << wxT("</item>");
    }
    str << wxT("</content>");

    text << XRCPrefix() << XRCValue() << XRCStyle() << XRCSize() << XRCCommonAttributes();
    if (!HasStyle(wxCB_READONLY) && !PropertyString(PROP_HINT).empty()) {
        // set hint for non-read-only comboBox
        text << "<hint>" << wxCrafter::CDATA(PropertyString(PROP_HINT)) << "</hint>";
    }
    text << XRCSelection() << str << XRCSuffix();
}

void ComboBoxWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    // The Options (content) property has a default, so we must clear it even if the node is absent
    wxString multistring;
    wxXmlNode* propertyNode = XmlUtils::FindFirstByTagName(node, wxT("content"));
    if (propertyNode) {
        multistring = XmlUtils::ChildNodesContentToString(
            propertyNode, "", "\\n"); // wxC stores comboBox items as a \n-separated string, not ';' like wxChoice
    }
    SetPropertyString(PROP_CB_CHOICES, multistring);

    propertyNode = XmlUtils::FindFirstByTagName(node, wxT("selection"));
    if (propertyNode) {
        SetPropertyString(PROP_SELECTION, propertyNode->GetNodeContent());
    }

    propertyNode = XmlUtils::FindFirstByTagName(node, wxT("value"));
    if (propertyNode) {
        SetPropertyString(PROP_VALUE, propertyNode->GetNodeContent());
    }
}

void ComboBoxWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    // The Options (content) property has a default, so we must clear it even if the node is absent
    wxString multistring;
    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("content"));
    if (propertynode) {
        multistring = XmlUtils::ChildNodesContentToString(propertynode, "", "\\n");
    }
    SetPropertyString(PROP_CB_CHOICES, multistring);

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("selection"));
    if (propertynode) {
        SetPropertyString(PROP_SELECTION, propertynode->GetNodeContent());
    }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("value")); // wxSmith doesn't seem to do this, but...
    if (propertynode) {
        SetPropertyString(PROP_VALUE, propertynode->GetNodeContent());
    }
}

void ComboBoxWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    // The "choices" property has a default, so we should clear it even if the node was absent
    wxXmlNode* propertyNode = XmlUtils::FindNodeByName(node, "property", "choices");
    wxString content;
    if (propertyNode) {
        content = ImportFromwxFB::ConvertFBOptionsString(propertyNode->GetNodeContent(), "\\n");
    }
    SetPropertyString(PROP_CB_CHOICES, content);

    propertyNode = XmlUtils::FindNodeByName(node, "property", "selection");
    if (propertyNode) {
        SetPropertyString(PROP_SELECTION, propertyNode->GetNodeContent());
    }

    propertyNode = XmlUtils::FindNodeByName(node, "property", "value");
    if (propertyNode) {
        SetPropertyString(PROP_VALUE, propertyNode->GetNodeContent());
    }
}
