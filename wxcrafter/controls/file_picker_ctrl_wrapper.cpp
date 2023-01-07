#include "file_picker_ctrl_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/filepicker.h>

FilePickerCtrlWrapper::FilePickerCtrlWrapper()
    : wxcWidget(ID_WXFILEPICKER)
{
    SetPropertyString(_("Common Settings"), "wxFilePickerCtrl");
    AddProperty(new StringProperty(PROP_VALUE, wxT(""), _("Default value")));
    AddProperty(new StringProperty(PROP_MESSAGE, _("Select a file"), _("Message to show to the user")));
    AddProperty(new StringProperty(PROP_WILDCARD, wxT("*"), _("Wildcard")));

    PREPEND_STYLE(wxFLP_OPEN, false);
    PREPEND_STYLE(wxFLP_SAVE, false);
    PREPEND_STYLE(wxFLP_OVERWRITE_PROMPT, false);
    PREPEND_STYLE(wxFLP_FILE_MUST_EXIST, false);
    PREPEND_STYLE(wxFLP_CHANGE_DIR, false);
    PREPEND_STYLE(wxFLP_SMALL, true);

    PREPEND_STYLE(wxFLP_USE_TEXTCTRL, false);
    PREPEND_STYLE(wxFLP_DEFAULT_STYLE, true);

    RegisterEvent(wxT("wxEVT_COMMAND_FILEPICKER_CHANGED"), wxT("wxFileDirPickerEvent"),
                  _("The user changed the file selected in the control either using the button or using text "
                    "control\n(see wxFLP_USE_TEXTCTRL; note that in this case the event is fired\nonly if the user's "
                    "input is valid, e.g. an existing file path if wxFLP_FILE_MUST_EXIST was given)."));
    m_namePattern = wxT("m_filePicker");
    SetName(GenerateName());
}

FilePickerCtrlWrapper::~FilePickerCtrlWrapper() {}

wxcWidget* FilePickerCtrlWrapper::Clone() const { return new FilePickerCtrlWrapper(); }

wxString FilePickerCtrlWrapper::CppCtorCode() const
{
    // wxFilePickerCtrl(wxWindow *parent, wxWindowID id, const wxString& path = wxEmptyString, const wxString& message =
    // "Select a file", const wxString& wildcard = "*.*", const wxPoint& pos = wxDefaultPosition, const wxSize& size =
    // wxDefaultSize, long style = wxFLP_DEFAULT_STYLE, const wxValidator& validator = wxDefaultValidator, const
    // wxString& name = "filepickerctrl")
    wxString cpp;
    cpp << GetName() << wxT(" = new ") << GetRealClassName() << wxT("(") << GetWindowParent() << wxT(", ") << WindowID()
        << wxT(", ") << wxT("wxEmptyString, ") << wxCrafter::UNDERSCORE(PropertyString(PROP_MESSAGE)) << wxT(", ")
        << wxCrafter::WXT(PropertyString(PROP_WILDCARD)) << wxT(", ") << wxT("wxDefaultPosition, ") << SizeAsString()
        << wxT(", ") << StyleFlags(wxT("wxFLP_DEFAULT_STYLE")) << wxT(");\n");
    cpp << CPPCommonAttributes();
    return cpp;
}

void FilePickerCtrlWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/filepicker.h>"));
}

wxString FilePickerCtrlWrapper::GetWxClassName() const { return wxT("wxFilePickerCtrl"); }

void FilePickerCtrlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCCommonAttributes() << XRCSize() << wxT("<message>")
         << wxCrafter::XMLEncode(PropertyString(PROP_MESSAGE)) << wxT("</message>") << wxT("<wildcard>")
         << wxCrafter::XMLEncode(PropertyString(PROP_WILDCARD)) << wxT("</wildcard>") << wxT("<value>")
         << wxCrafter::XMLEncode(PropertyString(PROP_VALUE)) << wxT("</value>") << XRCSuffix();
}

void FilePickerCtrlWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("value"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, "message");
    if(propertynode) { SetPropertyString(PROP_MESSAGE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, "wildcard");
    if(propertynode) { SetPropertyString(PROP_WILDCARD, propertynode->GetNodeContent()); }
}

void FilePickerCtrlWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("path"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, "message");
    if(propertynode) { SetPropertyString(PROP_MESSAGE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, "wildcard");
    if(propertynode) { SetPropertyString(PROP_WILDCARD, propertynode->GetNodeContent()); }
}

void FilePickerCtrlWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "value");
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "message");
    if(propertynode) { SetPropertyString(PROP_MESSAGE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "wildcard");
    if(propertynode) { SetPropertyString(PROP_WILDCARD, propertynode->GetNodeContent()); }
}
