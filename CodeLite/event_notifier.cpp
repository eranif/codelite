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

#include <wx/app.h>

static EventNotifier* eventNotifier = nullptr;

EventNotifier* EventNotifier::Get()
{
    if (eventNotifier == nullptr)
        eventNotifier = new EventNotifier();
    return eventNotifier;
}

EventNotifier::EventNotifier()
{
    Bind(wxEVT_WORKSPACE_LOADED, &EventNotifier::OnWorkspaceOpened, this);
    Bind(wxEVT_WORKSPACE_CLOSED, &EventNotifier::OnWorkspaceClosed, this);
}

EventNotifier::~EventNotifier()
{
    Unbind(wxEVT_WORKSPACE_LOADED, &EventNotifier::OnWorkspaceOpened, this);
    Unbind(wxEVT_WORKSPACE_CLOSED, &EventNotifier::OnWorkspaceClosed, this);
}

void EventNotifier::Release()
{
    delete eventNotifier;
    eventNotifier = nullptr;
}

bool EventNotifier::SendCommandEvent(int eventId, void* clientData)
{
    wxCommandEvent evt(eventId);
    evt.SetClientData(clientData);
    return ProcessEvent(evt);
}

bool EventNotifier::SendCommandEvent(int eventId, void* clientData, const wxString& s)
{
    wxCommandEvent evt(eventId);
    evt.SetClientData(clientData);
    evt.SetString(s);
    return ProcessEvent(evt);
}

void EventNotifier::PostCommandEvent(int eventId, void* clientData)
{
    wxCommandEvent evt(eventId);
    evt.SetClientData(clientData);
    AddPendingEvent(evt);
}

#if wxUSE_GUI
wxFrame* EventNotifier::TopFrame() { return static_cast<wxFrame*>(wxTheApp->GetTopWindow()); }
#endif

void EventNotifier::PostFileSavedEvent(const wxString& filename)
{
    clCommandEvent event{wxEVT_FILE_SAVED};
    event.SetString(filename);
    event.SetFileName(filename);
    AddPendingEvent(event);
}

void EventNotifier::PostReloadExternallyModifiedEvent(bool prompt)
{
    wxCommandEvent event(prompt ? wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED : wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT);
    AddPendingEvent(event);
}

void EventNotifier::NotifyWorkspaceReloadEndEvent(const wxString& workspaceFile)
{
    clWorkspaceEvent event(wxEVT_WORKSPACE_RELOAD_ENDED);
    event.SetFileName(workspaceFile);
    ProcessEvent(event);
}

void EventNotifier::NotifyWorkspaceReloadStartEvet(const wxString& workspaceFile)
{
    clWorkspaceEvent event(wxEVT_WORKSPACE_RELOAD_STARTED);
    event.SetFileName(workspaceFile);
    ProcessEvent(event);
}

void EventNotifier::AddPendingEvent(const wxEvent& event) { wxEvtHandler::AddPendingEvent(event); }

bool EventNotifier::ProcessEvent(wxEvent& event) { return wxEvtHandler::ProcessEvent(event); }

EventFilterCallbackToken EventNotifier::AddEventTypeFilter(wxEventType type, EventFilterCallback callback)
{
    static EventFilterCallbackToken event_filter_id{0};
    auto& callbacks = m_eventFilterCallbacks[type];
    // Place it first
    ++event_filter_id;
    EventFilterCallbackContainer container{.id = event_filter_id, .callback = std::move(callback)};
    callbacks.insert(callbacks.begin(), std::move(container));
    return event_filter_id;
}

void EventNotifier::RemoveEventTypeFilter(wxEventType type, EventFilterCallbackToken token)
{
    auto iter = m_eventFilterCallbacks.find(type);
    if (iter == m_eventFilterCallbacks.end()) {
        return;
    }

    auto& v = iter->second;
    size_t count = std::erase_if(v, [token](const EventFilterCallbackContainer& c) { return c.id == token; });
    if (count && v.empty()) {
        m_eventFilterCallbacks.erase(iter);
    }
}

int EventNotifier::FilterEvent(wxEvent& event)
{
    if (m_eventFilterCallbacks.empty()) {
        return wxEventFilter::Event_Skip;
    }

    auto iter = m_eventFilterCallbacks.find(event.GetEventType());
    if (iter == m_eventFilterCallbacks.end()) {
        return wxEventFilter::Event_Skip;
    }

    for (auto& cb : iter->second) {
        if (cb.callback(event)) {
            return wxEventFilter::Event_Processed;
        }
    }
    return wxEventFilter::Event_Skip;
}

void EventNotifier::OnWorkspaceOpened(clWorkspaceEvent& event)
{
    event.Skip();
    std::lock_guard<std::mutex> lk{m_workspaceInfoMutex};
    m_workspaceInfo = WorkspaceInfo{
        .is_remote = event.IsRemote(),
    };
}

void EventNotifier::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    std::lock_guard<std::mutex> lk{m_workspaceInfoMutex};
    m_workspaceInfo.reset();
}

std::optional<EventNotifier::WorkspaceInfo> EventNotifier::GetWorkspaceInfo()
{
    std::lock_guard<std::mutex> lk{m_workspaceInfoMutex};
    return m_workspaceInfo;
}
