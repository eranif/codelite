#ifndef NODEJSOUPTUTPARSER_H
#define NODEJSOUPTUTPARSER_H

#include <wx/string.h>
#include <map>
#include "json_node.h"
#include <wx/treebase.h>
#include <wx/dataview.h>

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
    std::map<int, wxString> properties; // ref:name
    NodeJSHandle()
        : handleID(wxNOT_FOUND)
    {
    }

    bool IsOk() const { return handleID != wxNOT_FOUND; }
    bool IsString() const { return type.Lower() == "string"; }
    bool IsArray() const { return type.Lower() == "Array"; }
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
