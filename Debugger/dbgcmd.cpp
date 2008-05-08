#include "dbgcmd.h"
#include "wx/tokenzr.h"
#include "debuggergdb.h"
#include "gdblexer.h"
#include "precompiled_header.h"

#define GDB_LEX()\
	{\
		currentToken = "";\
		type = le_gdb_lex();\
		if(type == LE_GDB_STRING_LITERAL){\
			currentToken = le_gdb_string_word;\
		}else{\
			currentToken = le_gdb_text;\
		}\
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
	} else if (reason == wxT("breakpoint-hit")) {
		//just notify the container that we got control back from debugger
		m_observer->UpdateGotControl(DBG_BP_HIT);
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
		} else if(signame == wxT("EXC_BAD_ACCESS")){
			m_observer->UpdateGotControl(DBG_RECV_SIGNAL_EXC_BAD_ACCESS);
		} else {
			//default
			m_observer->UpdateGotControl(DBG_RECV_SIGNAL);
		}
	} else if (reason == wxT("exited-normally")) {
		m_observer->UpdateAddLine(wxT("Program exited normally."));

		//debugee program exit normally
		m_observer->UpdateGotControl(DBG_EXITED_NORMALLY);
	} else if (reason == wxT("function-finished")) {
		wxString message;
		int where = line.Find(wxT("return-value"));
		if (where != wxNOT_FOUND) {
			message = line.Mid(where+12);
			message = message.AfterFirst(wxT('"'));
			message = message.BeforeFirst(wxT('"'));
			message.Prepend(wxT("Function returned with value: "));
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
	//parse the line, incase we got error, keep this breakpoint in the queue
	if (line.StartsWith(wxT("^done"))) {
		//remove this breakpoint from the breakpoint list
		for (size_t i=0; i< m_bplist->size(); i++) {
			BreakpointInfo bp = m_bplist->at(i);
			if (bp.file == m_bp.file && bp.lineno == m_bp.lineno) {
				m_bplist->erase(m_bplist->begin()+i);
				break;
			}
		}
	}
	wxString msg;
	msg << wxT("Successfully set breakpoint at: ") << m_bp.file << wxT(":") << m_bp.lineno;
	m_observer->UpdateAddLine(msg);
	m_observer->UpdateBpAdded();
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
	}

	wxString strline(line), tmpline;
	TreeNode<wxString, NodeData> *tree = new TreeNode<wxString, NodeData>(data.name, data);

	if (m_evaluateExpression == Locals) {
		strline = strline.AfterFirst(wxT('['));
		strline = strline.BeforeLast(wxT(']'));
		if (strline.StartsWith(wxT("^done,locals=["), &tmpline)) {
			strline = tmpline;
		}

		if (strline.EndsWith(wxT("]"))) {
			strline = strline.RemoveLast();
		}
	} else {
		//
		//EvaluateExpression || This
		if (strline.StartsWith(wxT("^done,value="), &tmpline)) {
			strline = tmpline;
			wxString prestr;
			prestr << wxT("name=\\\"") << m_expression << wxT("\\\",value=");
			strline.Prepend(prestr);
		}
	}

	const wxCharBuffer scannerText =  _C(strline);
	le_gdb_set_input(scannerText.data());
	MakeTree(tree);
	le_gdb_lex_clean();

	if (m_evaluateExpression == Locals || m_evaluateExpression == This) {
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
		if (type != LE_GDB_NAME) {
			GDB_LEX();
			continue;
		}

		//wait for the '='
		GDB_LEX();
		GDB_BREAK('=');

		GDB_LEX();
		if (type != LE_GDB_WORD && type != LE_GDB_STRING_LITERAL) {
			break;
		}
		displayLine << _U(currentToken.c_str());

		//comma
		GDB_LEX();
		GDB_BREAK(',');
		//value
		GDB_LEX();
		if (type != LE_GDB_VALUE) {
			break;
		}
		GDB_LEX();
		GDB_BREAK('=');

		GDB_LEX();
		if (type == (int)'{') {
			if (displayLine.IsEmpty() == false) {
				//open a new node for the tree
				NodeData data;
				data.name = displayLine;
				TreeNode<wxString, NodeData> *child = parent->AddChild(data.name, data);
				MakeSubTree(child);
			}
		} else if (type == (int)'(') {
			//we encountered an expression of:
			//(class Notebook *) 0xffeedd00
			displayLine << wxT(" = ");
			while ( type != (int)'}') {
				displayLine << _U(currentToken.c_str()) << wxT(" ");
				GDB_LEX();
			}
			NodeData data;
			data.name = displayLine;
			parent->AddChild(data.name, data);
		} else {
			//simple case of name = value
			displayLine << wxT(" = ");
			wxString val(wxEmptyString);
			while (type == LE_GDB_CHAR_LITERAL || type == LE_GDB_STRING_LITERAL || type == LE_GDB_WORD || currentToken == "-") {
				if (type == LE_GDB_STRING_LITERAL) {
					val << wxT("\"") << _U(currentToken.c_str()) << wxT("\" ");
				} else {
					val << _U(currentToken.c_str()) << wxT(" ");
				}
				GDB_LEX();
			}

			if (val.IsEmpty()) {
				val = wxT("\"\"");
			}
			displayLine << val;
			NodeData data;
			data.name = displayLine;
			data.name.Replace(wxT("\\\\\\\\"), wxT("\\"));
			data.name.Replace(wxT("\\\\"), wxT("\\"));
			parent->AddChild(data.name, data);
		}
		displayLine.Empty();
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
		case LE_GDB_WORD:
		case LE_GDB_STRING_LITERAL:
			displayLine << _U(currentToken.c_str()) << wxT(" ");
			break;
		case (int)'=':
						displayLine << wxT("= ");
			break;
		case (int)'{': {
			//create the new child node
			wxString tmpValue;
			if (displayLine.EndsWith(wxT(" = "), &tmpValue)) {
				displayLine = tmpValue;
			}

			if (displayLine.IsEmpty() == false) {
				//make a sub node
				NodeData data;
				data.name = displayLine;
				TreeNode<wxString, NodeData> *child = parent->AddChild(data.name, data);
				MakeSubTree(child);
				displayLine.Empty();
			}
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
		}
		GDB_LEX();
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
