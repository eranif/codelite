//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : svnreportgeneratoraction.cpp              
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
 #include "svnreportgeneratoraction.h"
#include "wx/event.h"
#include "subversion.h"

SvnReportGeneratorAction::SvnReportGeneratorAction(SubversionPlugin *plugin, int eventId)
: m_eventId(eventId)
, m_plugin(plugin)
{
}

SvnReportGeneratorAction::~SvnReportGeneratorAction()
{
}

void SvnReportGeneratorAction::DoCommand()
{
	if(m_plugin) {
		wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, m_eventId);
		event.SetEventObject(m_plugin);
		wxPostEvent(m_plugin, event);
	}
}
