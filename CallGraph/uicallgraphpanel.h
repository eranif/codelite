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

	void CreateAndInserDataToTable(int nodethr);
	void UpdateImage();

	wxBitmap m_bmpOrig;
	wxBitmap m_bmpScaled;
	IManager *m_mgr;
	wxString pathimage;
	wxString pathproject;
	LineParserList m_lines;
	ConfCallGraph confData; // stored configuration data
	wxPoint m_viewPortOrigin;
	wxPoint m_startigPoint;
	float m_scale;
};

#endif // UICALLGRAPHPANEL_H
