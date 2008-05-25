//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : interactiveprocess.cpp              
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
 #include "interactiveprocess.h"
#include "wx/txtstrm.h"
#include "wx/app.h"

void InteractiveProcess::StopProcess()
{
	m_stop = true;
	if(m_proc){
		m_canUse = false;
		m_proc->Terminate();
		CleanUp();
	}
}

void InteractiveProcess::OnTimer(wxTimerEvent &WXUNUSED(event))
{
	if( m_stop ){
		m_canUse = false;
		m_proc->Terminate();
		return;
	}
}

void InteractiveProcess::OnProcessEnd(wxProcessEvent& event)
{
	wxUnusedVar(event);
	m_canUse = false;
	//read all input before stopping the timer
	if( !m_stop ){
		CleanUp();
	}
	event.Skip();
}

void InteractiveProcess::CleanUp()
{
	m_timer->Stop();
	m_busy = false;
	m_stop = false;
}

bool InteractiveProcess::Write(const wxString &cmd)
{
	if(!IsBusy() || !m_proc){
		//process is not up
		return false;
	}

	if(m_proc->GetOutputStream()){
		wxTextOutputStream os(*m_proc->GetOutputStream());
		os.WriteString(cmd + wxT("\n"));
		return true;
	}
	return false;
}

//this function may be called from another thread
//so make it thread-safe
bool InteractiveProcess::ReadLine(wxString &ostr, int timeout)
{
	if(!m_canUse){
		return false;
	}

	bool res(false);
	for(int i=0; i<timeout; i++){
		if(!m_proc->HasInput(ostr)){
			wxMilliSleep(1);
			continue;
		}else{
			res = true;
			break;
		}
	}
	return res;
}

bool InteractiveProcess::ReadAllData(wxString &output)
{
	return m_proc->ReadAll(output);
}

