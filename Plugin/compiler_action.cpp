//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : compiler_action.cpp              
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
 #include "environmentconfig.h"
#include "compiler_action.h"
#include "wx/tokenzr.h"

DEFINE_EVENT_TYPE(wxEVT_BUILD_ADDLINE)
DEFINE_EVENT_TYPE(wxEVT_BUILD_STARTED)
DEFINE_EVENT_TYPE(wxEVT_BUILD_ENDED)

CompilerAction::CompilerAction(wxEvtHandler *owner)
: m_proc(NULL)
, m_owner(owner)
, m_busy(false)
, m_stop(false)
{
	m_timer = new wxTimer(this);
}

void CompilerAction::AppendLine(const wxString &line)
{
	if( !m_owner)
		return;

	wxCommandEvent event(wxEVT_BUILD_ADDLINE);
	event.SetString(line);
	m_owner->ProcessEvent(event);

	m_lines.Add(line);
}

void CompilerAction::Stop()
{
	m_stop = true;
	//kill the build process
	if(m_proc){
		m_proc->Terminate();
		CleanUp();
	}
}

void CompilerAction::SendStartMsg()
{
	if( !m_owner)
		return;

	wxCommandEvent event(wxEVT_BUILD_STARTED);
	m_owner->ProcessEvent(event);
}

void CompilerAction::SendEndMsg()
{
	if( !m_owner)
		return;

	wxCommandEvent event(wxEVT_BUILD_ENDED);
	m_owner->ProcessEvent(event);
}

void CompilerAction::OnTimer(wxTimerEvent &event)
{
	wxUnusedVar(event);
	if( m_stop ){
		m_proc->Terminate();
		return;
	}
	PrintOutput();
}

void CompilerAction::PrintOutput()
{
	wxString data, errors;
	m_proc->HasInput(data, errors);
	DoPrintOutput(data, errors);
}

void CompilerAction::OnProcessEnd(wxProcessEvent& event)
{
	wxUnusedVar(event);
	if( !m_stop ){
		//read all input before stopping the timer
		wxString err;
		wxString out;
		while(m_proc->HasInput(out, err)){
			DoPrintOutput(out, err);
			out.Empty();
			err.Empty();
		}
		CleanUp();
	}
	
	EnvironmentConfig::Instance()->UnApplyEnv();
	//give back the focus to the editor
	event.Skip();
}

void CompilerAction::DoPrintOutput(const wxString &out, const wxString &err)
{
	//loop over the lines read from the compiler
	wxStringTokenizer tkz(out, wxT("\n"));
	while(tkz.HasMoreTokens()){
		wxString line = tkz.NextToken();
		if( line.Contains(wxT("Entering directory")) || 
			line.Contains(wxT("Leaving directory"))  ||
			line.Contains(wxT("type attributes are honored only at type definition")) ||
			line.Contains(wxT("type attributes ignored after type is already defined")) ||
			line.StartsWith(wxT("# "))
			){
				//skip it
				continue;
			}else{
				//print it
				AppendLine(line + wxT("\n"));
			}
	}

	if(!err.IsEmpty()){
		wxStringTokenizer tt(err, wxT("\n"));
		while(tt.HasMoreTokens()){
			AppendLine(tt.NextToken() + wxT("\n"));
		}
	}
}

void CompilerAction::CleanUp()
{
	m_timer->Stop();
	m_busy = false;
	m_stop = false;
	SendEndMsg();
}
