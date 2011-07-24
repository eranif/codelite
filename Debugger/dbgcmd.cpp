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
#include "wx/tokenzr.h"
#include "debuggergdb.h"
#include "gdb_result_parser.h"
#include <wx/regex.h>
#include "gdb_parser_incl.h"
#include "procutils.h"

static bool IS_WINDOWNS = (wxGetOsVersion() & wxOS_WINDOWS);

#define GDB_LEX()\
	{\
		type = gdb_result_lex();\
		currentToken = gdb_result_string;\
	}

#define GDB_BREAK(ch)\
	if(type != (int)ch){\
		break;\
	}

static void wxGDB_STRIP_QUOATES(wxString &currentToken)
{
	size_t where = currentToken.find(wxT("\""));
	if (where != std::string::npos && where == 0) {
		currentToken.erase(0, 1);
	}

	where = currentToken.rfind(wxT("\""));
	if (where != std::string::npos && where == currentToken.length()-1) {
		currentToken.erase(where);
	}

	where = currentToken.find(wxT("\"\\\\"));
	if (where != std::string::npos && where == 0) {
		currentToken.erase(0, 3);
	}

	where = currentToken.rfind(wxT("\"\\\\"));
	if (where != std::string::npos && where == currentToken.length()-3) {
		currentToken.erase(where);
	}
}

static void wxRemoveQuotes(wxString &str)
{
	if ( str.IsEmpty() ) {
		return;
	}

	str.RemoveLast();
	if ( str.IsEmpty() == false ) {
		str.Remove(0, 1);
	}
}

static wxString wxGdbFixValue(const wxString &value)
{
	int         type(0);
	std::string currentToken;

	// GDB MI tends to mess the strings...
	// use our Flex lexer to normalize the value
	setGdbLexerInput(value.mb_str(wxConvUTF8).data(), true, true);
	GDB_LEX();
	wxString display_line;
	while ( type != 0 ) {
		display_line << wxString(currentToken.c_str(), wxConvUTF8);
		GDB_LEX();
	}
	gdb_result_lex_clean();
	return display_line;
}

static wxString NextValue(wxString &line, wxString &key)
{
	//extract the key name first
	if (line.StartsWith(wxT(","))) {
		line.Remove(0, 1);
	}

	key = line.BeforeFirst(wxT('='));
	line = line.AfterFirst(wxT('"'));
	wxString token;
	bool cont(true);

	while (!line.IsEmpty() && cont) {
		wxChar ch = line.GetChar(0);
		line.Remove(0, 1);

		if (ch == wxT('"')) {
			cont = false;
		} else {
			token << ch;
		}
	}
	return token;
}

static void ParseStackEntry(const wxString &line, StackEntry &entry)
{
	wxString tmp(line);
	wxString key, value;

	value = NextValue(tmp, key);
	while ( value.IsEmpty() == false ) {
		if (key == wxT("level")) {
			entry.level = value;
		} else if (key == wxT("addr")) {
			entry.address = value;
		} else if (key == wxT("func")) {
			entry.function = value;
		} else if (key == wxT("file")) {
			entry.file = value;
		} else if (key == wxT("line")) {
			entry.line = value;
		} else if (key == wxT("fullname")) {
			entry.file = value;
		}
		value = NextValue(tmp, key);
	}
}

wxString ExtractGdbChild(const std::map<std::string, std::string >& attr, const wxString &name)
{
	std::map<std::string, std::string >::const_iterator iter = attr.find(name.mb_str(wxConvUTF8).data());
	if(iter == attr.end()) {
		return wxT("");
	}
	wxString val = wxString(iter->second.c_str(), wxConvUTF8);
	val.Trim().Trim(false);

	wxRemoveQuotes( val );
	val = wxGdbFixValue(val);

	return val;
}

// Keep a cache of all file paths converted from
// Cygwin path into native path
static std::map<wxString, wxString> g_fileCache;

bool DbgCmdHandlerGetLine::ProcessOutput(const wxString &line)
{
#if defined (__WXGTK__)
	//Output from "-stack-info-frame"
	//^done,frame={level="0",addr="0x000000000043b227",func="MyClass::DoFoo",file="./Foo.cpp",fullname="/full/path/to/Foo.cpp",line="30"}

	wxString tmpLine;
	line.StartsWith(wxT("^done,frame={"), &tmpLine);
	tmpLine.RemoveLast();
	if (tmpLine.IsEmpty()) {
		return false;
	}

	StackEntry entry;
	ParseStackEntry(tmpLine, entry);

	long line_number;
	entry.line.ToLong(&line_number);
	m_observer->UpdateFileLine(entry.file, line_number);

#elif defined (__WXMSW__)
	//Output of -file-list-exec-source-file
	//^done,line="36",file="a.cpp",fullname="C:/testbug1/a.cpp"

	// By default we use the 'fullname' as our file name. however,
	// if we are under Windows and the fullname contains the string '/cygdrive'
	// we fallback to use the 'filename' since cygwin gdb report fullname in POSIX / Cygwin paths
	// which can not be used by codelite
	wxString strLine, fullName, filename;
	wxStringTokenizer tkz(line, wxT(","));
	if (tkz.HasMoreTokens()) {
		//skip first
		tkz.NextToken();
	}
	// line=
	if (tkz.HasMoreTokens()) {
		strLine = tkz.NextToken();
	} else {
		return false;
	}
	// file=
	if (tkz.HasMoreTokens()) {
		filename = tkz.NextToken();
	}

	// fullname=
	if (tkz.HasMoreTokens()) {
		fullName = tkz.NextToken();
	} else {
		return false;
	}

	//line="36"
	strLine = strLine.AfterFirst(wxT('"'));
	strLine = strLine.BeforeLast(wxT('"'));
	long lineno;
	strLine.ToLong(&lineno);

	//remove quotes
	fullName = fullName.AfterFirst(wxT('"'));
	fullName = fullName.BeforeLast(wxT('"'));
	fullName.Replace(wxT("\\\\"), wxT("\\"));
	fullName.Trim().Trim(false);

#ifdef __WXMSW__
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
				if( !cygwinConvertPath.IsEmpty() ) {
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
#endif

	m_observer->UpdateFileLine(fullName, lineno);
#else

	// On Mac we use the stack info the
	// get the current file and line from the debugger
	wxString tmpLine(line);
	line.StartsWith(wxT("^done,stack=["), &tmpLine);

	tmpLine = tmpLine.Trim().Trim(false);
	tmpLine.RemoveLast();

	//--------------------------------------------------------
	//tmpLine contains now string with the following format:
	//frame={name="Value",...},frame={name="Value",...}
	wxString remainder(tmpLine);
	tmpLine		= tmpLine.AfterFirst(wxT('{'));
	if (tmpLine.IsEmpty()) {
		return false;
	}

	remainder	= tmpLine.AfterFirst(wxT('}'));
	tmpLine		= tmpLine.BeforeFirst(wxT('}'));

	StackEntry entry;
	ParseStackEntry(tmpLine, entry);

	long line_number;
	entry.line.ToLong(&line_number);
	m_observer->UpdateFileLine(entry.file, line_number);
#endif
	return true;
}

void DbgCmdHandlerAsyncCmd::UpdateGotControl(DebuggerReasons reason, const wxString &func)
{
	m_observer->UpdateGotControl(reason, func);
}

bool DbgCmdHandlerAsyncCmd::ProcessOutput(const wxString &line)
{
	wxString reason;
	//*stopped,reason="end-stepping-range",thread-id="1",frame={addr="0x0040156b",func="main",args=[{name="argc",value="1"},{name="argv",value="0x3e2c50"}],file="a.cpp",line="46"}
	//when reason is "end-stepping-range", it means that one of the following command was
	//completed:
	//-exec-step, -exec-stepi
	//-exec-next, -exec-nexti
	
	// try and get the debugee PID
	m_gdb->GetDebugeePID();

	// Get the reason
	std::vector<std::map<std::string, std::string> > children;
	gdbParseListChildren(line.mb_str(wxConvUTF8).data(), children);

	wxString func;
	bool foundFunc, foundReason;

	foundFunc   = false;
	foundReason = false;
	for (size_t i=0; i<children.size(); i++) {
		std::map<std::string, std::string> attr = children.at(i);
		std::map<std::string, std::string >::const_iterator iter;

		iter = attr.find("reason");
		if ( !foundReason && iter != attr.end() ) {
			foundReason = true;
			reason = wxString(iter->second.c_str(), wxConvUTF8);
			wxRemoveQuotes( reason );
		}

		if(foundReason)
			break;
	}

	if(reason.IsEmpty())
		return false;

	int where = line.Find(wxT("func=\""));
	if(where != wxNOT_FOUND) {
		func = line.Mid(where + 6);
		func = func.BeforeFirst(wxT('"'));
		foundFunc = true;
	}

	//Note:
	//This might look like a stupid if-else, since all taking
	//the same action at the end - return control to program, but this is done
	//for future use to allow different handling for every case
	if (reason == wxT("end-stepping-range")) {
		//just notify the container that we got control back from debugger
		UpdateGotControl(DBG_END_STEPPING, func);

	} else if ((reason == wxT("breakpoint-hit")) || (reason == wxT("watchpoint-trigger"))) {

		// Incase we break due to assertion, notify the observer with different break code
		if ( func.IsEmpty() && IS_WINDOWNS ) {
			if ( func == wxT("msvcrt!_assert") || // MinGW
			     func == wxT("__assert")       // Cygwin
			   ) {
				// assertion caught
				UpdateGotControl(DBG_BP_ASSERTION_HIT, func);
			}
		}

		// Now discover which bp was hit. Fortunately, that's in the next token: bkptno="12"
		// Except that it no longer is in gdb 7.0. It's now: ..disp="keep",bkptno="12". So:
		static wxRegEx reGetBreakNo(wxT("bkptno=\"([0-9]+)\""));
		if (reGetBreakNo.Matches(line)) {
			wxString number = reGetBreakNo.GetMatch(line, 1);
			long id;
			if (number.ToLong(&id)) {
				if(id != wxNOT_FOUND && m_gdb->m_internalBpId == id) {

					m_observer->UpdateAddLine(wxString::Format(_("Internal breakpoint was hit (id=%d), Applying user breakpoints and continuing"), m_gdb->m_internalBpId));

					// This is an internal breakpoint ID
					m_gdb->m_internalBpId = wxNOT_FOUND;

					// Apply the breakpoints
					m_gdb->SetBreakpoints();

					// Continue running
					m_gdb->Continue();

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


	} else if (reason == wxT("signal-received")) {
		//got signal
		//which signal?
		wxString signame;
		int where = line.Find(wxT("signal-name"));
		if (where != wxNOT_FOUND) {
			signame = line.Mid(where);
			signame = signame.AfterFirst(wxT('"'));
			signame = signame.BeforeFirst(wxT('"'));
		}

		if (signame == wxT("SIGSEGV")) {
			UpdateGotControl(DBG_RECV_SIGNAL_SIGSEGV, func);

		} else if (signame == wxT("EXC_BAD_ACCESS")) {
			UpdateGotControl(DBG_RECV_SIGNAL_EXC_BAD_ACCESS, func);

		} else if (signame == wxT("SIGABRT")) {
			UpdateGotControl(DBG_RECV_SIGNAL_SIGABRT, func);

		} else if (signame == wxT("SIGTRAP")) {
			UpdateGotControl(DBG_RECV_SIGNAL_SIGTRAP, func);

		} else {
			//default
			UpdateGotControl(DBG_RECV_SIGNAL, func);
		}
	} else if (reason == wxT("exited-normally") || reason == wxT("exited")) {
		m_observer->UpdateAddLine(_("Program exited normally."));

		//debugee program exit normally
		UpdateGotControl(DBG_EXITED_NORMALLY, func);

	} else if (reason == wxT("function-finished")) {
		
		//debugee program exit normally
		UpdateGotControl(DBG_FUNC_FINISHED, func);
		
		// We finished an execution of a function.
		// Return to the caller the gdb-result-var since we might want
		// to create a variable object out of it
		wxString gdbVar;
		int where = line.Find(wxT("gdb-result-var"));
		if (where != wxNOT_FOUND) {
			
			gdbVar = line.Mid(where+14);
			gdbVar = gdbVar.AfterFirst(wxT('"'));
			gdbVar = gdbVar.BeforeFirst(wxT('"'));
			
			DebuggerEvent evt;
			evt.m_updateReason = DBG_UR_FUNCTIONFINISHED;
			evt.m_expression   = gdbVar;
			m_observer->DebuggerUpdate(evt);
		}
		
	} else {
		//by default return control to program
		UpdateGotControl(DBG_UNKNOWN, func);
	}
	return true;
}

bool DbgCmdHandlerBp::ProcessOutput(const wxString &line)
{
	// parse the line, in case we have an error, keep this breakpoint in the queue
	if (line.StartsWith(wxT("^done"))) {
		//remove this breakpoint from the breakpoint list
		for (size_t i=0; i< m_bplist->size(); i++) {
			BreakpointInfo b = m_bplist->at(i);
			if (b == m_bp) {
				m_bplist->erase(m_bplist->begin()+i);
				break;
			}
		}
	} else if (line.StartsWith(wxT("^error"))) {
		// Failed to place the breakpoint
		// Tell the bp manager to remove the bp from public view
		m_observer->UpdateBpAdded(m_bp.internal_id, -1);
		m_observer->UpdateAddLine(wxString::Format(_("ERROR: failed to place breakpoint: \"%s\""), line.c_str()), true);
		return true;
	}

	// so the breakpoint ID will come in form of
	// ^done,bkpt={number="2"....
	// ^done,wpt={number="2"
	static wxRegEx reBreak   (wxT("done,bkpt={number=\"([0-9]+)\""));
	static wxRegEx reWatch   (wxT("done,wpt={number=\"([0-9]+)\""));

	wxString number;
	long breakpointId(wxNOT_FOUND);

	if (reBreak.Matches(line)) {
		number = reBreak.GetMatch(line, 1);
		m_observer->UpdateAddLine(wxString::Format(_("Found the breakpoint ID!")), true);

	} else if (reWatch.Matches(line)) {
		number = reWatch.GetMatch(line, 1);
	}

	if (number.IsEmpty() == false) {
		if (number.ToLong(&breakpointId)) {
			// for debugging purpose
			m_observer->UpdateAddLine(wxString::Format(wxT("Storing debugger breakpoint Id=%ld"), breakpointId), true);
		}
	}

	m_observer->UpdateBpAdded(m_bp.internal_id, breakpointId);
	if (breakpointId == wxNOT_FOUND) {
		return true;	// If the bp wasn't matched, the most likely reason is that bp creation failed. So don't say it worked
	}

	wxString msg;
	switch (m_bpType) {
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
		switch (m_bp.watchpoint_type) {
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

	if (m_bpType == BP_type_watchpt) {
		msg <<  m_bp.watchpt_data;
	} else if (m_bp.memory_address.IsEmpty() == false) {
		msg <<  _("address ") << m_bp.memory_address;
	} else {
		if (! m_bp.file.IsEmpty()) {
			msg << m_bp.file << wxT(':');
		}
		if (! m_bp.function_name.IsEmpty()) {
			msg << m_bp.function_name;
		} else if (m_bp.lineno != -1) {
			msg << m_bp.lineno;
		}
	}

	m_observer->UpdateAddLine(msg);
	return true;
}

bool DbgCmdHandlerLocals::ProcessOutput(const wxString &line)
{
	LocalVariables locals;

	std::vector<std::map<std::string, std::string> > children;
	gdbParseListChildren(line.mb_str(wxConvUTF8).data(), children);

	for (size_t i=0; i<children.size(); i++) {
		std::map<std::string, std::string> attr = children.at(i);
		LocalVariable var;
		std::map<std::string, std::string >::const_iterator iter;

		iter = attr.find("name");
		if ( iter != attr.end() ) {
			var.name = wxString(iter->second.c_str(), wxConvUTF8);
			wxRemoveQuotes( var.name );
		}

		iter = attr.find("exp");
		if ( iter != attr.end() ) {
			// We got exp? are we on Mac!!??
			// Anyways, replace exp with name and keep name as gdbId
			var.gdbId = var.name;
			var.name = wxString(iter->second.c_str(), wxConvUTF8);
			wxRemoveQuotes( var.name );
		}

		// For primitive types, we also get the value
		iter = attr.find("value");
		if ( iter != attr.end() ) {
			if ( iter->second.empty() == false ) {
				wxString v (iter->second.c_str(), wxConvUTF8);
				wxRemoveQuotes( v );
				var.value = wxGdbFixValue(v);
			}
		}

		var.value.Trim().Trim(false);
		if(var.value.IsEmpty()) {
			var.value = wxT("{...}");
		}

		iter = attr.find("type");
		if ( iter != attr.end() ) {
			if ( iter->second.empty() == false ) {
				wxString t (iter->second.c_str(), wxConvUTF8);
				wxRemoveQuotes( t );
				var.type = t;
			}
		}

		locals.push_back( var );
	}
	m_observer->UpdateLocals( locals );
	return true;
}

bool DbgCmdHandlerFuncArgs::ProcessOutput(const wxString &line)
{
	LocalVariables locals;

	std::vector<std::map<std::string, std::string> > children;
	gdbParseListChildren(line.mb_str(wxConvUTF8).data(), children);

	for (size_t i=0; i<children.size(); i++) {
		std::map<std::string, std::string> attr = children.at(i);
		LocalVariable var;
		std::map<std::string, std::string >::const_iterator iter;

		iter = attr.find("name");
		if ( iter != attr.end() ) {
			var.name = wxString(iter->second.c_str(), wxConvUTF8);
			wxRemoveQuotes( var.name );
		}

		iter = attr.find("exp");
		if ( iter != attr.end() ) {
			// We got exp? are we on Mac!!??
			// Anyways, replace exp with name and keep name as gdbId
			var.gdbId = var.name;
			var.name = wxString(iter->second.c_str(), wxConvUTF8);
			wxRemoveQuotes( var.name );
		}

		// For primitive types, we also get the value
		iter = attr.find("value");
		if ( iter != attr.end() ) {
			if ( iter->second.empty() == false ) {
				wxString v (iter->second.c_str(), wxConvUTF8);
				wxRemoveQuotes( v );
				var.value = wxGdbFixValue(v);
			}
		}

		var.value.Trim().Trim(false);
		if(var.value.IsEmpty()) {
			var.value = wxT("{...}");
		}

		iter = attr.find("type");
		if ( iter != attr.end() ) {
			if ( iter->second.empty() == false ) {
				wxString t (iter->second.c_str(), wxConvUTF8);
				wxRemoveQuotes( t );
				var.type = t;
			}
		}

		locals.push_back( var );
	}
	m_observer->UpdateFunctionArguments( locals );
	return true;
}

bool DbgCmdHandlerVarCreator::ProcessOutput(const wxString &line)
{
	wxUnusedVar(line);
	return true;
}

bool DbgCmdHandlerEvalExpr::ProcessOutput(const wxString &line)
{
	//remove the ^done
	wxString tmpLine(line);
	line.StartsWith(wxT("^done,value=\""), &tmpLine);
	tmpLine.RemoveLast();
	wxString fixedStr = wxGdbFixValue( tmpLine );
	m_observer->UpdateExpression(m_expression, fixedStr);
	return true;
}

bool DbgCmdStackList::ProcessOutput(const wxString &line)
{
	wxString tmpLine(line);
	line.StartsWith(wxT("^done,stack=["), &tmpLine);

	tmpLine = tmpLine.Trim();
	tmpLine = tmpLine.Trim(false);

	tmpLine.RemoveLast();
	//--------------------------------------------------------
	//tmpLine contains now string with the following format:
	//frame={name="Value",...},frame={name="Value",...}
	wxString remainder(tmpLine);
	StackEntryArray stackArray;
	while (true) {
		tmpLine		= tmpLine.AfterFirst(wxT('{'));
		if (tmpLine.IsEmpty()) {
			break;
		}

		remainder	= tmpLine.AfterFirst(wxT('}'));
		tmpLine		= tmpLine.BeforeFirst(wxT('}'));

		StackEntry entry;
		ParseStackEntry(tmpLine, entry);
		stackArray.push_back(entry);

		tmpLine = remainder;
	}

	m_observer->UpdateStackList(stackArray);
	return true;
}

bool DbgCmdSelectFrame::ProcessOutput(const wxString &line)
{
	wxUnusedVar(line);

	DebuggerEvent e;
	e.m_updateReason  = DBG_UR_GOT_CONTROL;
	e.m_controlReason = DBG_END_STEPPING;
	e.m_frameInfo.function = wxEmptyString;
	m_observer->DebuggerUpdate( e );
	return true;
}

bool DbgCmdHandlerRemoteDebugging::ProcessOutput(const wxString& line)
{
	// We use this handler as a callback to indicate that gdb has connected to the debugger
	m_observer->UpdateRemoteTargetConnected(_("Successfully connected to debugger server"));

	// Apply the breakpoints
	m_observer->UpdateRemoteTargetConnected(_("Applying breakpoints..."));
	DbgGdb *gdb = dynamic_cast<DbgGdb*>(m_debugger);
	if(gdb) {
		gdb->SetBreakpoints();
	}
	m_observer->UpdateRemoteTargetConnected(_("Applying breakpoints... done"));
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
	// parse the output
	// ^done,name="var2",numchild="1",value="{...}",type="orxAABOX"
	const wxCharBuffer scannerText =  _C(line);
	setGdbLexerInput(scannerText.data(), true);

	int type;
	wxString cmd, var_name;
	wxString type_name, currentToken;

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

	} while (0);
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
	m_observer->UpdateTypeReolsved(m_expression, type_name);
	return true;
}

DbgCmdResolveTypeHandler::DbgCmdResolveTypeHandler(const wxString &expression, DbgGdb* debugger)
	: DbgCmdHandler(debugger->GetObserver())
	, m_debugger   (debugger)
	, m_expression (expression)
{}

bool DbgCmdCLIHandler::ProcessOutput(const wxString& line)
{
	// nothing to be done
	return true;
}

bool DbgCmdGetTipHandler::ProcessOutput(const wxString& line)
{
	wxString evaluated = GetOutput();
	evaluated = evaluated.Trim().Trim(false);
	//gdb displays the variable name as $<NUMBER>,
	//we simply replace it with the actual string
	static wxRegEx reGdbVar(wxT("^\\$[0-9]+"));
	static wxRegEx reGdbVar2(wxT("\\$[0-9]+ = "));

	reGdbVar.ReplaceFirst(&evaluated, m_expression);
	reGdbVar2.ReplaceAll (&evaluated, wxEmptyString);

	wxString fixedString = wxGdbFixValue( evaluated );

	// Update the observer
	m_observer->UpdateAsciiViewer(m_expression, fixedString);
	return true;
}

// Set condition handler
bool DbgCmdSetConditionHandler::ProcessOutput(const wxString& line)
{
	wxString dbg_output ( line );
	// If successful, the only output is ^done, so assume that means it worked
	if (dbg_output.Find(wxT("^done")) != wxNOT_FOUND) {
		if (m_bp.conditions.IsEmpty()) {
			m_observer->UpdateAddLine(wxString::Format(_("Breakpoint %d condition cleared"), m_bp.debugger_id));
		} else {
			m_observer->UpdateAddLine(wxString::Format(_("Condition %s set for breakpoint %d"), m_bp.conditions.c_str(), m_bp.debugger_id));
		}
		return true;
	}
	return false;
}

// -break-list output handler
bool DbgCmdBreakList::ProcessOutput(const wxString& line)
{
	wxString dbg_output( line );
	std::vector<BreakpointInfo> li;
	std::vector<std::map<std::string, std::string> > children;
	gdbParseListChildren(dbg_output.mb_str(wxConvUTF8).data(), children);

	// Children is a vector of map of attribues.
	// Each map represents an information about a breakpoint
	// the way gdb sees it
	for(size_t i=0; i<children.size(); i++) {
		BreakpointInfo breakpoint;
		std::map<std::string, std::string> attr = children.at(i);
		std::map<std::string, std::string >::const_iterator iter;

		iter = attr.find("what");
		if ( iter != attr.end() ) {
			breakpoint.what = wxString(iter->second.c_str(), wxConvUTF8);
			wxRemoveQuotes( breakpoint.what );
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
		if ( iter != attr.end() ) {
			breakpoint.file = wxString(iter->second.c_str(), wxConvUTF8);
			wxRemoveQuotes( breakpoint.file );
		}

		iter = attr.find("fullname");
		if ( iter != attr.end() ) {
			breakpoint.file = wxString(iter->second.c_str(), wxConvUTF8);
			wxRemoveQuotes( breakpoint.file );
		}

		iter = attr.find("at");
		if ( iter != attr.end() ) {
			breakpoint.at = wxString(iter->second.c_str(), wxConvUTF8);
			wxRemoveQuotes( breakpoint.at );
		}

		// If we got fullname, use it instead of 'file'
		iter = attr.find("line");
		if ( iter != attr.end() ) {
			if ( iter->second.empty() == false ) {
				wxString lineNumber (iter->second.c_str(), wxConvUTF8);
				wxRemoveQuotes( lineNumber );
				breakpoint.lineno = wxAtoi(lineNumber);
			}
		}

		// get the 'ignore' attribute
		iter = attr.find("ignore");
		if ( iter != attr.end() ) {
			if ( iter->second.empty() == false ) {
				wxString ignore (iter->second.c_str(), wxConvUTF8);
				wxRemoveQuotes( ignore );
				breakpoint.ignore_number = wxAtoi(ignore);
			}
		}

		// breakpoint ID
		iter = attr.find("number");
		if ( iter != attr.end() ) {
			if ( iter->second.empty() == false ) {
				wxString bpId (iter->second.c_str(), wxConvUTF8);
				wxRemoveQuotes( bpId );
				breakpoint.debugger_id = wxAtoi(bpId);
			}
		}
		li.push_back( breakpoint );
	}

	m_observer->ReconcileBreakpoints(li);
	return true;
}

bool DbgCmdListThreads::ProcessOutput(const wxString& line)
{
	wxUnusedVar(line);
	static wxRegEx reCommand(wxT("^([0-9]{8})"));

	wxString output( GetOutput() );
	DebuggerEvent e;

	//parse the debugger output
	wxStringTokenizer tok(output, wxT("\n"), wxTOKEN_STRTOK);
	while ( tok.HasMoreTokens() ) {
		ThreadEntry entry;
		wxString line = tok.NextToken();
		line.Replace(wxT("\t"), wxT(" "));
		line = line.Trim().Trim(false);


		if (reCommand.Matches(line)) {
			//this is the ack line, ignore it
			continue;
		}

		wxString tmpline(line);
		if (tmpline.StartsWith(wxT("*"), &line)) {
			//active thread
			entry.active = true;
		} else {
			entry.active = false;
		}

		line = line.Trim().Trim(false);
		line.ToLong(&entry.dbgid);
		entry.more = line.AfterFirst(wxT(' '));
		e.m_threads.push_back( entry );
	}

	// Notify the observer
	e.m_updateReason  = DBG_UR_LISTTHRAEDS;
	m_observer->DebuggerUpdate( e );
	return true;
}

bool DbgCmdWatchMemory::ProcessOutput(const wxString& line)
{
	DebuggerEvent e;
	int divider (sizeof(unsigned long));
	int factor((int)(m_count/divider));

	if (m_count % divider != 0) {
		factor = (int)(m_count / divider) + 1;
	}

	// {addr="0x003d3e24",data=["0x65","0x72","0x61","0x6e"],ascii="eran"},
	// {addr="0x003d3e28",data=["0x00","0xab","0xab","0xab"],ascii="xxxx"}
	wxString dbg_output ( line ), output;

	// search for ,memory=[
	int where = dbg_output.Find(wxT(",memory="));
	if (where != wxNOT_FOUND) {
		dbg_output = dbg_output.Mid((size_t)(where + 9));

		const wxCharBuffer scannerText =  _C(dbg_output);
		setGdbLexerInput(scannerText.data(), true);

		int type;
		wxString currentToken;
		wxString currentLine;
		GDB_NEXT_TOKEN();

		for (int i=0; i<factor && type != 0; i++) {
			currentLine.Clear();

			while (type != GDB_ADDR) {

				if (type == 0) {
					break;
				}

				GDB_NEXT_TOKEN();
				continue;
			}

			// Eof?
			if (type == 0) {
				break;
			}

			GDB_NEXT_TOKEN();	//=
			GDB_NEXT_TOKEN();	//0x003d3e24
			wxGDB_STRIP_QUOATES(currentToken);
			currentLine << currentToken << wxT(": ");

			GDB_NEXT_TOKEN();	//,
			GDB_NEXT_TOKEN();	//data
			GDB_NEXT_TOKEN();	//=
			GDB_NEXT_TOKEN();	//[

			long v(0);
			wxString hex, asciiDump;
			for (int yy=0; yy<divider; yy++) {
				GDB_NEXT_TOKEN();	//"0x65"
				wxGDB_STRIP_QUOATES(currentToken);
				// convert the hex string into real value
				if (currentToken.ToLong(&v, 16)) {

					//	char ch = (char)v;
					if (wxIsprint((wxChar)v) || (wxChar)v == ' ') {
						if (v == 9) { //TAB
							v = 32; //SPACE
						}

						hex << (wxChar)v;
					} else {
						hex << wxT("?");
					}
				} else {
					hex << wxT("?");
				}

				currentLine << currentToken << wxT(" ");
				GDB_NEXT_TOKEN();	//, | ]
			}

			GDB_NEXT_TOKEN();	//,
			GDB_NEXT_TOKEN();	//GDB_ASCII
			GDB_NEXT_TOKEN();	//=
			GDB_NEXT_TOKEN();	//ascii_value
			currentLine << wxT(" : ") << hex;

			wxGDB_STRIP_QUOATES(currentToken);
			output << currentLine << wxT("\n");
			GDB_NEXT_TOKEN();
		}

		gdb_result_lex_clean();
	}
	e.m_updateReason = DBG_UR_WATCHMEMORY;
	e.m_evaluated  = output;
	e.m_expression = m_address;
	m_observer->DebuggerUpdate( e );
	return true;
}

bool DbgCmdCreateVarObj::ProcessOutput(const wxString& line)
{
	DebuggerEvent e;

	if(line.StartsWith(wxT("^error"))) {
		// Notify the observer we failed to create variable object
		e.m_updateReason = DBG_UR_VARIABLEOBJCREATEERR; // failed to create variable object
		e.m_expression   = m_expression;                // the variable object expression
		e.m_userReason   = m_userReason;
		m_observer->DebuggerUpdate( e );
		return true;
	}

	// Variable object was created
	// Output sample:
	// ^done,name="var1",numchild="2",value="{...}",type="ChildClass",thread-id="1",has_more="0"
	std::vector<std::map<std::string, std::string> > children;
	gdbParseListChildren(line.mb_str(wxConvUTF8).data(), children);

	if( children.size() ) {
		std::map<std::string, std::string> attr = children.at(0);
		VariableObject vo;
		std::map<std::string, std::string >::const_iterator iter;

		iter = attr.find("name");
		if ( iter != attr.end() ) {
			vo.gdbId = wxString(iter->second.c_str(), wxConvUTF8);
			wxRemoveQuotes( vo.gdbId );
		}

		iter = attr.find("numchild");
		if ( iter != attr.end() ) {
			if ( iter->second.empty() == false ) {
				wxString numChilds (iter->second.c_str(), wxConvUTF8);
				wxRemoveQuotes( numChilds );
				vo.numChilds = wxAtoi(numChilds);
			}
		}

		// For primitive types, we also get the value
		iter = attr.find("value");
		if ( iter != attr.end() ) {

			if ( iter->second.empty() == false ) {
				wxString v (iter->second.c_str(), wxConvUTF8);
				wxRemoveQuotes( v );
				wxString val = wxGdbFixValue(v);
				if ( val.IsEmpty()== false) {
					e.m_evaluated = val;
				}
			}
		}

		iter = attr.find("type");
		if ( iter != attr.end() ) {
			if ( iter->second.empty() == false ) {
				wxString t (iter->second.c_str(), wxConvUTF8);
				wxRemoveQuotes( t );
				vo.typeName = t;
			}

			if (vo.typeName.EndsWith(wxT(" *")) ) {
				vo.isPtr = true;
			}

			if (vo.typeName.EndsWith(wxT(" **")) ) {
				vo.isPtrPtr = true;
			}
		}

		if ( vo.gdbId.IsEmpty() == false  ) {

			// set frozeness of the variable object
			//wxString cmd;
			//cmd << wxT("-var-set-frozen ") << vo.gdbId << wxT(" 0");
			//m_debugger->WriteCommand(cmd, NULL);

			e.m_updateReason = DBG_UR_VARIABLEOBJ;
			e.m_variableObject = vo;
			e.m_expression = m_expression;
			e.m_userReason = m_userReason;
			m_observer->DebuggerUpdate( e );
		}
	}
	return true;
}

static VariableObjChild FromParserOutput(const std::map<std::string, std::string > & attr)
{
	VariableObjChild child;
	std::map<std::string, std::string >::const_iterator iter;

	child.type         = ExtractGdbChild(attr, wxT("type"));
	child.gdbId        = ExtractGdbChild(attr, wxT("name"));
	wxString numChilds = ExtractGdbChild(attr, wxT("numchild"));
	
	if(numChilds.IsEmpty() == false) {
		child.numChilds = wxAtoi(numChilds);
	}

	child.varName      = ExtractGdbChild(attr, wxT("exp"));
	if(child.varName.IsEmpty()                                                                                     ||
		child.type == child.varName                                                                                ||
		( child.varName == wxT("public") || child.varName == wxT("private") || child.varName == wxT("protected") ) ||
		( child.type.Contains(wxT("class ")) || child.type.Contains(wxT("struct ")))) {
			
		child.isAFake = true;
	}

	// For primitive types, we also get the value
	iter = attr.find("value");
	if ( iter != attr.end() ) {
		if ( iter->second.empty() == false ) {
			wxString v (iter->second.c_str(), wxConvUTF8);
			wxRemoveQuotes( v );
			child.value = wxGdbFixValue(v);

			if( child.value.IsEmpty() == false ) {
				child.varName << wxT(" = ") << child.value;
			}
		}
	}
	return child;
}

bool DbgCmdListChildren::ProcessOutput(const wxString& line)
{
	DebuggerEvent e;
	std::string cbuffer = line.mb_str(wxConvUTF8).data();

	std::vector< std::map<std::string, std::string > > children;
	gdbParseListChildren(cbuffer, children);

	// Convert the parser output to codelite data structure
	for (size_t i=0; i<children.size(); i++) {
		e.m_varObjChildren.push_back( FromParserOutput( children.at(i) ) );
	}

	if ( children.size() > 0 ) {
		e.m_updateReason = DBG_UR_LISTCHILDREN;
		e.m_expression = m_variable;
		e.m_userReason = m_userReason;
		m_observer->DebuggerUpdate( e );
	}
	return true;
}

bool DbgCmdEvalVarObj::ProcessOutput(const wxString& line)
{
	std::string cbuffer = line.mb_str(wxConvUTF8).data();
	std::vector< std::map<std::string, std::string > > children;
	gdbParseListChildren(cbuffer, children);
	
	if(children.empty() == false) {
		wxString display_line = ExtractGdbChild(children.at(0), wxT("value"));
		display_line.Trim().Trim(false);
		if ( display_line.IsEmpty() == false ) {
			if(m_userReason == DBG_USERR_WATCHTABLE || display_line != wxT("{...}")) {
				DebuggerEvent e;
				e.m_updateReason = DBG_UR_EVALVARIABLEOBJ;
				e.m_expression    = m_variable;
				e.m_evaluated     = display_line;
				e.m_userReason    = m_userReason;
				m_observer->DebuggerUpdate( e );
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
	static wxRegEx reBreak   (wxT("done,bkpt={number=\"([0-9]+)\""));
	wxString number;
	long     breakpointId(wxNOT_FOUND);

	reBreak.Matches(line);
	number = reBreak.GetMatch(line, 1);
	if (number.IsEmpty() == false) {
		if (number.ToLong(&breakpointId)) {
			// for debugging purpose
			m_observer->UpdateAddLine(wxString::Format(wxT("Storing internal breakpoint ID=%ld"), breakpointId), true);
			m_debugger->SetInternalMainBpID( breakpointId );
		}
	}
	return true;
}

bool DbgCmdHandlerStackDepth::ProcessOutput(const wxString& line)
{
	DebuggerEvent e;
	long frameLevel(-1);
	static wxRegEx reFrameDepth(wxT("depth=\"([0-9]+)\""));
	if(reFrameDepth.Matches(line)) {
		wxString strFrameDepth = reFrameDepth.GetMatch(line, 1);
		if(strFrameDepth.ToLong(&frameLevel) && frameLevel != -1) {
			e.m_updateReason = DBG_UR_FRAMEDEPTH;
			e.m_frameInfo.level = strFrameDepth;
			m_observer->DebuggerUpdate( e );
		}
	}
	return true;
}

bool DbgVarObjUpdate::ProcessOutput(const wxString& line)
{
	DebuggerEvent e;

	if(line.StartsWith(wxT("^error"))) {
		// Notify the observer we failed to create variable object
		e.m_updateReason = DBG_UR_VARIABLEOBJUPDATEERR; // failed to create variable object
		e.m_expression   = m_variableName;              // the variable object expression
		e.m_userReason   = m_userReason;
		m_observer->DebuggerUpdate( e );
		return false; // let the default loop to handle this as well by passing DBG_CMD_ERR to the observer
	}

	std::string cbuffer = line.mb_str(wxConvUTF8).data();
	std::vector< std::map<std::string, std::string > > children;
	gdbParseListChildren(cbuffer, children);

	for(size_t i=0; i<children.size(); i++) {
		wxString name         = ExtractGdbChild(children.at(i), wxT("name"));
		wxString in_scope     = ExtractGdbChild(children.at(i), wxT("in_scope"));
		wxString type_changed = ExtractGdbChild(children.at(i), wxT("type_changed"));
		if(in_scope == wxT("false") || type_changed == wxT("true")) {
			e.m_varObjUpdateInfo.removeIds.Add(name);

		} else if(in_scope == wxT("true")) {
			e.m_varObjUpdateInfo.refreshIds.Add(name);
		}
	}
	e.m_updateReason = DBG_UR_VAROBJUPDATE;
	e.m_expression   = m_variableName;
	e.m_userReason   = m_userReason;
	m_observer->DebuggerUpdate( e );
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
		DebuggerEvent e;
		e.m_updateReason  = DBG_UR_GOT_CONTROL;
		e.m_controlReason = DBG_EXIT_WITH_ERROR;
		e.m_text          = errmsg;
		m_observer->DebuggerUpdate(e);
		return true;
		
	} else {
		return DbgCmdHandlerAsyncCmd::ProcessOutput(line);
		
	}
}

bool DbgCmdJumpHandler::ProcessOutput(const wxString& line)
{
	//TODO: handle questions like "Line 24 is not in `main(int, char**)'.  Jump anyway? (y or n) "
	return false;
}
