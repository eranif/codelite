//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 The CodeLite Team
// File name            : XDebugCommandHandler.h
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

#ifndef XDEBUGCOMMANDHANDLER_H
#define XDEBUGCOMMANDHANDLER_H

#include <map>
#include <wx/sharedptr.h>

/// Base class for XDebug command handlers
class XDebugManager;
class wxXmlNode;
class wxSocketBase;
class XDebugCommandHandler
{
public:
    typedef wxSharedPtr<XDebugCommandHandler> Ptr_t;
    typedef std::map<int, XDebugCommandHandler::Ptr_t> Map_t;

protected:
    XDebugManager *m_mgr;
    int m_transactionId;

public:
    XDebugCommandHandler(XDebugManager* mgr, int transcationId);
    virtual ~XDebugCommandHandler();

    virtual void Process(const wxXmlNode* response) = 0;
    void SetTransactionId(int transactionId) {
        this->m_transactionId = transactionId;
    }
    int GetTransactionId() const {
        return m_transactionId;
    }
};

#endif // XDEBUGCOMMANDHANDLER_H
