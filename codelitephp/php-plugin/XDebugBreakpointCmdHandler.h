//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : XDebugBreakpointCmdHandler.h
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

#ifndef XDEBUGBREAKPOINTCMDHANDLER_H
#define XDEBUGBREAKPOINTCMDHANDLER_H

#include "XDebugCommandHandler.h"
#include "XDebugBreakpoint.h"

class wxXmlNode;
class wxSocketBase;
class XDebugBreakpointCmdHandler : public XDebugCommandHandler
{
    XDebugBreakpoint &m_breakpoint;
    
public:
    XDebugBreakpointCmdHandler(XDebugManager* mgr, int transactionId, XDebugBreakpoint &breakpoint);
    virtual ~XDebugBreakpointCmdHandler() = default;

    virtual void Process(const wxXmlNode* response);
};

#endif // XDEBUGBREAKPOINTCMDHANDLER_H
