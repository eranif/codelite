#include "winid_property.h"
#include "wxgui_defs.h"
#include <algorithm>
#include <wx/choice.h>
#include <wx/combobox.h>

const wxEventType wxEVT_WINID_UPDATED = ::wxNewEventType();

wxArrayString WinIdProperty::s_options;
wxStringSet_t WinIdProperty::m_winIdSet;

WinIdProperty::WinIdProperty()
    : PropertyBase(PROP_WINDOW_ID)
{
    SetLabel(PROP_WINDOW_ID);
    m_winid = wxT("wxID_ANY");

    if(s_options.IsEmpty()) {
        s_options.Add(wxT("wxID_ANY"));
        s_options.Add(wxT("wxID_SEPARATOR"));
        s_options.Add(wxT("wxID_OPEN"));
        s_options.Add(wxT("wxID_CLOSE"));
        s_options.Add(wxT("wxID_NEW"));
        s_options.Add(wxT("wxID_SAVE"));
        s_options.Add(wxT("wxID_SAVEAS"));
        s_options.Add(wxT("wxID_REVERT"));
        s_options.Add(wxT("wxID_EXIT"));
        s_options.Add(wxT("wxID_UNDO"));
        s_options.Add(wxT("wxID_REDO"));
        s_options.Add(wxT("wxID_HELP"));
        s_options.Add(wxT("wxID_PRINT"));
        s_options.Add(wxT("wxID_PRINT_SETUP"));
        s_options.Add(wxT("wxID_PAGE_SETUP"));
        s_options.Add(wxT("wxID_PREVIEW"));
        s_options.Add(wxT("wxID_ABOUT"));
        s_options.Add(wxT("wxID_HELP_CONTENTS"));
        s_options.Add(wxT("wxID_HELP_INDEX"));
        s_options.Add(wxT("wxID_HELP_SEARCH"));
        s_options.Add(wxT("wxID_HELP_COMMANDS"));
        s_options.Add(wxT("wxID_HELP_PROCEDURES"));
        s_options.Add(wxT("wxID_HELP_CONTEXT"));
        s_options.Add(wxT("wxID_CLOSE_ALL"));
        s_options.Add(wxT("wxID_PREFERENCES"));
        s_options.Add(wxT("wxID_EDIT"));
        s_options.Add(wxT("wxID_CUT"));
        s_options.Add(wxT("wxID_COPY"));
        s_options.Add(wxT("wxID_PASTE"));
        s_options.Add(wxT("wxID_CLEAR"));
        s_options.Add(wxT("wxID_FIND"));
        s_options.Add(wxT("wxID_DUPLICATE"));
        s_options.Add(wxT("wxID_SELECTALL"));
        s_options.Add(wxT("wxID_DELETE"));
        s_options.Add(wxT("wxID_REPLACE"));
        s_options.Add(wxT("wxID_REPLACE_ALL"));
        s_options.Add(wxT("wxID_PROPERTIES"));
        s_options.Add(wxT("wxID_VIEW_DETAILS"));
        s_options.Add(wxT("wxID_VIEW_LARGEICONS"));
        s_options.Add(wxT("wxID_VIEW_SMALLICONS"));
        s_options.Add(wxT("wxID_VIEW_LIST"));
        s_options.Add(wxT("wxID_VIEW_SORTDATE"));
        s_options.Add(wxT("wxID_VIEW_SORTNAME"));
        s_options.Add(wxT("wxID_VIEW_SORTSIZE"));
        s_options.Add(wxT("wxID_VIEW_SORTTYPE"));
        s_options.Add(wxT("wxID_OK"));
        s_options.Add(wxT("wxID_CANCEL"));
        s_options.Add(wxT("wxID_APPLY"));
        s_options.Add(wxT("wxID_YES"));
        s_options.Add(wxT("wxID_NO"));
        s_options.Add(wxT("wxID_STATIC"));
        s_options.Add(wxT("wxID_FORWARD"));
        s_options.Add(wxT("wxID_BACKWARD"));
        s_options.Add(wxT("wxID_DEFAULT"));
        s_options.Add(wxT("wxID_MORE"));
        s_options.Add(wxT("wxID_SETUP"));
        s_options.Add(wxT("wxID_RESET"));
        s_options.Add(wxT("wxID_CONTEXT_HELP"));
        s_options.Add(wxT("wxID_YESTOALL"));
        s_options.Add(wxT("wxID_NOTOALL"));
        s_options.Add(wxT("wxID_ABORT"));
        s_options.Add(wxT("wxID_RETRY"));
        s_options.Add(wxT("wxID_IGNORE"));
        s_options.Add(wxT("wxID_ADD"));
        s_options.Add(wxT("wxID_REMOVE"));
        s_options.Add(wxT("wxID_UP"));
        s_options.Add(wxT("wxID_DOWN"));
        s_options.Add(wxT("wxID_HOME"));
        s_options.Add(wxT("wxID_REFRESH"));
        s_options.Add(wxT("wxID_STOP"));
        s_options.Add(wxT("wxID_INDEX"));
        s_options.Add(wxT("wxID_BOLD"));
        s_options.Add(wxT("wxID_ITALIC"));
        s_options.Add(wxT("wxID_JUSTIFY_CENTER"));
        s_options.Add(wxT("wxID_JUSTIFY_FILL"));
        s_options.Add(wxT("wxID_JUSTIFY_RIGHT"));
        s_options.Add(wxT("wxID_JUSTIFY_LEFT"));
        s_options.Add(wxT("wxID_UNDERLINE"));
        s_options.Add(wxT("wxID_INDENT"));
        s_options.Add(wxT("wxID_UNINDENT"));
        s_options.Add(wxT("wxID_ZOOM_100"));
        s_options.Add(wxT("wxID_ZOOM_FIT"));
        s_options.Add(wxT("wxID_ZOOM_IN"));
        s_options.Add(wxT("wxID_ZOOM_OUT"));
        s_options.Add(wxT("wxID_UNDELETE"));
        s_options.Add(wxT("wxID_REVERT_TO_SAVED"));
        s_options.Add(wxT("wxID_CDROM"));
        s_options.Add(wxT("wxID_CONVERT"));
        s_options.Add(wxT("wxID_EXECUTE"));
        s_options.Add(wxT("wxID_FLOPPY"));
        s_options.Add(wxT("wxID_HARDDISK"));
        s_options.Add(wxT("wxID_BOTTOM"));
        s_options.Add(wxT("wxID_FIRST"));
        s_options.Add(wxT("wxID_LAST"));
        s_options.Add(wxT("wxID_TOP"));
        s_options.Add(wxT("wxID_INFO"));
        s_options.Add(wxT("wxID_JUMP_TO"));
        s_options.Add(wxT("wxID_NETWORK"));
        s_options.Add(wxT("wxID_SELECT_COLOR"));
        s_options.Add(wxT("wxID_SELECT_FONT"));
        s_options.Add(wxT("wxID_SORT_ASCENDING"));
        s_options.Add(wxT("wxID_SORT_DESCENDING"));
        s_options.Add(wxT("wxID_SPELL_CHECK"));
        s_options.Add(wxT("wxID_STRIKETHROUGH"));
        std::sort(s_options.begin(), s_options.end());

        m_winIdSet.insert(s_options.begin(), s_options.end());
    }
}

WinIdProperty::~WinIdProperty() {}

JSONElement WinIdProperty::Serialize() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty(wxT("type"), wxT("winid"));
    DoBaseSerialize(json);
    json.addProperty(wxT("m_winid"), m_winid);
    return json;
}

void WinIdProperty::UnSerialize(const JSONElement& json)
{
    DoBaseUnSerialize(json);
    // Backward compatability
    if(json.hasNamedObject(wxT("m_winid")))
        m_winid = json.namedObject(wxT("m_winid")).toString();
    else
        m_winid = json.namedObject(wxT("m_value")).toString();
}
