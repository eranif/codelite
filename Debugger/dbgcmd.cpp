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
#include "precompiled_header.h"
#include "dbgcmd.h"
#include "event_notifier.h"
#include "wx/tokenzr.h"
#include "debuggergdb.h"
#include "gdb_result_parser.h"
#include <wx/regex.h>
#include "gdb_parser_incl.h"
#include "procutils.h"
#include "gdbmi_parse_thread_info.h"
#include "event_notifier.h"
#include "debuggermanager.h"
#include "cl_command_event.h"
#include <algorithm>

static bool IS_WINDOWNS = (wxGetOsVersion() & wxOS_WINDOWS);

#define GDB_LEX()                         \
    {                                     \
        type = gdb_result_lex();          \
        currentToken = gdb_result_string; \
    }

#define GDB_BREAK(ch)     \
    if(type != (int)ch) { \
        break;            \
    }

static void wxGDB_STRIP_QUOATES(wxString& currentToken)
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

static void wxRemoveQuotes(wxString& str)
{
    if(str.IsEmpty()) {
        return;
    }

    str.RemoveLast();
    if(str.IsEmpty() == false) {
        str.Remove(0, 1);
    }
}

static wxString wxGdbFixValue(const wxString& value)
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

static wxString NextValue(wxString& line, wxString& key)
{
    // extract the key name first
    if(line.StartsWith(wxT(","))) {
        line.Remove(0, 1);
    }

    key = line.BeforeFirst(wxT('='));
    line = line.AfterFirst(wxT('"'));
    wxString token;
    bool cont(true);

    while(!line.IsEmpty() && cont) {
        wxChar ch = line.GetChar(0);
        line.Remove(0, 1);

        if(ch == wxT('"')) {
            cont = false;
        } else {
            token << ch;
        }
    }
    return token;
}

static void ParseStackEntry(const wxString& line, StackEntry& entry)
{
    wxString tmp(line);
    wxString key, value;

    value = NextValue(tmp, key);
    while(value.IsEmpty() == false) {
        if(key == wxT("level")) {
            entry.level = value;
        } else if(key == wxT("addr")) {
            entry.address = value;
        } else if(key == wxT("func")) {
            entry.function = value;
        } else if(key == wxT("file")) {
            entry.file = value;
        } else if(key == wxT("line")) {
            entry.line = value;
        } else if(key == wxT("fullname")) {
            entry.file = value;
        }
        value = NextValue(tmp, key);
    }
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
static std::map<wxString, wxString> g_fileCache;

bool DbgCmdHandlerGetLine::ProcessOutput(const wxString& line)
{
#if defined(__WXGTK__) || defined(__WXMAC__)
    // Output from "-stack-info-frame"
    //^done,frame={level="0",addr="0x000000000043b227",func="MyClass::DoFoo",file="./Foo.cpp",fullname="/full/path/to/Foo.cpp",line="30"}

    wxString tmpLine;
    line.StartsWith(wxT("^done,frame={"), &tmpLine);
    tmpLine.RemoveLast();
    if(tmpLine.IsEmpty()) {
        return false;
    }

    StackEntry entry;
    ParseStackEntry(tmpLine, entry);

    long line_number;
    entry.line.ToLong(&line_number);
    m_observer->UpdateFileLine(entry.file, line_number);

    clCommandEvent evtFileLine(wxEVT_DEBUGGER_QUERY_FILELINE);
    DebuggerEventData* ded = new DebuggerEventData;
    ded->m_file = entry.file;
    ded->m_line = line_number;
    evtFileLine.SetClientObject(ded);
    EventNotifier::Get()->AddPendingEvent(evtFileLine);

#else
    // Output of -file-list-exec-source-file
    //^done,line="36",file="a.cpp",fullname="C:/testbug1/a.cpp"
    //^done,line="2",file="main.cpp",fullname="/Users/eran/main.cpp"

    // By default we use the 'fullname' as our file name. however,
    // if we are under Windows and the fullname contains the string '/cygdrive'
    // we fallback to use the 'filename' since cygwin gdb report fullname in POSIX / Cygwin paths
    // which can not be used by codelite
    wxString strLine, fullName, filename;
    wxStringTokenizer tkz(line, wxT(","));
    if(tkz.HasMoreTokens()) {
        // skip first
        tkz.NextToken();
    }
    // line=
    if(tkz.HasMoreTokens()) {
        strLine = tkz.NextToken();
    } else {
        return false;
    }
    // file=
    if(tkz.HasMoreTokens()) {
        filename = tkz.NextToken();
    }

    // fullname=
    if(tkz.HasMoreTokens()) {
        wxString tmpfull_name = tkz.NextToken();
        if(tmpfull_name.StartsWith("fullname")) {
            fullName = tmpfull_name;
        }
    } else {
        return false;
    }

    // line="36"
    strLine = strLine.AfterFirst(wxT('"'));
    strLine = strLine.BeforeLast(wxT('"'));
    long lineno;
    strLine.ToLong(&lineno);

    // remove quotes
    fullName = fullName.AfterFirst(wxT('"'));
    fullName = fullName.BeforeLast(wxT('"'));
    fullName.Replace(wxT("\\\\"), wxT("\\"));
    fullName.Trim().Trim(false);

    if(fullName.StartsWith(wxT("/"))) {
        // fallback to use file="<..>"
        filename = filename.AfterFirst(wxT('"'));
        filename = filename.BeforeLast(wxT('"'));
        filename.Replace(wxT("\\\\"), wxT("\\"));

        filename.Trim().Trim(false);
        fullName = filename;

        // FIXME: change cypath => fullName
        if(fullName.StartsWith(wxT("/"))) {

            if(g_fileCache.find(fullName) != g_fileCache.end()) {
                fullName = g_fileCache.find(fullName)->second;

            } else {

                // file attribute also contains cygwin path
                wxString cygwinConvertPath = m_gdb->GetDebuggerInformation().cygwinPathCommand;
                cygwinConvertPath.Trim().Trim(false);
                if(!cygwinConvertPath.IsEmpty()) {
                    // we got a conversion command from the user, use it
                    cygwinConvertPath.Replace(wxT("$(File)"), wxString::Format(wxT("%s"), fullName.c_str()));
                    wxArrayString cmdOutput;
                    ProcUtils::SafeExecuteCommand(cygwinConvertPath, cmdOutput);
                    if(cmdOutput.IsEmpty() == false) {
                        cmdOutput.Item(0).Trim().Trim(false);
                        wxString convertedPath = cmdOutput.Item(0);

                        // Keep the file in the cache ( regardless of the validity of the result)
                        g_fileCache[fullName] = convertedPath;

                        // if the convertedPath does exists on the disk,
                        // replace the file name with it
                        if(wxFileName::FileExists(convertedPath)) {
                            fullName = convertedPath;
                        }
                    }
                }
            }
        }
    }
    m_observer->UpdateFileLine(fullName, lineno);

    clCommandEvent evtFileLine(wxEVT_DEBUGGER_QUERY_FILELINE);
    DebuggerEventData* ded = new DebuggerEventData;
    ded->m_file = fullName;
    ded->m_line = lineno;
    evtFileLine.SetClientObject(ded);
    EventNotifier::Get()->AddPendingEvent(evtFileLine);

#endif
    return true;
}

void DbgCmdHandlerAsyncCmd::UpdateGotControl(DebuggerReasons reason, const wxString& func)
{
    m_observer->UpdateGotControl(reason, func);
}

bool DbgCmdHandlerAsyncCmd::ProcessOutput(const wxString& line)
{
    wxString reason;
    //*stopped,reason="end-stepping-range",thread-id="1",frame={addr="0x0040156b",func="main",args=[{name="argc",value="1"},{name="argv",value="0x3e2c50"}],file="a.cpp",line="46"}
    // when reason is "end-stepping-range", it means that one of the following command was
    // completed:
    //-exec-step, -exec-stepi
    //-exec-next, -exec-nexti

    // try and get the debugee PID
    m_gdb->GetDebugeePID(line);

    // Get the reason
    GdbChildrenInfo info;
    gdbParseListChildren(line.mb_str(wxConvUTF8).data(), info);

    wxString func;
    bool foundReason;

    foundReason = false;
    for(size_t i = 0; i < info.children.size(); i++) {
        std::map<std::string, std::string> attr = info.children.at(i);
        std::map<std::string, std::string>::const_iterator iter;

        iter = attr.find("reason");
        if(!foundReason && iter != attr.end()) {
            foundReason = true;
            reason = wxString(iter->second.c_str(), wxConvUTF8);
            wxRemoveQuotes(reason);
        }

        if(foundReason) break;
    }

    if(reason.IsEmpty()) return false;

    int where = line.Find(wxT("func=\""));
    if(where != wxNOT_FOUND) {
        func = line.Mid(where + 6);
        func = func.BeforeFirst(wxT('"'));
    }

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
                    const std::vector<BreakpointInfo>& bpList = m_gdb->GetBpList();
                    std::vector<BreakpointInfo>::const_iterator iter = bpList.begin();
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

    } else if(reason == wxT("signal-received") && !m_gdb->IsGoingDown()) {

        // got signal
        // which signal?
        wxString signame;
        int where = line.Find(wxT("signal-name"));
        if(where != wxNOT_FOUND) {
            signame = line.Mid(where);
            signame = signame.AfterFirst(wxT('"'));
            signame = signame.BeforeFirst(wxT('"'));
        }

        if(signame == wxT("SIGSEGV")) {
            UpdateGotControl(DBG_RECV_SIGNAL_SIGSEGV, func);

        } else if(signame == wxT("EXC_BAD_ACCESS")) {
            UpdateGotControl(DBG_RECV_SIGNAL_EXC_BAD_ACCESS, func);

        } else if(signame == wxT("SIGABRT")) {
            UpdateGotControl(DBG_RECV_SIGNAL_SIGABRT, func);

        } else if(signame == wxT("SIGTRAP")) {
            UpdateGotControl(DBG_RECV_SIGNAL_SIGTRAP, func);

        } else {
            // default
            UpdateGotControl(DBG_RECV_SIGNAL, func);
        }
    } else if(reason == wxT("exited-normally") || reason == wxT("exited")) {
        m_observer->UpdateAddLine(_("Program exited normally."));

        // debugee program exit normally
        UpdateGotControl(DBG_EXITED_NORMALLY, func);

    } else if(reason == wxT("function-finished")) {

        // debugee program exit normally
        UpdateGotControl(DBG_FUNC_FINISHED, func);

        // We finished an execution of a function.
        // Return to the caller the gdb-result-var since we might want
        // to create a variable object out of it
        wxString gdbVar;
        int where = line.Find(wxT("gdb-result-var"));
        if(where != wxNOT_FOUND) {

            gdbVar = line.Mid(where + 14);
            gdbVar = gdbVar.AfterFirst(wxT('"'));
            gdbVar = gdbVar.BeforeFirst(wxT('"'));

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
            BreakpointInfo b = m_bplist->at(i);
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
    LocalVariables locals;

    GdbChildrenInfo info;
    gdbParseListChildren(line.mb_str(wxConvUTF8).data(), info);

    for(size_t i = 0; i < info.children.size(); i++) {
        std::map<std::string, std::string> attr = info.children.at(i);
        LocalVariable var;
        std::map<std::string, std::string>::const_iterator iter;

        iter = attr.find("name");
        if(iter != attr.end()) {
            var.name = wxString(iter->second.c_str(), wxConvUTF8);
            wxRemoveQuotes(var.name);
        }

        iter = attr.find("exp");
        if(iter != attr.end()) {
            // We got exp? are we on Mac!!??
            // Anyways, replace exp with name and keep name as gdbId
            var.gdbId = var.name;
            var.name = wxString(iter->second.c_str(), wxConvUTF8);
            wxRemoveQuotes(var.name);
        }

        // For primitive types, we also get the value
        iter = attr.find("value");
        if(iter != attr.end()) {
            if(iter->second.empty() == false) {
                wxString v(iter->second.c_str(), wxConvUTF8);
                wxRemoveQuotes(v);
                var.value = wxGdbFixValue(v);
            }
        }

        var.value.Trim().Trim(false);
        if(var.value.IsEmpty()) {
            var.value = wxT("{...}");
        }

        iter = attr.find("type");
        if(iter != attr.end()) {
            if(iter->second.empty() == false) {
                wxString t(iter->second.c_str(), wxConvUTF8);
                wxRemoveQuotes(t);
                var.type = t;
            }
        }

        locals.push_back(var);
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

bool DbgCmdHandlerFuncArgs::ProcessOutput(const wxString& line)
{
    LocalVariables locals;

    GdbChildrenInfo info;
    gdbParseListChildren(line.mb_str(wxConvUTF8).data(), info);

    for(size_t i = 0; i < info.children.size(); i++) {
        std::map<std::string, std::string> attr = info.children.at(i);
        LocalVariable var;
        std::map<std::string, std::string>::const_iterator iter;

        iter = attr.find("name");
        if(iter != attr.end()) {
            var.name = wxString(iter->second.c_str(), wxConvUTF8);
            wxRemoveQuotes(var.name);
        }

        iter = attr.find("exp");
        if(iter != attr.end()) {
            // We got exp? are we on Mac!!??
            // Anyways, replace exp with name and keep name as gdbId
            var.gdbId = var.name;
            var.name = wxString(iter->second.c_str(), wxConvUTF8);
            wxRemoveQuotes(var.name);
        }

        // For primitive types, we also get the value
        iter = attr.find("value");
        if(iter != attr.end()) {
            if(iter->second.empty() == false) {
                wxString v(iter->second.c_str(), wxConvUTF8);
                wxRemoveQuotes(v);
                var.value = wxGdbFixValue(v);
            }
        }

        var.value.Trim().Trim(false);
        if(var.value.IsEmpty()) {
            var.value = wxT("{...}");
        }

        iter = attr.find("type");
        if(iter != attr.end()) {
            if(iter->second.empty() == false) {
                wxString t(iter->second.c_str(), wxConvUTF8);
                wxRemoveQuotes(t);
                var.type = t;
            }
        }

        locals.push_back(var);
    }
    m_observer->UpdateFunctionArguments(locals);
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
    wxString tmpLine(line);
    line.StartsWith(wxT("^done,stack=["), &tmpLine);

    tmpLine = tmpLine.Trim();
    tmpLine = tmpLine.Trim(false);

    tmpLine.RemoveLast();
    //--------------------------------------------------------
    // tmpLine contains now string with the following format:
    // frame={name="Value",...},frame={name="Value",...}
    wxString remainder(tmpLine);
    StackEntryArray stackArray;
    while(true) {
        tmpLine = tmpLine.AfterFirst(wxT('{'));
        if(tmpLine.IsEmpty()) {
            break;
        }

        remainder = tmpLine.AfterFirst(wxT('}'));
        tmpLine = tmpLine.BeforeFirst(wxT('}'));

        StackEntry entry;
        ParseStackEntry(tmpLine, entry);
        stackArray.push_back(entry);

        tmpLine = remainder;
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
    const wxCharBuffer scannerText = _C(line);
    setGdbLexerInput(scannerText.data(), true);

    int type;
    wxString cmd, var_name;
    wxString type_name, currentToken;
    wxString err_msg;
    // parse the output
    // ^done,name="var2",numchild="1",value="{...}",type="orxAABOX"
    if(line.StartsWith("^error")) {
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

        do {
            // ^done
            GDB_NEXT_TOKEN();
            GDB_ABORT('^');
            GDB_NEXT_TOKEN();
            GDB_ABORT(GDB_DONE);

            // ,name="..."
            GDB_NEXT_TOKEN();
            GDB_ABORT(',');
            GDB_NEXT_TOKEN();
            GDB_ABORT(GDB_NAME);
            GDB_NEXT_TOKEN();
            GDB_ABORT('=');
            GDB_NEXT_TOKEN();
            GDB_ABORT(GDB_STRING);
            var_name = currentToken;

            // ,numchild="..."
            GDB_NEXT_TOKEN();
            GDB_ABORT(',');
            GDB_NEXT_TOKEN();
            GDB_ABORT(GDB_NUMCHILD);
            GDB_NEXT_TOKEN();
            GDB_ABORT('=');
            GDB_NEXT_TOKEN();
            GDB_ABORT(GDB_STRING);
// On Mac this part does not seem to be reported by GDB
#ifndef __WXMAC__
            // ,value="..."
            GDB_NEXT_TOKEN();
            GDB_ABORT(',');
            GDB_NEXT_TOKEN();
            GDB_ABORT(GDB_VALUE);
            GDB_NEXT_TOKEN();
            GDB_ABORT('=');
            GDB_NEXT_TOKEN();
            GDB_ABORT(GDB_STRING);
#endif
            // ,type="..."
            GDB_NEXT_TOKEN();
            GDB_ABORT(',');
            GDB_NEXT_TOKEN();
            GDB_ABORT(GDB_TYPE);
            GDB_NEXT_TOKEN();
            GDB_ABORT('=');
            GDB_NEXT_TOKEN();
            type_name = currentToken;
        } while(0);
    }
    gdb_result_lex_clean();

    wxGDB_STRIP_QUOATES(type_name);
    wxGDB_STRIP_QUOATES(var_name);

    // delete the variable object
    cmd.Clear();
    cmd << wxT("-var-delete ") << var_name;

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
            m_observer->UpdateAddLine(wxString::Format(_("Breakpoint %d condition cleared"), m_bp.debugger_id));
        } else {
            m_observer->UpdateAddLine(
                wxString::Format(_("Condition %s set for breakpoint %d"), m_bp.conditions.c_str(), m_bp.debugger_id));
        }
        return true;
    }
    return false;
}

// -break-list output handler
bool DbgCmdBreakList::ProcessOutput(const wxString& line)
{
    wxString dbg_output(line);
    dbg_output.Replace("bkpt=", "");
    std::vector<BreakpointInfo> li;
    GdbChildrenInfo info;
    gdbParseListChildren(dbg_output.mb_str(wxConvUTF8).data(), info);

    // Children is a vector of map of attribues.
    // Each map represents an information about a breakpoint
    // the way gdb sees it
    for(size_t i = 0; i < info.children.size(); i++) {
        BreakpointInfo breakpoint;
        std::map<std::string, std::string> attr = info.children.at(i);
        std::map<std::string, std::string>::const_iterator iter;

        iter = attr.find("what");
        if(iter != attr.end()) {
            breakpoint.what = wxString(iter->second.c_str(), wxConvUTF8);
            wxRemoveQuotes(breakpoint.what);
        }

        //		iter = attr.find("func");
        //		if ( iter != attr.end() ) {
        //			breakpoint.function_name = wxString(iter->second.c_str(), wxConvUTF8);
        //			wxRemoveQuotes( breakpoint.function_name );
        //		}
        //
        //		iter = attr.find("addr");
        //		if ( iter != attr.end() ) {
        //			breakpoint.memory_address = wxString(iter->second.c_str(), wxConvUTF8);
        //			wxRemoveQuotes( breakpoint.memory_address );
        //		}

        iter = attr.find("file");
        if(iter != attr.end()) {
            breakpoint.file = wxString(iter->second.c_str(), wxConvUTF8);
            wxRemoveQuotes(breakpoint.file);
        }

        iter = attr.find("fullname");
        if(iter != attr.end()) {
            breakpoint.file = wxString(iter->second.c_str(), wxConvUTF8);
            wxRemoveQuotes(breakpoint.file);
        }

        iter = attr.find("at");
        if(iter != attr.end()) {
            breakpoint.at = wxString(iter->second.c_str(), wxConvUTF8);
            wxRemoveQuotes(breakpoint.at);
        }

        // If we got fullname, use it instead of 'file'
        iter = attr.find("line");
        if(iter != attr.end()) {
            if(iter->second.empty() == false) {
                wxString lineNumber(iter->second.c_str(), wxConvUTF8);
                wxRemoveQuotes(lineNumber);
                breakpoint.lineno = wxAtoi(lineNumber);
            }
        }

        // get the 'ignore' attribute
        iter = attr.find("ignore");
        if(iter != attr.end()) {
            if(iter->second.empty() == false) {
                wxString ignore(iter->second.c_str(), wxConvUTF8);
                wxRemoveQuotes(ignore);
                breakpoint.ignore_number = wxAtoi(ignore);
            }
        }

        // breakpoint ID
        iter = attr.find("number");
        if(iter != attr.end()) {
            if(iter->second.empty() == false) {
                wxString bpId(iter->second.c_str(), wxConvUTF8);
                wxRemoveQuotes(bpId);
                bpId.ToCDouble(&breakpoint.debugger_id);
            }
        }
        li.push_back(breakpoint);
    }

    // Filter duplicate file:line breakpoints
    std::vector<BreakpointInfo> uniqueBreakpoints;
    std::for_each(li.begin(), li.end(), [&](const BreakpointInfo& bp) {
        if(bp.debugger_id == (long)bp.debugger_id) {
            // real breakpoint ID
            uniqueBreakpoints.push_back(bp);
        }
    });
    
    li.swap(uniqueBreakpoints);
    m_observer->ReconcileBreakpoints(li);
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
    int divider(m_columns);
    int factor((int)(m_count / divider));

    if(m_count % divider != 0) {
        factor = (int)(m_count / divider) + 1;
    }

    // {addr="0x003d3e24",data=["0x65","0x72","0x61","0x6e"],ascii="eran"},
    // {addr="0x003d3e28",data=["0x00","0xab","0xab","0xab"],ascii="xxxx"}
    wxString dbg_output(line), output;

    // search for ,memory=[
    int where = dbg_output.Find(wxT(",memory="));
    if(where != wxNOT_FOUND) {
        dbg_output = dbg_output.Mid((size_t)(where + 9));

        const wxCharBuffer scannerText = _C(dbg_output);
        setGdbLexerInput(scannerText.data(), true);

        int type;
        wxString currentToken;
        wxString currentLine;
        GDB_NEXT_TOKEN();

        for(int i = 0; i < factor && type != 0; i++) {
            currentLine.Clear();

            while(type != GDB_ADDR) {

                if(type == 0) {
                    break;
                }

                GDB_NEXT_TOKEN();
                continue;
            }

            // Eof?
            if(type == 0) {
                break;
            }

            GDB_NEXT_TOKEN(); //=
            GDB_NEXT_TOKEN(); // 0x003d3e24
            wxGDB_STRIP_QUOATES(currentToken);
            currentLine << currentToken << wxT(": ");

            GDB_NEXT_TOKEN(); //,
            GDB_NEXT_TOKEN(); // data
            GDB_NEXT_TOKEN(); //=
            GDB_NEXT_TOKEN(); //[

            long v(0);
            wxString hex, asciiDump;
            for(int yy = 0; yy < divider; yy++) {
                GDB_NEXT_TOKEN(); //"0x65"
                wxGDB_STRIP_QUOATES(currentToken);
                // convert the hex string into real value
                if(currentToken.ToLong(&v, 16)) {

                    //	char ch = (char)v;
                    if(wxIsprint((wxChar)v) || (wxChar)v == ' ') {
                        if(v == 9) { // TAB
                            v = 32;  // SPACE
                        }

                        hex << (wxChar)v;
                    } else {
                        hex << wxT("?");
                    }
                } else {
                    hex << wxT("?");
                }

                currentLine << currentToken << wxT(" ");
                GDB_NEXT_TOKEN(); //, | ]
            }

            GDB_NEXT_TOKEN(); //,
            GDB_NEXT_TOKEN(); // GDB_ASCII
            GDB_NEXT_TOKEN(); //=
            GDB_NEXT_TOKEN(); // ascii_value
            currentLine << wxT(" : ") << hex;

            wxGDB_STRIP_QUOATES(currentToken);
            output << currentLine << wxT("\n");
            GDB_NEXT_TOKEN();
        }

        gdb_result_lex_clean();
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

static VariableObjChild FromParserOutput(const std::map<std::string, std::string>& attr)
{
    VariableObjChild child;
    std::map<std::string, std::string>::const_iterator iter;

    child.type = ExtractGdbChild(attr, wxT("type"));
    child.gdbId = ExtractGdbChild(attr, wxT("name"));
    wxString numChilds = ExtractGdbChild(attr, wxT("numchild"));
    wxString dynamic = ExtractGdbChild(attr, wxT("dynamic"));

    if(numChilds.IsEmpty() == false) {
        child.numChilds = wxAtoi(numChilds);
    }

    if(child.numChilds == 0 && dynamic == "1") {
        child.numChilds = 1;
    }

    child.varName = ExtractGdbChild(attr, wxT("exp"));
    if(child.varName.IsEmpty() || child.type == child.varName ||
       (child.varName == wxT("public") || child.varName == wxT("private") || child.varName == wxT("protected")) ||
       (child.type.Contains(wxT("class ")) || child.type.Contains(wxT("struct ")))) {

        child.isAFake = true;
    }

    // For primitive types, we also get the value
    iter = attr.find("value");
    if(iter != attr.end()) {
        if(iter->second.empty() == false) {
            wxString v(iter->second.c_str(), wxConvUTF8);
            wxRemoveQuotes(v);
            child.value = wxGdbFixValue(v);

            if(child.value.IsEmpty() == false) {
                child.varName << wxT(" = ") << child.value;
            }
        }
    }
    return child;
}

bool DbgCmdListChildren::ProcessOutput(const wxString& line)
{
    DebuggerEventData e;
    std::string cbuffer = line.mb_str(wxConvUTF8).data();

    GdbChildrenInfo info;
    gdbParseListChildren(cbuffer, info);

    // Convert the parser output to codelite data structure
    for(size_t i = 0; i < info.children.size(); i++) {
        e.m_varObjChildren.push_back(FromParserOutput(info.children.at(i)));
    }

    if(info.children.size() > 0) {
        e.m_updateReason = DBG_UR_LISTCHILDREN;
        e.m_expression = m_variable;
        e.m_userReason = m_userReason;
        m_observer->DebuggerUpdate(e);

        clCommandEvent evtList(wxEVT_DEBUGGER_LIST_CHILDREN);
        evtList.SetClientObject(new DebuggerEventData(e));
        EventNotifier::Get()->AddPendingEvent(evtList);
    }
    return true;
}

bool DbgCmdEvalVarObj::ProcessOutput(const wxString& line)
{
    std::string cbuffer = line.mb_str(wxConvUTF8).data();
    GdbChildrenInfo info;
    gdbParseListChildren(cbuffer, info);

    if(info.children.empty() == false) {
        wxString display_line = ExtractGdbChild(info.children.at(0), wxT("value"));
        display_line.Trim().Trim(false);
        if(display_line.IsEmpty() == false) {
            if(m_userReason == DBG_USERR_WATCHTABLE || display_line != wxT("{...}")) {
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
    return false;
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
            m_numberToName.insert(std::make_pair(counter, reg_name));

            GDB_NEXT_TOKEN(); // remove the ','
            if(type != ',')
                // end of the list
                break;
            ++counter;
        }
    }
    gdb_result_lex_clean();
    // Now trigger the register values call
    return m_gdb->WriteCommand("-data-list-register-values N",
                               new DbgCmdHandlerRegisterValues(m_observer, m_gdb, m_numberToName));
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
