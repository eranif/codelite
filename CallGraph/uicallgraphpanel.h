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
	uicallgraphpanel(wxWindow *parent, IManager *mgr, const wxString& imagepath, const wxString& projectpath, LineParserList *pLines);
	virtual ~uicallgraphpanel();

protected:
	virtual void OnPaint(wxPaintEvent& event);
	virtual void OnSaveCallGraph(wxCommandEvent& event);
	virtual void OnClosePanel(wxCommandEvent& event);
	virtual void OnRefreshClick(wxCommandEvent& event);

	void CreateAndInserDataToTable(int nodethr);
	void UpdateImage();

	wxBitmap m_Bmp;
	IManager *m_mgr;
	wxString pathimage;
	wxString pathproject;
	LineParserList m_lines;
	ConfCallGraph confData; // stored configuration data
};

#endif // UICALLGRAPHPANEL_H
