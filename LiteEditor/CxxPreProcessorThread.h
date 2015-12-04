//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : CxxPreProcessorThread.h
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

#ifndef CXXPREPROCESSORTHREAD_H
#define CXXPREPROCESSORTHREAD_H

#include "worker_thread.h" // Base class: WorkerThread

class CxxPreProcessorThread : public WorkerThread
{
public:
    struct Request : public ThreadRequest
    {
        wxString filename;
        wxArrayString definitions;
        wxArrayString includePaths;

        Request()
        {
        }
    };

public:
    CxxPreProcessorThread();
    virtual ~CxxPreProcessorThread();

public:
    virtual void ProcessRequest(ThreadRequest* request);

    void QueueFile(const wxString& filename, const wxArrayString& definitions, const wxArrayString& includePaths);
};

#endif // CXXPREPROCESSORTHREAD_H
