//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : event_notifier.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "event_notifier.h"

#include "codelite_events.h"

#include <wx/app.h>

static EventNotifier* eventNotifier = NULL;

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

EventNotifier::~EventNotifier() {}

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

#if wxUSE_GUI
wxFrame* EventNotifier::TopFrame() { return static_cast<wxFrame*>(wxTheApp->GetTopWindow()); }
#endif

void EventNotifier::PostFileSavedEvent(const wxString& filename)
{
    if(_eventsDiabled)
        return;

    clCommandEvent event{ wxEVT_FILE_SAVED };
    event.SetString(filename);
    event.SetFileName(filename);
    AddPendingEvent(event);
}

void EventNotifier::PostReloadExternallyModifiedEvent(bool prompt)
{
    if(_eventsDiabled)
        return;
    wxCommandEvent event(prompt ? wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED : wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT);
    AddPendingEvent(event);
}

void EventNotifier::PostFileRemovedEvent(const wxArrayString& files)
{
    if(_eventsDiabled)
        return;
    clCommandEvent filesRemovedEvent(wxEVT_PROJ_FILE_REMOVED);
    filesRemovedEvent.SetStrings(files);
    AddPendingEvent(filesRemovedEvent);
}

void EventNotifier::NotifyWorkspaceReloadEndEvent(const wxString& workspaceFile)
{
    if(_eventsDiabled)
        return;
    clWorkspaceEvent event(wxEVT_WORKSPACE_RELOAD_ENDED);
    event.SetFileName(workspaceFile);
    ProcessEvent(event);
}

void EventNotifier::NotifyWorkspaceReloadStartEvet(const wxString& workspaceFile)
{
    if(_eventsDiabled)
        return;
    clWorkspaceEvent event(wxEVT_WORKSPACE_RELOAD_STARTED);
    event.SetFileName(workspaceFile);
    ProcessEvent(event);
}

void EventNotifier::AddPendingEvent(const wxEvent& event)
{
    if(_eventsDiabled) {
        return;
    }
    wxEvtHandler::AddPendingEvent(event);
}

bool EventNotifier::ProcessEvent(wxEvent& event)
{
    if(_eventsDiabled) {
        return false;
    }
    return wxEvtHandler::ProcessEvent(event);
}
