#include "event_notifier.h"
#include <wx/app.h>
#include "cl_command_event.h"
#include "../Interfaces/codelite_events.h"

static EventNotifier *eventNotifier = NULL;

EventNotifier* EventNotifier::Get()
{
    if(eventNotifier == NULL)
        eventNotifier = new EventNotifier();
    return eventNotifier;
}

void EventNotifier::Release()
{
    if(eventNotifier)
        delete eventNotifier;
    eventNotifier = NULL;
}

EventNotifier::EventNotifier()
    : _eventsDiabled(false)
{
}

EventNotifier::~EventNotifier()
{
}

bool EventNotifier::SendCommandEvent(int eventId, void* clientData)
{
    if(_eventsDiabled)
        return false;

    wxCommandEvent evt(eventId);
    evt.SetClientData(clientData);
    return ProcessEvent(evt);
}

bool EventNotifier::SendCommandEvent(int eventId, void* clientData, const wxString& s)
{
    if(_eventsDiabled)
        return false;

    wxCommandEvent evt(eventId);
    evt.SetClientData(clientData);
    evt.SetString(s);
    return ProcessEvent(evt);
}

void EventNotifier::PostCommandEvent(int eventId, void* clientData)
{
    if(_eventsDiabled)
        return;

    wxCommandEvent evt(eventId);
    evt.SetClientData(clientData);
    AddPendingEvent(evt);
}

wxFrame* EventNotifier::TopFrame()
{
    return static_cast<wxFrame*>(wxTheApp->GetTopWindow());
}

void EventNotifier::PostFileSavedEvent(const wxString& filename)
{
    clCommandEvent event( wxEVT_FILE_SAVED );
    event.SetString( filename );
    event.SetFileName( filename );
    AddPendingEvent( event );
}

void EventNotifier::PostReloadExternallyModifiedEvent(bool prompt)
{
    wxCommandEvent event( prompt ? wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED : wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT );
    AddPendingEvent( event );
}

void EventNotifier::PostFileRemovedEvent(const wxArrayString& files)
{
    clCommandEvent filesRemovedEvent(wxEVT_PROJ_FILE_REMOVED);
    filesRemovedEvent.SetStrings( files );
    AddPendingEvent( filesRemovedEvent );
}
