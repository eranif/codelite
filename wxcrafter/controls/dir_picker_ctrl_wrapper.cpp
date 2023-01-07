#include "dir_picker_ctrl_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/filepicker.h>

DirPickerCtrlWrapper::DirPickerCtrlWrapper()
    : wxcWidget(ID_WXDIRPICKER)
{
    SetPropertyString(_("Common Settings"), "wxDirPickerCtrl");
    AddProperty(new StringProperty(PROP_VALUE, wxT(""), _("Value")));
    AddProperty(new StringProperty(PROP_MESSAGE, _("Select a folder"), _("Message to show to the user")));

    PREPEND_STYLE(wxDIRP_CHANGE_DIR, false);
    PREPEND_STYLE(wxDIRP_DIR_MUST_EXIST, false);
    PREPEND_STYLE(wxDIRP_USE_TEXTCTRL, false);
    PREPEND_STYLE(wxDIRP_DEFAULT_STYLE, true);
    PREPEND_STYLE(wxDIRP_SMALL, true);

    RegisterEvent(wxT("wxEVT_COMMAND_DIRPICKER_CHANGED"), wxT("wxFileDirPickerEvent"),
                  _("The user changed the directory selected in the control either using the button or using text "
                    "control\n(see wxDIRP_USE_TEXTCTRL; note that in this case the event is fired only if the user's "
                    "input is valid, e.g. an existing directory path)."));
    m_namePattern = wxT("m_dirPicker");
    SetName(GenerateName());
}

DirPickerCtrlWrapper::~DirPickerCtrlWrapper() {}

wxcWidget* DirPickerCtrlWrapper::Clone() const { return new DirPickerCtrlWrapper(); }

wxString DirPickerCtrlWrapper::CppCtorCode() const
{
    // wxDirPickerCtrl(wxWindow *parent, wxWindowID id, const wxString& path = wxEmptyString, const wxString& message =
    // "Select a folder", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style =
    // wxDIRP_DEFAULT_STYLE, const wxValidator& validator = wxDefaultValidator, const wxString& name = "dirpickerctrl")
    wxString cpp;
    cpp << GetName() << wxT(" = new ") << GetRealClassName() << wxT("(") << GetWindowParent() << wxT(", ") << WindowID()
        << wxT(", ") << wxT("wxEmptyString, ") << wxCrafter::UNDERSCORE(PropertyString(PROP_MESSAGE)) << wxT(", ")
        << wxT("wxDefaultPosition, ") << SizeAsString() << wxT(", ") << StyleFlags(wxT("wxDIRP_DEFAULT_STYLE"))
        << wxT(");\n");
    cpp << CPPCommonAttributes();
    return cpp;
}

void DirPickerCtrlWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/filepicker.h>"));
}

wxString DirPickerCtrlWrapper::GetWxClassName() const { return wxT("wxDirPickerCtrl"); }

void DirPickerCtrlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCCommonAttributes() << XRCSize() << wxT("<message>")
         << wxCrafter::XMLEncode(PropertyString(PROP_MESSAGE)) << wxT("</message>") << wxT("<value>")
         << wxCrafter::XMLEncode(PropertyString(PROP_VALUE)) << wxT("</value>") << XRCSuffix();
}

void DirPickerCtrlWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("value"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, "message");
    if(propertynode) { SetPropertyString(PROP_MESSAGE, propertynode->GetNodeContent()); }
}

void DirPickerCtrlWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("path"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, "message");
    if(propertynode) { SetPropertyString(PROP_MESSAGE, propertynode->GetNodeContent()); }
}

void DirPickerCtrlWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "value");
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "message");
    if(propertynode) { SetPropertyString(PROP_MESSAGE, propertynode->GetNodeContent()); }
}
