#include "rich_text_ctrl_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include <wx/richtext/richtextctrl.h>

RichTextCtrlWrapper::RichTextCtrlWrapper()
    : wxcWidget(ID_WXRICHTEXT)
{
    PREPEND_STYLE(wxTE_PROCESS_ENTER, true);
    PREPEND_STYLE(wxTE_PROCESS_TAB, true);
    PREPEND_STYLE(wxTE_MULTILINE, true);
    PREPEND_STYLE(wxTE_READONLY, false);
    PREPEND_STYLE(wxTE_AUTO_URL, false);

    EnableStyle(wxT("wxWANTS_CHARS"), true);

    RegisterEvent(wxT("wxEVT_COMMAND_RICHTEXT_CHARACTER"), wxT("wxRichTextEvent"),
                  wxT("Process a wxEVT_COMMAND_RICHTEXT_CHARACTER event, generated when the user presses a character "
                      "key.\nValid event functions: GetFlags, GetPosition, GetCharacter."));
    RegisterEvent(wxT("wxEVT_COMMAND_RICHTEXT_DELETE"), wxT("wxRichTextEvent"),
                  wxT("Process a wxEVT_COMMAND_RICHTEXT_DELETE event, generated when the user presses the backspace or "
                      "delete key.\nValid event functions: GetFlags, GetPosition"));
    RegisterEvent(wxT("wxEVT_COMMAND_RICHTEXT_RETURN"), wxT("wxRichTextEvent"),
                  wxT("Process a wxEVT_COMMAND_RICHTEXT_RETURN event, generated when the user presses the return "
                      "key.\nValid event functions: GetFlags, GetPosition."));
    RegisterEvent(wxT("wxEVT_COMMAND_RICHTEXT_STYLE_CHANGED"), wxT("wxRichTextEvent"),
                  wxT("Process a wxEVT_COMMAND_RICHTEXT_STYLE_CHANGED event, generated when styling has been applied "
                      "to the control.\nValid event functions: GetPosition, GetRange."));
    RegisterEvent(wxT("wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGING"), wxT("wxRichTextEvent"),
                  wxT("Process a wxEVT_COMMAND_RICHTEXT_STYLESHEET_CHANGING event, generated when the control's "
                      "stylesheet has changed,\nfor example the user added, edited or deleted a style.\nValid event "
                      "functions: GetRange, GetPosition."));
    RegisterEvent(wxT("wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACING"), wxT("wxRichTextEvent"),
                  wxT("Process a wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACING event, generated when the control's "
                      "stylesheet is about to be replaced\nfor example when a file is loaded into the control.\nValid "
                      "event functions: Veto, GetOldStyleSheet, GetNewStyleSheet."));
    RegisterEvent(wxT("wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACED"), wxT("wxRichTextEvent"),
                  wxT("Process a wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACED event, generated when the control's "
                      "stylesheet has been replaced\nfor example when a file is loaded into the control\nValid event "
                      "functions: GetOldStyleSheet, GetNewStyleSheet."));
    RegisterEvent(wxT("wxEVT_COMMAND_RICHTEXT_CONTENT_INSERTED"), wxT("wxRichTextEvent"),
                  wxT("Process a wxEVT_COMMAND_RICHTEXT_CONTENT_INSERTED event generated when content has been "
                      "inserted into the control.\nValid event functions: GetPosition, GetRange."));
    RegisterEvent(wxT("wxEVT_COMMAND_RICHTEXT_CONTENT_DELETED"), wxT("wxRichTextEvent"),
                  wxT("Process a wxEVT_COMMAND_RICHTEXT_CONTENT_DELETED event, generated when content has been deleted "
                      "from the control\nValid event functions: GetPosition, GetRange."));
    RegisterEvent(
        wxT("wxEVT_COMMAND_RICHTEXT_BUFFER_RESET"), wxT("wxRichTextEvent"),
        wxT("Process a wxEVT_COMMAND_RICHTEXT_BUFFER_RESET event, generated when the buffer has been reset by deleting "
            "all content.\nYou can use this to set a default style for the first new paragraph."));
    RegisterEventCommand(wxT("wxEVT_COMMAND_TEXT_UPDATED"),
                         wxT("Respond to a wxEVT_COMMAND_TEXT_UPDATED event, generated when the text changes.\nNotice "
                             "that this event will be sent when the text controls contents changes\n - whether this is "
                             "due to user input or comes from the program itself\n(for example, if SetValue() is "
                             "called); see ChangeValue() for a function which does not send this event."));
    RegisterEventCommand(wxT("wxEVT_COMMAND_TEXT_ENTER"),
                         wxT("Respond to a wxEVT_COMMAND_TEXT_ENTER event, generated when enter is pressed in a text "
                             "control\n(which must have wxTE_PROCESS_ENTER style for this event to be generated)."));

    SetPropertyString(_("Common Settings"), "wxRichTextCtrl");
    AddProperty(new StringProperty(PROP_VALUE, wxT("wxRichTextCtrl!"), _("Value")));

    m_namePattern = wxT("m_richTextCtrl");
    SetName(GenerateName());
}

RichTextCtrlWrapper::~RichTextCtrlWrapper() {}

wxcWidget* RichTextCtrlWrapper::Clone() const { return new RichTextCtrlWrapper(); }

wxString RichTextCtrlWrapper::CppCtorCode() const
{
    wxString code;
    code << CPPStandardWxCtorWithValue(wxT("wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxWANTS_CHARS"));
    return code;
}

void RichTextCtrlWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/richtext/richtextctrl.h>"));
}

wxString RichTextCtrlWrapper::GetWxClassName() const { return wxT("wxRichTextCtrl"); }

void RichTextCtrlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type == XRC_LIVE) {
        text << XRCUnknown();

    } else {
        text << XRCPrefix() << XRCStyle() << XRCCommonAttributes() << XRCValue() << XRCSize() << XRCSuffix();
    }
}
