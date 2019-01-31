#include "choice_wrapper.h"
#include "allocator_mgr.h"
#include "import_from_wxFB.h"
#include "multi_strings_property.h"
#include "string_property.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"

ChoiceWrapper::ChoiceWrapper()
    : wxcWidget(ID_WXCHOICE)
{
    SetPropertyString(_("Common Settings"), "wxChoice");
    AddProperty(
        new MultiStringsProperty(PROP_OPTIONS, wxT("The Choice drop down options. A semi-colon list of strings")));
    AddProperty(new StringProperty(PROP_SELECTION, wxT(""), wxT("Selected string index")));

    RegisterEventCommand(wxT("wxEVT_COMMAND_CHOICE_SELECTED"),
                         wxT("Process a wxEVT_COMMAND_CHOICE_SELECTED event, when an item on the list is selected."));

    m_namePattern = wxT("m_choice");
    SetName(GenerateName());
}

ChoiceWrapper::~ChoiceWrapper() {}

wxcWidget* ChoiceWrapper::Clone() const { return new ChoiceWrapper(); }

wxString ChoiceWrapper::CppCtorCode() const
{
    wxString code;
    wxArrayString options = wxCrafter::Split(PropertyString(PROP_OPTIONS), wxT(";"));

    code << wxT("wxArrayString ") << GetName() << wxT("Arr;\n");
    for(size_t i = 0; i < options.GetCount(); i++) {
        code << GetName() << wxT("Arr.Add(wxT(\"") << options.Item(i) << wxT("\"));\n");
    }

    code << GetName() << wxT(" = new ") << GetRealClassName() << "(" << GetWindowParent() << wxT(", ") << WindowID()
         << wxT(", ") << wxT("wxDefaultPosition, ") << SizeAsString() << wxT(", ") << GetName() << wxT("Arr")
         << wxT(", ") << StyleFlags(wxT("0")) << wxT(");\n");

    code << CPPCommonAttributes();
    long sel = wxCrafter::ToNumber(PropertyString(PROP_SELECTION), -1);
    if(sel != -1 && sel < (long)options.GetCount()) {
        code << GetName() << wxT("->SetSelection(") << sel << wxT(");\n");
    }
    return code;
}

void ChoiceWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/choice.h>"));
    headers.Add(wxT("#include <wx/arrstr.h>"));
}

wxString ChoiceWrapper::GetWxClassName() const { return wxT("wxChoice"); }

void ChoiceWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCCommonAttributes() << XRCStyle() << XRCSize() << XRCContentItems() << XRCSelection()
         << XRCSuffix();
}

void ChoiceWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    // The Options (content) property has a default, so we must clear it even if the node is absent
    wxString multistring;
    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("content"));
    if(propertynode) { multistring = XmlUtils::ChildNodesContentToString(propertynode); }
    SetPropertyString(PROP_OPTIONS, multistring);

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("selection"));
    if(propertynode) { SetPropertyString(PROP_SELECTION, propertynode->GetNodeContent()); }
}

void ChoiceWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    // The Options (content) property has a default, so we must clear it even if the node is absent
    wxString multistring;
    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("content"));
    if(propertynode) { multistring = XmlUtils::ChildNodesContentToString(propertynode); }
    SetPropertyString(PROP_OPTIONS, multistring);

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("selection"));
    if(propertynode) { SetPropertyString(PROP_SELECTION, propertynode->GetNodeContent()); }
}

void ChoiceWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    // The "choices" property has a default, so we should clear it even if the node was absent
    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "choices");
    wxString content;
    if(propertynode) { content = ImportFromwxFB::ConvertFBOptionsString(propertynode->GetNodeContent()); }
    SetPropertyString(PROP_OPTIONS, content);

    propertynode = XmlUtils::FindNodeByName(node, "property", "selection");
    if(propertynode) { SetPropertyString(PROP_SELECTION, propertynode->GetNodeContent()); }
}
