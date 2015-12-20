//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : XDebugRunCmdHandler.h
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

#ifndef XDEBUGRUNCMDHANDLER_H
#define XDEBUGRUNCMDHANDLER_H

#include "XDebugCommandHandler.h" // Base class: XDebugCommandHandler

class XDebugManager;
class wxXmlNode;
class wxSocketBase;
class XDebugRunCmdHandler : public XDebugCommandHandler
{
public:
    XDebugRunCmdHandler(XDebugManager* mgr, int transcationId);
    virtual ~XDebugRunCmdHandler();

public:
    virtual void Process(const wxXmlNode* response);
};

#endif // XDEBUGRUNCMDHANDLER_H
