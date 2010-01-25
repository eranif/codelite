#include "cl_editor_tip_window.h"
#include <wx/msgdlg.h>
#include "drawingutils.h"
#include <wx/settings.h>
#include <wx/dcbuffer.h>

BEGIN_EVENT_TABLE(clEditorTipWindow, wxPanel)
EVT_PAINT(clEditorTipWindow::OnPaint)
EVT_ERASE_BACKGROUND(clEditorTipWindow::OnEraseBg)
END_EVENT_TABLE()

#define TIP_SPACER 4

clEditorTipWindow::clEditorTipWindow(wxWindow *parent)
: wxPanel(parent)
, m_highlighIndex(0)
{
	Hide();
}

clEditorTipWindow::~clEditorTipWindow()
{
}

void clEditorTipWindow::OnEraseBg(wxEraseEvent& e)
{
	wxUnusedVar(e);
}

void clEditorTipWindow::OnPaint(wxPaintEvent& e)
{
	wxUnusedVar(e);
	wxBufferedPaintDC dc(this);
	wxFont font        = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont disableFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	disableFont.SetStyle(wxFONTSTYLE_ITALIC);
	SetSizeHints(DoGetTipSize());
	
	wxRect rr = GetClientRect();
	
	dc.SetPen  ( wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW) );
	dc.SetBrush( wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
	dc.DrawRectangle(rr);
	dc.SetFont(font);
	
	// Highlight the text
	clCallTipPtr tip = GetTip();
	int secondLineY = (rr.GetHeight()/2);
	int firstLineY  = TIP_SPACER;
	if(tip) {
		wxString txt;
		txt << tip->GetCurr()+1 << wxT(" of ") << tip->Count();
		int txtLen = DoGetTextLen(txt);
		
		int summaryLineXText (rr.GetWidth());
		summaryLineXText -= (txtLen + TIP_SPACER);
		
		// Draw the summary line
		dc.SetFont(disableFont);
		dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
		dc.DrawText(txt, summaryLineXText, secondLineY + TIP_SPACER/2);
		
		int start(-1), len(-1);
		tip->GetHighlightPos(m_highlighIndex, start, len);
		if(len != -1 && start != -1 ) {
			wxString txtBefore  = m_tipText.Mid(0, start);
			wxString txtInclude = m_tipText.Mid(start, len);
			
			int x = DoGetTextLen(txtBefore);
			int w = DoGetTextLen(txtInclude);
			
			dc.SetBrush( wxBrush( DrawingUtils::LightColour(wxT("BLUE"), 9)) );
			dc.SetPen  ( wxPen  ( DrawingUtils::LightColour(wxT("BLUE"), 6)) );
			dc.DrawRectangle(x + TIP_SPACER, firstLineY-(TIP_SPACER/2), w, (rr.GetHeight()/2));
		}
	}
	
	// Draw the Tip text
	dc.SetFont(font);
	dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
	dc.DrawText(m_tipText, wxPoint(TIP_SPACER, firstLineY));
}

void clEditorTipWindow::Add(clCallTipPtr tip)
{
	if(tip && tip->Count()) {
		TipInfo ti;
		ti.tip            = tip;
		ti.highlightIndex = 0;
		m_highlighIndex   = 0;
		m_tips.push_back(ti);
	}
}

clCallTipPtr clEditorTipWindow::GetTip()
{
	if(m_tips.empty())
		return NULL;
		
	return m_tips.at(m_tips.size()-1).tip;
}

bool clEditorTipWindow::IsEmpty()
{
	return m_tips.empty();
}

void clEditorTipWindow::Remove()
{
	if(!m_tips.empty()) {
		m_tips.pop_back();
		
		if(!m_tips.empty()) {
			m_highlighIndex = m_tips.at(m_tips.size()-1).highlightIndex;
		}
	}
	
	if(m_tips.empty())
		Deactivate();
}

void clEditorTipWindow::Clear()
{
	m_tips.clear();
	m_tipText.Clear();
	
	m_highlighIndex = 0;
	m_point         = wxPoint();
	m_lineHeight    = 0;
}

bool clEditorTipWindow::IsActive()
{
	return IsShown();
}

void clEditorTipWindow::Highlight(int argIdxToHilight)
{
	clCallTipPtr tip = GetTip();
	if(tip) {
		m_tipText       = tip->Current();
		m_highlighIndex = argIdxToHilight;
		DoLayoutTip();
	} else {
		Deactivate();
	}
}

void clEditorTipWindow::SelectNext(int argIdxToHilight)
{
	clCallTipPtr tip = GetTip();
	if(tip) {
		m_tipText       = tip->Next();
		m_highlighIndex = argIdxToHilight;
		DoLayoutTip();
	}
}

void clEditorTipWindow::SelectPrev(int argIdxToHilight)
{
	clCallTipPtr tip = GetTip();
	if(tip) {
		m_tipText       = tip->Prev();
		m_highlighIndex = argIdxToHilight;
		DoLayoutTip();
	}
}

wxString clEditorTipWindow::GetText()
{
	clCallTipPtr tip = GetTip();
	if(tip) {
		return tip->All();
	}
	return wxT("");
}

void clEditorTipWindow::Activate(wxPoint pt, int lineHeight)
{
	if(m_tips.empty())
		return;
		
	m_point      = pt;
	m_lineHeight = lineHeight;
	
	DoAdjustPosition();
	
	if(!IsActive()) {
		Show();
	}
}

void clEditorTipWindow::Deactivate()
{
	Clear();
	if(IsShown())
		Hide();
}

wxSize clEditorTipWindow::DoGetTipSize()
{
	int xx, yy;
	
	wxSize sz;
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxWindow::GetTextExtent(m_tipText, &sz.x, &sz.y, NULL, NULL, &font);
	wxWindow::GetTextExtent(wxT("100 of 100"), &xx, &yy, NULL, NULL, &font);
	
	sz.y *= 2;
	sz.y += (2*TIP_SPACER);
	sz.x += (2*TIP_SPACER);
	
	if(sz.x < xx) {
		sz.x = xx;
	}
	return sz;
}

int clEditorTipWindow::DoGetTextLen(const wxString& txt)
{
	int xx, yy;
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxWindow::GetTextExtent(txt, &xx, &yy, NULL, NULL, &font);
	return xx;
}

void clEditorTipWindow::DoAdjustPosition()
{
	wxPoint pt = m_point;
	wxSize sz         = DoGetTipSize();
	wxSize parentSize = GetParent()->GetSize();
	
	pt.y += m_lineHeight;
	
	if(pt.x + sz.x > parentSize.x) {
		// our tip can not fit into the screen, shift it left
		pt.x -= ((pt.x + sz.x) - parentSize.x);
	}
	
	Move(pt);
}

void clEditorTipWindow::DoLayoutTip()
{
	SetSize(DoGetTipSize());
	DoAdjustPosition();
	Layout();
	Refresh();
}
