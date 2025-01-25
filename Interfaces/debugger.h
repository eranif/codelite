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

#include "AsyncProcess/asyncprocess.h"
#include "BreakpointInfoArray.hpp"
#include "archive.h"
#include "clDebuggerBreakpoint.hpp"
#include "cl_standard_paths.h"
#include "macros.h"
#include "serialized_object.h"

#include <vector>
#include <wx/arrstr.h>
#include <wx/event.h>
#include <wx/stdpaths.h>
#include <wx/string.h>

enum DebuggerCommands {
    DBG_PAUSE = 0,
    DBG_NEXT,
    DBG_STEPIN,
    DBG_STEPI,
    DBG_STEPOUT,
    DBG_SHOW_CURSOR,
    DBG_NEXTI,
};

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
    bool active = false;
};

struct ThreadEntry {
    bool active = false;
    long dbgid = 0;
    wxString file;
    wxString line;
    wxString function;
};

struct VariableObjChild {
    int numChilds = 0; // If this child has children (i.e. is this child a node or leaf)
    wxString varName;  // the name of the variable object node
    wxString gdbId;    // A unique name given by gdb which holds this node information for further queries
    wxString value;
    bool isAFake = false; // Sets to true of this variable object is a fake node
    wxString type;

    VariableObjChild() = default;
};

struct VariableObject {
    bool isPtr = false;    // if this variable object is of type pointer
    bool isPtrPtr = false; // if this variable object is of type pointer pointer
    wxString gdbId;        // GDB unique identifier for this variable object
    wxString typeName;     // the type of this variable object
    int numChilds = 0;     // Number of children
    bool has_more = false; // has_more?
    VariableObject() = default;
};

struct LocalVariable {
    wxString name;
    wxString value;
    wxString type;
    bool updated = false;
    wxString gdbId; // Mac generates variable object for locals as well...

    LocalVariable() = default;
};

struct VariableObjectUpdateInfo {
    wxArrayString removeIds;
    wxArrayString refreshIds;
};

struct DisassembleEntry {
    wxString m_address;
    wxString m_function;
    wxString m_offset;
    wxString m_inst;
};

struct DbgRegister {
    wxString reg_name;
    wxString reg_value;
};

using VariableObjChildren = std::vector<VariableObjChild>;
using StackEntryArray = std::vector<StackEntry>;
using ThreadEntryArray = std::vector<ThreadEntry>;
using LocalVariables = std::vector<LocalVariable>;
using DisassembleEntryVec_t = std::vector<DisassembleEntry>;
using DbgRegistersVec_t = std::vector<DbgRegister>;

#define READ_CONFIG_PARAM(name, value) \
    {                                  \
        decltype(value) dummy;         \
        if (arch.Read(name, dummy)) {  \
            value = dummy;             \
        }                              \
    }

class DebuggerInformation : public SerializedObject
{
public:
    enum eGdbFlags {
        kPrintObjectOff = (1 << 0),
        kRunAsSuperuser = (1 << 1), // Run GDB as superuser
    };

    wxString name;
    wxString path;
    bool enableDebugLog = false;
    bool enablePendingBreakpoints = true;
    bool breakAtWinMain = false;
    bool showTerminal = false;
    wxString consoleCommand = TERMINAL_CMD;
    bool useRelativeFilePaths = false;
    int maxCallStackFrames = 500;
    bool catchThrow = false;
    bool showTooltipsOnlyWithControlKeyIsDown = true;
    bool debugAsserts = false;
    wxString initFileCommands;
    int maxDisplayStringSize = 200;
    int maxDisplayElements = 100;
    bool resolveLocals = true;
    bool autoExpandTipItems = true;
    bool applyBreakpointsAfterProgramStarted = false;
    bool whenBreakpointHitRaiseCodelite = true;
    wxString cygwinPathCommand;
    bool charArrAsPtr = false;
    bool enableGDBPrettyPrinting = true;
    bool defaultHexDisplay = false;
    size_t flags = 0; // see eGdbFlags

public:
    DebuggerInformation() = default;
    ~DebuggerInformation() override = default;

    void Serialize(Archive& arch)
    {
        arch.Write("name", name);
        arch.Write("path", path);
        arch.Write("enableDebugLog", enableDebugLog);
        arch.Write("enablePendingBreakpoints", enablePendingBreakpoints);
        arch.Write("breakAtWinMain", breakAtWinMain);
        arch.Write("showTerminal", showTerminal);
        arch.Write("consoleCommand", consoleCommand);
        arch.Write("useRelativeFilePaths", useRelativeFilePaths);
        arch.Write("maxCallStackFrames", maxCallStackFrames);
        arch.Write("catchThrow", catchThrow);
        arch.Write("showTooltipsOnlyWithControlKeyIsDown", showTooltipsOnlyWithControlKeyIsDown);
        arch.Write("debugAsserts", debugAsserts);
        arch.WriteCData("startupCommands", initFileCommands);
        arch.Write("maxDisplayStringSize", maxDisplayStringSize);
        arch.Write("maxDisplayElements", maxDisplayElements);
        arch.Write("resolveLocals", resolveLocals);
        arch.Write("autoExpandTipItems", autoExpandTipItems);
        arch.Write("applyBreakpointsAfterProgramStarted", applyBreakpointsAfterProgramStarted);
        arch.Write("whenBreakpointHitRaiseCodelite", whenBreakpointHitRaiseCodelite);
        arch.Write("charArrAsPtr", charArrAsPtr);
        arch.Write("enableGDBPrettyPrinting", enableGDBPrettyPrinting);
        arch.Write("defaultHexDisplay", defaultHexDisplay);
        arch.Write("flags", flags);
        arch.Write("cygwinPathCommand", cygwinPathCommand);
    }

    void DeSerialize(Archive& arch)
    {
        READ_CONFIG_PARAM("name", name);
        READ_CONFIG_PARAM("path", path);
        READ_CONFIG_PARAM("enableDebugLog", enableDebugLog);
        READ_CONFIG_PARAM("enablePendingBreakpoints", enablePendingBreakpoints);
        READ_CONFIG_PARAM("breakAtWinMain", breakAtWinMain);
        READ_CONFIG_PARAM("showTerminal", showTerminal);
        READ_CONFIG_PARAM("consoleCommand", consoleCommand);
        READ_CONFIG_PARAM("useRelativeFilePaths", useRelativeFilePaths);
        READ_CONFIG_PARAM("maxCallStackFrames", maxCallStackFrames);
        READ_CONFIG_PARAM("catchThrow", catchThrow);
        READ_CONFIG_PARAM("showTooltipsOnlyWithControlKeyIsDown", showTooltipsOnlyWithControlKeyIsDown);
        READ_CONFIG_PARAM("debugAsserts", debugAsserts);

        arch.ReadCData(wxT("startupCommands"), initFileCommands);

        wxFileName fnPrettyPrintersDir = wxFileName(clStandardPaths::Get().GetUserDataDir(), wxEmptyString);
        fnPrettyPrintersDir.AppendDir("gdb_printers");
        initFileCommands.Replace("${CODELITE_GDB_PRINTERS_DIR}", fnPrettyPrintersDir.GetPath());
        initFileCommands.Trim();

        // use Linux style syntax
        initFileCommands.Replace("\\", "/");

        READ_CONFIG_PARAM("maxDisplayStringSize", maxDisplayStringSize);
        READ_CONFIG_PARAM("maxDisplayElements", maxDisplayElements);
        READ_CONFIG_PARAM("resolveLocals", resolveLocals);
        READ_CONFIG_PARAM("autoExpandTipItems", autoExpandTipItems);
        READ_CONFIG_PARAM("applyBreakpointsAfterProgramStarted", applyBreakpointsAfterProgramStarted);
        READ_CONFIG_PARAM("whenBreakpointHitRaiseCodelite", whenBreakpointHitRaiseCodelite);
        READ_CONFIG_PARAM("charArrAsPtr", charArrAsPtr);
        READ_CONFIG_PARAM("enableGDBPrettyPrinting", enableGDBPrettyPrinting);
        READ_CONFIG_PARAM("defaultHexDisplay", defaultHexDisplay);
        READ_CONFIG_PARAM("flags", flags);
        READ_CONFIG_PARAM("cygwinPathCommand", cygwinPathCommand);
    }
};

class IDebugger;

// sent by debugger start events
class DebuggerStartupInfo
{
public:
    enum DebugType { DebugProject = 1, AttachProcess = 2, QuickDebug = 3 };

    long pid = wxNOT_FOUND;
    wxString project;
    IDebugger* debugger = nullptr;

    DebuggerStartupInfo() = default;

    explicit DebuggerStartupInfo(long pid)
        : pid(pid)
    {
    }

    explicit DebuggerStartupInfo(const wxString& project)
        : project(project)
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
    wxString debuggerPath;                    /// The debugger to use
    wxString exeName;                         /// Executable to debug
    int PID = wxNOT_FOUND;                    /// Process ID to attach
    wxString cwd;                             /// Working directory
    std::vector<clDebuggerBreakpoint> bpList; /// Breakpoint list
    wxArrayString cmds;               /// Start-up commands (to be run after gdb starts, but before the user interacts)
    wxString ttyName;                 /// TTY to use
    wxArrayString searchPaths;        /// Additional search paths to pass to the debugger
    bool enablePrettyPrinting = true; /// Should we enable pretty printing?
    bool isSSHDebugging = false;      /// Debugging over SSH
    wxString sshAccountName;          /// When isSSHDebugging is set true, holds the ssh account name
    wxString init_file_content;       /// content to place in a .gdbinit like file
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
    IDebuggerObserver* m_observer = nullptr;
    DebuggerInformation m_info;
    EnvironmentConfig* m_env = nullptr;
    wxString m_name;
    bool m_isRemoteDebugging = false;
    bool m_isRemoteExtended = false;
    bool m_isSSHDebugging = false;
    wxString m_debuggeeProjectName;
    wxString m_sshAccount;

    //! Command to execute right after connect to the remote target
    wxString m_debuggerPostRemoteConnectCommands;

public:
    IDebugger() = default;

    virtual ~IDebugger() = default;
    void SetProjectName(const wxString& project) { m_debuggeeProjectName = project; }
    void SetName(const wxString& name) { m_name = name; }
    wxString GetName() const { return m_name; }

    void SetObserver(IDebuggerObserver* observer) { m_observer = observer; }

    const wxString& GetSshAccount() const { return m_sshAccount; }
    IDebuggerObserver* GetObserver() { return m_observer; }

    void SetEnvironment(EnvironmentConfig* env) { m_env = env; }

    virtual void SetDebuggerInformation(const DebuggerInformation& info) { m_info = info; }

    DebuggerInformation GetDebuggerInformation() { return m_info; }

    void SetIsRemoteDebugging(bool isRemoteDebugging) { this->m_isRemoteDebugging = isRemoteDebugging; }
    void SetIsRemoteExtended(bool isRemoteExtended) { this->m_isRemoteExtended = isRemoteExtended; }
    void SetPostRemoteConnectCommands(const wxString& commands)
    {
        this->m_debuggerPostRemoteConnectCommands = commands;
    }

    bool GetIsRemoteDebugging() const { return m_isRemoteDebugging; }
    bool GetIsRemoteExtended() const { return m_isRemoteExtended; }
    const wxString& GetPostRemoteConnectCommands() const { return this->m_debuggerPostRemoteConnectCommands; }

    void SetIsSSHDebugging(bool isSSHDebugging) { this->m_isSSHDebugging = isSSHDebugging; }
    bool IsSSHDebugging() const { return m_isSSHDebugging; }
    void SetSshAccount(const wxString& sshAccount) { this->m_sshAccount = sshAccount; }

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
    virtual bool Start(const DebugSessionInfo& info, clEnvList_t* env_list) = 0;

    /**
     * \brief start the debugger by attaching to a process
     * \return true on success, false otherwise
     */
    virtual bool Attach(const DebugSessionInfo& info, clEnvList_t* env_list) = 0;

    /**
     * \brief Run the program under the debugger. This method must be called *after* Start() has been called
     * \param args arguments to pass to the debuggee process
     * \param comm the preferred communication string, if this string is not empty, the debugger assumes remote
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
    virtual bool Break(const clDebuggerBreakpoint& bp) = 0;
    /**
     * @brief restart the debugging session. (similar to 'run' command on GDB)
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
    virtual bool SetCondition(const clDebuggerBreakpoint& bp) = 0;
    /**
     * \brief Set a command-list for this breakpoint
     */
    virtual bool SetCommands(const clDebuggerBreakpoint& bp) = 0;
    /**
     * \brief ask the debugger to query about its file & line.
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

    // We provide two ways of evaluating an expression:
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
    // GDB, implement all the 'Variable Object' related method
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
     * @param persistent make a persistent watch, else create a floating watch which is not bound to the creation frame
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
using GET_DBG_CREATE_FUNC = IDebugger* (*)();
using GET_DBG_INFO_FUNC = DebuggerInfo (*)();

#endif // DEBUGGER_H
