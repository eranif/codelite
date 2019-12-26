//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : pipedprocess.cpp
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
#include "cl_standard_paths.h"
#include "pipedprocess.h" 
#include <wx/stdpaths.h>
#include <wx/txtstrm.h>
#include <wx/sstream.h>
#include "wx/msgdlg.h"
#include <wx/filename.h>

PipedProcess::PipedProcess(int id, const wxString &cmdLine)
: wxProcess(NULL, id)
, m_pid(-1)
, m_cmd(cmdLine)
{
}

PipedProcess::~PipedProcess()
{
}

long PipedProcess::GetPid()
{
	return m_pid;
}

void PipedProcess::SetPid(long pid)
{
	m_pid = pid;
}

void PipedProcess::Terminate()
{
#ifdef __WXGTK__
	wxString cmd;
	wxFileName exePath(clStandardPaths::Get().GetExecutablePath());
	wxFileName script(exePath.GetPath(), wxT("codelite_kill_children"));
	cmd << wxT("/bin/bash -f ") << script.GetFullPath() << wxT(" ") << GetPid();
	wxExecute(cmd, wxEXEC_ASYNC);
#else
	wxKillError rc;
	wxKill(GetPid(), wxSIGKILL, &rc, wxKILL_CHILDREN);
#endif
}

long PipedProcess::Start(bool hide)
{
	Redirect();
	long flags = wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER ;
	if(!hide){
		flags |= wxEXEC_NOHIDE;
	}

	m_pid = wxExecute(m_cmd, flags, this);
	return m_pid;
}

//read single line from input/error of the debugger
bool PipedProcess::HasInput(wxString &input)
{
	bool hasInput = false;
	bool cont1(true), cont2(true);
	while(cont1 || cont2){
		cont1 = false;
		cont2 = false;
		while( IsInputAvailable() )
		{
			wxTextInputStream tis(*GetInputStream());
			// this assumes that the output is always line buffered
			wxChar ch = tis.GetChar();
			input << ch;
			hasInput = true;
			if(ch == wxT('\n')){
				cont1 = false;
				break;
			}else{
				cont1 = true;
			}
		}

		while( IsErrorAvailable() )
		{
			wxTextInputStream tis(*GetErrorStream());
			// this assumes that the output is always line buffered
			wxChar ch = tis.GetChar();
			input << ch;
			hasInput = true;
			if(ch == wxT('\n')){
				cont2 = false;
				break;
			}else{
				cont2 = true;
			}
		}

	}
	return hasInput;
}

bool PipedProcess::ReadAll(wxString &input)
{
	bool hasInput = false;
	bool cont1(true), cont2(true);

	wxTextInputStream tis(*GetInputStream());
	wxTextInputStream tie(*GetErrorStream());
	while(cont1 || cont2){
		cont1 = false;
		cont2 = false;
		while( IsInputAvailable() )
		{
			// this assumes that the output is always line buffered
			wxChar ch = tis.GetChar();
			input << ch;
			hasInput = true;
			cont1 = true;
		}

		while( IsErrorAvailable() )
		{
			// this assumes that the output is always line buffered
			wxChar ch = tie.GetChar();
			input << ch;
			hasInput = true;
			cont2 = true;
		}
	}
	return hasInput;
}


