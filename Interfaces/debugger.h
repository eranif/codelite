//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : debugger.h
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
#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "archive.h"
#include "serialized_object.h"

#include "wx/string.h"
#include "wx/arrstr.h"
#include "wx/event.h"
#include "vector"
#include "macros.h"
#include <wx/stdpaths.h>
#include "cl_standard_paths.h"

enum DebuggerCommands {
    DBG_PAUSE = 0,
    DBG_NEXT,
    DBG_STEPIN,
    DBG_STEPI,
    DBG_STEPOUT,
    DBG_SHOW_CURSOR,
    DBG_NEXTI,
};

// Breakpoint types. If you add more, LEditor::FillBPtoMarkerArray will also need altering
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

enum DisplayFormat { DBG_DF_NATURAL = 0, DBG_DF_HEXADECIMAL, DBG_DF_BINARY, DBG_DF_DECIMAL, DBG_DF_OCTAL };

//-------------------------------------------------------
// Data structures used by the debugger
//-------------------------------------------------------
struct DebuggerInfo {
    wxString name;
    wxString initFuncName;
    wxString version;
    wxString author;
};

struct StackEntry {
    wxString level;
    wxString address;
    wxString function;
    wxString file;
    wxString line;
    bool active;
};

struct ThreadEntry {
    bool active;
    long dbgid;
    wxString file;
    wxString line;
    wxString function;
};

struct VariableObjChild {
    int numChilds;    // If this child has children (i.e. is this child a node or leaf)
    wxString varName; // the name of the variable object node
    wxString gdbId;   // A unique name given by gdb which holds this node information for further queries
    wxString value;
    bool isAFake; // Sets to true of this variable object is a fake node
    wxString type;
    VariableObjChild()
        : numChilds(0)
        , isAFake(false)
    {
    }
};

struct VariableObject {
    bool isPtr;        // if this variable object is of type pointer
    bool isPtrPtr;     // if this variable object is of type pointer pointer
    wxString gdbId;    // GDB unique identifier for this variable object
    wxString typeName; // the type of this variable object
    int numChilds;     // Number of children
    bool has_more;     // has_nore?
    VariableObject()
        : isPtr(false)
        , isPtrPtr(false)
        , numChilds(0)
        , has_more(false)
    {
    }
};

struct LocalVariable {
    wxString name;
    wxString value;
    wxString type;
    bool updated;
    wxString gdbId; // Mac generates variable object for locals as well...

    LocalVariable()
        : updated(false)
    {
    }
    ~LocalVariable() {}
};

struct VariableObjectUpdateInfo {
    wxArrayString removeIds;
    wxArrayString refreshIds;
};

struct DisassembleEntry {
public:
    wxString m_address;
    wxString m_function;
    wxString m_offset;
    wxString m_inst;
};

struct DbgRegister {
    wxString reg_name;
    wxString reg_value;
};

typedef std::vector<VariableObjChild> VariableObjChildren;
typedef std::vector<StackEntry> StackEntryArray;
typedef std::vector<ThreadEntry> ThreadEntryArray;
typedef std::vector<LocalVariable> LocalVariables;
typedef std::vector<DisassembleEntry> DisassembleEntryVec_t;
typedef std::vector<DbgRegister> DbgRegistersVec_t;

class BreakpointInfo : public SerializedObject
{
public:
    class PredicateByFileAndLine
    {
        wxString m_filename;
        int m_line;

    public:
        PredicateByFileAndLine(const wxString& file, int line)
            : m_filename(file)
            , m_line(line)
        {
        }
        bool operator()(const BreakpointInfo& bp) const { return m_filename == bp.file && m_line == bp.lineno; }
    };
    typedef std::vector<BreakpointInfo> Vec_t;

public:
    // Where the bp is: file/lineno, function name (e.g. main()) or the memory location
    wxString file;
    int lineno;
    wxString watchpt_data;
    wxString function_name;
    bool regex; // Is the function_name a regex?
    wxString memory_address;
    // How to identify the bp. Because the debugger won't always be running, we need an internal id as well as the
    // debugger's one
    double internal_id;
    double debugger_id;     // -1 signifies not set
    BreakpointType bp_type; // Is it a plain vanilla breakpoint, or a temporary one, or a watchpoint, or...
    unsigned int
        ignore_number; // 0 means 'not ignored'. >0 is the number of times the bp must be hit before it becomes enabled
    bool is_enabled;
    bool is_temp;
    WatchpointType watchpoint_type; // If this is a watchpoint, holds which sort it is
    wxString commandlist;
    wxString conditions;
    wxString at;
    wxString what;
    BreakpointOrigin origin;

    BreakpointInfo(const BreakpointInfo& BI)
        : file(BI.file)
        , lineno(BI.lineno)
        , watchpt_data(BI.watchpt_data)
        , function_name(BI.function_name)
        , regex(BI.regex)
        , memory_address(BI.memory_address)
        , internal_id(BI.internal_id)
        , debugger_id(BI.debugger_id)
        , bp_type(BI.bp_type)
        , ignore_number(BI.ignore_number)
        , is_enabled(BI.is_enabled)
        , is_temp(BI.is_temp)
        , watchpoint_type(BI.watchpoint_type)
        , commandlist(BI.commandlist)
        , conditions(BI.conditions)
        , at(BI.at)
        , what(BI.what)
        , origin(BI.origin)
    {
        // Normalize the file name
        if(file.IsEmpty() == false) {
            wxFileName fn(file);
            fn.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_LONG);
            file = fn.GetFullPath();
        }
    }

    BreakpointInfo()
        : lineno(-1)
        , regex(false)
        , internal_id(wxNOT_FOUND)
        , debugger_id(wxNOT_FOUND)
        , bp_type(BP_type_break)
        , ignore_number(0)
        , is_enabled(true)
        , is_temp(false)
        , watchpoint_type(WP_watch)
        , origin(BO_Other)
    {
    }

    bool IsConditional() { return !conditions.IsEmpty(); }

    double GetId() const
    {
        int best_id = (this->debugger_id == -1 ? this->internal_id : this->debugger_id);
        return best_id;
    }

    void Create(wxString filename, int line, int int_id, int ext_id = -1)
    {
        wxFileName fn(filename);
        fn.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_LONG);

        bp_type = BP_type_break;
        lineno = line;
        file = filename.IsEmpty() ? wxString() : fn.GetFullPath();
        internal_id = int_id;
        debugger_id = ext_id;
    }

    BreakpointInfo& operator=(const BreakpointInfo& BI)
    {
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

    bool operator==(const BreakpointInfo& BI)
    {
        return ((origin == BI.origin) && (what == BI.what) && (at == BI.at) && (file == BI.file) &&
            (lineno == BI.lineno) && (function_name == BI.function_name) && (memory_address == BI.memory_address) &&
            (bp_type == BI.bp_type) && (watchpt_data == BI.watchpt_data) && (is_enabled == BI.is_enabled) &&
            (ignore_number == BI.ignore_number) && (conditions == BI.conditions) && (commandlist == BI.commandlist) &&
            (is_temp == BI.is_temp) && (bp_type == BP_type_watchpt ? (watchpoint_type == BI.watchpoint_type) : true) &&
            (!function_name.IsEmpty() ? (regex == BI.regex) : true));
    }

    bool IsNull() const { return internal_id == wxNOT_FOUND && debugger_id == wxNOT_FOUND; }

protected:
    // SerializedObject interface
    virtual void Serialize(Archive& arch)
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
        arch.Write(wxT("ignore_number"), (int)ignore_number);
        arch.Write(wxT("conditions"), conditions);
        arch.Write(wxT("origin"), (int)origin);
    }

    virtual void DeSerialize(Archive& arch)
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
};
typedef std::vector<BreakpointInfo> BreakpointInfoVec_t;

/**
 * @class BreakpointInfoArray a wrapper class to allow saving and reading breakpoint array to and
 * from the disk
 * @author eran
 * @date 07/06/09
 * @file debugger.h
 * @brief
 */
class BreakpointInfoArray : public SerializedObject
{
    std::vector<BreakpointInfo> m_breakpoints;

public:
    BreakpointInfoArray() {}

    virtual ~BreakpointInfoArray() {}

    BreakpointInfoArray& operator=(const std::vector<BreakpointInfo>& breakpoints)
    {
        m_breakpoints = breakpoints;
        return *this;
    }

    void SetBreakpoints(const std::vector<BreakpointInfo>& breakpoints) { this->m_breakpoints = breakpoints; }
    const std::vector<BreakpointInfo>& GetBreakpoints() const { return m_breakpoints; }

public:
    virtual void DeSerialize(Archive& arch)
    {

        size_t bt_count(0);
        m_breakpoints.clear();
        arch.Read(wxT("Count"), bt_count);

        for(size_t i = 0; i < bt_count; i++) {
            wxString name = wxString::Format(wxT("Breakpoint%u"), (unsigned int)i);
            BreakpointInfo bkpt;
            arch.Read(name, (SerializedObject*)&bkpt);
            m_breakpoints.push_back(bkpt);
        }
    }

    virtual void Serialize(Archive& arch)
    {

        arch.Write(wxT("Count"), (size_t)m_breakpoints.size());
        for(size_t i = 0; i < m_breakpoints.size(); i++) {
            wxString name = wxString::Format(wxT("Breakpoint%u"), (unsigned int)i);
            arch.Write(name, (SerializedObject*)&m_breakpoints.at(i));
        }
    }
};

class DebuggerInformation : public SerializedObject
{
public:
    enum eGdbFlags {
        kPrintObjectOff = (1 << 0),
        kRunAsSuperuser = (1 << 1), // Run GDB as superuser
    };

    wxString name;
    wxString path;
    bool enableDebugLog;
    bool enablePendingBreakpoints;
    bool breakAtWinMain;
    bool showTerminal;
    wxString consoleCommand;
    bool useRelativeFilePaths;
    int maxCallStackFrames;
    bool catchThrow;
    bool showTooltipsOnlyWithControlKeyIsDown;
    bool debugAsserts;
    wxString startupCommands;
    int maxDisplayStringSize;
    bool resolveLocals;
    bool autoExpandTipItems;
    bool applyBreakpointsAfterProgramStarted;
    bool whenBreakpointHitRaiseCodelite;
    wxString cygwinPathCommand;
    bool charArrAsPtr;
    bool enableGDBPrettyPrinting;
    bool defaultHexDisplay;
    size_t flags; // see eGdbFlags

public:
    DebuggerInformation()
        : name(wxEmptyString)
        , path(wxEmptyString)
        , enableDebugLog(false)
        , enablePendingBreakpoints(true)
        , breakAtWinMain(false)
        , showTerminal(false)
        , consoleCommand(TERMINAL_CMD)
        , useRelativeFilePaths(false)
        , maxCallStackFrames(500)
        , catchThrow(false)
        , showTooltipsOnlyWithControlKeyIsDown(false)
        , debugAsserts(false)
        , startupCommands(wxEmptyString)
        , maxDisplayStringSize(200)
        , resolveLocals(true)
        , autoExpandTipItems(true)
        , applyBreakpointsAfterProgramStarted(false)
        , whenBreakpointHitRaiseCodelite(true)
        , charArrAsPtr(false)
        , enableGDBPrettyPrinting(true)
        , defaultHexDisplay(false)
        , flags(0)
    {
    }

    virtual ~DebuggerInformation() {}

    void Serialize(Archive& arch)
    {
        arch.Write(wxT("name"), name);
        arch.Write(wxT("path"), path);
        arch.Write(wxT("enableDebugLog"), enableDebugLog);
        arch.Write(wxT("enablePendingBreakpoints"), enablePendingBreakpoints);
        arch.Write(wxT("breakAtWinMain"), breakAtWinMain);
        arch.Write(wxT("showTerminal"), showTerminal);
        arch.Write(wxT("consoleCommand"), consoleCommand);
        arch.Write(wxT("useRelativeFilePaths"), useRelativeFilePaths);
        arch.Write(wxT("maxCallStackFrames"), maxCallStackFrames);
        arch.Write(wxT("catchThrow"), catchThrow);
        arch.Write(wxT("showTooltips"), showTooltipsOnlyWithControlKeyIsDown);
        arch.Write(wxT("debugAsserts"), debugAsserts);
        arch.WriteCData(wxT("startupCommands"), startupCommands);
        arch.Write(wxT("maxDisplayStringSize"), maxDisplayStringSize);
        arch.Write(wxT("resolveLocals"), resolveLocals);
        arch.Write(wxT("autoExpandTipItems"), autoExpandTipItems);
        arch.Write(wxT("applyBreakpointsAfterProgramStarted"), applyBreakpointsAfterProgramStarted);
        arch.Write(wxT("whenBreakpointHitRaiseCodelite"), whenBreakpointHitRaiseCodelite);
        arch.Write(wxT("cygwinPathCommand"), cygwinPathCommand);
        arch.Write(wxT("charArrAsPtr"), charArrAsPtr);
        arch.Write(wxT("enableGDBPrettyPrinting"), enableGDBPrettyPrinting);
        arch.Write(wxT("defaultHexDisplay"), defaultHexDisplay);
        arch.Write("flags", flags);
    }

    void DeSerialize(Archive& arch)
    {
        arch.Read(wxT("name"), name);
        arch.Read(wxT("path"), path);
        arch.Read(wxT("enableDebugLog"), enableDebugLog);
        arch.Read(wxT("enablePendingBreakpoints"), enablePendingBreakpoints);
        arch.Read(wxT("breakAtWinMain"), breakAtWinMain);
        arch.Read(wxT("showTerminal"), showTerminal);
        arch.Read(wxT("consoleCommand"), consoleCommand);
        arch.Read(wxT("useRelativeFilePaths"), useRelativeFilePaths);
        arch.Read(wxT("maxCallStackFrames"), maxCallStackFrames);
        arch.Read(wxT("catchThrow"), catchThrow);
        arch.Read(wxT("showTooltips"), showTooltipsOnlyWithControlKeyIsDown);
        arch.Read(wxT("debugAsserts"), debugAsserts);

        arch.ReadCData(wxT("startupCommands"), startupCommands);

        wxFileName codeliteInstallDir = wxFileName(clStandardPaths::Get().GetUserDataDir(), "gdb_printers");
        startupCommands.Replace("$CodeLiteGdbPrinters", codeliteInstallDir.GetFullPath());
        startupCommands.Trim();

        arch.Read(wxT("maxDisplayStringSize"), maxDisplayStringSize);
        arch.Read(wxT("resolveLocals"), resolveLocals);
        arch.Read(wxT("autoExpandTipItems"), autoExpandTipItems);
        arch.Read(wxT("applyBreakpointsAfterProgramStarted"), applyBreakpointsAfterProgramStarted);
        arch.Read(wxT("whenBreakpointHitRaiseCodelite"), whenBreakpointHitRaiseCodelite);
        arch.Read(wxT("cygwinPathCommand"), cygwinPathCommand);
        arch.Read(wxT("charArrAsPtr"), charArrAsPtr);
        arch.Read(wxT("enableGDBPrettyPrinting"), enableGDBPrettyPrinting);
        arch.Read(wxT("defaultHexDisplay"), defaultHexDisplay);
        arch.Read("flags", flags);
    }
};

class IDebugger;

// sent by debugger start events
class DebuggerStartupInfo
{
public:
    enum DebugType { DebugProject = 1, AttachProcess = 2, QuickDebug = 3 };

    long pid;
    wxString project;
    IDebugger* debugger;

    DebuggerStartupInfo()
        : pid(wxNOT_FOUND)
        , project(wxEmptyString)
        , debugger(NULL)
    {
    }

    DebuggerStartupInfo(long pid)
        : pid(pid)
        , project(wxEmptyString)
        , debugger(NULL)
    {
    }

    DebuggerStartupInfo(const wxString& project)
        : pid(wxNOT_FOUND)
        , project(project)
        , debugger(NULL)
    {
    }

    DebugType GetDebugType() const
    {
        return pid != wxNOT_FOUND ? AttachProcess : project.IsEmpty() ? QuickDebug : DebugProject;
    }
};

/// This structure contains the information needed
/// by the debugger to start a debug session
class DebugSessionInfo
{
public:
    wxString debuggerPath;              /// The debugger to use
    wxString exeName;                   /// Executable to debug
    int PID;                            /// Process ID to attach
    wxString cwd;                       /// Working directory
    std::vector<BreakpointInfo> bpList; /// Breakpoint list
    wxArrayString cmds;                 /// Startup commands
    wxString ttyName;                   /// TTY to use
    wxArrayString searchPaths;          /// Additional search paths to pass to the debugger
    bool enablePrettyPrinting;          /// Should we enable pretty printing?
    DebugSessionInfo()
        : PID(wxNOT_FOUND)
        , enablePrettyPrinting(false)
    {
    }
};

class IDebuggerObserver;
class EnvironmentConfig;

//-------------------------------------------------------
// The debugger interface
//-------------------------------------------------------
/**
 * \ingroup Interfaces
 * Defines the *GDB* debugger interface
 *
 * \version 1.0
 * first version
 *
 * \date 08-22-2007
 *
 * \author Eran
 *
 */
class IDebugger
{
protected:
    IDebuggerObserver* m_observer;
    DebuggerInformation m_info;
    EnvironmentConfig* m_env;
    wxString m_name;
    bool m_isRemoteDebugging;
    bool m_isRemoteExtended;
    wxString m_debuggeeProjectName;

public:
    IDebugger()
        : m_env(NULL)
        , m_isRemoteDebugging(false){};
    virtual ~IDebugger(){};
    void SetProjectName(const wxString& project) { m_debuggeeProjectName = project; }
    void SetName(const wxString& name) { m_name = name; }
    wxString GetName() const { return m_name; }

    void SetObserver(IDebuggerObserver* observer) { m_observer = observer; }

    IDebuggerObserver* GetObserver() { return m_observer; }

    void SetEnvironment(EnvironmentConfig* env) { m_env = env; }

    virtual void SetDebuggerInformation(const DebuggerInformation& info) { m_info = info; }

    DebuggerInformation GetDebuggerInformation() { return m_info; }

    void SetIsRemoteDebugging(bool isRemoteDebugging) { this->m_isRemoteDebugging = isRemoteDebugging; }

    bool GetIsRemoteDebugging() const { return m_isRemoteDebugging; }

    void SetIsRemoteExtended(bool isRemoteExtended) { this->m_isRemoteExtended = isRemoteExtended; }

    bool GetIsRemoteExtended() const { return m_isRemoteExtended; }

    /**
     * \brief Sets the logging level 'on the fly'
     * \param level the new level
     */
    void EnableLogging(bool level) { m_info.enableDebugLog = level; }

    /**
     * \brief Gets the current logging level
     * \return the current level
     */
    bool IsLoggingEnabled() const { return m_info.enableDebugLog; }

    //-------------------------------------------------------------
    // Debugger operations
    //-------------------------------------------------------------

    /**
     * \brief start the debugger by running an executable
     * \return true on success, false otherwise
     */
    virtual bool Start(const DebugSessionInfo& info) = 0;

    /**
     * \brief start the debugger by attaching to a process
     * \return true on success, false otherwise
     */
    virtual bool Attach(const DebugSessionInfo& info) = 0;

    /**
     * \brief Run the program under the debugger. This method must be called *after* Start() has been called
     * \param args arguments to pass to the debuggee process
     * \param comm the preferemd communication string, if this string is not empty, the debugger assumes remote
     * debugging is on
     * and will execute a different set of commands for connecting to the debuggee.
     * comm is in the format of HOST:PORT or tty for serial debugging - this feature is currently enabled in GDB only
     * \return true on success, false otherwise
     */
    virtual bool Run(const wxString& args, const wxString& comm) = 0;

    /**
     * \brief Stop the debugger
     * \return true on success, false otherwise
     */
    virtual bool Stop() = 0;
    /**
     * \brief attempt to interrupt the running debugger
     * \return true on success, false otherwise
     */
    virtual bool Interrupt() = 0;
    /**
     * \brief return true of the debugger is currently running
     * \return true on success, false otherwise
     */
    virtual bool IsRunning() = 0;
    /**
     * \brief step to next line
     * \return true on success, false otherwise
     */
    virtual bool Next() = 0;

    /**
     * @brief perform "next instruction" command
     * @return true on success, false otherwise
     */
    virtual bool NextInstruction() = 0;
    /**
     * \brief continue execution of the debugger, until next breakpoint is hit, or program terminates
     * \return true on success, false otherwise
     */
    virtual bool Continue() = 0;

    /**
     * @brief for debuggers that support this, enable/disable the reverse debugging.
     * If the debugger does not support this - return false, otherwise return true
     */
    virtual void EnableReverseDebugging(bool b) = 0;

    /**
     * @brief start / stop recording for reverse debugging
     */
    virtual void EnableRecording(bool b) = 0;

    /**
     * @brief is reverse debugging recording is enabled?
     */
    virtual bool IsRecording() const = 0;

    /**
     * @brief return true if reverse debugging is enabled
     */
    virtual bool IsReverseDebuggingEnabled() const = 0;

    /**
     * \brief step into method
     * \return true on success, false otherwise
     */
    virtual bool StepIn() = 0;
    /**
     * \brief step into method
     * \return true on success, false otherwise
     */
    virtual bool StepInInstruction() = 0;
    /**
     * \brief step out the current method (gdb's 'finish' command)
     * \return true on success, false otherwise
     */
    virtual bool StepOut() = 0;
    /**
     * \brief set break point at given file and line, or function
     */
    virtual bool Break(const BreakpointInfo& bp) = 0;
    /**
     * @brief restart the debuggin session. (similar to 'run' command on GDB)
     * @return true on success false otherwise
     */
    virtual bool Restart() = 0;
    /**
     * \brief remove breakpoint by its ID
     */
    virtual bool RemoveBreak(double bid) = 0;
    /**
     * \brief clear all breakpoints set (gdb's 'clear' command)
     */
    virtual bool RemoveAllBreaks() = 0;
    /**
     * \brief Enable or Disable a breakpoint
     */
    virtual bool SetEnabledState(double bid, const bool enable) = 0;
    /**
     * \brief Set this breakpoint's Ignore count
     */
    virtual bool SetIgnoreLevel(double bid, const int ignorecount) = 0;
    /**
     * \brief Set this breakpoint's condition
     */
    virtual bool SetCondition(const BreakpointInfo& bp) = 0;
    /**
     * \brief Set a command-list for this breakpoint
     */
    virtual bool SetCommands(const BreakpointInfo& bp) = 0;
    /**
     * \brief ask the debugger to query about its file & line. Once the result arrives, the observer's UpdateFileLine()
     * will be invoked
     */
    virtual bool QueryFileLine() = 0;

    /**
     * @brief disassemble the current file
     * @param filename is the name of the file to disassemble
     * @param lineNumber is the line number to disassemble around
     */
    virtual bool Disassemble(const wxString& filename, int lineNumber) = 0;
    /**
     * \brief executes a command that does not yield any output from the debugger
     */
    virtual bool ExecuteCmd(const wxString& cmd) = 0;

    /**
     * \brief ask the debugger to print the current stack local variable. When the results arrives, the
     * observer->UpdateLocals() is called
     * \return true on success, false otherwise
     */
    virtual bool QueryLocals() = 0;
    /**
     * \brief list the current stack trace. once the results are received from the debugger,
     * m_observer->UpdateStackList() is invoked
     * \return true on success, false otherwise
     */
    virtual bool ListFrames() = 0;

    /**
     * @brief execute the equivalent of gdb's 'info reg' command
     */
    virtual bool ListRegisters() = 0;

    /**
     * \brief set the frame to be the active frame
     * \param frame frame to set active
     * \return true on success, false otherwise
     */
    virtual bool SetFrame(int frame) = 0;
    /**
     * \brief return list of threads.
     * \return true on success, false otherwise
     */
    virtual bool ListThreads() = 0;
    /**
     * \brief select threadId to be active
     * \param threadId thread id
     * \return true on success, false otherwise
     */
    virtual bool SelectThread(long threadId) = 0;
    /**
     * \brief the Poke() method is called at Idle() time by the application so the debugger can read the actual debugger
     * process output and process it
     */
    virtual void Poke() = 0;

    /**
     * @brief return string to show the user as tip for expression. this is an async call. When this function is done,
     * it will trigger a call to
     * IDebuggerObserver::UpdateTip()
     * @param dbgCommand debugger command to evaluate the tip (e.g. "print")
     * @param expression expression to evaluate
     * @return true if evaluation succeeded, false otherwise
     */
    virtual bool GetAsciiViewerContent(const wxString& dbgCommand, const wxString& expression) = 0;

    /**
     * \brief resolve expression and return its actual type this is an async call. When this function is done, it will
     * trigger a call to
     * IDebuggerObserver::UpdateTypeResolved()
     * \param expression expression to evaluate
     * \return true on success, false otherwise
     */
    virtual bool ResolveType(const wxString& expression, int userReason) = 0;

    // We provide two ways of evulating an expressions:
    // The short one, which returns a string, and long one
    // which returns a tree of the result
    virtual bool EvaluateExpressionToString(const wxString& expression, const wxString& format) = 0;

    /**
     * \brief a request to display memory from address -> address + count. This is a synchronous call
     * \param address starting address
     * \param count address range
     * \return true on success, false otherwise
     */
    virtual bool WatchMemory(const wxString& address, size_t count, size_t columns) = 0;

    /**
     * \brief set memory at given address and of size count. the value to set must be a space delimited
     * hex values (e.g. '0x01 0x02 0x03')
     */
    virtual bool SetMemory(const wxString& address, size_t count, const wxString& hex_value) = 0;

    /**
     * \brief have the debugger list all breakpoints
     */
    virtual void BreakList() = 0;

    /**
     * @brief assign new value to expression
     * @param expression expression
     * @param newValue new value
     * this method does not produce any output
     */
    virtual void AssignValue(const wxString& expression, const wxString& newValue) = 0;

    // ----------------------------------------------------------------------------------------
    // Variable object manipulation (GDB only)
    // If you wish to implement a debugger other than
    // GDB, implement all the 'Variable Object' releated method
    // with an empty implementation
    // ----------------------------------------------------------------------------------------
    /**
     * @brief list the children of a variable object
     * @param name
     */
    virtual bool ListChildren(const wxString& name, int userReason) = 0;

    /**
     * @brief create variable object from a given expression
     * @param expression the expression to create a variable object for
     * @param persistent make a presistent watch, else create a floating watch which is not bound to the creation frame
     */
    virtual bool CreateVariableObject(const wxString& expression, bool persistent, int userReason) = 0;

    /**
     * @brief delete variable object
     * @param name
     */
    virtual bool DeleteVariableObject(const wxString& name) = 0;

    /**
     * @brief evaluate variable object
     * @param name variable object
     */
    virtual bool EvaluateVariableObject(const wxString& name, int userReason) = 0;

    /**
     * @brief set the display format of a variable object
     * @param name
     * @param displayFormat
     * @return
     */
    virtual bool SetVariableObbjectDisplayFormat(const wxString& name, DisplayFormat displayFormat) = 0;

    /**
     * @brief update the variable object content
     * @param name
     * @return
     */
    virtual bool UpdateVariableObject(const wxString& name, int userReason) = 0;

    /**
     * @brief update watch
     */
    virtual bool UpdateWatch(const wxString& name) = 0;

    /**
     * @brief set next statement to run at given file and line
     */
    virtual bool Jump(wxString filename, int line) = 0;
};

//-----------------------------------------------------------
// Each debugger module must implement these two functions
//-----------------------------------------------------------
typedef IDebugger* (*GET_DBG_CREATE_FUNC)();
typedef DebuggerInfo (*GET_DBG_INFO_FUNC)();

#endif // DEBUGGER_H
