//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clTernWorkerThread.h
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

#ifndef CLTERNWORKERTHREAD_H
#define CLTERNWORKERTHREAD_H

#include "worker_thread.h" // Base class: WorkerThread

class clTernServer;
class clTernWorkerThread : public WorkerThread
{
    clTernServer* m_ternSerer;
    int m_port;

public:
    enum eRequestType {
        kCodeCompletion,
        kFunctionTip,
        kFindDefinition,
        kReset,
        kReparse,
    };
    
    struct Request : public ThreadRequest {
        char* jsonRequest;
        wxString filename;
        eRequestType type;
    };

    struct Reply {
        wxString json;
        wxString filename;
        eRequestType requestType;
    };

public:
    clTernWorkerThread(clTernServer* ternServer);
    virtual ~clTernWorkerThread();

public:
    virtual void ProcessRequest(ThreadRequest* request);
};

#endif // CLTERNWORKERTHREAD_H
