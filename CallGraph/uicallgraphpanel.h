//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : uicallgraphpanel.h
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
 * Name:      uicallgraphpanel.h
 * Purpose:   Header to create panel and show call graph picture.
 * Author:    Václav Špruček
 * Created:   2012-03-04
 * Copyright: Václav Špruček
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef UICALLGRAPHPANEL_H
#define UICALLGRAPHPANEL_H

#include "lineparser.h"
#include "confcallgraph.h"
#include "plugin.h"
#include "uicallgraph.h" // Base class: uicallgraph


class uicallgraphpanel : public uicallgraph
{

public:
	uicallgraphpanel(wxWindow *parent, IManager *mgr, const wxString& imagepath, const wxString& projectpath, int suggestedThreshold, LineParserList *pLines);
	virtual ~uicallgraphpanel();

protected:
	virtual void OnPaint(wxPaintEvent& event);
	virtual void OnSaveCallGraph(wxCommandEvent& event);
	virtual void OnClosePanel(wxCommandEvent& event);
	virtual void OnRefreshClick(wxCommandEvent& event);
	virtual void OnLeftDown(wxMouseEvent& event);
	virtual void OnLeftUp(wxMouseEvent& event);
	virtual void OnMouseMove(wxMouseEvent& event);
	virtual void OnMouseWheel(wxMouseEvent& event);
	virtual void OnZoom100(wxCommandEvent& event);
	virtual void OnZoomIn(wxCommandEvent& event);
	virtual void OnZoomOut(wxCommandEvent& event);
	virtual void OnZoomOriginal(wxCommandEvent& event);

	int CreateAndInserDataToTable(int nodethr);	// returns min_threshold
	void UpdateImage();

	wxBitmap m_bmpOrig;
	wxBitmap m_bmpScaled;
	IManager *m_mgr;
	wxString m_pathimage;
	wxString m_pathproject;
	LineParserList m_lines;
	ConfCallGraph confData; // stored configuration data
	wxPoint m_viewPortOrigin;
	wxPoint m_startigPoint;
	float m_scale;
};

#endif // UICALLGRAPHPANEL_H
