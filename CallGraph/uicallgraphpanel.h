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


class uicallgraphpanel : public uicallgraph {

public:
	uicallgraphpanel(wxWindow *parent, IManager *mgr, const wxString& imagepath, LineParserList *pLines);
	virtual ~uicallgraphpanel();

protected:
	virtual void OnPaint(wxPaintEvent& event);
	virtual void OnSaveCallGraph(wxCommandEvent& event);
	virtual void OnClosePanel(wxCommandEvent& event);
	//void NumberOfPrimaryLinesAndSize();
	void CreateAndInserDataToTable();
	
	wxBitmap m_Bmp;
	IManager *m_mgr;
	wxString pathimage;
	LineParserList *mlines;
	ConfCallGraph confData; // stored configuration data
	//int rows;
	//int row_label_size;
};

#endif // UICALLGRAPHPANEL_H
