//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : XDebugStackGetCmdHandler.h
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

#ifndef XDEBUGSTACKGETCMDHANDLER_H
#define XDEBUGSTACKGETCMDHANDLER_H

#include "XDebugCommandHandler.h"
class XDebugStackGetCmdHandler : public XDebugCommandHandler
{
    int m_requestedStack;
public:
    XDebugStackGetCmdHandler(XDebugManager* mgr, int transcationId, int requestedStack = 0);
    virtual ~XDebugStackGetCmdHandler() = default;

    virtual void Process(const wxXmlNode* response);
};

#endif // XDEBUGSTACKGETCMDHANDLER_H
