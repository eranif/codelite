//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : singleinstancethreadjob.h
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

#ifndef __singleinstancethreadjob__
#define __singleinstancethreadjob__

#include <wx/event.h>
#include "cl_command_event.h"
#include <wx/thread.h>
#include "SocketAPI/clSocketServer.h"

wxDECLARE_EVENT(wxEVT_CMD_SINGLE_INSTANCE_THREAD_OPEN_FILES, clCommandEvent);
wxDECLARE_EVENT(wxEVT_CMD_SINGLE_INSTANCE_THREAD_RAISE_APP, clCommandEvent);

#ifdef __WXMSW__
#ifdef NDEBUG
#define SINGLE_INSTANCE_PORT 13617
#else
#define SINGLE_INSTANCE_PORT 16617
#endif
#else 
#include <unistd.h>
#ifdef NDEBUG
#define SINGLE_INSTANCE_PORT ((getuid() % 57) + 13617)
#else
#define SINGLE_INSTANCE_PORT ((getuid() % 57) + 16617)
#endif
#endif

class clSingleInstanceThread : public wxThread
{
    clSocketServer m_serverSocket;

public:
    /**
     * @brief the thread main loop
     */
    virtual void* Entry();
    
    /**
     * @brief start the single instance thread
     */
    void Start()
    {
        Create();
        Run();
    }

    /**
     * @brief stop the thread
     */
    void Stop()
    {
        // Notify the thread to exit and
        // wait for it
        if(IsAlive()) {
            Delete(NULL, wxTHREAD_WAIT_BLOCK);

        } else {
            Wait(wxTHREAD_WAIT_BLOCK);
        }
    }

    clSingleInstanceThread();
    virtual ~clSingleInstanceThread();
};
#endif // __singleinstancethreadjob__
