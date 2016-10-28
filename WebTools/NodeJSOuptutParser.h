//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NodeJSOuptutParser.h
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

#ifndef NODEJSOUPTUTPARSER_H
#define NODEJSOUPTUTPARSER_H

#include <wx/string.h>
#include <map>
#include "json_node.h"
#include <wx/treebase.h>
#include <wx/dataview.h>
#include <vector>

struct PendingLookupDV {
    wxDataViewItem parent;
    int refID;
    wxString name;
};

struct PendingLookupT {
    wxTreeItemId parent;
    int refID;
    wxString name;
};

struct NodeJSHandle {
    int handleID;
    wxString name;
    wxString value;
    wxString type;
    std::vector<std::pair<int, wxString> > properties; // ref:name
    NodeJSHandle()
        : handleID(wxNOT_FOUND)
    {
    }

    bool IsOk() const { return handleID != wxNOT_FOUND; }
    bool IsString() const { return type.Lower() == "string"; }
    bool IsArray() const { return type.Lower() == "array"; }
};

class NodeJSOuptutParser
{
public:
    NodeJSOuptutParser();
    virtual ~NodeJSOuptutParser();

    /**
     * @brief parse node js debugger "ref" object and return a handle
     * @param ref JSON object as returned by NodeJS debugger
     * @param handles [output] add the handle to the map
     */
    NodeJSHandle ParseRef(const JSONElement& ref, std::map<int, NodeJSHandle>& handles);
};

#endif // NODEJSOUPTUTPARSER_H
