//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : worker_thread.cpp
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
#include "processreaderthread.h"
#include "asyncprocess.h"

const wxEventType wxEVT_PROC_DATA_READ   = 10950;
const wxEventType wxEVT_PROC_TERMINATED  = 10951;

#if defined(__WXGTK__) || defined(__WXMAC__)
#include <sys/wait.h>
#endif

ProcessReaderThread::ProcessReaderThread()
    : wxThread(wxTHREAD_JOINABLE)
    , m_notifiedWindow( NULL )
    , m_process       ( NULL )
{
}

ProcessReaderThread::~ProcessReaderThread()
{
    m_notifiedWindow = NULL;
}

void* ProcessReaderThread::Entry()
{
    while ( true ) {
        // Did we get a request to terminate?
        if (TestDestroy()) {
            break;
        }

        if ( m_process ) {
            wxString buff;
            if(m_process->Read( buff )) {
                if( buff.IsEmpty() == false ) {

                    // If we got a callback object, use it
                    if ( m_process && m_process->GetCallback() ) {
                        m_process->GetCallback()->CallAfter( &IProcessCallback::OnProcessOutput, buff );

                    } else {
                        // fallback to the event system
                        // we got some data, send event to parent
                        wxCommandEvent e(wxEVT_PROC_DATA_READ);
                        ProcessEventData *ed = new ProcessEventData();
                        ed->SetData(buff);
                        ed->SetProcess( m_process );

                        e.SetClientData( ed );
                        if ( m_notifiedWindow ) {
                            m_notifiedWindow->AddPendingEvent( e );

                        } else {
                            wxDELETE(ed);
                        }

                    }
                }
            } else {

                // Process terminated, exit
                // If we got a callback object, use it
                if ( m_process && m_process->GetCallback() ) {
                    m_process->GetCallback()->CallAfter( &IProcessCallback::OnProcessTerminated );

                } else {
                    // fallback to the event system
                    wxCommandEvent e(wxEVT_PROC_TERMINATED);
                    ProcessEventData *ed = new ProcessEventData();
                    ed->SetProcess( m_process );
                    e.SetClientData( ed );

                    if ( m_notifiedWindow ) {
                        m_notifiedWindow->AddPendingEvent( e );
                    } else {
                        wxDELETE(ed);
                    }
                }
                break;
            }
        }
    }

    m_process = NULL;
    return NULL;
}

void ProcessReaderThread::Stop()
{

#if wxVERSION_NUMBER < 2904
    if(IsAlive()) {
        Delete();
    }
    Wait();
#else
    // Notify the thread to exit and
    // wait for it
    if ( IsAlive() ) {
        Delete(NULL, wxTHREAD_WAIT_BLOCK);

    } else {
        Wait(wxTHREAD_WAIT_BLOCK);

    }
#endif

}

void ProcessReaderThread::Start(int priority)
{
    Create();
    SetPriority(priority);
    Run();
}
