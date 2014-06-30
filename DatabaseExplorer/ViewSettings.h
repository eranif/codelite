//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : ViewSettings.h
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

#ifndef VIEWSETTINGS_H
#define VIEWSETTINGS_H

#include <wx/wx.h>
#include <wx/validate.h>
#include "GUI.h" // Base class: _TableSettings
#include <wx/wxsf/DiagramManager.h>

#include "IDbAdapter.h"
#include "view.h"

/*! \brief Dialog for editing database view */
class ViewSettings : public _ViewSettings {

public:
	/*! \brief Default constructor */
	ViewSettings( wxWindow* parent,IDbAdapter* pDbAdapter);
	/*! \brief Default desctructor */
	virtual ~ViewSettings();
	/*! \brief Load view into dialog */
	void SetView(View* pView, wxSFDiagramManager* pManager);

	virtual void OnOKClick(wxCommandEvent& event);
	
protected:
	IDbAdapter* m_pDbAdapter;
	View* m_pView;
	wxSFDiagramManager* m_pDiagramManager;
};

#endif // VIEWSETTINGS_H
