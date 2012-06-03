#include "uicallgraphpanel.h"
#include "workspace.h"
#include <wx/dcbuffer.h>
#include <wx/bitmap.h>
#include <wx/filedlg.h>
#include <wx/filefn.h>
#include <wx/xrc/xmlres.h> 

uicallgraphpanel::uicallgraphpanel(wxWindow *parent, IManager *mgr, const wxString& imagepath, LineParserList *pLines) : uicallgraph(parent)
{
	m_mgr = mgr;
	pathimage = imagepath;
	mlines = pLines;
	
	m_Bmp.LoadFile(pathimage, wxBITMAP_TYPE_PNG);
	int x = m_Bmp.GetWidth() + 30;
	int y = m_Bmp.GetHeight() + 30;
	
	m_scrolledWindow->SetBackgroundColour(wxColour(255, 255, 255));
	m_scrolledWindow->SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	m_scrolledWindow->SetVirtualSize(x, y);
	
	// inizialise and create table for insert data from LineParserList
	m_mgr->GetConfigTool()->ReadObject(wxT("CallGraph"), &confData);
	CreateAndInserDataToTable();
	
}

uicallgraphpanel::~uicallgraphpanel()
{
}

void uicallgraphpanel::OnPaint(wxPaintEvent& event)
{
	wxRect rectUpdate = m_scrolledWindow->GetUpdateRegion().GetBox();
	wxPoint ptVirt = m_scrolledWindow->CalcUnscrolledPosition(rectUpdate.GetTopLeft());
	// move from origin
	ptVirt.x -= 20;
	ptVirt.y -= 20;
		
	wxAutoBufferedPaintDC dc(m_scrolledWindow);
	wxMemoryDC memDC(m_Bmp);
	
	dc.SetBrush( wxBrush( m_scrolledWindow->GetBackgroundColour() ) );
	dc.Clear();
	dc.Blit(rectUpdate.GetTopLeft(), rectUpdate.GetSize(), &memDC, ptVirt, wxCOPY);
}

void uicallgraphpanel::OnSaveCallGraph(wxCommandEvent& event)
{	
		//wxString projectName = m_mgr->GetWorkspace()->GetActiveProjectName();

		wxFileDialog saveFileDialog(this, _("Save call graph..."), wxT(""), wxT("CallGraph"), _("png files (*.png)|*.png"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

        if (saveFileDialog.ShowModal() == wxID_CANCEL)
            return;
		
		m_Bmp.SaveFile(saveFileDialog.GetPath(), wxBITMAP_TYPE_PNG);
}

void uicallgraphpanel::OnClosePanel(wxCommandEvent& event)
{
	wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_file"));
	m_mgr->GetTheApp()->GetTopWindow()->GetEventHandler()->AddPendingEvent(evt);
}

/*
void uicallgraphpanel::NumberOfPrimaryLinesAndSize()
{
	row_label_size = 0;
	rows = 0;
	LineParserList::compatibility_iterator it = mlines->GetFirst();
	
	while(it)
	{
		LineParser *line = it->GetData();
		
		if(line->pline)
		{
			int rls = line->name.Len();
			if (rls > row_label_size) row_label_size = rls;
			rows ++;			
		}
		
		it = it->GetNext();
	}
	row_label_size = row_label_size + 10;
	rows = rows + 1;
}*/

void uicallgraphpanel::CreateAndInserDataToTable()
{
	LineParserList::compatibility_iterator it = mlines->GetFirst();
	int nr = 0;
	while(it)
	{
		LineParser *line = it->GetData();
		if(line->pline && wxRound(line->time) >= confData.GetTresholdNode())
		{
			//int rls = line->name.Len();
			//if (rls > row_label_size) row_label_size = rls;
			//
			m_grid->AppendRows(1, true);
			//name   time %   self  children    called
			m_grid->SetCellValue(nr, 0, line->name);
			m_grid->SetCellValue(nr, 1, wxString::Format(wxT("%.2f"),line->time));
			m_grid->SetCellValue(nr, 2, wxString::Format(wxT("%.2f"),line->self + line->children));

			//m_grid->SetCellValue(nr, 3, wxString::Format(wxT("%.2f"),line->childern));
			//
			int callsum;
			if(line->called0 != -1) 
			{
				callsum = line->called0;
				if(line->called1 != -1) callsum += line->called1;
			}
			else callsum = 1;
			//
			m_grid->SetCellValue(nr, 3, wxString::Format(wxT("%i"),callsum));
			nr ++;
		}
		it = it->GetNext();
	}
	
	m_grid->AutoSize();
	m_grid->Fit();
}
