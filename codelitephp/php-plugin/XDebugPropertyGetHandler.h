//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : XDebugPropertyGetHandler.h
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

#ifndef XDEBUGPROPERTYGETHANDLER_H
#define XDEBUGPROPERTYGETHANDLER_H

#include <wx/string.h>
#include "XDebugCommandHandler.h" // Base class: XDebugCommandHandler

class XDebugPropertyGetHandler : public XDebugCommandHandler
{
    wxString m_property;
    
public:
    XDebugPropertyGetHandler(XDebugManager* mgr, int transcationId, const wxString &property);
    virtual ~XDebugPropertyGetHandler() = default;

public:
    virtual void Process(const wxXmlNode* response);
};

#endif // XDEBUGPROPERTYGETHANDLER_H
