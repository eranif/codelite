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

#include "wx/wx.h"
#include "wx/string.h"
#include "uisettings.h"
#include "confcallgraph.h"


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
