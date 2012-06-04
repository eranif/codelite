#include "uicallgraphpanel.h"
#include "workspace.h"
#include <wx/dcbuffer.h>
#include <wx/bitmap.h>
#include <wx/filedlg.h>
#include <wx/filefn.h>
#include <wx/xrc/xmlres.h>
#include "callgraph.h"

uicallgraphpanel::uicallgraphpanel(wxWindow *parent, IManager *mgr, const wxString& imagepath, const wxString& projectpath, LineParserList *pLines) : uicallgraph(parent)
{
	m_mgr = mgr;
	pathimage = imagepath;
	pathproject = projectpath;

	// copy lines to local storage
	m_lines.DeleteContents( true );
	for(LineParserList::iterator it = pLines->begin(); it != pLines->end(); ++it)
		m_lines.Append((*it)->Clone());

	UpdateImage();

	m_scrolledWindow->SetBackgroundColour(wxColour(255, 255, 255));
	m_scrolledWindow->SetBackgroundStyle(wxBG_STYLE_CUSTOM);

	m_mgr->GetConfigTool()->ReadObject(wxT("CallGraph"), &confData);
	CreateAndInserDataToTable(confData.GetTresholdNode());

	m_spinNT->SetValue(confData.GetTresholdNode());
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

	m_grid->AutoSize();
	m_grid->Fit();
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

		UpdateImage();
		m_scrolledWindow->Refresh();

	} else
		wxMessageBox(wxT("CallGraph failed to save file with DOT language, please build the project again."), wxT("CallGraph"), wxOK | wxICON_INFORMATION);

	// update call table
	CreateAndInserDataToTable(m_spinNT->GetValue());
}

void uicallgraphpanel::UpdateImage()
{
	m_Bmp.LoadFile(pathimage, wxBITMAP_TYPE_PNG);
	int x = m_Bmp.GetWidth() + 30;
	int y = m_Bmp.GetHeight() + 30;
	m_scrolledWindow->SetVirtualSize(x, y);
}
