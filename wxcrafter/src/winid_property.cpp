#include "winid_property.h"

#include "StdToWX.h"
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
        s_options = StdToWX::ToArrayString({ wxT("wxID_ANY"),
                                             wxT("wxID_SEPARATOR"),
                                             wxT("wxID_OPEN"),
                                             wxT("wxID_CLOSE"),
                                             wxT("wxID_NEW"),
                                             wxT("wxID_SAVE"),
                                             wxT("wxID_SAVEAS"),
                                             wxT("wxID_REVERT"),
                                             wxT("wxID_EXIT"),
                                             wxT("wxID_UNDO"),
                                             wxT("wxID_REDO"),
                                             wxT("wxID_HELP"),
                                             wxT("wxID_PRINT"),
                                             wxT("wxID_PRINT_SETUP"),
                                             wxT("wxID_PAGE_SETUP"),
                                             wxT("wxID_PREVIEW"),
                                             wxT("wxID_ABOUT"),
                                             wxT("wxID_HELP_CONTENTS"),
                                             wxT("wxID_HELP_INDEX"),
                                             wxT("wxID_HELP_SEARCH"),
                                             wxT("wxID_HELP_COMMANDS"),
                                             wxT("wxID_HELP_PROCEDURES"),
                                             wxT("wxID_HELP_CONTEXT"),
                                             wxT("wxID_CLOSE_ALL"),
                                             wxT("wxID_PREFERENCES"),
                                             wxT("wxID_EDIT"),
                                             wxT("wxID_CUT"),
                                             wxT("wxID_COPY"),
                                             wxT("wxID_PASTE"),
                                             wxT("wxID_CLEAR"),
                                             wxT("wxID_FIND"),
                                             wxT("wxID_DUPLICATE"),
                                             wxT("wxID_SELECTALL"),
                                             wxT("wxID_DELETE"),
                                             wxT("wxID_REPLACE"),
                                             wxT("wxID_REPLACE_ALL"),
                                             wxT("wxID_PROPERTIES"),
                                             wxT("wxID_VIEW_DETAILS"),
                                             wxT("wxID_VIEW_LARGEICONS"),
                                             wxT("wxID_VIEW_SMALLICONS"),
                                             wxT("wxID_VIEW_LIST"),
                                             wxT("wxID_VIEW_SORTDATE"),
                                             wxT("wxID_VIEW_SORTNAME"),
                                             wxT("wxID_VIEW_SORTSIZE"),
                                             wxT("wxID_VIEW_SORTTYPE"),
                                             wxT("wxID_OK"),
                                             wxT("wxID_CANCEL"),
                                             wxT("wxID_APPLY"),
                                             wxT("wxID_YES"),
                                             wxT("wxID_NO"),
                                             wxT("wxID_STATIC"),
                                             wxT("wxID_FORWARD"),
                                             wxT("wxID_BACKWARD"),
                                             wxT("wxID_DEFAULT"),
                                             wxT("wxID_MORE"),
                                             wxT("wxID_SETUP"),
                                             wxT("wxID_RESET"),
                                             wxT("wxID_CONTEXT_HELP"),
                                             wxT("wxID_YESTOALL"),
                                             wxT("wxID_NOTOALL"),
                                             wxT("wxID_ABORT"),
                                             wxT("wxID_RETRY"),
                                             wxT("wxID_IGNORE"),
                                             wxT("wxID_ADD"),
                                             wxT("wxID_REMOVE"),
                                             wxT("wxID_UP"),
                                             wxT("wxID_DOWN"),
                                             wxT("wxID_HOME"),
                                             wxT("wxID_REFRESH"),
                                             wxT("wxID_STOP"),
                                             wxT("wxID_INDEX"),
                                             wxT("wxID_BOLD"),
                                             wxT("wxID_ITALIC"),
                                             wxT("wxID_JUSTIFY_CENTER"),
                                             wxT("wxID_JUSTIFY_FILL"),
                                             wxT("wxID_JUSTIFY_RIGHT"),
                                             wxT("wxID_JUSTIFY_LEFT"),
                                             wxT("wxID_UNDERLINE"),
                                             wxT("wxID_INDENT"),
                                             wxT("wxID_UNINDENT"),
                                             wxT("wxID_ZOOM_100"),
                                             wxT("wxID_ZOOM_FIT"),
                                             wxT("wxID_ZOOM_IN"),
                                             wxT("wxID_ZOOM_OUT"),
                                             wxT("wxID_UNDELETE"),
                                             wxT("wxID_REVERT_TO_SAVED"),
                                             wxT("wxID_CDROM"),
                                             wxT("wxID_CONVERT"),
                                             wxT("wxID_EXECUTE"),
                                             wxT("wxID_FLOPPY"),
                                             wxT("wxID_HARDDISK"),
                                             wxT("wxID_BOTTOM"),
                                             wxT("wxID_FIRST"),
                                             wxT("wxID_LAST"),
                                             wxT("wxID_TOP"),
                                             wxT("wxID_INFO"),
                                             wxT("wxID_JUMP_TO"),
                                             wxT("wxID_NETWORK"),
                                             wxT("wxID_SELECT_COLOR"),
                                             wxT("wxID_SELECT_FONT"),
                                             wxT("wxID_SORT_ASCENDING"),
                                             wxT("wxID_SORT_DESCENDING"),
                                             wxT("wxID_SPELL_CHECK"),
                                             wxT("wxID_STRIKETHROUGH") });
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
