//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NodeJSHandlerBase.h
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

#ifndef NODEJSHANDLERBASE_H
#define NODEJSHANDLERBASE_H

#include "smart_ptr.h"
#include <wx/string.h>

class NodeJSDebugger;
class NodeJSHandlerBase
{
public:
    typedef SmartPtr<NodeJSHandlerBase> Ptr_t;
    
    NodeJSHandlerBase();
    virtual ~NodeJSHandlerBase();
    
    virtual void Process(NodeJSDebugger* debugger, const wxString& output) = 0;
};

#endif // NODEJSHANDLERBASE_H
