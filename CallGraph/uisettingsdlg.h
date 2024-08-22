//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : uisettingsdlg.h
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

/***************************************************************
 * Name:      uisettingsdlg.h
 * Purpose:   Header to create settings dialor for plugin.
 * Author:    Václav Špruček
 * Created:   2012-03-04
 * Copyright: Václav Špruček 
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef __uisettingsdlg__
#define __uisettingsdlg__

#include "confcallgraph.h"
#include "uisettings.h"

#include <wx/wx.h>
#include <wx/string.h>

class IManager;
//class CallGraph;

class uisettingsdlg : public uisettings
{
	IManager *m_mgr;
	//CallGraph *m_mplugin;
	
protected:
	// Handlers for FormBuildSettingsBaseDlg events.
	void OnButton_click_select_gprof( wxCommandEvent& event );
	void OnButton_click_select_dot( wxCommandEvent& event );
	void OnButton_click_ok( wxCommandEvent& event );
	void OnButton_click_cancel( wxCommandEvent& event );
	void OnCheckName( wxCommandEvent& event );
	void OnCheckParam( wxCommandEvent& event );
	//
	ConfCallGraph confData;
	
public:
	/** Constructor */
	uisettingsdlg( wxWindow* parent, IManager *mgr);//, CallGraph *plugin );
	virtual ~uisettingsdlg();
};

#endif
