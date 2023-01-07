#include "myxh_textctrl.h"
#include <wx/textctrl.h>

MyTextCtrlXrcHandler::MyTextCtrlXrcHandler()
    : wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxTE_NO_VSCROLL);
    XRC_ADD_STYLE(wxTE_PROCESS_ENTER);
    XRC_ADD_STYLE(wxTE_PROCESS_TAB);
    XRC_ADD_STYLE(wxTE_MULTILINE);
    XRC_ADD_STYLE(wxTE_PASSWORD);
    XRC_ADD_STYLE(wxTE_READONLY);
    XRC_ADD_STYLE(wxHSCROLL);
    XRC_ADD_STYLE(wxTE_RICH);
    XRC_ADD_STYLE(wxTE_RICH2);
    XRC_ADD_STYLE(wxTE_AUTO_URL);
    XRC_ADD_STYLE(wxTE_NOHIDESEL);
    XRC_ADD_STYLE(wxTE_LEFT);
    XRC_ADD_STYLE(wxTE_CENTRE);
    XRC_ADD_STYLE(wxTE_RIGHT);
    XRC_ADD_STYLE(wxTE_DONTWRAP);
#if WXWIN_COMPATIBILITY_2_6
    XRC_ADD_STYLE(wxTE_LINEWRAP);
#endif // WXWIN_COMPATIBILITY_2_6
    XRC_ADD_STYLE(wxTE_CHARWRAP);
    XRC_ADD_STYLE(wxTE_WORDWRAP);

    // this style doesn't exist since wx 2.9.0 but we still support it (by
    // ignoring it silently) in XRC files to avoid unimportant warnings when
    // using XRC produced by old tools
    AddStyle(wxT("wxTE_AUTO_SCROLL"), 0);

    AddWindowStyles();
}

wxObject* MyTextCtrlXrcHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(text, wxTextCtrl)

    text->Create(m_parentAsWindow, GetID(), GetText(wxT("value")), GetPosition(), GetSize(), GetStyle(),
                 wxDefaultValidator, GetName());

    SetupWindow(text);

    if(HasParam(wxT("maxlength"))) text->SetMaxLength(GetLong(wxT("maxlength")));

    if(HasParam("hint") && !(text->GetWindowStyle() & wxTE_MULTILINE)) { text->SetHint(GetText("hint")); }
    return text;
}

bool MyTextCtrlXrcHandler::CanHandle(wxXmlNode* node) { return IsOfClass(node, wxT("wxTextCtrl")); }
