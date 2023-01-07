#include "search_ctrl_wrapper.h"
#include "allocator_mgr.h"
#include "bool_property.h"
#include "wxgui_defs.h"
#include <wx/textctrl.h>

SearchCtrlWrapper::SearchCtrlWrapper()
    : wxcWidget(ID_WXSEARCHCTRL)
{
    PREPEND_STYLE(wxTE_PROCESS_ENTER, true);
    PREPEND_STYLE(wxTE_PROCESS_TAB, false);
    PREPEND_STYLE(wxTE_NOHIDESEL, false);
    PREPEND_STYLE(wxTE_LEFT, false);
    PREPEND_STYLE(wxTE_CENTRE, false);
    PREPEND_STYLE(wxTE_RIGHT, false);
    PREPEND_STYLE(wxTE_CAPITALIZE, false);

    SetPropertyString(_("Common Settings"), "wxSearchCtrl");
    AddProperty(new StringProperty(PROP_VALUE, wxT(""), _("Default text value.")));
    AddProperty(new BoolProperty(PROP_SHOW_CANCEL_BTN, false, _("Show the 'Cancel' button")));
    AddProperty(new BoolProperty(PROP_SHOW_SEARCH_BTN, true, _("Show the 'Search' button")));

    RegisterEventCommand(wxT("wxEVT_COMMAND_TEXT_UPDATED"),
                         _("Respond to a wxEVT_COMMAND_TEXT_UPDATED event, generated when the text changes.\nNotice "
                           "that this event will be sent when the text controls contents changes\n - whether this is "
                           "due to user input or comes from the program itself\n(for example, if SetValue() is "
                           "called); see ChangeValue() for a function which does not send this event."));
    RegisterEventCommand(wxT("wxEVT_COMMAND_TEXT_ENTER"),
                         _("Respond to a wxEVT_COMMAND_TEXT_ENTER event, generated when enter is pressed in a text "
                           "control\n(which must have wxTE_PROCESS_ENTER style for this event to be generated)."));
    RegisterEventCommand(wxT("wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN"),
                         _("Respond to a wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN event\ngenerated when the search button "
                           "is clicked. Note that this does not initiate a search."));
    RegisterEventCommand(
        wxT("wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN"),
        _("Respond to a wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN event, generated when the cancel button is clicked."));

    m_namePattern = wxT("m_searchCtrl");
    SetName(GenerateName());
}

SearchCtrlWrapper::~SearchCtrlWrapper() {}

wxcWidget* SearchCtrlWrapper::Clone() const { return new SearchCtrlWrapper(); }

wxString SearchCtrlWrapper::CppCtorCode() const
{

    wxString cpp;
    cpp << CPPStandardWxCtorWithValue(wxT("0"));
    cpp << GetName() << wxT("->ShowSearchButton(") << PropertyBool(PROP_SHOW_SEARCH_BTN) << wxT(");\n");
    cpp << GetName() << wxT("->ShowCancelButton(") << PropertyBool(PROP_SHOW_CANCEL_BTN) << wxT(");\n");

    return cpp;
}

void SearchCtrlWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/srchctrl.h>")); }

wxString SearchCtrlWrapper::GetWxClassName() const { return wxT("wxSearchCtrl"); }

void SearchCtrlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCSize() << XRCStyle() << XRCCommonAttributes() << XRCValue() << wxT("<cancelbtn>")
         << PropertyString(PROP_SHOW_CANCEL_BTN) << wxT("</cancelbtn>") << wxT("<searchbtn>")
         << PropertyString(PROP_SHOW_SEARCH_BTN) << wxT("</searchbtn>") << XRCSuffix();
}
