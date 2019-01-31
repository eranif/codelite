#include "text_ctrl_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/textctrl.h>

TextCtrlWrapper::TextCtrlWrapper()
    : wxcWidget(ID_WXTEXTCTRL)
{
    PREPEND_STYLE(wxTE_AUTO_URL, false);
    PREPEND_STYLE(wxTE_CENTRE, false);
    PREPEND_STYLE(wxTE_CHARWRAP, false);
    PREPEND_STYLE(wxTE_DONTWRAP, false);
    PREPEND_STYLE(wxTE_LEFT, false);
    PREPEND_STYLE(wxTE_MULTILINE, false);
    PREPEND_STYLE(wxTE_NOHIDESEL, false);
    PREPEND_STYLE(wxTE_NO_VSCROLL, false);
    PREPEND_STYLE(wxTE_PASSWORD, false);
    PREPEND_STYLE(wxTE_PROCESS_ENTER, false);
    PREPEND_STYLE(wxTE_PROCESS_TAB, false);
    PREPEND_STYLE(wxTE_READONLY, false);
    PREPEND_STYLE(wxTE_RICH, false);
    PREPEND_STYLE(wxTE_RICH2, false);
    PREPEND_STYLE(wxTE_RIGHT, false);
    PREPEND_STYLE(wxTE_WORDWRAP, false);

    RegisterEventCommand(wxT("wxEVT_COMMAND_TEXT_UPDATED"),
                         wxT("Respond to a wxEVT_COMMAND_TEXT_UPDATED event, generated when the text changes.\nNotice "
                             "that this event will be sent when the text controls contents changes\n - whether this is "
                             "due to user input or comes from the program itself\n(for example, if SetValue() is "
                             "called); see ChangeValue() for a function which does not send this event."));
    RegisterEventCommand(wxT("wxEVT_COMMAND_TEXT_ENTER"),
                         wxT("Respond to a wxEVT_COMMAND_TEXT_ENTER event, generated when enter is pressed in a text "
                             "control\n(which must have wxTE_PROCESS_ENTER style for this event to be generated)."));
    RegisterEventCommand(wxT("wxEVT_COMMAND_TEXT_MAXLEN"),
                         wxT("User tried to enter more text into the control than the limit set by SetMaxLength."));
    RegisterEventCommand(wxT("wxEVT_COMMAND_TEXT_URL"),
                         wxT("A mouse event occurred over an URL in the text control (wxMSW and wxGTK2 only)"));

    SetPropertyString(_("Common Settings"), "wxTextCtrl");
    AddProperty(new StringProperty(PROP_VALUE, wxT(""), wxT("Default text control value")));
    AddProperty(new StringProperty(PROP_HINT, "", _("Sets a hint shown in an empty unfocused text control")));
    AddProperty(new StringProperty(PROP_MAXLENGTH, wxT("0"),
                                   wxT("The maximum length of user entered text. Only for single-line wxTextCtrls.")));
    AddProperty(new BoolProperty(
        PROP_AUTO_COMPLETE_DIRS, false,
        _("Enable auto-completion of the text using the file system directories. Notice that currently this function "
          "is only implemented in wxMSW port and does nothing under the other platforms.")));
    AddProperty(new BoolProperty(PROP_AUTO_COMPLETE_FILES, false,
                                 _("Enable auto-completion of the text typed in a single-line text control using all "
                                   "valid file system path. Notice that currently this function is only implemented in "
                                   "wxMSW port and does nothing under the other platforms.")));
    m_namePattern = wxT("m_textCtrl");
    SetName(GenerateName());
}

TextCtrlWrapper::~TextCtrlWrapper() {}

void TextCtrlWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/textctrl.h>")); }

wxString TextCtrlWrapper::GetWxClassName() const { return wxT("wxTextCtrl"); }

wxString TextCtrlWrapper::CppCtorCode() const
{
    wxString code;
    code << CPPStandardWxCtorWithValue(wxT("0"));

    if(!HasStyle(wxTE_MULTILINE)) {

        // Only supported for wx30 and later
        code << wxCrafter::WX30_BLOCK_START();
        code << GetName() << "->SetHint(" << wxCrafter::UNDERSCORE(PropertyString(PROP_HINT)) << ");\n";
        code << wxCrafter::WXVER_CHECK_BLOCK_END();
    }

    if(IsPropertyChecked(PROP_AUTO_COMPLETE_DIRS)) {
        code << GetName() << "->AutoCompleteDirectories();\n";

    } else if(IsPropertyChecked(PROP_AUTO_COMPLETE_FILES)) {
        code << GetName() << "->AutoCompleteFileNames();\n";
    }
    return code;
}

void TextCtrlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    // Filter out maxlen if the textctrl is multiline (otherwise wx2.9 asserts); similarly hint
    wxString maxlen, hint;

    if(!HasStyle(wxTE_MULTILINE)) {
        if(wxCrafter::ToNumber(PropertyString(PROP_MAXLENGTH), -1) > 0) {
            maxlen = "<maxlength>" + PropertyString(PROP_MAXLENGTH) << "</maxlength>";
        }
        if(!PropertyString(PROP_HINT).empty()) {
            hint = "<hint>" + wxCrafter::CDATA(PropertyString(PROP_HINT)) << "</hint>";
        }
    }

    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes() << XRCValue() << maxlen << hint
         << XRCSuffix();
}

void TextCtrlWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("value"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("maxlength"));
    if(propertynode) { SetPropertyString(PROP_MAXLENGTH, propertynode->GetNodeContent()); }
}

void TextCtrlWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("value"));
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("maxlength"));
    if(propertynode) { SetPropertyString(PROP_MAXLENGTH, propertynode->GetNodeContent()); }
}

void TextCtrlWrapper::LoadPropertiesFromwxFB(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxFB(node);

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "value");
    if(propertynode) { SetPropertyString(PROP_VALUE, propertynode->GetNodeContent()); }

    propertynode = XmlUtils::FindNodeByName(node, "property", "maxlength");
    if(propertynode) { SetPropertyString(PROP_MAXLENGTH, propertynode->GetNodeContent()); }
}

wxcWidget* TextCtrlWrapper::Clone() const { return new TextCtrlWrapper(); }
