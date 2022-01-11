#include "events_database.h"

#include "wxgui_helpers.h"

void ConnectDetails::GenerateFunctionName(const wxString& controlName)
{
    wxString member = controlName;
    member.StartsWith("m_", &member);
    member.StartsWith("_", &member);

    wxString str = m_eventName;
    str.Replace("wxEVT_", "");

    str = wxCrafter::CamelCase(str);
    wxString camelCaseControlName = wxCrafter::CamelCase(member);

    m_functionNameAndSignature.Clear();
    m_functionNameAndSignature << "On" << camelCaseControlName << str << "(" << m_eventClass << "& event)";
}

void ConnectDetails::MakeSignatureForName(const wxString& name)
{
    wxString tmpname = name;
    tmpname.Trim().Trim(false);
    if(tmpname.IsEmpty())
        return;

    m_functionNameAndSignature.Clear();
    m_functionNameAndSignature << name << "(" << m_eventClass << "& event)";
}

wxString ConnectDetails::GetFunctionDecl() const
{
    wxString decl;
    decl << "    virtual void " << GetFunctionNameAndSignature() << ";"
         << "\n";
    wxCrafter::WrapInIfBlock(GetIfBlock(), decl);
    return decl;
}

wxString ConnectDetails::GetFunctionImpl(const wxString& classname) const
{
    wxString impl;
    impl << "void " << classname << "::" << GetFunctionNameAndSignature();
    impl << "\n{\n";
    impl << "}\n";
    wxCrafter::WrapInIfBlock(GetIfBlock(), impl);
    return impl;
}

void ConnectDetails::SetFunctionNameAndSignature(const wxString& functionNameAndSignature)
{
    // In theory the supplied string should be sane. In practice it might instead be:
    //   an import from wxFB, entered by a user who might have typed any daft thing; or
    //   an import from XRCed (in which case it would've been empty had we not constructed a fake one)
    if(!functionNameAndSignature.empty()) {
        wxString sig = functionNameAndSignature.AfterFirst('(').BeforeFirst(')');
        if(sig.empty()) {
            MakeSignatureForName(functionNameAndSignature);
            return;
        }
        // If we reached here, there wasn't a problem
        m_functionNameAndSignature = functionNameAndSignature;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EventsDatabase::EventsDatabase() {}

EventsDatabase::~EventsDatabase() { Clear(); }

void EventsDatabase::Add(const ConnectDetails& ed)
{
    m_events.PushBack(ed.GetEventName(), ed);
    m_menuIdToName[ed.GetMenuItemId()] = ed.GetEventName();
}

void EventsDatabase::Add(const wxString& eventName, const wxString& className, const wxString& description,
                         const wxString& functionNameAndSig /*=""*/, bool noBody /*=false*/)
{
    m_events.PushBack(eventName, ConnectDetails(eventName, className, description, noBody, functionNameAndSig));
    m_menuIdToName[m_events.Item(eventName).GetMenuItemId()] = eventName;
}

bool EventsDatabase::Exists(int menuId) const { return m_menuIdToName.count(menuId) > 0; }

ConnectDetails EventsDatabase::Item(int menuId) const
{
    if(Exists(menuId)) {
        MapMenuIdToName_t::const_iterator iter = m_menuIdToName.find(menuId);
        return m_events.Item(iter->second);
    }
    return ConnectDetails();
}

void EventsDatabase::FillCommonEvents()
{
    ////////////////////////////////////////////////
    // Create the common events KB
    ////////////////////////////////////////////////
    Clear();

    // Key Events
    m_events.PushBack("wxEVT_KEY_DOWN", ConnectDetails("wxEVT_KEY_DOWN", "wxKeyEvent",
                                                       _("Process a wxEVT_KEY_DOWN event (any key has been pressed)")));
    m_events.PushBack("wxEVT_KEY_UP", ConnectDetails("wxEVT_KEY_UP", "wxKeyEvent",
                                                     _("Process a wxEVT_KEY_UP event (any key has been released)")));
    m_events.PushBack("wxEVT_CHAR", ConnectDetails("wxEVT_CHAR", "wxKeyEvent", _("Process a wxEVT_CHAR event")));

    // Menu
    m_events.PushBack(
        "wxEVT_CONTEXT_MENU",
        ConnectDetails("wxEVT_CONTEXT_MENU", "wxContextMenuEvent",
                       _("A right click (or other context menu command depending on platform) has been detected")));

    // Mouse Events
    m_events.PushBack("wxEVT_LEFT_DOWN",
                      ConnectDetails("wxEVT_LEFT_DOWN", "wxMouseEvent",
                                     _("Process a wxEVT_LEFT_DOWN event. The handler of this event should normally "
                                       "call event.Skip() to allow the default processing to take place as otherwise "
                                       "the window under mouse wouldn't get the focus.")));
    m_events.PushBack("wxEVT_LEFT_UP",
                      ConnectDetails("wxEVT_LEFT_UP", "wxMouseEvent", _("Process a wxEVT_LEFT_UP event.")));
    m_events.PushBack("wxEVT_LEFT_DCLICK",
                      ConnectDetails("wxEVT_LEFT_DCLICK", "wxMouseEvent", _("Process a wxEVT_LEFT_DCLICK event.")));
    m_events.PushBack("wxEVT_MIDDLE_DOWN",
                      ConnectDetails("wxEVT_MIDDLE_DOWN", "wxMouseEvent", _("Process a wxEVT_MIDDLE_DOWN event")));
    m_events.PushBack("wxEVT_MIDDLE_UP",
                      ConnectDetails("wxEVT_MIDDLE_UP", "wxMouseEvent", _("Process a wxEVT_MIDDLE_UP event")));
    m_events.PushBack("wxEVT_MIDDLE_DCLICK",
                      ConnectDetails("wxEVT_MIDDLE_DCLICK", "wxMouseEvent", _("Process a wxEVT_MIDDLE_DCLICK event.")));
    m_events.PushBack("wxEVT_RIGHT_DOWN",
                      ConnectDetails("wxEVT_RIGHT_DOWN", "wxMouseEvent", _("Process a wxEVT_RIGHT_DOWN event")));
    m_events.PushBack("wxEVT_RIGHT_UP",
                      ConnectDetails("wxEVT_RIGHT_UP", "wxMouseEvent", _("Process a wxEVT_RIGHT_UP event")));
    m_events.PushBack("wxEVT_RIGHT_DCLICK",
                      ConnectDetails("wxEVT_RIGHT_DCLICK", "wxMouseEvent", _("Process a wxEVT_RIGHT_DCLICK event")));
    m_events.PushBack("wxEVT_MOTION",
                      ConnectDetails("wxEVT_MOTION", "wxMouseEvent", _("Process a wxEVT_MOTION event")));
    m_events.PushBack("wxEVT_ENTER_WINDOW",
                      ConnectDetails("wxEVT_ENTER_WINDOW", "wxMouseEvent", _("Process a wxEVT_ENTER_WINDOW event")));
    m_events.PushBack("wxEVT_LEAVE_WINDOW",
                      ConnectDetails("wxEVT_LEAVE_WINDOW", "wxMouseEvent", _("Process a wxEVT_LEAVE_WINDOW event.")));
    m_events.PushBack("wxEVT_MOUSEWHEEL",
                      ConnectDetails("wxEVT_MOUSEWHEEL", "wxMouseEvent", _("Process a wxEVT_MOUSEWHEEL event")));
    m_events.PushBack(
        "wxEVT_MOUSE_CAPTURE_LOST",
        ConnectDetails("wxEVT_MOUSE_CAPTURE_LOST", "wxMouseCaptureLostEvent",
                       wxT("A mouse capture lost event is sent to a window that had obtained mouse capture, which was "
                           "subsequently lost due to an \"external\" event (for example, when a dialog box is shown or "
                           "if another application captures the mouse)")));

    // Focus
    m_events.PushBack("wxEVT_SET_FOCUS",
                      ConnectDetails("wxEVT_SET_FOCUS", "wxFocusEvent", _("Process a wxEVT_SET_FOCUS event")));
    m_events.PushBack("wxEVT_KILL_FOCUS",
                      ConnectDetails("wxEVT_KILL_FOCUS", "wxFocusEvent", _("Process a wxEVT_KILL_FOCUS event")));

    // UI
    m_events.PushBack("wxEVT_PAINT", ConnectDetails("wxEVT_PAINT", "wxPaintEvent", _("Process a wxEVT_PAINT event")));
    m_events.PushBack("wxEVT_ERASE_BACKGROUND", ConnectDetails("wxEVT_ERASE_BACKGROUND", "wxEraseEvent",
                                                               _("Process a wxEVT_ERASE_BACKGROUND event.")));
    m_events.PushBack("wxEVT_SIZE", ConnectDetails("wxEVT_SIZE", "wxSizeEvent", _("Process a wxEVT_SIZE event")));
    m_events.PushBack("wxEVT_MOVE",
                      ConnectDetails("wxEVT_MOVE", "wxMoveEvent",
                                     _("Process a wxEVT_MOVE event, which is generated when a window is moved.")));
    m_events.PushBack("wxEVT_UPDATE_UI",
                      ConnectDetails("wxEVT_UPDATE_UI", "wxUpdateUIEvent", _("Process a wxEVT_UPDATE_UI event")));

    // Misc
    m_events.PushBack("wxEVT_IDLE", ConnectDetails("wxEVT_IDLE", "wxIdleEvent", _("Process a wxEVT_IDLE event")));

    MapEvents_t::const_iterator iter = m_events.begin();
    for(; iter != m_events.end(); iter++) {
        m_menuIdToName[iter->second.GetMenuItemId()] = iter->second.GetEventName();
    }
}

wxMenu* EventsDatabase::CreateMenu() const
{
    MapEvents_t::const_iterator iter = m_events.begin();
    wxMenu* menu = new wxMenu;
    for(; iter != m_events.end(); iter++) {
        ConnectDetails cd = iter->second;
        if(cd.GetEventName().IsEmpty()) {
            menu->AppendSeparator();

        } else {
            menu->Append(cd.GetMenuItemId(), cd.GetEventName(), wxEmptyString, wxITEM_CHECK);
        }
    }
    return menu;
}

void EventsDatabase::Clear()
{
    m_events.Clear();
    m_menuIdToName.clear();
}
