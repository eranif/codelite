//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : uicallgraphpanel.cpp
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

#include "uicallgraphpanel.h"
#include "workspace.h"
#include <wx/dcbuffer.h>
#include <wx/bitmap.h>
#include <wx/filedlg.h>
#include <wx/filefn.h>
#include <wx/xrc/xmlres.h>
#include "callgraph.h"

uicallgraphpanel::uicallgraphpanel(wxWindow *parent, IManager *mgr, const wxString& imagepath, const wxString& projectpath, int suggestedThreshold, LineParserList *pLines) : uicallgraph(parent)
{
	m_mgr = mgr;
	m_pathimage = imagepath;
	m_pathproject = projectpath;
	m_scale = 1;
    
    m_scrolledWindow->SetBackgroundColour(wxColour(255, 255, 255));
    m_scrolledWindow->SetBackgroundStyle(wxBG_STYLE_PAINT);

	// copy lines to local storage
	m_lines.DeleteContents( true );
	for(LineParserList::iterator it = pLines->begin(); it != pLines->end(); ++it)
		m_lines.Append((*it)->Clone());

	if( m_bmpOrig.LoadFile(m_pathimage, wxBITMAP_TYPE_PNG) ) UpdateImage();
    
	m_mgr->GetConfigTool()->ReadObject(wxT("CallGraph"), &confData);
	if( suggestedThreshold == -1 ) suggestedThreshold = confData.GetTresholdNode();

	CreateAndInserDataToTable(suggestedThreshold);

	m_spinNT->SetValue(suggestedThreshold);
	m_spinET->SetValue(confData.GetTresholdEdge());
	m_checkBoxHP->SetValue(confData.GetHideParams());
	m_checkBoxHN->SetValue(confData.GetHideNamespaces());
	m_grid->Update();
}

uicallgraphpanel::~uicallgraphpanel()
{
	m_lines.Clear();
}

void uicallgraphpanel::OnPaint(wxPaintEvent& event)
{
	wxPoint ptVirt = m_scrolledWindow->CalcUnscrolledPosition( wxPoint(0, 0) );
	// move from origin
	ptVirt.x -= 20;
	ptVirt.y -= 20;

	wxAutoBufferedPaintDC dc(m_scrolledWindow);
    dc.SetDeviceOrigin( -ptVirt.x, -ptVirt.y );
    dc.Clear();
    dc.DrawBitmap( m_bmpScaled, 0, 0 );
}

void uicallgraphpanel::OnSaveCallGraph(wxCommandEvent& event)
{
	//wxString projectName = m_mgr->GetWorkspace()->GetActiveProjectName();

	wxFileDialog saveFileDialog(this, _("Save call graph..."), wxT(""), wxT("CallGraph"), wxT("png files (*.png)|*.png"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

	if (saveFileDialog.ShowModal() == wxID_CANCEL)
		return;

	m_bmpOrig.SaveFile(saveFileDialog.GetPath(), wxBITMAP_TYPE_PNG);
}

void uicallgraphpanel::OnClosePanel(wxCommandEvent& event)
{
	wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_file"));
	m_mgr->GetTheApp()->GetTopWindow()->GetEventHandler()->AddPendingEvent(evt);
}

int uicallgraphpanel::CreateAndInserDataToTable(int node_thr)
{
	LineParserList::compatibility_iterator it = m_lines.GetFirst();
	int nr = 0;
	float max_time = -2;
	
	while(it) {
		LineParser *line = it->GetData();
		
		if (max_time < line->time)	max_time = line->time;
		
		if(line->pline && wxRound(line->time) >= node_thr) {
			m_grid->AppendRows(1, true);
			//name   time %   self  children    called
			m_grid->SetCellValue(nr, 0, line->name);
			m_grid->SetCellValue(nr, 1, wxString::Format(wxT("%.2f"),line->time));
			m_grid->SetCellValue(nr, 2, wxString::Format(wxT("%.2f"),line->self + line->children));

			int callsum;
			if(line->called0 != -1) {
				callsum = line->called0;
				if(line->called1 != -1) callsum += line->called1;
			} else callsum = 1;

			m_grid->SetCellValue(nr, 3, wxString::Format(wxT("%i"),callsum));
			nr ++;
		}
		it = it->GetNext();
	}
	
	return wxRound(max_time);
}

void uicallgraphpanel::OnRefreshClick(wxCommandEvent& event)
{
	if( m_grid->GetNumberRows() ) m_grid->DeleteRows(0, (m_grid->GetNumberRows() - 1));

	// write to output png file
	DotWriter dw;
	dw.SetLineParser(&m_lines);
	dw.SetDotWriterFromDetails(confData.GetColorsNode(),
	                           confData.GetColorsEdge(),
	                           m_spinNT->GetValue(),
	                           m_spinET->GetValue(),
	                           m_checkBoxHP->GetValue(),
	                           confData.GetStripParams(),
							   m_checkBoxHN->GetValue());

	dw.WriteToDotLanguage();

	wxFileName	cfn(m_pathproject, DOT_FILENAME_TXT);
	cfn.AppendDir(CALLGRAPH_DIR);
	cfn.Normalize();
		
	wxString	dot_fn = cfn.GetFullPath();

	bool	ok = dw.SendToDotAppOutputDirectory(dot_fn);
	if (ok)
	{	// delete any existing PNG
		if (wxFileExists(m_pathimage))	wxRemoveFile(m_pathimage);
	
		wxString cmddot_ln;
	
		cmddot_ln << confData.GetDotPath() << " -Tpng -o" << m_pathimage << " " << dot_fn;
		
		wxExecute(cmddot_ln, wxEXEC_SYNC | wxEXEC_HIDE_CONSOLE);

		if( m_bmpOrig.LoadFile(m_pathimage, wxBITMAP_TYPE_PNG) ) UpdateImage();
        
	}
	else
        wxMessageBox(_("CallGraph failed to save file with DOT language, please build the project again."), wxT("CallGraph"), wxOK | wxICON_INFORMATION);

	// update call table
	CreateAndInserDataToTable(m_spinNT->GetValue());
}

void uicallgraphpanel::UpdateImage()
{
	wxBusyCursor busy;

    if( m_bmpOrig.IsOk() ) {
        wxImage img = m_bmpOrig.ConvertToImage();
        if( img.IsOk() ) { 
            img.Rescale( m_bmpOrig.GetWidth() * m_scale, m_bmpOrig.GetHeight() * m_scale, wxIMAGE_QUALITY_HIGH );
            m_bmpScaled = wxBitmap( img );

            int x = m_bmpScaled.GetWidth() + 30;
            int y = m_bmpScaled.GetHeight() + 30;
            m_scrolledWindow->SetVirtualSize(x, y);
            m_scrolledWindow->Refresh(false);
        }
        
    } else {
        m_bmpScaled = wxBitmap();
    }
}

void uicallgraphpanel::OnLeftDown(wxMouseEvent& event)
{
	m_scrolledWindow->GetViewStart( &m_viewPortOrigin.x, &m_viewPortOrigin.y );
	m_startigPoint = event.GetPosition();
	m_scrolledWindow->SetCursor( wxCursor(wxCURSOR_SIZING) );
}

void uicallgraphpanel::OnMouseMove(wxMouseEvent& event)
{
	if( event.LeftIsDown() ) {
		int dx, dy;
		m_scrolledWindow->GetScrollPixelsPerUnit( &dx, &dy );
		m_scrolledWindow->Scroll( m_viewPortOrigin.x + (m_startigPoint.x - event.GetPosition().x)/dx,
		                          m_viewPortOrigin.y + (m_startigPoint.y - event.GetPosition().y)/dy );
	}

}
void uicallgraphpanel::OnLeftUp(wxMouseEvent& event)
{
	m_scrolledWindow->SetCursor( *wxSTANDARD_CURSOR );
}

void uicallgraphpanel::OnMouseWheel(wxMouseEvent& event)
{
	if( event.ControlDown() ) {
		m_scale += (float)event.GetWheelRotation()/(event.GetWheelDelta()*10);
		if( m_scale < 0.1 ) m_scale = 0.1;
		else if( m_scale > 1 ) m_scale = 1;
		UpdateImage();
	}
}

void uicallgraphpanel::OnZoom100(wxCommandEvent& event)
{
	float xscale = (float)(m_scrolledWindow->GetClientSize().x-40) / m_bmpOrig.GetWidth();
	float yscale = (float)(m_scrolledWindow->GetClientSize().y-40) / m_bmpOrig.GetHeight();
	
	m_scale = xscale < yscale ? xscale : yscale;
	
	if( m_scale < 0.1 ) m_scale = 0.1;
	else if( m_scale > 1 ) m_scale = 1;
	UpdateImage();	
}

void uicallgraphpanel::OnZoomIn(wxCommandEvent& event)
{
	m_scale = m_scale + 0.1;
	if( m_scale > 1 ) m_scale = 1;
	UpdateImage();
}

void uicallgraphpanel::OnZoomOut(wxCommandEvent& event)
{
	m_scale = m_scale - 0.1;
	if( m_scale < 0.1 ) m_scale = 0.1;
	UpdateImage();
}

void uicallgraphpanel::OnZoomOriginal(wxCommandEvent& event)
{
	m_scale = 1;
	UpdateImage();
}
