#ifndef CLDEBUGGERBREAKPOINT_HPP
#define CLDEBUGGERBREAKPOINT_HPP

#include "JSON.h"
#include "codelite_exports.h"
#include "serialized_object.h"
#include <vector>
#include <wx/string.h>

// Breakpoint types. If you add more, clEditor::FillBPtoMarkerArray will also need altering
enum BreakpointType {
    /*Convenient return-codes*/
    BP_type_invalid = -1,
    BP_type_none = 0,
    /*Real breakpoint-types*/
    BP_FIRST_ITEM,
    BP_type_break = BP_FIRST_ITEM,
    BP_type_cmdlistbreak,
    BP_type_condbreak,
    BP_type_ignoredbreak,
    BP_type_tempbreak,
    BP_LAST_MARKED_ITEM = BP_type_tempbreak,
    BP_type_watchpt,
    BP_LAST_ITEM = BP_type_watchpt
};

// Watchpoint subtypes: write,read and both
enum WatchpointType { WP_watch, WP_rwatch, WP_awatch };

// The breakpoint origin:
// Can be from the Editor (user clicked 'F9')
// or from any other source (direct command to gdb, from the start up command etc)
enum BreakpointOrigin { BO_Editor, BO_Other };

class WXDLLIMPEXP_CL clDebuggerBreakpoint : public SerializedObject
{
public:
    typedef std::vector<clDebuggerBreakpoint> Vec_t;

public:
    // Where the bp is: file/lineno, function name (e.g. main()) or the memory location
    wxString file;
    int lineno = -1;
    wxString watchpt_data;
    wxString function_name;
    bool regex = false; // Is the function_name a regex?
    wxString memory_address;
    // How to identify the bp. Because the debugger won't always be running, we need an internal id as well as the
    // debugger's one
    double internal_id = wxNOT_FOUND;
    double debugger_id = wxNOT_FOUND; // -1 signifies not set
    BreakpointType bp_type =
        BP_type_break; // Is it a plain vanilla breakpoint, or a temporary one, or a watchpoint, or...
    size_t ignore_number =
        0; // 0 means 'not ignored'. >0 is the number of times the bp must be hit before it becomes enabled
    bool is_enabled = true;
    bool is_temp = false;
    WatchpointType watchpoint_type = WP_watch; // If this is a watchpoint, holds which sort it is
    wxString commandlist;
    wxString conditions;
    wxString at;
    wxString what;
    BreakpointOrigin origin = BO_Other;

    clDebuggerBreakpoint(const clDebuggerBreakpoint& BI);
    clDebuggerBreakpoint();
    clDebuggerBreakpoint& operator=(const clDebuggerBreakpoint& BI);
    ~clDebuggerBreakpoint();

    bool IsConditional()
    {
        return !conditions.IsEmpty();
    }
    double GetId() const
    {
        int best_id = (this->debugger_id == -1 ? this->internal_id : this->debugger_id);
        return best_id;
    }

    void Create(const wxString& filename, int line, int int_id, int ext_id = -1)
    {
        bp_type = BP_type_break;
        lineno = line;
        file = filename;
        internal_id = int_id;
        debugger_id = ext_id;
    }

    bool operator==(const clDebuggerBreakpoint& BI)
    {
        return ((origin == BI.origin) && (what == BI.what) && (at == BI.at) && (file == BI.file) &&
                (lineno == BI.lineno) && (function_name == BI.function_name) && (memory_address == BI.memory_address) &&
                (bp_type == BI.bp_type) && (watchpt_data == BI.watchpt_data) && (is_enabled == BI.is_enabled) &&
                (ignore_number == BI.ignore_number) && (conditions == BI.conditions) &&
                (commandlist == BI.commandlist) && (is_temp == BI.is_temp) &&
                (bp_type == BP_type_watchpt ? (watchpoint_type == BI.watchpoint_type) : true) &&
                (!function_name.IsEmpty() ? (regex == BI.regex) : true));
    }

    bool IsNull() const
    {
        return internal_id == wxNOT_FOUND && debugger_id == wxNOT_FOUND;
    }

    // JSON serialization
    JSONItem ToJSON() const;
    void FromJSON(const JSONItem& json);

protected:
    // SerializedObject interface
    virtual void Serialize(Archive& arch);
    virtual void DeSerialize(Archive& arch);
};
typedef std::vector<clDebuggerBreakpoint> clDebuggerBreakpointVec_t;
#endif // CLDEBUGGERBREAKPOINT_HPP
