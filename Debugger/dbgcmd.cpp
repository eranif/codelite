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
#include "wx/tokenzr.h"
#include "debuggergdb.h"
#include "precompiled_header.h"
#include "gdb_result_parser.h"
#include <wx/regex.h>

extern int gdb_result_lex();
extern bool setGdbLexerInput(const std::string &in);
extern void gdb_result_lex_clean();
extern std::string gdb_result_string;
extern void gdb_result_push_buffer(const std::string &new_input);
extern void gdb_result_pop_buffer();

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

static void GDB_STRIP_QUOATES(std::string &currentToken)
{
	size_t where = currentToken.find("\"");
	if (where != std::string::npos && where == 0) {
		currentToken.erase(0, 1);
	}

	where = currentToken.rfind("\"");
	if (where != std::string::npos && where == currentToken.length()-1) {
		currentToken.erase(where);
	}

	where = currentToken.find("\"\\\\");
	if (where != std::string::npos && where == 0) {
		currentToken.erase(0, 3);
	}

	where = currentToken.rfind("\"\\\\");
	if (where != std::string::npos && where == currentToken.length()-3) {
		currentToken.erase(where);
	}
}

#define GDB_LEX()\
	{\
		type = gdb_result_lex();\
		currentToken = gdb_result_string;\
	}

#define GDB_BREAK(ch)\
	if(type != (int)ch){\
		break;\
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

static bool ParseFrameLocals(wxString &strline, wxString &name, wxString &v)
{
	// search for name="XXX"
	wxString rest;
	int where = strline.Find(wxT("name=\""));
	if (where != wxNOT_FOUND) {
		strline = strline.Mid((size_t)where + 6);

		// serarch for closing quoat
		for (size_t i=0; i<strline.size(); i++) {
			if (strline.GetChar(i) == '"') {
				if (i > 0 && strline.GetChar(i-1) != wxT('\\')) {
					// this is not an escaped string
					// remove this string from the original strline
					strline = strline.Mid(i);
					break;
				} else {
					v << strline.GetChar(i);
				}
			} else {
				name << strline.GetChar(i);
			}
		}
	}

	where = strline.Find(wxT(",value=\""));
	if (where != wxNOT_FOUND) {
		strline = strline.Mid((size_t)where+8);
		// search for
		// serarch for closing quoat
		for (size_t i=0; i<strline.size(); i++) {
			if (strline.GetChar(i) == '"') {
				if (i > 0 && strline.GetChar(i-1) != wxT('\\')) {
					// this is not an escaped string
					// remove this string from the original strline
					strline = strline.Mid(i);
					break;
				} else {
					v << strline.GetChar(i);
				}
			} else {
				v << strline.GetChar(i);
			}
		}
	}

	return !v.IsEmpty() && !name.IsEmpty();
}

bool DbgCmdHandlerGetLine::ProcessOutput(const wxString &line)
{
#if defined (__WXMSW__) || defined (__WXGTK__)
	//^done,line="36",file="a.cpp",fullname="C:/testbug1/a.cpp"
	wxString strLine, fileName;
	wxStringTokenizer tkz(line, wxT(","));
	if (tkz.HasMoreTokens()) {
		//skip first
		tkz.NextToken();
	}
	if (tkz.HasMoreTokens()) {
		strLine = tkz.NextToken();
	} else {
		return false;
	}
	if (tkz.HasMoreTokens()) {
		tkz.NextToken();//skip
	}
	if (tkz.HasMoreTokens()) {
		fileName = tkz.NextToken();
	} else {
		return false;
	}

	//line="36"
	strLine = strLine.AfterFirst(wxT('"'));
	strLine = strLine.BeforeLast(wxT('"'));
	long lineno;
	strLine.ToLong(&lineno);

	//remove quotes
	fileName = fileName.AfterFirst(wxT('"'));
	fileName = fileName.BeforeLast(wxT('"'));
	fileName.Replace(wxT("\\\\"), wxT("\\"));

	m_observer->UpdateFileLine(fileName, lineno);
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

bool DbgCmdHandlerAsyncCmd::ProcessOutput(const wxString &line)
{
	wxString reason;
	//*stopped,reason="end-stepping-range",thread-id="1",frame={addr="0x0040156b",func="main",args=[{name="argc",value="1"},{name="argv",value="0x3e2c50"}],file="a.cpp",line="46"}
	//when reason is "end-stepping-range", it means that one of the following command was
	//completed:
	//-exec-step, -exec-stepi
	//-exec-next, -exec-nexti
	wxStringTokenizer tkz(line, wxT(","));
	if (tkz.HasMoreTokens()) {
		tkz.NextToken();//skip *stopped
	} else {
		return false;
	}
	//get the reason
	if (tkz.HasMoreTokens()) {
		reason = tkz.NextToken();
		reason = reason.AfterFirst(wxT('"'));
		reason = reason.BeforeLast(wxT('"'));
	} else {
		return false;
	}

	//Note:
	//This might look like a stupid if-else, since all taking
	//the same action at the end - return control to program, but this is done
	//for future use to allow different handling for every case
	if (reason == wxT("end-stepping-range")) {
		//just notify the container that we got control back from debugger
		m_observer->UpdateGotControl(DBG_END_STEPPING);
	} else if ((reason == wxT("breakpoint-hit")) || (reason == wxT("watchpoint-trigger"))) {
		static wxRegEx reFuncName(wxT("func=\"([a-zA-Z!_0-9]+)\""));

		// Incase we break due to assertion, notify the observer with different break code
#ifdef __WXMSW__
		if ( reFuncName.Matches(line) )
#else // Mac / Linux
		if ( false )
#endif
		{
			wxString func_name = reFuncName.GetMatch(line, 1);
			if ( func_name == wxT("msvcrt!_assert") || // MinGW
			        func_name == wxT("__assert")          // Cygwin
			   ) {
				// assertion caught
				m_observer->UpdateGotControl(DBG_BP_ASSERTION_HIT);
			} else {
				m_observer->UpdateGotControl(DBG_BP_HIT);
			}
		} else {
			// Notify the container that we got control back from debugger
			m_observer->UpdateGotControl(DBG_BP_HIT);
			// Now discover which bp was hit. Fortunately, that's in the next token: bkptno="12"
			static wxRegEx reGetBreakNo(wxT("[0-9]+"));
			wxString whichBP = tkz.NextToken();
			if (reGetBreakNo.Matches(whichBP)) {
				wxString number = reGetBreakNo.GetMatch(whichBP);
				long id;
				if (number.ToLong(&id)) {
					m_observer->UpdateBpHit((int)id);
				}
			}
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
			m_observer->UpdateGotControl(DBG_RECV_SIGNAL_SIGSEGV);

		} else if (signame == wxT("EXC_BAD_ACCESS")) {
			m_observer->UpdateGotControl(DBG_RECV_SIGNAL_EXC_BAD_ACCESS);

		} else if (signame == wxT("SIGABRT")) {
			m_observer->UpdateGotControl(DBG_RECV_SIGNAL_SIGABRT);

		} else {
			//default
			m_observer->UpdateGotControl(DBG_RECV_SIGNAL);
		}
	} else if (reason == wxT("exited-normally")) {
		m_observer->UpdateAddLine(_("Program exited normally."));

		//debugee program exit normally
		m_observer->UpdateGotControl(DBG_EXITED_NORMALLY);
	} else if (reason == wxT("function-finished")) {
		wxString message;
		int where = line.Find(wxT("return-value"));
		if (where != wxNOT_FOUND) {
			message = line.Mid(where+12);
			message = message.AfterFirst(wxT('"'));
			message = message.BeforeFirst(wxT('"'));
			message.Prepend(_("Function returned with value: "));
			m_observer->UpdateAddLine(message);
		}

		//debugee program exit normally
		m_observer->UpdateGotControl(DBG_FUNC_FINISHED);
	} else {
		//by default return control to program
		m_observer->UpdateGotControl(DBG_UNKNOWN);
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
		// failed to place the breakpoint
		m_observer->UpdateAddLine(wxString::Format(wxT("ERROR: failed to place breakpoint: \"%s\""), line.c_str()));
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
		m_observer->UpdateAddLine(wxString::Format(wxT("Found the breakpoint ID!")));

	} else if (reWatch.Matches(line)) {
		number = reWatch.GetMatch(line, 1);
	}

	if (number.IsEmpty() == false) {
		if (number.ToLong(&breakpointId)) {
			// for debugging purpose
			m_observer->UpdateAddLine(wxString::Format(wxT("Storing debugger breakpoint Id=%d"), breakpointId));
		}
	}

	m_observer->UpdateBpAdded(m_bp.internal_id, breakpointId);
	if (breakpointId == wxNOT_FOUND) {
		return true;	// If the bp wasn't matched, the most likely reason is that bp creation failed. So don't say it worked
	}

	wxString msg;
	switch (m_bpType) {
	case BP_type_break:
		msg = wxString::Format(_("Successfully set breakpoint %d at: "), breakpointId);
		break;
	case BP_type_condbreak:
		msg = wxString::Format(_("Successfully set conditional breakpoint %d at: "), breakpointId);
		break;
	case BP_type_tempbreak:
		msg = wxString::Format(_("Successfully set temporary breakpoint %d at: "), breakpointId);
		break;
	case BP_type_watchpt:
		switch (m_bp.watchpoint_type) {
		case WP_watch:
			msg = wxString::Format(_("Successfully set watchpoint %d watching: "), breakpointId);
			break;
		case WP_rwatch:
			msg = wxString::Format(_("Successfully set read watchpoint %d watching: "), breakpointId);
			break;
		case WP_awatch:
			msg = wxString::Format(_("Successfully set read/write watchpoint %d watching: "), breakpointId);
			break;
		}
	}

	if (m_bpType == BP_type_watchpt) {
		msg <<  m_bp.watchpt_data;
	} else if (m_bp.memory_address != -1) {
		msg <<  wxT("address ") << m_bp.memory_address;
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
	NodeData data;
	bool bEvalExpr(false);
	switch (m_evaluateExpression) {
	case EvaluateExpression:
		data.name = wxT("Quick Watch");
		bEvalExpr = true;
		break;
	case Locals:
		data.name = wxT("Locals");
		break;
	case This:
		data.name = wxT("*this");
		break;
	case FunctionArguments:
		data.name = wxT("Function Arguments");
		break;
	}

	wxString strline(line), tmpline;
	TreeNode<wxString, NodeData> *tree = new TreeNode<wxString, NodeData>(data.name, data);

	if (m_evaluateExpression == Locals) {
#ifdef __WXMAC__
		strline.Replace(wxT(" \\n "), wxT(""));
		strline.Replace(wxT("{\\n "), wxT("{"));
		strline.Replace(wxT("\\n}"), wxT("}"));

		strline = strline.AfterFirst(wxT('{'));
		strline = strline.BeforeLast(wxT('}'));
		if (strline.EndsWith(wxT("}")))
#else
		strline = strline.AfterFirst(wxT('['));
		strline = strline.BeforeLast(wxT(']'));
		if (strline.EndsWith(wxT("]")))
#endif
		{
			strline = strline.RemoveLast();
		}
	} else if (m_evaluateExpression == FunctionArguments) {
#ifdef __WXMAC__
		if (strline.StartsWith(wxT("^done,stack-args={frame={level=\"0\",args={"), &tmpline))
#else
		if (strline.StartsWith(wxT("^done,stack-args=[frame={level=\"0\",args=["), &tmpline))
#endif
		{
			strline = tmpline;
		}

		if (strline.EndsWith(wxT("]}]")), &tmpline) {
			strline = tmpline;
		}

	} else {
		//EvaluateExpression || This
		if (strline.StartsWith(wxT("^done,value="), &tmpline)) {
			strline = tmpline;
			wxString prestr;
			prestr << wxT("name=\\\"") << m_expression << wxT("\\\",value=");
			strline.Prepend(prestr);
		}
	}

	try {
		if (m_evaluateExpression == FunctionArguments) {
			MakeTreeFromFrame(strline, tree);
		} else {
			const wxCharBuffer scannerText =  _C(strline);
			setGdbLexerInput(scannerText.data());
			MakeTree(tree);
		}
	} catch ( ... ) {
		// something really bad happend :)
		// print error message and the output which caused the error and
		// bailout
		gdb_result_lex_clean();
		wxLogMessage(wxT("Debugger: ERROR: cought a std exception while processing output '%s'"), line.c_str());
		return false;
	}

	gdb_result_lex_clean();

	if (m_evaluateExpression == Locals || m_evaluateExpression == This || m_evaluateExpression == FunctionArguments) {
		m_observer->UpdateLocals(tree);
	} else {
		m_observer->UpdateQuickWatch(m_expression, tree);
	}
	return true;
}

void DbgCmdHandlerLocals::MakeTree(TreeNode<wxString, NodeData> *parent)
{
	wxString displayLine;
	std::string currentToken;
	int type(0);

	//remove prefix
	GDB_LEX();
	while (type != 0) {
		//pattern is *always* name="somename",value="somevalue"
		//however, value can be a sub tree value="{....}"
		if (type != GDB_NAME) {
			GDB_LEX();
			continue;
		}

		//wait for the '='
		GDB_LEX();
		GDB_BREAK('=');

		GDB_LEX();
		if (type != GDB_STRING && type != GDB_ESCAPED_STRING) {
			break;
		}

		// remove quoates from the name value
		GDB_STRIP_QUOATES(currentToken);

		displayLine << _U(currentToken.c_str());

		//comma
		GDB_LEX();
		GDB_BREAK(',');
		//value
		GDB_LEX();
		if (type != GDB_VALUE) {
			break;
		}
		GDB_LEX();
		GDB_BREAK('=');

		GDB_LEX();
		if (type != GDB_STRING) {
			break;
		}



		// remove the quoates from the value
		GDB_STRIP_QUOATES(currentToken);

		if ( currentToken.size() == 0 ) {
			// empty token?
			wxLogMessage(wxT("Debugger: ERROR: currentToke.size() == 0"));
			break;
		}

		if (currentToken.at(0) == '{') {
			if (displayLine.IsEmpty() == false) {
				//open a new node for the tree
				NodeData data;
				data.name = displayLine;
				TreeNode<wxString, NodeData> *child = parent->AddChild(data.name, data);

				// since we dont want a dummy <unnamed> node, we remove the false
				// open brace
				wxString tmp(_U(currentToken.c_str()));
				tmp = tmp.Mid(1);

				// also remove the last closing brace
				tmp = tmp.RemoveLast();

				const wxCharBuffer buff = _C(tmp);

				// set new buffer to the
				gdb_result_push_buffer(buff.data());

				MakeSubTree(child);

				// restore the previous buffer
				gdb_result_pop_buffer();
			}
		} else  {
			// simple case
			displayLine << wxT(" = ");

			// set new buffer to the
			gdb_result_push_buffer(currentToken);

			GDB_LEX();
			while (type != 0) {
				if (type == (int)'{') {
					//open a new node for the tree
					NodeData data;
					data.name = displayLine;
					TreeNode<wxString, NodeData> *child = parent->AddChild(data.name, data);

					MakeSubTree(child);

					displayLine.Empty();
					break;
				} else {
					displayLine << _U(currentToken.c_str()) << wxT(" ");
				}
				GDB_LEX();
			}
			// restore the previous buffer
			gdb_result_pop_buffer();

			if (displayLine.IsEmpty() == false) {
				NodeData data;
				data.name = displayLine;
				parent->AddChild(data.name, data);
				displayLine.Empty();
			}
		}
		displayLine.Empty();
		GDB_LEX();
	}
}

void DbgCmdHandlerLocals::MakeSubTree(TreeNode<wxString, NodeData> *parent)
{
	//the pattern here should be
	//key = value, ....
	//where value can be a complex value:
	//key = {...}
	wxString displayLine;
	wxString name, value;
	std::string currentToken;
	int type(0);

	GDB_LEX();
	while (type != 0) {
		switch (type) {
		case (int)'=':
						displayLine << wxT("= ");
			break;
		case (int)'{': {
			//create the new child node
			wxString tmpValue;
			if (displayLine.EndsWith(wxT(" = "), &tmpValue)) {
				displayLine = tmpValue;
			}

			// display line can be empty (in case of unnamed structures)
			if (displayLine.empty()) {
				displayLine = wxT("<unnamed>");
			}

			//make a sub node
			NodeData data;
			data.name = displayLine;
			TreeNode<wxString, NodeData> *child = parent->AddChild(data.name, data);
			MakeSubTree(child);
			displayLine.Empty();
		}
		break;
		case (int)',':
						if (displayLine.IsEmpty() == false) {
					NodeData nodeData;
					nodeData.name = displayLine;
					parent->AddChild(nodeData.name, nodeData);
					displayLine = wxEmptyString;
				}
			break;
		case (int)'}':
						if (displayLine.IsEmpty() == false) {
					NodeData nodeData;
					nodeData.name = displayLine;
					parent->AddChild(nodeData.name, nodeData);
					displayLine = wxEmptyString;
				}
			return;
		default:
			displayLine << _U(currentToken.c_str()) << wxT(" ");
			break;
		}
		GDB_LEX();
	}

	if (type == 0 && !displayLine.IsEmpty()) {
		NodeData nodeData;
		nodeData.name = displayLine;
		parent->AddChild(nodeData.name, nodeData);
		displayLine = wxEmptyString;
	}
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
	tmpLine.Replace(wxT("\\\""), wxT("\""));
	m_observer->UpdateExpression(m_expression, tmpLine);
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
	m_observer->UpdateGotControl(DBG_END_STEPPING);
	return true;
}

void DbgCmdHandlerLocals::MakeTreeFromFrame(wxString &strline, TreeNode<wxString, NodeData>* parent)
{
	wxString displayLine;
	wxString name, val;

	while (ParseFrameLocals(strline, name, val)) {
		wxString text;
		text << name << wxT("=") << val;

		// if the current stack argument is 'this' skip it, since we are already
		// passing '*this' by default
		if (name.Trim().Trim(false) == wxT("this")) {
			name.Clear();
			val.Clear();
			continue;
		}

		const wxCharBuffer scannerText =  _C(text);
		setGdbLexerInput(scannerText.data());

		MakeSubTree(parent);

		gdb_result_lex_clean();

		name.Clear();
		val.Clear();
	}
}

bool DbgCmdHandlerRemoteDebugging::ProcessOutput(const wxString& line)
{
	// We use this handler as a callback to indicate that gdb has connected to the debugger
	m_observer->UpdateRemoteTargetConnected(line);

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
	setGdbLexerInput(scannerText.data());
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

	evaluated.Replace(wxT("\\t"), wxT("\t"));
	// Update the observer
	m_observer->UpdateTip(m_expression, evaluated);
	return true;
}
