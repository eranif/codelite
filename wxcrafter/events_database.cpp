#include "events_database.h"
#include "wxgui_helpers.h"

void ConnectDetails::GenerateFunctionName(const wxString& controlName)
{
    wxString member = controlName;
    member.StartsWith(wxT("m_"), &member);
    member.StartsWith(wxT("_"), &member);

    wxString str = m_eventName;
    str.Replace(wxT("wxEVT_"), wxT(""));

    str = wxCrafter::CamelCase(str);
    wxString camelCaseControlName = wxCrafter::CamelCase(member);

    m_functionNameAndSignature.Clear();
    m_functionNameAndSignature << wxT("On") << camelCaseControlName << str << wxT("(") << m_eventClass
                               << wxT("& event)");
}

void ConnectDetails::MakeSignatureForName(const wxString& name)
{
    wxString tmpname = name;
    tmpname.Trim().Trim(false);
    if(tmpname.IsEmpty()) return;

    m_functionNameAndSignature.Clear();
    m_functionNameAndSignature << name << wxT("(") << m_eventClass << wxT("& event)");
}

wxString ConnectDetails::GetFunctionDecl() const
{
    wxString decl;
    decl << wxT("    virtual void ") << GetFunctionNameAndSignature() << wxT(";") << wxT("\n");
    wxCrafter::WrapInIfBlock(GetIfBlock(), decl);
    return decl;
}

wxString ConnectDetails::GetFunctionImpl(const wxString& classname) const
{
    wxString impl;
    impl << wxT("void ") << classname << wxT("::") << GetFunctionNameAndSignature();
    impl << wxT("\n{\n");
    impl << wxT("}\n");
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
                         const wxString& handlerName, const wxString& functionNameAndSig /*=""*/,
                         bool noBody /*=false*/)
{
    m_events.PushBack(eventName,
                      ConnectDetails(eventName, className, description, handlerName, functionNameAndSig, noBody));
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
    m_events.PushBack(wxT("wxEVT_KEY_DOWN"),
                      ConnectDetails(wxT("wxEVT_KEY_DOWN"), wxT("wxKeyEvent"),
                                     wxT("Process a wxEVT_KEY_DOWN event (any key has been pressed)"),
                                     wxT("wxKeyEventHandler")));
    m_events.PushBack(wxT("wxEVT_KEY_UP"),
                      ConnectDetails(wxT("wxEVT_KEY_UP"), wxT("wxKeyEvent"),
                                     wxT("Process a wxEVT_KEY_UP event (any key has been released)"),
                                     wxT("wxKeyEventHandler")));
    m_events.PushBack(wxT("wxEVT_CHAR"), ConnectDetails(wxT("wxEVT_CHAR"), wxT("wxKeyEvent"),
                                                        wxT("Process a wxEVT_CHAR event"), wxT("wxKeyEventHandler")));

    // Menu
    m_events.PushBack(
        wxT("wxEVT_CONTEXT_MENU"),
        ConnectDetails(wxT("wxEVT_CONTEXT_MENU"), wxT("wxContextMenuEvent"),
                       wxT("A right click (or other context menu command depending on platform) has been detected"),
                       wxT("wxContextMenuEventHandler")));

    // Mouse Events
    m_events.PushBack(wxT("wxEVT_LEFT_DOWN"),
                      ConnectDetails(wxT("wxEVT_LEFT_DOWN"), wxT("wxMouseEvent"),
                                     wxT("Process a wxEVT_LEFT_DOWN event. The handler of this event should normally "
                                         "call event.Skip() to allow the default processing to take place as otherwise "
                                         "the window under mouse wouldn't get the focus."),
                                     wxT("wxMouseEventHandler")));
    m_events.PushBack(wxT("wxEVT_LEFT_UP"),
                      ConnectDetails(wxT("wxEVT_LEFT_UP"), wxT("wxMouseEvent"), wxT("Process a wxEVT_LEFT_UP event."),
                                     wxT("wxMouseEventHandler")));
    m_events.PushBack(wxT("wxEVT_LEFT_DCLICK"),
                      ConnectDetails(wxT("wxEVT_LEFT_DCLICK"), wxT("wxMouseEvent"),
                                     wxT("Process a wxEVT_LEFT_DCLICK event."), wxT("wxMouseEventHandler")));
    m_events.PushBack(wxT("wxEVT_MIDDLE_DOWN"),
                      ConnectDetails(wxT("wxEVT_MIDDLE_DOWN"), wxT("wxMouseEvent"),
                                     wxT("Process a wxEVT_MIDDLE_DOWN event"), wxT("wxMouseEventHandler")));
    m_events.PushBack(wxT("wxEVT_MIDDLE_UP"),
                      ConnectDetails(wxT("wxEVT_MIDDLE_UP"), wxT("wxMouseEvent"),
                                     wxT("Process a wxEVT_MIDDLE_UP event"), wxT("wxMouseEventHandler")));
    m_events.PushBack(wxT("wxEVT_MIDDLE_DCLICK"),
                      ConnectDetails(wxT("wxEVT_MIDDLE_DCLICK"), wxT("wxMouseEvent"),
                                     wxT("Process a wxEVT_MIDDLE_DCLICK event."), wxT("wxMouseEventHandler")));
    m_events.PushBack(wxT("wxEVT_RIGHT_DOWN"),
                      ConnectDetails(wxT("wxEVT_RIGHT_DOWN"), wxT("wxMouseEvent"),
                                     wxT("Process a wxEVT_RIGHT_DOWN event"), wxT("wxMouseEventHandler")));
    m_events.PushBack(wxT("wxEVT_RIGHT_UP"),
                      ConnectDetails(wxT("wxEVT_RIGHT_UP"), wxT("wxMouseEvent"), wxT("Process a wxEVT_RIGHT_UP event"),
                                     wxT("wxMouseEventHandler")));
    m_events.PushBack(wxT("wxEVT_RIGHT_DCLICK"),
                      ConnectDetails(wxT("wxEVT_RIGHT_DCLICK"), wxT("wxMouseEvent"),
                                     wxT("Process a wxEVT_RIGHT_DCLICK event"), wxT("wxMouseEventHandler")));
    m_events.PushBack(wxT("wxEVT_MOTION"),
                      ConnectDetails(wxT("wxEVT_MOTION"), wxT("wxMouseEvent"), wxT("Process a wxEVT_MOTION event"),
                                     wxT("wxMouseEventHandler")));
    m_events.PushBack(wxT("wxEVT_ENTER_WINDOW"),
                      ConnectDetails(wxT("wxEVT_ENTER_WINDOW"), wxT("wxMouseEvent"),
                                     wxT("Process a wxEVT_ENTER_WINDOW event"), wxT("wxMouseEventHandler")));
    m_events.PushBack(wxT("wxEVT_LEAVE_WINDOW"),
                      ConnectDetails(wxT("wxEVT_LEAVE_WINDOW"), wxT("wxMouseEvent"),
                                     wxT("Process a wxEVT_LEAVE_WINDOW event."), wxT("wxMouseEventHandler")));
    m_events.PushBack(wxT("wxEVT_MOUSEWHEEL"),
                      ConnectDetails(wxT("wxEVT_MOUSEWHEEL"), wxT("wxMouseEvent"),
                                     wxT("Process a wxEVT_MOUSEWHEEL event"), wxT("wxMouseEventHandler")));
    m_events.PushBack(
        wxT("wxEVT_MOUSE_CAPTURE_LOST"),
        ConnectDetails(wxT("wxEVT_MOUSE_CAPTURE_LOST"), wxT("wxMouseCaptureLostEvent"),
                       wxT("A mouse capture lost event is sent to a window that had obtained mouse capture, which was "
                           "subsequently lost due to an \"external\" event (for example, when a dialog box is shown or "
                           "if another application captures the mouse)"),
                       wxT("wxMouseCaptureLostEventHandler")));

    // Focus
    m_events.PushBack(wxT("wxEVT_SET_FOCUS"),
                      ConnectDetails(wxT("wxEVT_SET_FOCUS"), wxT("wxFocusEvent"),
                                     wxT("Process a wxEVT_SET_FOCUS event"), wxT("wxFocusEventHandler")));
    m_events.PushBack(wxT("wxEVT_KILL_FOCUS"),
                      ConnectDetails(wxT("wxEVT_KILL_FOCUS"), wxT("wxFocusEvent"),
                                     wxT("Process a wxEVT_KILL_FOCUS event"), wxT("wxFocusEventHandler")));

    // UI
    m_events.PushBack(wxT("wxEVT_PAINT"),
                      ConnectDetails(wxT("wxEVT_PAINT"), wxT("wxPaintEvent"), wxT("Process a wxEVT_PAINT event"),
                                     wxT("wxPaintEventHandler")));
    m_events.PushBack(wxT("wxEVT_ERASE_BACKGROUND"),
                      ConnectDetails(wxT("wxEVT_ERASE_BACKGROUND"), wxT("wxEraseEvent"),
                                     wxT("Process a wxEVT_ERASE_BACKGROUND event."), wxT("wxEraseEventHandler")));
    m_events.PushBack(wxT("wxEVT_SIZE"), ConnectDetails(wxT("wxEVT_SIZE"), wxT("wxSizeEvent"),
                                                        wxT("Process a wxEVT_SIZE event"), wxT("wxSizeEventHandler")));
    m_events.PushBack(wxT("wxEVT_MOVE"),
                      ConnectDetails(wxT("wxEVT_MOVE"), wxT("wxMoveEvent"),
                                     wxT("Process a wxEVT_MOVE event, which is generated when a window is moved."),
                                     wxT("wxMoveEventHandler")));
    m_events.PushBack(wxT("wxEVT_UPDATE_UI"),
                      ConnectDetails(wxT("wxEVT_UPDATE_UI"), wxT("wxUpdateUIEvent"),
                                     wxT("Process a wxEVT_UPDATE_UI event"), wxT("wxUpdateUIEventHandler")));

    // Misc
    m_events.PushBack(wxT("wxEVT_IDLE"), ConnectDetails(wxT("wxEVT_IDLE"), wxT("wxIdleEvent"),
                                                        wxT("Process a wxEVT_IDLE event"), wxT("wxIdleEventHandler")));

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
