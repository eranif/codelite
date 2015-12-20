//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NodeJSGetScriptHandler.h
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

#ifndef NODEJSGETSCRIPTHANDLER_H
#define NODEJSGETSCRIPTHANDLER_H

#include "NodeJSHandlerBase.h" // Base class: NodeJSHandlerBase

class NodeJSGetScriptHandler : public NodeJSHandlerBase
{
    wxString m_filename;
    int m_line;
    
public:
    NodeJSGetScriptHandler(const wxString& filename, int line);
    virtual ~NodeJSGetScriptHandler();

public:
    virtual void Process(NodeJSDebugger* debugger, const wxString& output);
};

#endif // NODEJSGETSCRIPTHANDLER_H
