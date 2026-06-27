//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : event_notifier.h
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

#ifndef EVENTNOTIFIER_H
#define EVENTNOTIFIER_H

#include <functional>
#include <future>
#include <type_traits>
#include <unordered_map>
#include <wx/event.h>
#include <wx/eventfilter.h>
#include <wx/thread.h>

#if wxUSE_GUI
#include <wx/frame.h>
#endif

#include "codelite_exports.h"

using EventFilterCallback = std::function<bool(wxEvent&)>;
using EventFilterCallbackToken = size_t;

struct WXDLLIMPEXP_CL EventFilterCallbackContainer {
    EventFilterCallbackToken id{0};
    EventFilterCallback callback;
};

class WXDLLIMPEXP_CL EventNotifier : public wxEvtHandler
{
public:
    static EventNotifier* Get();
    static void Release();

    virtual void AddPendingEvent(const wxEvent& event);
    virtual bool ProcessEvent(wxEvent& event);

#if wxUSE_GUI
    wxFrame* TopFrame();
#endif
    bool SendCommandEvent(int eventId, void* clientData);
    void PostCommandEvent(int eventId, void* clientData);
    bool SendCommandEvent(int eventId, void* clientData, const wxString& s);

    // --------------------------------------------------------
    // About the following functions:
    // the below functions are meant to provide an easy
    // way to fire CodeLite's plugins events without the need
    // to create the event on the stack.
    // Post* are meant as Async event ("AddPendingEvent")
    // While Notify* are synchronous event ("ProcessEvent")
    //---------------------------------------------------------

    /**
     * @brief post a wxEVT_FILE_SAVED event
     */
    void PostFileSavedEvent(const wxString& filename);

    /**
     * @brief post a wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT or wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED
     * @param prompt
     */
    void PostReloadExternallyModifiedEvent(bool prompt = true);

    /**
     * @brief send a wxEVT_WORKSPACE_RELOAD_STARTED event (sync event)
     */
    void NotifyWorkspaceReloadStartEvet(const wxString& workspaceFile);

    /**
     * @brief send a wxEVT_WORKSPACE_RELOAD_ENDED event (sync event)
     */
    void NotifyWorkspaceReloadEndEvent(const wxString& workspaceFile);

    /**
     * @brief Executes callback(args...) on the main thread and blocks until it completes.
     */
    template <typename Callback, typename... Args>
    auto RunOnMain(Callback&& cb, Args&&... args) -> decltype(cb(std::forward<Args>(args)...))
    {
        using T = decltype(cb(std::forward<Args>(args)...));
        auto promise_ptr = std::make_shared<std::promise<T>>();
        auto f = promise_ptr->get_future();
        if (wxThread::IsMain()) {
            if constexpr (std::is_void_v<T>) {
                cb(std::forward<Args>(args)...);
                promise_ptr->set_value();
            } else {
                promise_ptr->set_value(cb(std::forward<Args>(args)...));
            }
        } else {
            auto wrapped =
                [promise_ptr, cb = std::forward<Callback>(cb), ... args = std::forward<Args>(args)]() mutable {
                    if constexpr (std::is_void_v<T>) {
                        cb(std::move(args)...);
                        promise_ptr->set_value();
                    } else {
                        promise_ptr->set_value(cb(std::move(args)...));
                    }
                };
            CallAfter(std::move(wrapped));
        }
        return f.get();
    }

    /**
     * @brief Posts callback(args...) to the main thread without blocking.
     */
    template <typename Callback, typename... Args>
    void PostToMain(Callback&& cb, Args&&... args)
    {
        if (wxThread::IsMain()) {
            cb(std::forward<Args>(args)...);
        } else {
            auto wrapped = [cb = std::forward<Callback>(cb), ... args = std::forward<Args>(args)]() mutable {
                cb(std::move(args)...);
            };
            CallAfter(std::move(wrapped));
        }
    }

    EventFilterCallbackToken AddEventTypeFilter(wxEventType type, EventFilterCallback callback);
    void RemoveEventTypeFilter(wxEventType type, EventFilterCallbackToken token);

    int FilterEvent(wxEvent& event);

private:
    EventNotifier() = default;
    virtual ~EventNotifier() = default;

    std::unordered_map<wxEventType, std::vector<EventFilterCallbackContainer>> m_eventFilterCallbacks;
};

/**
 * @brief Executes callback(args...) on the main (GUI) thread and blocks until it completes.
 * Returns the callback's return value. Safe to call from any thread.
 *
 * Example:
 * @code
 *   auto result = clRunOnMain(myCallback, arg1, std::move(arg2));
 * @endcode
 */
template <typename Callback, typename... Args>
auto clRunOnMain(Callback&& cb, Args&&... args) -> decltype(cb(std::forward<Args>(args)...))
{
    return EventNotifier::Get()->RunOnMain(std::forward<Callback>(cb), std::forward<Args>(args)...);
}

/**
 * @brief Posts callback(args...) to the main (GUI) thread without blocking (fire-and-forget).
 * If already on the main thread, the callback is invoked immediately.
 * Safe to call from any thread.
 *
 * Example:
 * @code
 *   clPostToMain(onComplete, std::move(result));
 * @endcode
 */
template <typename Callback, typename... Args>
void clPostToMain(Callback&& cb, Args&&... args)
{
    EventNotifier::Get()->PostToMain(std::forward<Callback>(cb), std::forward<Args>(args)...);
}
#endif // EVENTNOTIFIER_H
