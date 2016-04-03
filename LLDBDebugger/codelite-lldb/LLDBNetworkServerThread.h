//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBNetworkServerThread.h
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

#ifndef LLDBNETWORKSERVERTHREAD_H
#define LLDBNETWORKSERVERTHREAD_H

#include <wx/thread.h>
#include "SocketAPI/clSocketBase.h"

class CodeLiteLLDBApp;
class LLDBNetworkServerThread : public wxThread
{
    CodeLiteLLDBApp* m_app;
    clSocketBase::Ptr_t m_socket;
    
public:
    LLDBNetworkServerThread(CodeLiteLLDBApp* app, socket_t fd);
    virtual ~LLDBNetworkServerThread();

    virtual void* Entry();

    void Start() {
        Create();
        Run();
    }
};
#endif // LLDBNETWORKSERVERTHREAD_H
