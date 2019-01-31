#include "radio_box_wrapper.h"
#include "allocator_mgr.h"
#include "import_from_wxFB.h"
#include "multi_strings_property.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"

RadioBoxWrapper::RadioBoxWrapper()
    : wxcWidget(ID_WXRADIOBOX)
{
    PREPEND_STYLE(wxRA_SPECIFY_ROWS, false);
    PREPEND_STYLE(wxRA_SPECIFY_COLS, false);

    SetPropertyString(_("Common Settings"), "wxRadioBox");
    AddProperty(new StringProperty(PROP_LABEL, wxT("My RadioBox"), wxT("Label")));
    AddProperty(new MultiStringsProperty(PROP_OPTIONS, _("An array of choices with which to initialize the radiobox")));
    AddProperty(new StringProperty(PROP_SELECTION, wxT("0"), wxT("The zero-based position of the selected button")));
    AddProperty(new StringProperty(PROP_MAJORDIM, wxT("1"),
                                   _("Specifies the maximum number of rows (if style contains wxRA_SPECIFY_ROWS) or "
                                     "columns (if style contains wxRA_SPECIFY_COLS) for a two-dimensional radiobox")));

    RegisterEventCommand(wxT("wxEVT_COMMAND_RADIOBOX_SELECTED"),
                         wxT("Process a wxEVT_COMMAND_RADIOBOX_SELECTED event, when a radiobutton is clicked."));
    DoSetPropertyStringValue(PROP_OPTIONS, wxT("An option;Second Option"));

    m_namePattern = wxT("m_radioBox");
    SetName(GenerateName());
}

RadioBoxWrapper::~RadioBoxWrapper() {}

wxcWidget* RadioBoxWrapper::Clone() const { return new RadioBoxWrapper(); }

wxString RadioBoxWrapper::CppCtorCode() const
{
    wxString code, arrname;
    wxArrayString options = wxCrafter::Split(PropertyString(PROP_OPTIONS), wxT(";"));

    arrname << GetName() << wxT("Arr");

    code << wxT("wxArrayString ") << arrname << wxT(";\n");

    for(size_t i = 0; i < options.GetCount(); i++) {
        code << arrname << wxT(".Add(_(\"") << options.Item(i) << wxT("\"));\n");
    }

    // wxRadioBox(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& point, const wxSize& size,
    // const wxArrayString& choices, int majorDimension = 0, long style = wxRA_SPECIFY_COLS, const wxValidator&
    // validator = wxDefaultValidator, const wxString& name = "radioBox")
    code << GetName() << wxT(" = new ") << GetRealClassName() << "(" << GetWindowParent() << wxT(", ") << WindowID()
         << wxT(", ") << Label() << wxT(", ") << wxT("wxDefaultPosition, ") << SizeAsString() << wxT(", ") << arrname
         << wxT(", ") << PropertyString(PROP_MAJORDIM) << wxT(", ") << StyleFlags(wxT("0")) << wxT(");\n");
    code << CPPCommonAttributes();
    int sel = wxCrafter::ToNumber(PropertyString(PROP_SELECTION), -1);
    if(sel >= 0 && sel < (int)options.GetCount()) {
        code << GetName() << wxT("->SetSelection(") << PropertyString(PROP_SELECTION) << wxT(");\n");
    }
    return code;
}

void RadioBoxWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/radiobox.h>")); }

wxString RadioBoxWrapper::GetWxClassName() const { return wxT("wxRadioBox"); }

void RadioBoxWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCLabel() << XRCSize() << XRCStyle() << XRCCommonAttributes() << wxT("<dimension>")
         << PropertyString(PROP_MAJORDIM) << wxT("</dimension>") << XRCSelection() << XRCContentItems(true)
         << XRCSuffix();
}

void RadioBoxWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
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

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("dimension"));
    if(propertynode) { SetPropertyString(PROP_MAJORDIM, propertynode->GetNodeContent()); }
}

void RadioBoxWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    // The Options (content) property has a default, so we must clear it even if the node is absent
    wxString multistring;
    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("content"));
    if(propertynode) { multistring = XmlUtils::ChildNodesContentToString(propertynode); }
    SetPropertyString(PROP_OPTIONS, multistring);

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("default"));
    if(propertynode) { SetPropertyString(PROP_SELECTION, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("dimension"));
    if(propertynode) { SetPropertyString(PROP_MAJORDIM, propertynode->GetNodeContent()); }
}

void RadioBoxWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
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

    propertynode = XmlUtils::FindNodeByName(node, "property", "majorDimension");
    if(propertynode) { SetPropertyString(PROP_MAJORDIM, propertynode->GetNodeContent()); }
}
