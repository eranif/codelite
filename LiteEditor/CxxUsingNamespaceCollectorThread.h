//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : CxxUsingNamespaceCollectorThread.h
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

#ifndef CXXUSINGNAMESPACECOLLECTORTHREAD_H
#define CXXUSINGNAMESPACECOLLECTORTHREAD_H

#include "worker_thread.h" // Base class: WorkerThread

class CxxUsingNamespaceCollectorThread : public WorkerThread
{
public:
    struct Request : public ThreadRequest {
        wxString filename;
        wxArrayString includePaths;
        Request() {}
    };

public:
    CxxUsingNamespaceCollectorThread();
    virtual ~CxxUsingNamespaceCollectorThread();

    virtual void ProcessRequest(ThreadRequest* request);
public:
    void QueueFile(const wxString &filename, const wxArrayString &searchPaths);
};

#endif // CXXUSINGNAMESPACECOLLECTORTHREAD_H
