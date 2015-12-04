//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : JavaScriptSyntaxColourThread.h
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

#ifndef JAVASCRIPTSYNTAXCOLOURTHREAD_H
#define JAVASCRIPTSYNTAXCOLOURTHREAD_H

#include "worker_thread.h" // Base class: WorkerThread

class WebTools;
class JavaScriptSyntaxColourThread : public WorkerThread
{
private:
    WebTools* m_plugin;

public:
    struct Request : public ThreadRequest {
        wxString filename;
        wxString content;
        Request() {}
    };

    struct Reply {
        wxString filename;
        wxString functions;
        wxString properties;
    };

public:
    JavaScriptSyntaxColourThread(WebTools* plugin);
    virtual ~JavaScriptSyntaxColourThread();

public:
    virtual void ProcessRequest(ThreadRequest* request);
    void QueueFile(const wxString& filename);
    void QueueBuffer(const wxString& filename, const wxString& content);
};

#endif // JAVASCRIPTSYNTAXCOLOURTHREAD_H
