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
	pathimage = imagepath;
	pathproject = projectpath;
	m_scale = 1;

	// copy lines to local storage
	m_lines.DeleteContents( true );
	for(LineParserList::iterator it = pLines->begin(); it != pLines->end(); ++it)
		m_lines.Append((*it)->Clone());

	m_bmpOrig.LoadFile(pathimage, wxBITMAP_TYPE_PNG);
	UpdateImage();

	m_scrolledWindow->SetBackgroundColour(wxColour(255, 255, 255));
	m_scrolledWindow->SetBackgroundStyle(wxBG_STYLE_CUSTOM);

	m_mgr->GetConfigTool()->ReadObject(wxT("CallGraph"), &confData);
	if( suggestedThreshold == -1 ) suggestedThreshold = confData.GetTresholdNode();

	CreateAndInserDataToTable(suggestedThreshold);

	m_spinNT->SetValue(suggestedThreshold);
	m_spinET->SetValue(confData.GetTresholdEdge());
	m_checkBoxSN->SetValue(confData.GetBoxName());
	m_grid->Update();
}

uicallgraphpanel::~uicallgraphpanel()
{
	m_lines.Clear();
}

void uicallgraphpanel::OnPaint(wxPaintEvent& event)
{
	wxRect rectUpdate = m_scrolledWindow->GetUpdateRegion().GetBox();
	wxPoint ptVirt = m_scrolledWindow->CalcUnscrolledPosition(rectUpdate.GetTopLeft());
	// move from origin
	ptVirt.x -= 20;
	ptVirt.y -= 20;

	wxAutoBufferedPaintDC dc(m_scrolledWindow);
	wxMemoryDC memDC(m_bmpScaled);

	dc.SetBrush( wxBrush( m_scrolledWindow->GetBackgroundColour() ) );
	dc.Clear();
	dc.Blit(rectUpdate.GetTopLeft(), rectUpdate.GetSize(), &memDC, ptVirt, wxCOPY);
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

void uicallgraphpanel::CreateAndInserDataToTable(int nodethr)
{
	LineParserList::compatibility_iterator it = m_lines.GetFirst();
	int nr = 0;
	while(it) {
		LineParser *line = it->GetData();
		if(line->pline && wxRound(line->time) >= nodethr) {
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
}

void uicallgraphpanel::OnRefreshClick(wxCommandEvent& event)
{
	if( m_grid->GetRows() ) m_grid->DeleteRows(0, m_grid->GetRows());

	// write to output png file
	DotWriter dw;
	dw.SetLineParser(&m_lines);
	dw.SetDotWriterFromDetails(confData.GetColorsNode(),
	                           confData.GetColorsEdge(),
	                           m_spinNT->GetValue(),
	                           m_spinET->GetValue(),
	                           m_checkBoxSN->GetValue(),
	                           confData.GetBoxParam());

	dw.WriteToDotLanguage();
	dw.SendToDotAppOutputDirectory(pathproject);

	if (dw.DotFileExist(pathproject)) {
		//-Gcharset=Latin1		//UTF-8
		wxString cmddot = thePlugin->GetDotPath() + stvariables::sw + wxT("-Tpng -o") + stvariables::sw + pathproject + stvariables::dotfilesdir + stvariables::sd + stvariables::dotpngname  + stvariables::sw + pathproject + stvariables::dotfilesdir + stvariables::sd + stvariables::dottxtname;
		wxProcess cmddotProcess;
		cmddotProcess.Redirect();
		wxExecute(cmddot, wxEXEC_SYNC, &cmddotProcess);

		m_bmpOrig.LoadFile(pathimage, wxBITMAP_TYPE_PNG);
		UpdateImage();

	} else
		wxMessageBox(_("CallGraph failed to save file with DOT language, please build the project again."), wxT("CallGraph"), wxOK | wxICON_INFORMATION);

	// update call table
	CreateAndInserDataToTable(m_spinNT->GetValue());
}

void uicallgraphpanel::UpdateImage()
{
	wxBusyCursor busy;
	
	wxImage img = m_bmpOrig.ConvertToImage();
	m_bmpScaled = wxBitmap(img.Scale( m_bmpOrig.GetWidth()*m_scale,
									m_bmpOrig.GetHeight()*m_scale,
									wxIMAGE_QUALITY_HIGH ));

	int x = m_bmpScaled.GetWidth() + 30;
	int y = m_bmpScaled.GetHeight() + 30;
	m_scrolledWindow->SetVirtualSize(x, y);
	m_scrolledWindow->Refresh(false);
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
	m_scale = 1;
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
