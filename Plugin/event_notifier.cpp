#include "event_notifier.h"
#include <wx/app.h>

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

