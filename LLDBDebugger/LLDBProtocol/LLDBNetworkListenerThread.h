//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : LLDBNetworkListenerThread.h
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

#ifndef LLDBNETWORKLISTENERTHREAD_H
#define LLDBNETWORKLISTENERTHREAD_H

#include <wx/thread.h>
#include <wx/event.h>
#include "SocketAPI/clSocketBase.h"
#include "LLDBPivot.h"

/**
 * @class LLDBNetworkListenerThread
 * @author eran
 * @brief This thread listens on the LLDB port from codelite-lldb, accepts LLDBReply objects, unserialize
 * them and convert them into LLDBEvent
 * These events are later posted to the thread owner event handler
 */
class LLDBNetworkListenerThread : public wxThread
{
    wxEvtHandler *m_owner;
    clSocketBase::Ptr_t m_socket;
    LLDBPivot m_pivot;
public:
    LLDBNetworkListenerThread(wxEvtHandler *owner, const LLDBPivot& pivot, int fd);
    virtual ~LLDBNetworkListenerThread();

    /**
     * @brief create the start the thread
     */
    void Start() {
        Create();
        Run();
    }

    /**
     * @brief the thread entry point
     */
    virtual void* Entry();

protected:
    /**
     * @brief stop and join the thread
     */
    void Stop() {
        if ( IsAlive() ) {
            Delete(NULL, wxTHREAD_WAIT_BLOCK);
        } else {
            Wait(wxTHREAD_WAIT_BLOCK);
        }
    }
};

#endif // LLDBNETWORKLISTENERTHREAD_H
