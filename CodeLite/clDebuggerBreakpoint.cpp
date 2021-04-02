#include "clDebuggerBreakpoint.hpp"

#ifdef __WXMSW__
#define is_windows true
#else
#define is_windows false
#endif

clDebuggerBreakpoint::clDebuggerBreakpoint()
{
}

clDebuggerBreakpoint::~clDebuggerBreakpoint()
{
}

clDebuggerBreakpoint::clDebuggerBreakpoint(const clDebuggerBreakpoint& BI)
{
    if(this == &BI) {
        return;
    }

    // call operator=
    *this = BI;

    if(!is_windows || (is_windows && !file.Contains("/"))) {
        // Normalize the file name
        if(!file.IsEmpty()) {
            wxFileName fn(file);
            fn.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_LONG);
            file = fn.GetFullPath();
        }
    }
}

clDebuggerBreakpoint& clDebuggerBreakpoint::operator=(const clDebuggerBreakpoint& BI)
{
    if(this == &BI) {
        return *this;
    }

    file = BI.file;
    lineno = BI.lineno;
    watchpt_data = BI.watchpt_data;
    function_name = BI.function_name;
    regex = BI.regex;
    memory_address = BI.memory_address;
    internal_id = BI.internal_id;
    debugger_id = BI.debugger_id;
    bp_type = BI.bp_type;
    ignore_number = BI.ignore_number;
    is_enabled = BI.is_enabled;
    is_temp = BI.is_temp;
    watchpoint_type = BI.watchpoint_type;
    commandlist = BI.commandlist;
    conditions = BI.conditions;
    at = BI.at;     // Provided by the debugger, no need to serialize
    what = BI.what; // Provided by the debugger, no need to serialize
    origin = BI.origin;
    return *this;
}

void clDebuggerBreakpoint::DeSerialize(Archive& arch)
{
    arch.Read(wxT("file"), file);
    arch.Read(wxT("lineno"), lineno);
    arch.Read(wxT("function_name"), function_name);
    arch.Read(wxT("memory_address"), memory_address);

    int tmpint;
    arch.Read(wxT("bp_type"), tmpint);
    bp_type = (BreakpointType)tmpint;

    arch.Read(wxT("watchpoint_type"), tmpint);
    watchpoint_type = (WatchpointType)tmpint;
    arch.Read(wxT("watchpt_data"), watchpt_data);
    arch.ReadCData(wxT("commandlist"), commandlist);
    commandlist.Trim().Trim(false); // ReadCData tends to add white-space to the commands e.g. a terminal \n
    arch.Read(wxT("regex"), regex);
    arch.Read(wxT("is_temp"), is_temp);
    arch.Read(wxT("is_enabled"), is_enabled);

    arch.Read(wxT("ignore_number"), tmpint);
    ignore_number = (unsigned int)tmpint;
    arch.Read(wxT("conditions"), conditions);

    arch.Read(wxT("origin"), tmpint);
    origin = (BreakpointOrigin)tmpint;
}

void clDebuggerBreakpoint::Serialize(Archive& arch)
{
    arch.Write(wxT("file"), file);
    arch.Write(wxT("lineno"), lineno);
    arch.Write(wxT("function_name"), function_name);
    arch.Write(wxT("memory_address"), memory_address);
    arch.Write(wxT("bp_type"), bp_type);
    arch.Write(wxT("watchpoint_type"), watchpoint_type);
    arch.Write(wxT("watchpt_data"), watchpt_data);
    // WriteCDate tends to write white-space even for empty commandlists
    arch.WriteCData(wxT("commandlist"), commandlist.Trim().Trim(false));
    arch.Write(wxT("regex"), regex);
    arch.Write(wxT("is_temp"), is_temp);
    arch.Write(wxT("is_enabled"), is_enabled);
    arch.Write(wxT("ignore_number"), ignore_number);
    arch.Write(wxT("conditions"), conditions);
    arch.Write(wxT("origin"), (int)origin);
}

JSONItem clDebuggerBreakpoint::ToJSON() const
{
    auto json = JSONItem::createObject();
    json.addProperty("file", file);
    json.addProperty("lineno", lineno);
    json.addProperty("function_name", function_name);
    json.addProperty("bp_type", (int)bp_type);
    json.addProperty("watchpoint_type", (int)watchpoint_type);
    json.addProperty("watchpt_data", watchpt_data);
    wxString command_list(commandlist);
    command_list.Trim(false).Trim(true);
    json.addProperty("commandlist", command_list);
    json.addProperty("ignore_number", ignore_number);
    json.addProperty("conditions", conditions);
    return json;
}

void clDebuggerBreakpoint::FromJSON(const JSONItem& json)
{
    file = json["file"].toString();
    lineno = json["lineno"].toInt();
    function_name = json["function_name"].toString();
    bp_type = (BreakpointType)json["bp_type"].toInt();
    watchpoint_type = (WatchpointType)json["watchpoint_type"].toInt();
    watchpt_data = json["watchpt_data"].toString();
    commandlist = json["commandlist"].toString();
    ignore_number = json["ignore_number"].toSize_t();
    conditions = json["conditions"].toString();
}
