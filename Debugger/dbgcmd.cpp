//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : dbgcmd.cpp
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
#include "dbgcmd.h"

#include "clFileName.hpp"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "debuggergdb.h"
#include "debuggermanager.h"
#include "event_notifier.h"
#include "gdb_parser_incl.h"
#include "gdb_result_parser.h"
#include "gdbmi.hpp"
#include "gdbmi_parse_thread_info.h"
#include "precompiled_header.h"
#include "procutils.h"
#include "wx/buffer.h"
#include "wx/tokenzr.h"

#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <wx/regex.h>

static bool IS_WINDOWNS = (wxGetOsVersion() & wxOS_WINDOWS);

#define GDB_LEX()                         \
    {                                     \
        type = gdb_result_lex();          \
        currentToken = gdb_result_string; \
    }

namespace
{
void wxGDB_STRIP_QUOATES(wxString& currentToken)
{
    size_t where = currentToken.find(wxT("\""));
    if(where != std::string::npos && where == 0) {
        currentToken.erase(0, 1);
    }

    where = currentToken.rfind(wxT("\""));
    if(where != std::string::npos && where == currentToken.length() - 1) {
        currentToken.erase(where);
    }

    where = currentToken.find(wxT("\"\\\\"));
    if(where != std::string::npos && where == 0) {
        currentToken.erase(0, 3);
    }

    where = currentToken.rfind(wxT("\"\\\\"));
    if(where != std::string::npos && where == currentToken.length() - 3) {
        currentToken.erase(where);
    }
}

void wxRemoveQuotes(wxString& str)
{
    if(str.IsEmpty()) {
        return;
    }

    str.RemoveLast();
    if(str.IsEmpty() == false) {
        str.Remove(0, 1);
    }
}

wxString wxGdbFixValue(const wxString& value)
{
    int type(0);
    std::string currentToken;

    // GDB MI tends to mess the strings...
    // use our Flex lexer to normalize the value
    setGdbLexerInput(value.mb_str(wxConvUTF8).data(), true, true);
    GDB_LEX();
    wxString display_line;
    while(type != 0) {
        display_line << wxString(currentToken.c_str(), wxConvUTF8);
        GDB_LEX();
    }
    gdb_result_lex_clean();
    return display_line;
}

template <typename T> wxString get_file_name(const T& node)
{
    wxString file_name;
    if(!node["fullname"].value.empty()) {
        file_name = node["fullname"].value;
    } else if (!node["pending"].value.empty()) {
        file_name = node["pending"].value;
        if (file_name.AfterLast(':').IsNumber()) {
            file_name = file_name.BeforeLast(':');
        }
    } 
    // in case file is using cygwin path -> change it to Windows style
    file_name = clFileName::FromCygwin(file_name);
    return file_name;
}

void ParseStackEntry(gdbmi::Node& frame, StackEntry& entry)
{
    entry.level = frame["level"].value;
    entry.address = frame["addr"].value;
    entry.function = frame["func"].value;
    entry.file = get_file_name(frame);
    entry.line = frame["line"].value;
}

wxString ExtractGdbChild(const std::map<std::string, std::string>& attr, const wxString& name)
{
    std::map<std::string, std::string>::const_iterator iter = attr.find(name.mb_str(wxConvUTF8).data());
    if(iter == attr.end()) {
        return wxT("");
    }
    wxString val = wxString(iter->second.c_str(), wxConvUTF8);
    val.Trim().Trim(false);

    wxRemoveQuotes(val);
    val = wxGdbFixValue(val);

    return val;
}

// Keep a cache of all file paths converted from
// Cygwin path into native path
std::map<wxString, wxString> g_fileCache;
} // namespace

bool DbgCmdHandlerGetLine::ProcessOutput(const wxString& line)
{
    //  ^done,line="9",file="C:\\src\\gdbmi_parser\\src\\main.cpp",fullname="C:\\src\\gdbmi_parser\\src\\main.cpp",macro-info="0"
    gdbmi::Parser parser;
    gdbmi::ParsedResult result;
    parser.parse(line, &result);

    wxString filename;
    wxString lineNumber;
    long line_number = 0;

    // read the file name, giving fullname priority
    filename = get_file_name(result);

    if(!result["line"].value.empty()) {
        lineNumber = result["line"].value;
        lineNumber.ToCLong(&line_number);
    }

    clDebugEvent evtFileLine(wxEVT_DEBUG_SET_FILELINE);
    evtFileLine.SetFileName(filename);
    evtFileLine.SetLineNumber(line_number);
    evtFileLine.SetSshAccount(m_gdb->GetSshAccount());
    evtFileLine.SetIsSSHDebugging(m_gdb->IsSSHDebugging());
    EventNotifier::Get()->AddPendingEvent(evtFileLine);
    return true;
}

void DbgCmdHandlerAsyncCmd::UpdateGotControl(DebuggerReasons reason, const wxString& func)
{
    m_observer->UpdateGotControl(reason, func);
}

bool DbgCmdHandlerAsyncCmd::ProcessOutput(const wxString& line)
{
    //*stopped,reason="end-stepping-range",thread-id="1",frame={addr="0x0040156b",func="main",args=[{name="argc",value="1"},{name="argv",value="0x3e2c50"}],file="a.cpp",line="46"}
    // when reason is "end-stepping-range", it means that one of the following command was
    // completed:
    //-exec-step, -exec-stepi
    //-exec-next, -exec-nexti

    // *stopped,reason="signal-received",signal-name="SIGTRAP",signal-meaning="Trace/breakpoint
    // trap",frame={addr="0x00007ffb4c333151",func="ntdll!DbgBreakPoint",args=[],from="C:\\Windows\\SYSTEM32\\ntdll.dll",arch="i386:x86-64"},thread-id="5",stopped-threads="all"

    // *stopped,reason="function-finished",frame={addr="0x00007ff6a84b15ed",func="main",args=[{name="argc",value="1"},{name="argv",value="0x28996f51900"}],file="C:\\src\\gdbmi_parser\\src\\main.cpp",fullname="C:\\src\\gdbmi_parser\\src\\main.cpp",line="23",arch="i386:x86-64"},thread-id="1",stopped-threads="all"

    // try and get the debugee PID
    m_gdb->GetDebugeePID(line);

    gdbmi::Parser parser;
    gdbmi::ParsedResult result;
    parser.parse(line, &result);

    // sanity
    if(result.line_type != gdbmi::LT_EXEC_ASYNC_OUTPUT || result.line_type_context.empty() /* stopped */) {
        return false;
    }

    wxString func = result["frame"]["func"].value;
    wxString reason = result["reason"].value;
    wxString signal_name = result["signal-name"].value;

    // Note:
    // This might look like a stupid if-else, since all taking
    // the same action at the end - return control to program, but this is done
    // for future use to allow different handling for every case
    if(reason == wxT("end-stepping-range")) {
        // just notify the container that we got control back from debugger
        UpdateGotControl(DBG_END_STEPPING, func);

    } else if((reason == wxT("breakpoint-hit")) || (reason == wxT("watchpoint-trigger"))) {

        // Incase we break due to assertion, notify the observer with different break code
        if(func.IsEmpty() && IS_WINDOWNS) {
            if(func == wxT("msvcrt!_assert") || // MinGW
               func == wxT("__assert")          // Cygwin
            ) {
                // assertion caught
                UpdateGotControl(DBG_BP_ASSERTION_HIT, func);
            }
        }

        // Now discover which bp was hit. Fortunately, that's in the next token: bkptno="12"
        // Except that it no longer is in gdb 7.0. It's now: ..disp="keep",bkptno="12". So:
        static wxRegEx reGetBreakNo(wxT("bkptno=\"([0-9]+)\""));
        if(reGetBreakNo.Matches(line)) {
            wxString number = reGetBreakNo.GetMatch(line, 1);
            long id;
            if(number.ToLong(&id)) {
                if(id != wxNOT_FOUND && m_gdb->m_internalBpId == id) {

                    //*stopped,reason="breakpoint-hit",disp="del",bkptno="1",frame={addr="0x0040131e",func="main",args=[{name="argc",value="1"},
                    // {name="argv",value="0x602420"}],file="C:/src/TestArea/TestEXE/main.cpp",fullname="C:\\src\\TestArea\\TestEXE\\main.cpp",line="5"},thread-id="1",stopped-threads="all"

                    // try to locate the file name + line number:
                    int filePos = line.Find(wxT("fullname=\""));
                    int length = wxStrlen(wxT("fullname=\""));
                    wxString filename;
                    long curline = -1;
                    wxFileName curfile;

                    if(filePos == wxNOT_FOUND) {
                        filePos = line.Find(wxT("file=\""));
                        length = wxStrlen(wxT("file=\""));
                    }

                    int linePos = line.Find(wxT("line=\""));
                    if(linePos != wxNOT_FOUND) {
                        wxString tmpLine = line.Mid(linePos + wxStrlen(wxT("line=\"")));
                        tmpLine = tmpLine.BeforeFirst(wxT('"'));
                        tmpLine.ToLong(&curline);
                    }

                    if(filePos != wxNOT_FOUND) {
                        filename = line.Mid(filePos + length);
                        filename = filename.BeforeFirst(wxT('"'));
                        filename.Replace(wxT("\\"), wxT("/"));
                        filename.Replace(wxT("//"), wxT("/"));
                        curfile = filename;
                    }

                    m_observer->UpdateAddLine(wxString::Format(
                        _("Internal breakpoint was hit (id=%d), Applying user breakpoints and continuing"),
                        m_gdb->m_internalBpId));

                    // This is an internal breakpoint ID
                    m_gdb->m_internalBpId = wxNOT_FOUND;

                    // Apply the breakpoints
                    m_gdb->SetBreakpoints();

                    bool hasBreakOnMain = false;
                    const std::vector<clDebuggerBreakpoint>& bpList = m_gdb->GetBpList();
                    std::vector<clDebuggerBreakpoint>::const_iterator iter = bpList.begin();
                    for(; iter != bpList.end(); ++iter) {
                        wxFileName fn(iter->file);
                        int lineNo = iter->lineno;

                        wxString bpFile = fn.GetFullPath();
                        wxString gdbLine = curfile.GetFullPath();
                        if(bpFile == gdbLine && curline == lineNo) {
                            hasBreakOnMain = true;
                            break;
                        }
                    }

                    // Continue running; but only if the user didn't _want_ to break-at-main anyway!
                    // Continue running; but only if the user didn't _want_ to break-at-main anyway!
                    if(!m_gdb->GetShouldBreakAtMain() && !hasBreakOnMain) {
                        m_gdb->Continue();

                    } else {
                        // If we're not Continue()ing, we need to do UpdateGotControl(), otherwise the user can't tell
                        // what's happened
                        UpdateGotControl(DBG_BP_HIT, func);
                    }

                } else {

                    // Notify the container that we got control back from debugger
                    UpdateGotControl(DBG_BP_HIT, func); // User breakpoint
                    m_observer->UpdateBpHit((int)id);
                }
            } else {
                // In case of failure, pass control to user
                UpdateGotControl(DBG_BP_HIT, func);
            }
        } else {
            // In case of failure, pass control to user
            UpdateGotControl(DBG_BP_HIT, func);
        }

    } else if(reason == "signal-received" && !m_gdb->IsGoingDown()) {
        if(signal_name == "SIGSEGV") {
            UpdateGotControl(DBG_RECV_SIGNAL_SIGSEGV, func);

        } else if(signal_name == "EXC_BAD_ACCESS") {
            UpdateGotControl(DBG_RECV_SIGNAL_EXC_BAD_ACCESS, func);

        } else if(signal_name == "SIGABRT") {
            UpdateGotControl(DBG_RECV_SIGNAL_SIGABRT, func);

        } else if(signal_name == "SIGTRAP") {
            UpdateGotControl(DBG_RECV_SIGNAL_SIGTRAP, func);
        } else if(signal_name == "SIGPIPE") {
            UpdateGotControl(DBG_RECV_SIGNAL_SIGPIPE, func);
        } else {
            // default
            UpdateGotControl(DBG_RECV_SIGNAL, func);
        }
    } else if(reason == "exited-normally" || reason == "exited") {
        m_observer->UpdateAddLine(_("Program exited normally."));

        // debugee program exit normally
        UpdateGotControl(DBG_EXITED_NORMALLY, func);

    } else if(reason == "function-finished") {

        // debugee program exit normally
        UpdateGotControl(DBG_FUNC_FINISHED, func);

        // We finished an execution of a function.
        // Return to the caller the gdb-result-var since we might want
        // to create a variable object out of it
        if(result.exists("gdb-result-var")) {
            wxString gdbVar = result["gdb-result-var"].value;
            DebuggerEventData evt;
            evt.m_updateReason = DBG_UR_FUNCTIONFINISHED;
            evt.m_expression = gdbVar;
            m_observer->DebuggerUpdate(evt);
        }

    } else {
        // by default return control to program
        UpdateGotControl(DBG_UNKNOWN, func);
    }
    return true;
}

bool DbgCmdHandlerBp::ProcessOutput(const wxString& line)
{
    // parse the line, in case we have an error, keep this breakpoint in the queue
    if(line.StartsWith(wxT("^done"))) {
        // remove this breakpoint from the breakpoint list
        for(size_t i = 0; i < m_bplist->size(); i++) {
            clDebuggerBreakpoint b = m_bplist->at(i);
            if(b == m_bp) {
                m_bplist->erase(m_bplist->begin() + i);
                break;
            }
        }
    } else if(line.StartsWith(wxT("^error"))) {
        // Failed to place the breakpoint
        // Tell the bp manager to remove the bp from public view
        m_observer->UpdateBpAdded(m_bp.internal_id, -1);
        m_observer->UpdateAddLine(wxString::Format(_("ERROR: failed to place breakpoint: \"%s\""), line.c_str()), true);
        return true;
    }

    // so the breakpoint ID will come in form of
    // ^done,bkpt={number="2"....
    // ^done,wpt={number="2"
    static wxRegEx reBreak(wxT("done,bkpt={number=\"([0-9]+)\""));
    static wxRegEx reWatch(wxT("done,wpt={number=\"([0-9]+)\""));

    wxString number;
    long breakpointId(wxNOT_FOUND);

    if(reBreak.Matches(line)) {
        number = reBreak.GetMatch(line, 1);
        m_observer->UpdateAddLine(wxString::Format(_("Found the breakpoint ID!")), true);

    } else if(reWatch.Matches(line)) {
        number = reWatch.GetMatch(line, 1);
    }

    if(number.IsEmpty() == false) {
        if(number.ToLong(&breakpointId)) {
            // for debugging purpose
            m_observer->UpdateAddLine(wxString::Format(wxT("Storing debugger breakpoint Id=%ld"), breakpointId), true);
        }
    }

    m_observer->UpdateBpAdded(m_bp.internal_id, breakpointId);
    if(breakpointId == wxNOT_FOUND) {
        return true; // If the bp wasn't matched, the most likely reason is that bp creation failed. So don't say it
                     // worked
    }

    wxString msg;
    switch(m_bpType) {
    case BP_type_break:
        msg = wxString::Format(_("Successfully set breakpoint %ld at: "), breakpointId);
        break;
    case BP_type_condbreak:
        msg = wxString::Format(_("Successfully set conditional breakpoint %ld at: "), breakpointId);
        break;
    case BP_type_tempbreak:
        msg = wxString::Format(_("Successfully set temporary breakpoint %ld at: "), breakpointId);
        break;
    case BP_type_watchpt:
        switch(m_bp.watchpoint_type) {
        case WP_watch:
            msg = wxString::Format(_("Successfully set watchpoint %ld watching: "), breakpointId);
            break;
        case WP_rwatch:
            msg = wxString::Format(_("Successfully set read watchpoint %ld watching: "), breakpointId);
            break;
        case WP_awatch:
            msg = wxString::Format(_("Successfully set read/write watchpoint %ld watching: "), breakpointId);
            break;
        }
    }

    if(m_bpType == BP_type_watchpt) {
        msg << m_bp.watchpt_data;
    } else if(m_bp.memory_address.IsEmpty() == false) {
        msg << _("address ") << m_bp.memory_address;
    } else {
        if(!m_bp.file.IsEmpty()) {
            msg << m_bp.file << wxT(':');
        }
        if(!m_bp.function_name.IsEmpty()) {
            msg << m_bp.function_name;
        } else if(m_bp.lineno != -1) {
            msg << m_bp.lineno;
        }
    }

    m_observer->UpdateAddLine(msg);
    return true;
}

bool DbgCmdHandlerLocals::ProcessOutput(const wxString& line)
{
    // ^done,variables=[{name="result",value="{line_type = gdbmi::LT_EXEC_ASYNC_OUTPUT, line_type_context = {m_pdata =
    // 0x241a4e42cb1 \"stopped,reason=\\\"end-stepping-range\\\"\", m_length = 7}, txid = {m_pdata = 0x0, m_length = 0},
    // tree = std::shared_ptr<gdbmi::Node> (use count -1530317792, weak count 576) = {get() =
    // 0x241a4e43d50}}"},{name="argc",arg="1",value="1"},{name="argv",arg="1",value="0x241a4c91900"},{name="input_str",value="\"*stopped,reason=\\\"end-stepping-range\\\"\""},{name="input_str_2",value="\"00001547^done,variables=[{name=\\\"vt\\\",value=\\\"{first
    // = <error reading variable: Cannot access memory at address 0x1>\\\"},{name=\\\"__for_range\\\",value=\\\"<error
    // reading variable>\\\"},{name=\\\"__for_begin\\\",value=\\\"{ct\"..."},{name="parser",value="{<No data fields>}"}]
    LocalVariables locals;

    gdbmi::Parser parser;
    gdbmi::ParsedResult result;
    parser.parse(line, &result);

    // sanity
    if(result.line_type != gdbmi::LT_RESULT || result.line_type_context.to_string() != "done") {
        return false;
    }

    auto variables = result["variables"].children;
    if(!variables.empty()) {
        // no children
        locals.reserve(variables.size());
        for(size_t i = 0; i < variables.size(); ++i) {
            // each entry in the list is also represented as a list
            // with the index as its name
            LocalVariable var;
            var.name = (*variables[i])["name"].value;
            var.value = (*variables[i])["value"].value;
            if(var.value.empty()) {
                var.value = "{..}";
            }
            locals.push_back(var);
        }
    }

    m_observer->UpdateLocals(locals);

    // The new way of notifying: send a wx's event
    clCommandEvent evtLocals(wxEVT_DEBUGGER_QUERY_LOCALS);
    DebuggerEventData data;
    data.m_updateReason = DBG_UR_LOCALS;
    data.m_userReason = DBG_USERR_LOCALS;
    data.m_locals = locals;
    evtLocals.SetClientObject(new DebuggerEventData(data));
    EventNotifier::Get()->AddPendingEvent(evtLocals);
    return true;
}

bool DbgCmdHandlerVarCreator::ProcessOutput(const wxString& line)
{
    wxUnusedVar(line);
    return true;
}

bool DbgCmdHandlerEvalExpr::ProcessOutput(const wxString& line)
{
    // remove the ^done
    wxString tmpLine(line);
    line.StartsWith(wxT("^done,value=\""), &tmpLine);
    tmpLine.RemoveLast();
    wxString fixedStr = wxGdbFixValue(tmpLine);
    m_observer->UpdateExpression(m_expression, fixedStr);
    return true;
}

bool DbgCmdStackList::ProcessOutput(const wxString& line)
{
    // 00000372^done,stack=[frame={level="0",addr="0x00007ff77a9853b0",func="gdbmi::ParsedResult::operator[]",file="C:/src/gdbmi_parser/src/gdbmi.hpp",fullname="C:\\src\\gdbmi_parser\\src\\gdbmi.hpp",line="132",arch="i386:x86-64"},
    //                      frame={level="1",addr="0x00007ff77a9816ec",func="main",file="C:\\src\\gdbmi_parser\\src\\main.cpp",fullname="C:\\src\\gdbmi_parser\\src\\main.cpp",line="20",arch="i386:x86-64"}]

    gdbmi::Parser parser;
    gdbmi::ParsedResult result;

    parser.parse(line, &result);
    if(result["stack"].children.empty()) {
        return false;
    }

    const auto& stack = result["stack"];

    StackEntryArray stackArray;
    stackArray.reserve(stack.children.size());
    for(size_t i = 0; i < stack.children.size(); ++i) {
        StackEntry entry;
        ParseStackEntry(stack[i], entry);
        stackArray.push_back(entry);
    }

    // Send it as an event
    clCommandEvent evt(wxEVT_DEBUGGER_LIST_FRAMES);
    DebuggerEventData* data = new DebuggerEventData();
    data->m_stack.swap(stackArray);
    evt.SetClientObject(data);
    EventNotifier::Get()->AddPendingEvent(evt);
    return true;
}

bool DbgCmdSelectFrame::ProcessOutput(const wxString& line)
{
    wxUnusedVar(line);
    clCommandEvent evt(wxEVT_DEBUGGER_FRAME_SELECTED);
    EventNotifier::Get()->AddPendingEvent(evt);
    return true;
}

bool DbgCmdHandlerRemoteDebugging::ProcessOutput(const wxString& line)
{
    // We use this handler as a callback to indicate that gdb has connected to the debugger
    m_observer->UpdateRemoteTargetConnected(_("Successfully connected to debugger server"));

    // Apply the breakpoints
    m_observer->UpdateAddLine(_("Applying breakpoints..."));
    DbgGdb* gdb = dynamic_cast<DbgGdb*>(m_debugger);
    if(gdb) {
        gdb->SetBreakpoints();
    }
    m_observer->UpdateAddLine(_("Applying breakpoints... done"));
    // continue execution
    return m_debugger->Continue();
}

bool DbgCmdDisplayOutput::ProcessOutput(const wxString& line)
{
    // Hopefully, display whatever output gdb has generated, without pruning
    m_observer->UpdateAddLine(line);
    return true;
}

bool DbgCmdResolveTypeHandler::ProcessOutput(const wxString& line)
{
    wxString var_name;
    wxString type_name;
    wxString err_msg;

    gdbmi::Parser parser;
    gdbmi::ParsedResult result;
    parser.parse(line, &result);

    // parse the output
    // ^done,name="var2",numchild="1",value="{...}",type="orxAABOX"
    if(result.line_type != gdbmi::LT_RESULT && result.line_type_context.to_string() == "error") {
        err_msg = line.AfterFirst('=');
        err_msg.Prepend("GDB ERROR: ");

        clCommandEvent evt(wxEVT_DEBUGGER_TYPE_RESOLVE_ERROR);
        DebuggerEventData* data = new DebuggerEventData();
        data->m_expression = m_expression;
        data->m_text = err_msg;
        data->m_userReason = m_userReason;
        evt.SetClientObject(data);
        EventNotifier::Get()->AddPendingEvent(evt);
        return true;

    } else {
        var_name = result["name"].value;
        type_name = result["type"].value;
    }

    // delete the variable object
    wxString cmd;
    cmd << "-var-delete " << var_name;

    // since the above gdb command yields an output, we use the sync command
    // to get it as well to avoid errors in future calls to the gdb

    m_debugger->WriteCommand(cmd, NULL); // pass in NULL handler so the output of this command will be ignored

    // Update the observer
    DebuggerEventData e;
    e.m_updateReason = DBG_UR_TYPE_RESOLVED;
    e.m_userReason = m_userReason;
    e.m_expression = m_expression;
    e.m_evaluated = type_name;
    m_observer->DebuggerUpdate(e);
    return true;
}

DbgCmdResolveTypeHandler::DbgCmdResolveTypeHandler(const wxString& expression, DbgGdb* debugger, int userReason)
    : DbgCmdHandler(debugger->GetObserver())
    , m_debugger(debugger)
    , m_expression(expression)
    , m_userReason(userReason)
{
}

bool DbgCmdCLIHandler::ProcessOutput(const wxString& line)
{
    // nothing to be done
    return true;
}

bool DbgCmdGetTipHandler::ProcessOutput(const wxString& line)
{
    wxString evaluated = GetOutput();
    evaluated = evaluated.Trim().Trim(false);
    // gdb displays the variable name as $<NUMBER>,
    // we simply replace it with the actual string
    static wxRegEx reGdbVar(wxT("^\\$[0-9]+"));
    static wxRegEx reGdbVar2(wxT("\\$[0-9]+ = "));

    reGdbVar.ReplaceFirst(&evaluated, m_expression);
    reGdbVar2.ReplaceAll(&evaluated, wxEmptyString);

    wxString fixedString = wxGdbFixValue(evaluated);

    // Update the observer
    m_observer->UpdateAsciiViewer(m_expression, fixedString);
    return true;
}

// Set condition handler
bool DbgCmdSetConditionHandler::ProcessOutput(const wxString& line)
{
    wxString dbg_output(line);
    // If successful, the only output is ^done, so assume that means it worked
    if(dbg_output.Find(wxT("^done")) != wxNOT_FOUND) {
        if(m_bp.conditions.IsEmpty()) {
            m_observer->UpdateAddLine(wxString::Format(_("Breakpoint %i condition cleared"), (int)m_bp.debugger_id));
        } else {
            m_observer->UpdateAddLine(wxString::Format(_("Condition %s set for breakpoint %i"), m_bp.conditions.c_str(),
                                                       (int)m_bp.debugger_id));
        }
        return true;
    }
    return false;
}

// -break-list output handler
bool DbgCmdBreakList::ProcessOutput(const wxString& line)
{
    // ^done,BreakpointTable={nr_rows="1",nr_cols="6",hdr=[{width="7",alignment="-1",col_name="number",colhdr="Num"},{width="14",alignment="-1",col_name="type",colhdr="Type"},{width="4",alignment="-1",col_name="disp",colhdr="Disp"},{width="3",alignment="-1",col_name="enabled",colhdr="Enb"},{width="18",alignment="-1",col_name="addr",colhdr="Address"},{width="40",alignment="2",col_name="what",colhdr="What"}],body=[bkpt={number="1",type="breakpoint",disp="keep",enabled="y",addr="0x000000000000ac60",func="main(int,
    // char**)",file="/home/ANT.AMAZON.COM/eifrah/Documents/HelloWorld/HelloWorld/main.cpp",fullname="/home/ANT.AMAZON.COM/eifrah/Documents/HelloWorld/HelloWorld/main.cpp",line="292",thread-groups=["i1"],times="0",original-location="main"}]}
    gdbmi::Parser parser;
    gdbmi::ParsedResult result;
    parser.parse(line, &result);

    std::vector<clDebuggerBreakpoint> li;

    // sanity
    if(result.line_type != gdbmi::LT_RESULT || result.line_type_context.to_string() != "done") {
        return false;
    }

    const auto& body = result["BreakpointTable"]["body"].children;
    if(body.empty()) {
        return false;
    }

    li.reserve(body.size());
    // convert gdbmi breakpoint info into clDebuggerBreakpoint construct
    for(size_t i = 0; i < body.size(); i++) {
        clDebuggerBreakpoint breakpoint;
        auto& bkpt = *body[i];

        // "consume" the values by swapping them
        breakpoint.what.swap(bkpt["what"].value);
        breakpoint.at.swap(bkpt["at"].value);
        breakpoint.file = get_file_name(bkpt);

        wxString lineNumber = bkpt["line"].value;
        if(!lineNumber.empty()) {
            breakpoint.lineno = wxAtoi(lineNumber);
        }
        wxString ignore = bkpt["ignore"].value;
        if(!ignore.empty()) {
            breakpoint.ignore_number = wxAtoi(ignore);
        }

        wxString bpId = bkpt["number"].value;
        if(!bpId.empty()) {
            breakpoint.debugger_id = wxAtof(bpId);
        }
        li.push_back(breakpoint);
    }

    // Filter duplicate file:line breakpoints
    std::vector<clDebuggerBreakpoint> uniqueBreakpoints;
    uniqueBreakpoints.reserve(li.size());

    std::for_each(li.begin(), li.end(), [&](const clDebuggerBreakpoint& bp) {
        if(bp.debugger_id == bp.debugger_id) {
            // real breakpoint ID
            uniqueBreakpoints.push_back(bp);
        }
    });

    // Update the UI
    clDebugEvent event_breakpoints(wxEVT_DEBUG_BREAKPOINTS_LIST);
    event_breakpoints.SetSshAccount(m_gdb->GetSshAccount());
    event_breakpoints.SetIsSSHDebugging(m_gdb->IsSSHDebugging());
    event_breakpoints.GetBreakpoints().swap(uniqueBreakpoints);
    EventNotifier::Get()->AddPendingEvent(event_breakpoints);
    return true;
}

bool DbgCmdListThreads::ProcessOutput(const wxString& line)
{
    GdbMIThreadInfoParser parser;
    parser.Parse(line);
    DebuggerEventData e;
    const GdbMIThreadInfoVec_t& threads = parser.GetThreads();
    for(size_t i = 0; i < threads.size(); ++i) {
        e.m_threads.push_back(threads.at(i).ToThreadEntry());
    }

    // Notify the observer
    e.m_updateReason = DBG_UR_LISTTHRAEDS;
    m_observer->DebuggerUpdate(e);
    return true;
}

bool DbgCmdWatchMemory::ProcessOutput(const wxString& line)
{
    DebuggerEventData e;

    // 00000016^done,addr="0x0000004a789ff8f0",nr-bytes="32",total-bytes="32",next-row="0x0000004a789ff900",prev-row="0x0000004a789ff8e0",next-page="0x0000004a789ff910",prev-page="0x0000004a789ff8d0",
    // memory=[{addr="0x0000004a789ff8f0",data=["0x00","0x00","0x00","0x00","0xd7","0x01","0x00","0x00","0x19","0x2d","0xa4","0x9d","0xd7","0x01","0x00","0x00"],ascii="?????????-??????"},{addr="0x0000004a789ff900",data=["0x04","0x00","0x00","0x00","0x00","0x00","0x00","0x00","0x10","0x2d","0xa4","0x9d","0xd7","0x01","0x00","0x00"],ascii="?????????-??????"}]
    gdbmi::ParsedResult result;
    gdbmi::Parser parser;
    parser.parse(line, &result);

    wxString output;
    wxString current_line;
    size_t rows_count = result["memory"].children.size();
    if(rows_count) {
        for(size_t i = 0; i < rows_count; ++i) {
            const auto& row = result["memory"][i];
            current_line << row["addr"].value << " ";

            // add the data
            size_t data_size = row["data"].children.size();
            for(size_t x = 0; x < data_size; ++x) {
                current_line << row["data"][x].value << " ";
            }

            if(row.exists("ascii")) {
                current_line << row["ascii"].value;
            }
            output << current_line << "\n";
            current_line.clear();
        }
        output.RemoveLast(); // remove the trailing \n
    }

    e.m_updateReason = DBG_UR_WATCHMEMORY;
    e.m_evaluated = output;
    e.m_expression = m_address;
    m_observer->DebuggerUpdate(e);
    return true;
}

bool DbgCmdCreateVarObj::ProcessOutput(const wxString& line)
{
    DebuggerEventData e;

    if(line.StartsWith(wxT("^error"))) {
        // Notify the observer we failed to create variable object
        e.m_updateReason = DBG_UR_VARIABLEOBJCREATEERR; // failed to create variable object
        e.m_expression = m_expression;                  // the variable object expression
        e.m_userReason = m_userReason;
        m_observer->DebuggerUpdate(e);
        return true;
    }

    // Variable object was created
    // Output sample:
    // ^done,name="var1",numchild="2",value="{...}",type="ChildClass",thread-id="1",has_more="0"
    GdbChildrenInfo info;
    gdbParseListChildren(line.mb_str(wxConvUTF8).data(), info);

    if(info.children.empty() == false) {
        std::map<std::string, std::string> attr = info.children.at(0);
        VariableObject vo;
        std::map<std::string, std::string>::const_iterator iter;

        iter = attr.find("name");
        if(iter != attr.end()) {
            vo.gdbId = wxString(iter->second.c_str(), wxConvUTF8);
            wxRemoveQuotes(vo.gdbId);
        }

        iter = attr.find("numchild");
        if(iter != attr.end()) {
            if(iter->second.empty() == false) {
                wxString numChilds(iter->second.c_str(), wxConvUTF8);
                wxRemoveQuotes(numChilds);
                vo.numChilds = wxAtoi(numChilds);
            }
        }

        // For primitive types, we also get the value
        iter = attr.find("value");
        if(iter != attr.end()) {

            if(iter->second.empty() == false) {
                wxString v(iter->second.c_str(), wxConvUTF8);
                wxRemoveQuotes(v);
                wxString val = wxGdbFixValue(v);
                if(val.IsEmpty() == false) {
                    e.m_evaluated = val;
                }
            }
        }

        iter = attr.find("type");
        if(iter != attr.end()) {
            if(iter->second.empty() == false) {
                wxString t(iter->second.c_str(), wxConvUTF8);
                wxRemoveQuotes(t);
                vo.typeName = t;
            }

            if(vo.typeName.EndsWith(wxT(" *"))) {
                vo.isPtr = true;
            }

            if(vo.typeName.EndsWith(wxT(" **"))) {
                vo.isPtrPtr = true;
            }
        }

        vo.has_more = info.has_more;

        if(vo.gdbId.IsEmpty() == false) {

            e.m_updateReason = DBG_UR_VARIABLEOBJ;
            e.m_variableObject = vo;
            e.m_expression = m_expression;
            e.m_userReason = m_userReason;
            m_observer->DebuggerUpdate(e);

            clCommandEvent evtCreate(wxEVT_DEBUGGER_VAROBJECT_CREATED);
            evtCreate.SetClientObject(new DebuggerEventData(e));
            EventNotifier::Get()->AddPendingEvent(evtCreate);
        }
    }
    return true;
}

static VariableObjChild FromParserOutput(const gdbmi::Node& child)
{
    VariableObjChild var_child;

    var_child.varName = child["exp"].value;
    var_child.type = child["type"].value;
    var_child.gdbId = child["name"].value;
    wxString numChilds = child["numchild"].value;
    wxString dynamic = child["dynamic"].value;

    if(numChilds.IsEmpty() == false) {
        var_child.numChilds = wxAtoi(numChilds);
    }

    if(var_child.numChilds == 0 && dynamic == "1") {
        var_child.numChilds = 1;
    }

    // child.varName = ExtractGdbChild(attr, wxT("exp"));
    // if(child.varName.IsEmpty() || child.type == child.varName ||
    //    (child.varName == wxT("public") || child.varName == wxT("private") || child.varName == wxT("protected"))
    //    || (child.type.Contains(wxT("class ")) || child.type.Contains(wxT("struct ")))) {
    //
    //     child.isAFake = true;
    // }

    // For primitive types, we also get the value
    var_child.value = child["value"].value;
    if(!var_child.value.empty()) {
        var_child.varName << " = " << var_child.value;
    }
    return var_child;
}

bool DbgCmdListChildren::ProcessOutput(const wxString& line)
{
    DebuggerEventData e;
    gdbmi::Parser parser;
    gdbmi::ParsedResult result;

    parser.parse(line, &result);

    if(result.line_type != gdbmi::LT_RESULT || result.line_type_context.to_string() != "done") {
        return false;
    }

    const auto& children = result["children"].children;
    if(children.empty()) {
        return true;
    }

    e.m_varObjChildren.reserve(children.size());

    // Convert the parser output to codelite data structure
    for(size_t i = 0; i < children.size(); ++i) {
        e.m_varObjChildren.push_back(FromParserOutput(*children[i]));
    }

    e.m_updateReason = DBG_UR_LISTCHILDREN;
    e.m_expression = m_variable;
    e.m_userReason = m_userReason;
    m_observer->DebuggerUpdate(e);

    clCommandEvent evtList(wxEVT_DEBUGGER_LIST_CHILDREN);
    evtList.SetClientObject(new DebuggerEventData(e));
    EventNotifier::Get()->AddPendingEvent(evtList);
    return true;
}

bool DbgCmdEvalVarObj::ProcessOutput(const wxString& line)
{
    gdbmi::ParsedResult result;
    gdbmi::Parser parser;
    parser.parse(line, &result);

    wxString display_line = result["value"].value;

    if(!display_line.empty()) {
        if(m_userReason == DBG_USERR_WATCHTABLE || display_line != "{...}") {
            DebuggerEventData e;
            e.m_updateReason = DBG_UR_EVALVARIABLEOBJ;
            e.m_expression = m_variable;
            e.m_evaluated = display_line;
            e.m_userReason = m_userReason;
            m_observer->DebuggerUpdate(e);

            clCommandEvent evtList(wxEVT_DEBUGGER_VAROBJ_EVALUATED);
            evtList.SetClientObject(new DebuggerEventData(e));
            EventNotifier::Get()->AddPendingEvent(evtList);
        }
    }
    return true;
}

bool DbgFindMainBreakpointIdHandler::ProcessOutput(const wxString& line)
{
    // so the breakpoint ID will come in form of
    // ^done,bkpt={number="2"....
    static wxRegEx reBreak(wxT("done,bkpt={number=\"([0-9]+)\""));
    wxString number;
    long breakpointId(wxNOT_FOUND);

    reBreak.Matches(line);
    number = reBreak.GetMatch(line, 1);
    if(number.IsEmpty() == false) {
        if(number.ToLong(&breakpointId)) {
            // for debugging purpose
            m_observer->UpdateAddLine(wxString::Format(wxT("Storing internal breakpoint ID=%ld"), breakpointId), true);
            m_debugger->SetInternalMainBpID(breakpointId);
        }
    }
    return true;
}

bool DbgCmdHandlerStackDepth::ProcessOutput(const wxString& line)
{
    DebuggerEventData e;
    long frameLevel(-1);
    static wxRegEx reFrameDepth(wxT("depth=\"([0-9]+)\""));
    if(reFrameDepth.Matches(line)) {
        wxString strFrameDepth = reFrameDepth.GetMatch(line, 1);
        if(strFrameDepth.ToLong(&frameLevel) && frameLevel != -1) {
            e.m_updateReason = DBG_UR_FRAMEDEPTH;
            e.m_frameInfo.level = strFrameDepth;
            m_observer->DebuggerUpdate(e);
        }
    }
    return true;
}

bool DbgVarObjUpdate::ProcessOutput(const wxString& line)
{
    DebuggerEventData e;

    if(line.StartsWith(wxT("^error"))) {
        // Notify the observer we failed to create variable object
        e.m_updateReason = DBG_UR_VARIABLEOBJUPDATEERR; // failed to create variable object
        e.m_expression = m_variableName;                // the variable object expression
        e.m_userReason = m_userReason;
        m_observer->DebuggerUpdate(e);
        return false; // let the default loop to handle this as well by passing DBG_CMD_ERR to the observer
    }

    std::string cbuffer = line.mb_str(wxConvUTF8).data();
    GdbChildrenInfo info;
    gdbParseListChildren(cbuffer, info);

    for(size_t i = 0; i < info.children.size(); i++) {
        wxString name = ExtractGdbChild(info.children.at(i), wxT("name"));
        wxString in_scope = ExtractGdbChild(info.children.at(i), wxT("in_scope"));
        wxString type_changed = ExtractGdbChild(info.children.at(i), wxT("type_changed"));
        if(in_scope == wxT("false") || type_changed == wxT("true")) {
            e.m_varObjUpdateInfo.removeIds.Add(name);

        } else if(in_scope == wxT("true")) {
            e.m_varObjUpdateInfo.refreshIds.Add(name);
        }
    }
    e.m_updateReason = DBG_UR_VAROBJUPDATE;
    e.m_expression = m_variableName;
    e.m_userReason = m_userReason;
    m_observer->DebuggerUpdate(e);
    return true;
}

bool DbgCmdHandlerExecRun::ProcessOutput(const wxString& line)
{
    if(line.StartsWith(wxT("^error"))) {
        // ^error,msg="..."
        wxString errmsg = line.Mid(11); // skip |^error,msg=| (11 chars)
        errmsg.Replace(wxT("\\\""), wxT("\""));
        errmsg.Replace(wxT("\\n"), wxT("\n"));

        // exec-run failed, notify about it
        DebuggerEventData e;
        e.m_updateReason = DBG_UR_GOT_CONTROL;
        e.m_controlReason = DBG_EXIT_WITH_ERROR;
        e.m_text = errmsg;
        m_observer->DebuggerUpdate(e);
        return true;

    } else {
        return DbgCmdHandlerAsyncCmd::ProcessOutput(line);
    }
}

bool DbgCmdJumpHandler::ProcessOutput(const wxString& line)
{
    // TODO: handle questions like "Line 24 is not in `main(int, char**)'.  Jump anyway? (y or n) "
    return false;
}

bool DbgCmdStopHandler::ProcessOutput(const wxString& line)
{
    wxUnusedVar(line);
    wxCommandEvent event(wxEVT_GDB_STOP_DEBUGGER);
    EventNotifier::Get()->AddPendingEvent(event);
    return true;
}

bool DbgCmdHandlerDisasseble::ProcessOutput(const wxString& line)
{
    clCommandEvent event(wxEVT_DEBUGGER_DISASSEBLE_OUTPUT);
    GdbChildrenInfo info;
    ::gdbParseListChildren(line.mb_str(wxConvUTF8).data(), info);

    DebuggerEventData* evtData = new DebuggerEventData();
    for(size_t i = 0; i < info.children.size(); ++i) {

        DisassembleEntry entry;

        GdbStringMap_t& attrs = info.children.at(i);
        if(attrs.count("address")) {
            entry.m_address = attrs["address"].c_str();
            wxGDB_STRIP_QUOATES(entry.m_address);
        }

        if(attrs.count("inst")) {
            entry.m_inst = attrs["inst"].c_str();
            wxGDB_STRIP_QUOATES(entry.m_inst);
        }

        if(attrs.count("func-name")) {
            entry.m_function = attrs["func-name"].c_str();
            wxGDB_STRIP_QUOATES(entry.m_function);
        }

        if(attrs.count("offset")) {
            entry.m_offset = attrs["offset"].c_str();
            wxGDB_STRIP_QUOATES(entry.m_offset);
        }
        evtData->m_disassembleLines.push_back(entry);
    }

    event.SetClientObject(evtData);
    EventNotifier::Get()->AddPendingEvent(event);
    return true;
}

bool DbgCmdHandlerDisassebleCurLine::ProcessOutput(const wxString& line)
{
    clCommandEvent event(wxEVT_DEBUGGER_DISASSEBLE_CURLINE);
    GdbChildrenInfo info;
    ::gdbParseListChildren(line.mb_str(wxConvUTF8).data(), info);

    DebuggerEventData* evtData = new DebuggerEventData();
    if(info.children.empty() == false) {

        DisassembleEntry entry;
        GdbStringMap_t& attrs = info.children.at(0);
        if(attrs.count("address")) {
            entry.m_address = attrs["address"].c_str();
            wxGDB_STRIP_QUOATES(entry.m_address);
        }

        if(attrs.count("inst")) {
            entry.m_inst = attrs["inst"].c_str();
            wxGDB_STRIP_QUOATES(entry.m_inst);
        }

        if(attrs.count("func-name")) {
            entry.m_function = attrs["func-name"].c_str();
            wxGDB_STRIP_QUOATES(entry.m_function);
        }

        if(attrs.count("offset")) {
            entry.m_offset = attrs["offset"].c_str();
            wxGDB_STRIP_QUOATES(entry.m_offset);
        }
        evtData->m_disassembleLines.push_back(entry);
    }

    event.SetClientObject(evtData);
    EventNotifier::Get()->AddPendingEvent(event);
    return true;
}

// +++-----------------------------
// DbgCmdHandlerRegisterNames
// +++-----------------------------

bool DbgCmdHandlerRegisterNames::ProcessOutput(const wxString& line)
{
    // Sample output:
    // ^done,register-names=["eax","ecx","edx","ebx","esp","ebp","esi","edi","eip","eflags","cs","ss","ds","es","fs","gs","st0","st1","st2","st3","st4","st5","st6","st7","fctrl","fstat","ftag","fiseg","fioff","foseg","fooff","fop","xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7","mxcsr","","","","","","","","","al","cl","dl","bl","ah","ch","dh","bh","ax","cx","dx","bx","","bp","si","di","mm0","mm1","mm2","mm3","mm4","mm5","mm6","mm7"]
    const wxCharBuffer scannerText = _C(line);
    setGdbLexerInput(scannerText.data(), true);

    int type;
    wxString cmd, var_name;
    wxString type_name, currentToken;
    wxString err_msg;

    int counter = 0;
    m_numberToName.clear();
    // ^done,register-values=[{number="0",value="4195709"},...]
    if(line.StartsWith("^done")) {
        // ^done,register-values=[
        GDB_NEXT_TOKEN(); // ^
        GDB_NEXT_TOKEN(); // done
        GDB_NEXT_TOKEN(); // ,
        GDB_NEXT_TOKEN(); // register-names
        GDB_NEXT_TOKEN(); // =
        GDB_NEXT_TOKEN(); // [

        while(true) {
            wxString reg_name;
            GDB_NEXT_TOKEN();
            if(type == 0) {
                // EOF?
                break;
            }
            reg_name = currentToken;
            wxGDB_STRIP_QUOATES(reg_name);

            // Don't include empty register names
            if(!reg_name.IsEmpty()) {
                m_numberToName.insert(std::make_pair(counter, reg_name));
            }
            GDB_NEXT_TOKEN(); // remove the ','
            if(type != ',') {
                // end of the list
                break;
            }
            ++counter;
        }
    }
    gdb_result_lex_clean();
    wxString command = "-data-list-register-values N";
    return m_gdb->WriteCommand(command, new DbgCmdHandlerRegisterValues(m_observer, m_gdb, m_numberToName));
}

// +++-----------------------------
// DbgCmdHandlerRegisterValues
// +++-----------------------------
bool DbgCmdHandlerRegisterValues::ProcessOutput(const wxString& line)
{
    // Process the output and report it back to codelite
    clCommandEvent event(wxEVT_DEBUGGER_LIST_REGISTERS);
    DbgRegistersVec_t registers;

    const wxCharBuffer scannerText = _C(line);
    setGdbLexerInput(scannerText.data(), true);
    int type;
    wxString cmd, var_name;
    wxString type_name, currentToken;
    wxString err_msg;

    // ^done,register-values=[{number="0",value="4195709"},...]
    if(line.StartsWith("^done")) {
        DebuggerEventData* data = new DebuggerEventData();

        // ^done,register-values=[
        GDB_NEXT_TOKEN(); // ^
        GDB_NEXT_TOKEN(); // done
        GDB_NEXT_TOKEN(); // ,
        GDB_NEXT_TOKEN(); // register-values
        GDB_NEXT_TOKEN(); // =
        GDB_NEXT_TOKEN(); // [

        while(true) {
            DbgRegister reg;

            GDB_NEXT_TOKEN(); // {
            GDB_ABORT('{');
            GDB_NEXT_TOKEN(); // number
            GDB_NEXT_TOKEN(); // =
            GDB_NEXT_TOKEN(); // "0"

            long regId = 0;

            wxGDB_STRIP_QUOATES(currentToken);
            currentToken.ToCLong(&regId);

            // find this register in the map
            std::map<int, wxString>::iterator iter = m_numberToName.find(regId);
            if(iter != m_numberToName.end()) {
                reg.reg_name = iter->second;
            }

            GDB_NEXT_TOKEN(); // ,
            GDB_NEXT_TOKEN(); // value
            GDB_NEXT_TOKEN(); // =
            GDB_NEXT_TOKEN(); // "..."
            reg.reg_value = currentToken;
            wxGDB_STRIP_QUOATES(reg.reg_value);

            // Add the register
            if(!reg.reg_name.IsEmpty()) {
                registers.push_back(reg);
            }

            // Read the next token
            GDB_NEXT_TOKEN(); // }
            GDB_NEXT_TOKEN();
            wxGDB_STRIP_QUOATES(currentToken);
            if(currentToken != ",") {
                // no more registers
                break;
            }

            if(type == 0) {
                // EOF?
                break;
            }
        }

        data->m_registers = registers;
        event.SetClientObject(data);
        EventNotifier::Get()->AddPendingEvent(event);
    }
    gdb_result_lex_clean();
    return true;
}

bool DbgCmdRecordHandler::ProcessOutput(const wxString& line)
{
    // parse the line, in case we have an error, keep this breakpoint in the queue
    if(line.StartsWith(wxT("^done"))) {
        m_gdb->SetIsRecording(true);
    } else if(line.StartsWith(wxT("^error"))) {
        m_gdb->SetIsRecording(false);
    }
    return true;
}
